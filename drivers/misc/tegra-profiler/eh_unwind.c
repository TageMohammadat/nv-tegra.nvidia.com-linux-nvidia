/*
 * drivers/misc/tegra-profiler/exh_tables.c
 *
 * Copyright (c) 2014, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/rcupdate.h>

#include <linux/tegra_profiler.h>

#include "eh_unwind.h"
#include "backtrace.h"
#include "comm.h"

#define QUADD_EXTABS_SIZE	0x100

#define GET_NR_PAGES(a, l) \
	((PAGE_ALIGN((a) + (l)) - ((a) & PAGE_MASK)) / PAGE_SIZE)

enum regs {
	FP_THUMB = 7,
	FP_ARM = 11,

	SP = 13,
	LR = 14,
	PC = 15
};

struct extab_info {
	unsigned long addr;
	unsigned long length;

	unsigned long mmap_offset;
};

struct extables {
	struct extab_info extab;
	struct extab_info exidx;
};

struct ex_region_info {
	unsigned long vm_start;
	unsigned long vm_end;

	struct extables tabs;
	struct quadd_extabs_mmap *mmap;

	struct list_head list;
};

struct regions_data {
	struct ex_region_info *entries;

	unsigned long curr_nr;
	unsigned long size;

	struct rcu_head rcu;
};

struct quadd_unwind_ctx {
	struct regions_data *rd;

	pid_t pid;
	unsigned long ex_tables_size;
	spinlock_t lock;
};

struct unwind_idx {
	u32 addr_offset;
	u32 insn;
};

struct stackframe {
	unsigned long fp_thumb;
	unsigned long fp_arm;

	unsigned long sp;
	unsigned long lr;
	unsigned long pc;
};

struct unwind_ctrl_block {
	u32 vrs[16];		/* virtual register set */
	const u32 *insn;	/* pointer to the current instr word */
	int entries;		/* number of entries left */
	int byte;		/* current byte in the instr word */
};

struct pin_pages_work {
	struct work_struct work;
	unsigned long vm_start;
};

struct quadd_unwind_ctx ctx;

static inline int
validate_stack_addr(unsigned long addr,
		    struct vm_area_struct *vma,
		    unsigned long nbytes)
{
	if (addr & 0x03)
		return 0;

	return is_vma_addr(addr, vma, nbytes);
}

static inline int
validate_pc_addr(unsigned long addr, unsigned long nbytes)
{
	return addr && addr < TASK_SIZE - nbytes;
}

static inline int
validate_mmap_addr(struct quadd_extabs_mmap *mmap,
		   unsigned long addr, unsigned long nbytes)
{
	struct vm_area_struct *vma = mmap->mmap_vma;
	unsigned long size = vma->vm_end - vma->vm_start;
	unsigned long data = (unsigned long)mmap->data;

	if (addr & 0x03) {
		pr_err_once("%s: error: unaligned address: %#lx, data: %#lx-%#lx, vma: %#lx-%#lx\n",
			    __func__, addr, data, data + size,
		       vma->vm_start, vma->vm_end);
		return 0;
	}

	if (addr < data || addr >= data + (size - nbytes)) {
		pr_err_once("%s: error: addr: %#lx, data: %#lx-%#lx, vma: %#lx-%#lx\n",
			    __func__, addr, data, data + size,
		       vma->vm_start, vma->vm_end);
		return 0;
	}

	return 1;
}

#define read_user_data(addr, retval)			\
({							\
	long ret;					\
	ret = probe_kernel_address(addr, retval);	\
	if (ret)					\
		ret = -QUADD_URC_EACCESS;		\
	ret;						\
})

static inline long
read_mmap_data(struct quadd_extabs_mmap *mmap, const u32 *addr, u32 *retval)
{
	if (!validate_mmap_addr(mmap, (unsigned long)addr, sizeof(u32)))
		return -QUADD_URC_EACCESS;

	*retval = *addr;
	return 0;
}

static inline unsigned long
ex_addr_to_mmap_addr(unsigned long addr,
		     struct ex_region_info *ri,
		     int exidx)
{
	unsigned long offset;
	struct extab_info *ei;

	ei = exidx ? &ri->tabs.exidx : &ri->tabs.extab;
	offset = addr - ei->addr;

	return ei->mmap_offset + offset + (unsigned long)ri->mmap->data;
}

static inline unsigned long
mmap_addr_to_ex_addr(unsigned long addr,
		     struct ex_region_info *ri,
		     int exidx)
{
	unsigned long offset;
	struct extab_info *ei;

	ei = exidx ? &ri->tabs.exidx : &ri->tabs.extab;
	offset = addr - ei->mmap_offset - (unsigned long)ri->mmap->data;

	return ei->addr + offset;
}

static inline u32
prel31_to_addr(const u32 *ptr)
{
	u32 value;
	s32 offset;

	if (read_user_data(ptr, value))
		return 0;

	/* sign-extend to 32 bits */
	offset = (((s32)value) << 1) >> 1;
	return (u32)(unsigned long)ptr + offset;
}

static unsigned long
mmap_prel31_to_addr(const u32 *ptr, struct ex_region_info *ri,
		    int is_src_exidx, int is_dst_exidx, int to_mmap)
{
	u32 value, addr;
	unsigned long addr_res;
	s32 offset;
	struct extab_info *ei_src, *ei_dst;

	ei_src = is_src_exidx ? &ri->tabs.exidx : &ri->tabs.extab;
	ei_dst = is_dst_exidx ? &ri->tabs.exidx : &ri->tabs.extab;

	value = *ptr;
	offset = (((s32)value) << 1) >> 1;

	addr = mmap_addr_to_ex_addr((unsigned long)ptr, ri, is_src_exidx);
	addr += offset;
	addr_res = addr;

	if (to_mmap)
		addr_res = ex_addr_to_mmap_addr(addr_res, ri, is_dst_exidx);

	return addr_res;
}

static int
add_ex_region(struct regions_data *rd,
	      struct ex_region_info *new_entry)
{
	unsigned int i_min, i_max, mid;
	struct ex_region_info *array = rd->entries;
	unsigned long size = rd->curr_nr;

	if (!array)
		return 0;

	if (size == 0) {
		memcpy(&array[0], new_entry, sizeof(*new_entry));
		return 1;
	} else if (size == 1 && array[0].vm_start == new_entry->vm_start) {
		return 0;
	}

	i_min = 0;
	i_max = size;

	if (array[0].vm_start > new_entry->vm_start) {
		memmove(array + 1, array,
			size * sizeof(*array));
		memcpy(&array[0], new_entry, sizeof(*new_entry));
		return 1;
	} else if (array[size - 1].vm_start < new_entry->vm_start) {
		memcpy(&array[size], new_entry, sizeof(*new_entry));
		return 1;
	}

	while (i_min < i_max) {
		mid = i_min + (i_max - i_min) / 2;

		if (new_entry->vm_start <= array[mid].vm_start)
			i_max = mid;
		else
			i_min = mid + 1;
	}

	if (array[i_max].vm_start == new_entry->vm_start) {
		return 0;
	} else {
		memmove(array + i_max + 1,
			array + i_max,
			(size - i_max) * sizeof(*array));
		memcpy(&array[i_max], new_entry, sizeof(*new_entry));
		return 1;
	}
}

static int
remove_ex_region(struct regions_data *rd,
		 struct ex_region_info *entry)
{
	unsigned int i_min, i_max, mid;
	struct ex_region_info *array = rd->entries;
	unsigned long size = rd->curr_nr;

	if (!array)
		return 0;

	if (size == 0)
		return 0;

	if (size == 1) {
		if (array[0].vm_start == entry->vm_start)
			return 1;
		else
			return 0;
	}

	if (array[0].vm_start > entry->vm_start)
		return 0;
	else if (array[size - 1].vm_start < entry->vm_start)
		return 0;

	i_min = 0;
	i_max = size;

	while (i_min < i_max) {
		mid = i_min + (i_max - i_min) / 2;

		if (entry->vm_start <= array[mid].vm_start)
			i_max = mid;
		else
			i_min = mid + 1;
	}

	if (array[i_max].vm_start == entry->vm_start) {
		memmove(array + i_max,
			array + i_max + 1,
			(size - i_max) * sizeof(*array));
		return 1;
	} else {
		return 0;
	}
}

static struct ex_region_info *
search_ex_region(struct ex_region_info *array,
		 unsigned long size,
		 unsigned long key,
		 struct ex_region_info *ri)
{
	unsigned int i_min, i_max, mid;

	if (size == 0)
		return NULL;

	i_min = 0;
	i_max = size;

	while (i_min < i_max) {
		mid = i_min + (i_max - i_min) / 2;

		if (key <= array[mid].vm_start)
			i_max = mid;
		else
			i_min = mid + 1;
	}

	if (array[i_max].vm_start == key) {
		memcpy(ri, &array[i_max], sizeof(*ri));
		return &array[i_max];
	}

	return NULL;
}

static long
__search_ex_region(unsigned long key, struct ex_region_info *ri)
{
	struct regions_data *rd;
	struct ex_region_info *ri_p = NULL;

	rcu_read_lock();

	rd = rcu_dereference(ctx.rd);
	if (!rd)
		goto out;

	ri_p = search_ex_region(rd->entries, rd->curr_nr, key, ri);

out:
	rcu_read_unlock();
	return ri_p ? 0 : -ENOENT;
}

static struct regions_data *rd_alloc(unsigned long size)
{
	struct regions_data *rd;

	rd = kzalloc(sizeof(*rd), GFP_KERNEL);
	if (!rd)
		return NULL;

	rd->entries = kzalloc(size * sizeof(*rd->entries), GFP_KERNEL);
	if (!rd->entries) {
		kfree(rd);
		return NULL;
	}

	rd->size = size;
	rd->curr_nr = 0;

	return rd;
}

static void rd_free(struct regions_data *rd)
{
	if (rd)
		kfree(rd->entries);

	kfree(rd);
}

static void rd_free_rcu(struct rcu_head *rh)
{
	struct regions_data *rd = container_of(rh, struct regions_data, rcu);
	rd_free(rd);
}

int quadd_unwind_set_extab(struct quadd_extables *extabs,
			   struct quadd_extabs_mmap *mmap)
{
	int err = 0;
	unsigned long nr_entries, nr_added, new_size;
	struct ex_region_info ri_entry;
	struct extab_info *ti;
	struct regions_data *rd, *rd_new;
	struct ex_region_info *ex_entry;

	spin_lock(&ctx.lock);

	rd = rcu_dereference(ctx.rd);
	if (!rd) {
		pr_warn("%s: warning: rd\n", __func__);
		new_size = QUADD_EXTABS_SIZE;
		nr_entries = 0;
	} else {
		new_size = rd->size;
		nr_entries = rd->curr_nr;
	}

	if (nr_entries >= new_size)
		new_size += new_size >> 1;

	rd_new = rd_alloc(new_size);
	if (IS_ERR_OR_NULL(rd_new)) {
		pr_err("%s: error: rd_alloc\n", __func__);
		err = -ENOMEM;
		goto error_out;
	}

	if (rd && nr_entries)
		memcpy(rd_new->entries, rd->entries,
		       nr_entries * sizeof(*rd->entries));

	rd_new->curr_nr = nr_entries;

	ri_entry.vm_start = extabs->vm_start;
	ri_entry.vm_end = extabs->vm_end;

	ri_entry.mmap = mmap;

	ti = &ri_entry.tabs.exidx;
	ti->addr = extabs->exidx.addr;
	ti->length = extabs->exidx.length;
	ti->mmap_offset = extabs->reserved[QUADD_EXT_IDX_EXIDX_OFFSET];
	ctx.ex_tables_size += ti->length;

	ti = &ri_entry.tabs.extab;
	ti->addr = extabs->extab.addr;
	ti->length = extabs->extab.length;
	ti->mmap_offset = extabs->reserved[QUADD_EXT_IDX_EXTAB_OFFSET];
	ctx.ex_tables_size += ti->length;

	nr_added = add_ex_region(rd_new, &ri_entry);
	if (nr_added == 0) {
		rd_free(rd_new);
		goto error_out;
	}
	rd_new->curr_nr += nr_added;

	ex_entry = kzalloc(sizeof(*ex_entry), GFP_KERNEL);
	if (!ex_entry) {
		err = -ENOMEM;
		goto error_out;
	}
	memcpy(ex_entry, &ri_entry, sizeof(*ex_entry));

	INIT_LIST_HEAD(&ex_entry->list);
	list_add_tail(&ex_entry->list, &mmap->ex_entries);

	rcu_assign_pointer(ctx.rd, rd_new);

	if (rd)
		call_rcu(&rd->rcu, rd_free_rcu);

	spin_unlock(&ctx.lock);

	return 0;

error_out:
	spin_unlock(&ctx.lock);
	return err;
}

static int
clean_mmap(struct regions_data *rd, struct quadd_extabs_mmap *mmap, int rm_ext)
{
	int nr_removed = 0;
	struct ex_region_info *entry, *next;

	if (!rd || !mmap)
		return 0;

	list_for_each_entry_safe(entry, next, &mmap->ex_entries, list) {
		if (rm_ext)
			nr_removed += remove_ex_region(rd, entry);

		list_del(&entry->list);
		kfree(entry);
	}

	return nr_removed;
}

void quadd_unwind_delete_mmap(struct quadd_extabs_mmap *mmap)
{
	unsigned long nr_entries, nr_removed, new_size;
	struct regions_data *rd, *rd_new;

	if (!mmap)
		return;

	spin_lock(&ctx.lock);

	rd = rcu_dereference(ctx.rd);
	if (!rd || !rd->curr_nr)
		goto error_out;

	nr_entries = rd->curr_nr;
	new_size = min_t(unsigned long, rd->size, nr_entries);

	rd_new = rd_alloc(new_size);
	if (IS_ERR_OR_NULL(rd_new)) {
		pr_err("%s: error: rd_alloc\n", __func__);
		goto error_out;
	}
	rd_new->size = new_size;
	rd_new->curr_nr = nr_entries;

	memcpy(rd_new->entries, rd->entries,
		nr_entries * sizeof(*rd->entries));

	nr_removed = clean_mmap(rd_new, mmap, 1);
	rd_new->curr_nr -= nr_removed;

	rcu_assign_pointer(ctx.rd, rd_new);
	call_rcu(&rd->rcu, rd_free_rcu);

error_out:
	spin_unlock(&ctx.lock);
}

static const struct unwind_idx *
unwind_find_idx(struct ex_region_info *ri, u32 addr)
{
	unsigned long length;
	u32 value;
	struct unwind_idx *start;
	struct unwind_idx *stop;
	struct unwind_idx *mid = NULL;
	length = ri->tabs.exidx.length / sizeof(*start);

	if (unlikely(!length))
		return NULL;

	start = (struct unwind_idx *)((char *)ri->mmap->data +
		ri->tabs.exidx.mmap_offset);
	stop = start + length - 1;

	value = (u32)mmap_prel31_to_addr(&start->addr_offset, ri, 1, 0, 0);
	if (addr < value)
		return NULL;

	value = (u32)mmap_prel31_to_addr(&stop->addr_offset, ri, 1, 0, 0);
	if (addr >= value)
		return NULL;

	while (start < stop - 1) {
		mid = start + ((stop - start) >> 1);

		value = (u32)mmap_prel31_to_addr(&mid->addr_offset,
						 ri, 1, 0, 0);

		if (addr < value)
			stop = mid;
		else
			start = mid;
	}

	return start;
}

static unsigned long
unwind_get_byte(struct quadd_extabs_mmap *mmap,
		struct unwind_ctrl_block *ctrl, long *err)
{
	unsigned long ret;
	u32 insn_word;

	*err = 0;

	if (ctrl->entries <= 0) {
		pr_err_once("%s: error: corrupt unwind table\n", __func__);
		*err = -QUADD_URC_TBL_IS_CORRUPT;
		return 0;
	}

	*err = read_mmap_data(mmap, ctrl->insn, &insn_word);
	if (*err < 0)
		return 0;

	ret = (insn_word >> (ctrl->byte * 8)) & 0xff;

	if (ctrl->byte == 0) {
		ctrl->insn++;
		ctrl->entries--;
		ctrl->byte = 3;
	} else
		ctrl->byte--;

	return ret;
}

/*
 * Execute the current unwind instruction.
 */
static long
unwind_exec_insn(struct quadd_extabs_mmap *mmap,
		 struct unwind_ctrl_block *ctrl)
{
	long err;
	unsigned int i;
	unsigned long insn = unwind_get_byte(mmap, ctrl, &err);

	if (err < 0)
		return err;

	pr_debug("%s: insn = %08lx\n", __func__, insn);

	if ((insn & 0xc0) == 0x00) {
		ctrl->vrs[SP] += ((insn & 0x3f) << 2) + 4;

		pr_debug("CMD_DATA_POP: vsp = vsp + %lu (new: %#x)\n",
			((insn & 0x3f) << 2) + 4, ctrl->vrs[SP]);
	} else if ((insn & 0xc0) == 0x40) {
		ctrl->vrs[SP] -= ((insn & 0x3f) << 2) + 4;

		pr_debug("CMD_DATA_PUSH: vsp = vsp – %lu (new: %#x)\n",
			((insn & 0x3f) << 2) + 4, ctrl->vrs[SP]);
	} else if ((insn & 0xf0) == 0x80) {
		unsigned long mask;
		u32 *vsp = (u32 *)(unsigned long)ctrl->vrs[SP];
		int load_sp, reg = 4;

		insn = (insn << 8) | unwind_get_byte(mmap, ctrl, &err);
		if (err < 0)
			return err;

		mask = insn & 0x0fff;
		if (mask == 0) {
			pr_debug("CMD_REFUSED: unwind: 'Refuse to unwind' instruction %04lx\n",
				   insn);
			return -QUADD_URC_REFUSE_TO_UNWIND;
		}

		/* pop R4-R15 according to mask */
		load_sp = mask & (1 << (13 - 4));
		while (mask) {
			if (mask & 1) {
				err = read_user_data(vsp++, ctrl->vrs[reg]);
				if (err < 0)
					return err;

				pr_debug("CMD_REG_POP: pop {r%d}\n", reg);
			}
			mask >>= 1;
			reg++;
		}
		if (!load_sp)
			ctrl->vrs[SP] = (unsigned long)vsp;

		pr_debug("new vsp: %#x\n", ctrl->vrs[SP]);
	} else if ((insn & 0xf0) == 0x90 &&
		   (insn & 0x0d) != 0x0d) {
		ctrl->vrs[SP] = ctrl->vrs[insn & 0x0f];
		pr_debug("CMD_REG_TO_SP: vsp = {r%lu}\n", insn & 0x0f);
	} else if ((insn & 0xf0) == 0xa0) {
		u32 *vsp = (u32 *)(unsigned long)ctrl->vrs[SP];
		unsigned int reg;

		/* pop R4-R[4+bbb] */
		for (reg = 4; reg <= 4 + (insn & 7); reg++) {
			err = read_user_data(vsp++, ctrl->vrs[reg]);
			if (err < 0)
				return err;

			pr_debug("CMD_REG_POP: pop {r%u}\n", reg);
		}

		if (insn & 0x08) {
			err = read_user_data(vsp++, ctrl->vrs[14]);
			if (err < 0)
				return err;

			pr_debug("CMD_REG_POP: pop {r14}\n");
		}

		ctrl->vrs[SP] = (u32)(unsigned long)vsp;
		pr_debug("new vsp: %#x\n", ctrl->vrs[SP]);
	} else if (insn == 0xb0) {
		if (ctrl->vrs[PC] == 0)
			ctrl->vrs[PC] = ctrl->vrs[LR];
		/* no further processing */
		ctrl->entries = 0;

		pr_debug("CMD_FINISH\n");
	} else if (insn == 0xb1) {
		unsigned long mask = unwind_get_byte(mmap, ctrl, &err);
		u32 *vsp = (u32 *)(unsigned long)ctrl->vrs[SP];
		int reg = 0;

		if (err < 0)
			return err;

		if (mask == 0 || mask & 0xf0) {
			pr_debug("unwind: Spare encoding %04lx\n",
			       (insn << 8) | mask);
			return -QUADD_URC_SPARE_ENCODING;
		}

		/* pop R0-R3 according to mask */
		while (mask) {
			if (mask & 1) {
				err = read_user_data(vsp++, ctrl->vrs[reg]);
				if (err < 0)
					return err;

				pr_debug("CMD_REG_POP: pop {r%d}\n", reg);
			}
			mask >>= 1;
			reg++;
		}

		ctrl->vrs[SP] = (u32)(unsigned long)vsp;
		pr_debug("new vsp: %#x\n", ctrl->vrs[SP]);
	} else if (insn == 0xb2) {
		unsigned long uleb128 = unwind_get_byte(mmap, ctrl, &err);
		if (err < 0)
			return err;

		ctrl->vrs[SP] += 0x204 + (uleb128 << 2);

		pr_debug("CMD_DATA_POP: vsp = vsp + %lu, new vsp: %#x\n",
			 0x204 + (uleb128 << 2), ctrl->vrs[SP]);
	} else if (insn == 0xb3 || insn == 0xc8 || insn == 0xc9) {
		unsigned long data, reg_from, reg_to;
		u32 *vsp = (u32 *)(unsigned long)ctrl->vrs[SP];

		data = unwind_get_byte(mmap, ctrl, &err);
		if (err < 0)
			return err;

		reg_from = (data & 0xf0) >> 4;
		reg_to = reg_from + (data & 0x0f);

		if (insn == 0xc8) {
			reg_from += 16;
			reg_to += 16;
		}

		for (i = reg_from; i <= reg_to; i++)
			vsp += 2;

		if (insn == 0xb3)
			vsp++;

		ctrl->vrs[SP] = (unsigned long)vsp;
		ctrl->vrs[SP] = (u32)(unsigned long)vsp;

		pr_debug("CMD_VFP_POP (%#lx %#lx): pop {D%lu-D%lu}\n",
			 insn, data, reg_from, reg_to);
		pr_debug("new vsp: %#x\n", ctrl->vrs[SP]);
	} else if ((insn & 0xf8) == 0xb8 || (insn & 0xf8) == 0xd0) {
		unsigned long reg_to;
		unsigned long data = insn & 0x07;
		u32 *vsp = (u32 *)(unsigned long)ctrl->vrs[SP];

		reg_to = 8 + data;

		for (i = 8; i <= reg_to; i++)
			vsp += 2;

		if ((insn & 0xf8) == 0xb8)
			vsp++;

		ctrl->vrs[SP] = (u32)(unsigned long)vsp;

		pr_debug("CMD_VFP_POP (%#lx): pop {D8-D%lu}\n",
			 insn, reg_to);
		pr_debug("new vsp: %#x\n", ctrl->vrs[SP]);
	} else {
		pr_debug("error: unhandled instruction %02lx\n", insn);
		return -QUADD_URC_UNHANDLED_INSTRUCTION;
	}

	pr_debug("%s: fp_arm: %#x, fp_thumb: %#x, sp: %#x, lr = %#x, pc: %#x\n",
		 __func__,
		 ctrl->vrs[FP_ARM], ctrl->vrs[FP_THUMB], ctrl->vrs[SP],
		 ctrl->vrs[LR], ctrl->vrs[PC]);

	return 0;
}

/*
 * Unwind a single frame starting with *sp for the symbol at *pc. It
 * updates the *pc and *sp with the new values.
 */
static long
unwind_frame(struct ex_region_info *ri,
	     struct stackframe *frame,
	     struct vm_area_struct *vma_sp)
{
	unsigned long high, low;
	const struct unwind_idx *idx;
	struct unwind_ctrl_block ctrl;
	long err = 0;
	u32 val;

	if (!validate_stack_addr(frame->sp, vma_sp, sizeof(u32)))
		return -QUADD_URC_SP_INCORRECT;

	/* only go to a higher address on the stack */
	low = frame->sp;
	high = vma_sp->vm_end;

	pr_debug("pc: %#lx, lr: %#lx, sp:%#lx, low/high: %#lx/%#lx\n",
		frame->pc, frame->lr, frame->sp, low, high);

	idx = unwind_find_idx(ri, frame->pc);
	if (IS_ERR_OR_NULL(idx))
		return -QUADD_URC_IDX_NOT_FOUND;

	pr_debug("index was found by pc (%#lx): %p\n", frame->pc, idx);

	ctrl.vrs[FP_THUMB] = frame->fp_thumb;
	ctrl.vrs[FP_ARM] = frame->fp_arm;

	ctrl.vrs[SP] = frame->sp;
	ctrl.vrs[LR] = frame->lr;
	ctrl.vrs[PC] = 0;

	err = read_mmap_data(ri->mmap, &idx->insn, &val);
	if (err < 0)
		return err;

	if (val == 1) {
		/* can't unwind */
		return -QUADD_URC_CANTUNWIND;
	} else if ((val & 0x80000000) == 0) {
		/* prel31 to the unwind table */
		ctrl.insn = (u32 *)(unsigned long)
				mmap_prel31_to_addr(&idx->insn, ri, 1, 0, 1);
		if (!ctrl.insn)
			return -QUADD_URC_EACCESS;
	} else if ((val & 0xff000000) == 0x80000000) {
		/* only personality routine 0 supported in the index */
		ctrl.insn = &idx->insn;
	} else {
		pr_debug("unsupported personality routine %#x in the index at %p\n",
			 val, idx);
		return -QUADD_URC_UNSUPPORTED_PR;
	}

	err = read_mmap_data(ri->mmap, ctrl.insn, &val);
	if (err < 0)
		return err;

	/* check the personality routine */
	if ((val & 0xff000000) == 0x80000000) {
		ctrl.byte = 2;
		ctrl.entries = 1;
	} else if ((val & 0xff000000) == 0x81000000) {
		ctrl.byte = 1;
		ctrl.entries = 1 + ((val & 0x00ff0000) >> 16);
	} else {
		pr_debug("unsupported personality routine %#x at %p\n",
			 val, ctrl.insn);
		return -QUADD_URC_UNSUPPORTED_PR;
	}

	while (ctrl.entries > 0) {
		err = unwind_exec_insn(ri->mmap, &ctrl);
		if (err < 0)
			return err;

		if (ctrl.vrs[SP] & 0x03 ||
		    ctrl.vrs[SP] < low || ctrl.vrs[SP] >= high)
			return -QUADD_URC_SP_INCORRECT;
	}

	if (ctrl.vrs[PC] == 0)
		ctrl.vrs[PC] = ctrl.vrs[LR];

	/* check for infinite loop */
	if (frame->pc == ctrl.vrs[PC])
		return -QUADD_URC_FAILURE;

	if (!validate_pc_addr(ctrl.vrs[PC], sizeof(u32)))
		return -QUADD_URC_PC_INCORRECT;

	frame->fp_thumb = ctrl.vrs[FP_THUMB];
	frame->fp_arm = ctrl.vrs[FP_ARM];

	frame->sp = ctrl.vrs[SP];
	frame->lr = ctrl.vrs[LR];
	frame->pc = ctrl.vrs[PC];

	return 0;
}

static void
unwind_backtrace(struct quadd_callchain *cc,
		 struct ex_region_info *ri,
		 struct pt_regs *regs,
		 struct vm_area_struct *vma_sp,
		 struct task_struct *task)
{
	struct ex_region_info ri_new;
	struct stackframe frame;

#ifdef CONFIG_ARM64
	frame.fp_thumb = regs->compat_usr(7);
	frame.fp_arm = regs->compat_usr(11);
#else
	frame.fp_thumb = regs->ARM_r7;
	frame.fp_arm = regs->ARM_fp;
#endif

	frame.pc = instruction_pointer(regs);
	frame.sp = quadd_user_stack_pointer(regs);
	frame.lr = quadd_user_link_register(regs);

	cc->unw_rc = QUADD_URC_FAILURE;

	pr_debug("fp_arm: %#lx, fp_thumb: %#lx, sp: %#lx, lr: %#lx, pc: %#lx\n",
		 frame.fp_arm, frame.fp_thumb, frame.sp, frame.lr, frame.pc);
	pr_debug("vma_sp: %#lx - %#lx, length: %#lx\n",
		 vma_sp->vm_start, vma_sp->vm_end,
		 vma_sp->vm_end - vma_sp->vm_start);

	while (1) {
		long err;
		unsigned long where = frame.pc;
		struct vm_area_struct *vma_pc;
		struct mm_struct *mm = task->mm;

		if (!mm)
			break;

		if (!validate_stack_addr(frame.sp, vma_sp, sizeof(u32))) {
			cc->unw_rc = -QUADD_URC_SP_INCORRECT;
			break;
		}

		vma_pc = find_vma(mm, frame.pc);
		if (!vma_pc)
			break;

		if (!is_vma_addr(ri->tabs.exidx.addr, vma_pc, sizeof(u32))) {
			err = __search_ex_region(vma_pc->vm_start, &ri_new);
			if (err) {
				cc->unw_rc = QUADD_URC_TBL_NOT_EXIST;
				break;
			}

			ri = &ri_new;
		}

		err = unwind_frame(ri, &frame, vma_sp);
		if (err < 0) {
			pr_debug("end unwind, urc: %ld\n", err);
			cc->unw_rc = -err;
			break;
		}

		pr_debug("function at [<%08lx>] from [<%08lx>]\n",
			 where, frame.pc);

		quadd_callchain_store(cc, frame.pc);

		cc->curr_sp = frame.sp;
		cc->curr_fp = frame.fp_arm;
	}
}

unsigned int
quadd_get_user_callchain_ut(struct pt_regs *regs,
			    struct quadd_callchain *cc,
			    struct task_struct *task)
{
	long err;
	unsigned long ip, sp;
	struct vm_area_struct *vma, *vma_sp;
	struct mm_struct *mm = task->mm;
	struct ex_region_info ri;

	cc->unw_method = QUADD_UNW_METHOD_EHT;
	cc->unw_rc = QUADD_URC_FAILURE;

#ifdef CONFIG_ARM64
	if (!compat_user_mode(regs)) {
		pr_warn_once("user_mode 64: unsupported\n");
		return 0;
	}
#endif

	if (!regs || !mm)
		return 0;

	ip = instruction_pointer(regs);
	sp = quadd_user_stack_pointer(regs);

	vma = find_vma(mm, ip);
	if (!vma)
		return 0;

	vma_sp = find_vma(mm, sp);
	if (!vma_sp)
		return 0;

	err = __search_ex_region(vma->vm_start, &ri);
	if (err) {
		cc->unw_rc = QUADD_URC_TBL_NOT_EXIST;
		return 0;
	}

	unwind_backtrace(cc, &ri, regs, vma_sp, task);

	return cc->nr;
}

int quadd_unwind_start(struct task_struct *task)
{
	struct regions_data *rd, *rd_old;

	spin_lock(&ctx.lock);

	rd_old = rcu_dereference(ctx.rd);
	if (rd_old)
		pr_warn("%s: warning: rd_old\n", __func__);

	rd = rd_alloc(QUADD_EXTABS_SIZE);
	if (IS_ERR_OR_NULL(rd)) {
		pr_err("%s: error: rd_alloc\n", __func__);
		spin_unlock(&ctx.lock);
		return -ENOMEM;
	}

	rcu_assign_pointer(ctx.rd, rd);

	if (rd_old)
		call_rcu(&rd_old->rcu, rd_free_rcu);

	ctx.pid = task->tgid;

	ctx.ex_tables_size = 0;

	spin_unlock(&ctx.lock);

	return 0;
}

void quadd_unwind_stop(void)
{
	int i;
	unsigned long nr_entries, size;
	struct regions_data *rd;
	struct ex_region_info *ri;

	spin_lock(&ctx.lock);

	ctx.pid = 0;

	rd = rcu_dereference(ctx.rd);
	if (!rd)
		goto out;

	nr_entries = rd->curr_nr;
	size = rd->size;

	for (i = 0; i < nr_entries; i++) {
		ri = &rd->entries[i];
		clean_mmap(rd, ri->mmap, 0);
	}

	rcu_assign_pointer(ctx.rd, NULL);
	call_rcu(&rd->rcu, rd_free_rcu);

out:
	spin_unlock(&ctx.lock);
	pr_info("exception tables size: %lu bytes\n", ctx.ex_tables_size);
}

int quadd_unwind_init(void)
{
	spin_lock_init(&ctx.lock);
	rcu_assign_pointer(ctx.rd, NULL);
	ctx.pid = 0;

	return 0;
}

void quadd_unwind_deinit(void)
{
	quadd_unwind_stop();
	rcu_barrier();
}
