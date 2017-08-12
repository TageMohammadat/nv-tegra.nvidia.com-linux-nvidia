/*
 * Copyright (c) 2017, NVIDIA CORPORATION.  All rights reserved.
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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

static const int coreb01_voltages_mv[MAX_DVFS_FREQS] = {
	650, 675, 700, 725, 750, 775, 800, 825, 850, 875, 900, 925, 950, 975, 1000, 1025, 1050
};

static struct dvfs coreb01_dvfs_table[] = {
/* Per-bin Tables */
	/* Core voltages(mV):				    650,     675,     700,     725,     750,     775,     800,     825,     850,     875,     900,     925,     950,     975,    1000,    1025,    1050 */
	CORE_DVFS("emc",		0, 0, 1, KHZ,	      1,       1,       1,       1,       1,       1,       1,       1, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000),

	CORE_DVFS("vic03",		0, 0, 1, KHZ,	 153600,  153600,  153600,  268800,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200),
	CORE_DVFS("nvjpg",		0, 0, 1, KHZ,	 153600,  153600,  153600,  268800,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200),
	CORE_DVFS("se",			0, 0, 1, KHZ,	 153600,  153600,  153600,  268800,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200),
	CORE_DVFS("tsecb",		0, 0, 1, KHZ,	 153600,  153600,  153600,  268800,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200),
	CORE_DVFS("c2bus",		0, 0, 1, KHZ,	 153600,  153600,  153600,  268800,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200),

	CORE_DVFS("nvenc",		0, 0, 1, KHZ,	 326400,  326400,  326400,  384000,  441600,  499200,  556800,  614400,  652800,  691200,  710400,  729600,  729600,  729600,  729600,  729600,  729600),
	CORE_DVFS("nvdec",		0, 0, 1, KHZ,	 326400,  326400,  326400,  384000,  441600,  499200,  556800,  614400,  652800,  691200,  710400,  729600,  729600,  729600,  729600,  729600,  729600),
	CORE_DVFS("c3bus",		0, 0, 1, KHZ,	 326400,  326400,  326400,  384000,  441600,  499200,  556800,  614400,  652800,  691200,  710400,  729600,  729600,  729600,  729600,  729600,  729600),

	CORE_DVFS("vi",			0, 0, 1, KHZ,	 364800,  364800,  364800,  384000,  422400,  460800,  499200,  518400,  556800,  595200,  652800,  691200,  748800,  806400,  940800, 1017600, 1075200),
	CORE_DVFS("cbus",		0, 0, 1, KHZ,	 364800,  364800,  364800,  384000,  422400,  460800,  499200,  518400,  556800,  595200,  652800,  691200,  748800,  806400,  940800, 1017600, 1075200),

	CORE_DVFS("abus",		0, 0, 1, KHZ,	 537600,  537600,  537600,  595200,  652800,  729600,  806400,  864000,  921600,  979200, 1036800, 1075200, 1132800, 1171200, 1190400, 1190400, 1190400),
	CORE_DVFS("aclk",		0, 0, 1, KHZ,	 537600,  537600,  537600,  595200,  652800,  729600,  806400,  864000,  921600,  979200, 1036800, 1075200, 1132800, 1171200, 1190400, 1190400, 1190400),

	CORE_DVFS("ape",		0, 0, 1, KHZ,	 408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000),
	CORE_DVFS("sbus",		0, 0, 1, KHZ,	 204000,  204000,  204000,  204000,  204000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000),
	CORE_DVFS("sclk",		0, 0, 1, KHZ,	 204000,  204000,  204000,  204000,  204000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000),

	CORE_DVFS("host1x",		0, 0, 1, KHZ,	 136000,  136000,  136000,  136000,  136000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),
	CORE_DVFS("tsec",		0, 0, 1, KHZ,	 255045,  255045,  255045,  332332,  397906,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000),
	CORE_DVFS("mselect",		0, 0, 1, KHZ,	 204000,  204000,  204000,  204000,  204000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000),

	CORE_DVFS("disp1",		0, 0, 0, KHZ,	 139875,  139875,  139875,  227539,  323330,  419140,  481198,  524222,  558885,  581514,  600645,  612344,  623984,  635498,  658472,  669772,  669772),
	CORE_DVFS("disp2",		0, 0, 0, KHZ,	 139875,  139875,  139875,  227539,  323330,  419140,  481198,  524222,  558885,  581514,  600645,  612344,  623984,  635498,  658472,  669772,  669772),

	/* Core voltages(mV):				    650,     675,     700,     725,     750,     775,     800,     825,     850,     875,     900,     925,     950,     975,    1000,    1025,    1050 */
	CORE_DVFS("emc",		0, 1, 1, KHZ,	      1,       1,       1,       1,       1,       1,       1,       1, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000),

	CORE_DVFS("vic03",		0, 1, 1, KHZ,	 268800,  268800,  268800,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200,  595200),
	CORE_DVFS("nvjpg",		0, 1, 1, KHZ,	 268800,  268800,  268800,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200,  595200),
	CORE_DVFS("se",			0, 1, 1, KHZ,	 268800,  268800,  268800,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200,  595200),
	CORE_DVFS("tsecb",		0, 1, 1, KHZ,	 268800,  268800,  268800,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200,  595200),
	CORE_DVFS("c2bus",		0, 1, 1, KHZ,	 268800,  268800,  268800,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200,  595200),

	CORE_DVFS("nvenc",		0, 1, 1, KHZ,	 384000,  384000,  384000,  441600,  499200,  556800,  614400,  652800,  691200,  710400,  729600,  729600,  729600,  729600,  729600,  729600,  729600),
	CORE_DVFS("nvdec",		0, 1, 1, KHZ,	 384000,  384000,  384000,  441600,  499200,  556800,  614400,  652800,  691200,  710400,  729600,  729600,  729600,  729600,  729600,  729600,  729600),
	CORE_DVFS("c3bus",		0, 1, 1, KHZ,	 384000,  384000,  384000,  441600,  499200,  556800,  614400,  652800,  691200,  710400,  729600,  729600,  729600,  729600,  729600,  729600,  729600),

	CORE_DVFS("vi",			0, 1, 1, KHZ,	 384000,  384000,  384000,  422400,  460800,  499200,  518400,  556800,  595200,  652800,  691200,  748800,  806400,  940800, 1017600, 1075200, 1075200),
	CORE_DVFS("cbus",		0, 1, 1, KHZ,	 384000,  384000,  384000,  422400,  460800,  499200,  518400,  556800,  595200,  652800,  691200,  748800,  806400,  940800, 1017600, 1075200, 1075200),

	CORE_DVFS("abus",		0, 1, 1, KHZ,	 595200,  595200,  595200,  652800,  729600,  806400,  864000,  921600,  979200, 1036800, 1075200, 1132800, 1171200, 1190400, 1190400, 1190400, 1190400),
	CORE_DVFS("aclk",		0, 1, 1, KHZ,	 595200,  595200,  595200,  652800,  729600,  806400,  864000,  921600,  979200, 1036800, 1075200, 1132800, 1171200, 1190400, 1190400, 1190400, 1190400),

	CORE_DVFS("ape",		0, 1, 1, KHZ,	 408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000),
	CORE_DVFS("sbus",		0, 1, 1, KHZ,	 204000,  204000,  204000,  204000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000),
	CORE_DVFS("sclk",		0, 1, 1, KHZ,	 204000,  204000,  204000,  204000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000),

	CORE_DVFS("host1x",		0, 1, 1, KHZ,	 136000,  136000,  136000,  136000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),
	CORE_DVFS("tsec",		0, 1, 1, KHZ,	 332332,  332332,  332332,  397906,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000),
	CORE_DVFS("mselect",		0, 1, 1, KHZ,	 204000,  204000,  204000,  204000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000),

	CORE_DVFS("disp1",		0, 1, 0, KHZ,	 227539,  227539,  227539,  323330,  419140,  481198,  524222,  558885,  581514,  600645,  612344,  623984,  635498,  658472,  669772,  669772,  669772),
	CORE_DVFS("disp2",		0, 1, 0, KHZ,	 227539,  227539,  227539,  323330,  419140,  481198,  524222,  558885,  581514,  600645,  612344,  623984,  635498,  658472,  669772,  669772,  669772),

	/* Core voltages(mV):				    650,     675,     700,     725,     750,     775,     800,     825,     850,     875,     900,     925,     950,     975,    1000,    1025,    1050 */
	CORE_DVFS("emc",		0, 2, 1, KHZ,	      1,       1,       1,       1,       1,       1,       1,       1, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000, 1600000),

	CORE_DVFS("vic03",		0, 2, 1, KHZ,	 345600,  345600,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200,  595200,  595200),
	CORE_DVFS("nvjpg",		0, 2, 1, KHZ,	 345600,  345600,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200,  595200,  595200),
	CORE_DVFS("se",			0, 2, 1, KHZ,	 345600,  345600,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200,  595200,  595200),
	CORE_DVFS("tsecb",		0, 2, 1, KHZ,	 345600,  345600,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200,  595200,  595200),
	CORE_DVFS("c2bus",		0, 2, 1, KHZ,	 345600,  345600,  345600,  422400,  480000,  499200,  518400,  537600,  537600,  556800,  556800,  576000,  595200,  595200,  595200,  595200,  595200),

	CORE_DVFS("nvenc",		0, 2, 1, KHZ,	 441600,  441600,  441600,  499200,  556800,  614400,  652800,  691200,  710400,  729600,  729600,  729600,  729600,  729600,  729600,  729600,  729600),
	CORE_DVFS("nvdec",		0, 2, 1, KHZ,	 441600,  441600,  441600,  499200,  556800,  614400,  652800,  691200,  710400,  729600,  729600,  729600,  729600,  729600,  729600,  729600,  729600),
	CORE_DVFS("c3bus",		0, 2, 1, KHZ,	 441600,  441600,  441600,  499200,  556800,  614400,  652800,  691200,  710400,  729600,  729600,  729600,  729600,  729600,  729600,  729600,  729600),

	CORE_DVFS("vi",			0, 2, 1, KHZ,	 422400,  422400,  422400,  460800,  499200,  518400,  556800,  595200,  652800,  691200,  748800,  806400,  940800, 1017600, 1075200, 1075200, 1075200),
	CORE_DVFS("cbus",		0, 2, 1, KHZ,	 422400,  422400,  422400,  460800,  499200,  518400,  556800,  595200,  652800,  691200,  748800,  806400,  940800, 1017600, 1075200, 1075200, 1075200),

	CORE_DVFS("abus",		0, 2, 1, KHZ,	 652800,  652800,  652800,  729600,  806400,  864000,  921600,  979200, 1036800, 1075200, 1132800, 1171200, 1190400, 1190400, 1190400, 1190400, 1190400),
	CORE_DVFS("aclk",		0, 2, 1, KHZ,	 652800,  652800,  652800,  729600,  806400,  864000,  921600,  979200, 1036800, 1075200, 1132800, 1171200, 1190400, 1190400, 1190400, 1190400, 1190400),

	CORE_DVFS("ape",		0, 2, 1, KHZ,	 408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000),
	CORE_DVFS("sbus",		0, 2, 1, KHZ,	 204000,  204000,  204000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000),
	CORE_DVFS("sclk",		0, 2, 1, KHZ,	 204000,  204000,  204000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000,  300000),

	CORE_DVFS("host1x",		0, 2, 1, KHZ,	 136000,  136000,  136000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),
	CORE_DVFS("tsec",		0, 2, 1, KHZ,	 397906,  397906,  397906,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000,  408000),
	CORE_DVFS("mselect",		0, 2, 1, KHZ,	 204000,  204000,  204000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000,  310000),

	CORE_DVFS("disp1",		0, 2, 0, KHZ,	 323330,  323330,  323330,  419140,  481198,  524222,  558885,  581514,  600645,  612344,  623984,  635498,  658472,  669772,  669772,  669772,  669772),
	CORE_DVFS("disp2",		0, 2, 0, KHZ,	 323330,  323330,  323330,  419140,  481198,  524222,  558885,  581514,  600645,  612344,  623984,  635498,  658472,  669772,  669772,  669772,  669772),

/* Common Tables */
	/* Core voltages(mV):				    650,     675,     700,     725,     750,     775,     800,     825,     850,     875,     900,     925,     950,     975,    1000,    1025,    1050 */
	CORE_DVFS("pll_a",		-1, -1, 1, KHZ,	1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000),
	CORE_DVFS("pll_c",		-1, -1, 1, KHZ,	1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000),
	CORE_DVFS("pll_c2",		-1, -1, 1, KHZ,	1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000),
	CORE_DVFS("pll_c3",		-1, -1, 1, KHZ,	1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000),
	CORE_DVFS("pll_c4_out0",	-1, -1, 1, KHZ,	1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000),
	CORE_DVFS("pll_d_out0",		-1, -1, 1, KHZ,	1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000),
	CORE_DVFS("pll_d2",		-1, -1, 1, KHZ,	1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000),
	CORE_DVFS("pll_dp",		-1, -1, 1, KHZ,	1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000, 1500000),

	/* Core voltages(mV):				    650,     675,     700,     725,     750,     775,     800,     825,     850,     875,     900,     925,     950,     975,    1000,    1025,    1050 */
	CORE_DVFS("csi",		-1, -1, 1, KHZ,	 750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000),
	CORE_DVFS("cilab",		-1, -1, 1, KHZ,	 102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),
	CORE_DVFS("cilcd",		-1, -1, 1, KHZ,	 102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),

	CORE_DVFS("clk72mhz",		-1, -1, 1, KHZ,	  68000,   68000,   68000,   68000,   68000,   68000,   68000,   68000,   68000,   68000,   68000,   68000,   68000,   68000,   68000,   68000,   68000),
	CORE_DVFS("dsia",		-1, -1, 1, KHZ,	 750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000),
	CORE_DVFS("dsib",		-1, -1, 1, KHZ,	 750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000,  750000),

	CORE_DVFS("dsialp",		-1, -1, 1, KHZ,	 102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000),
	CORE_DVFS("dsiblp",		-1, -1, 1, KHZ,	 102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000),

	CORE_DVFS("sor1",		-1, -1, 1, KHZ,	 297000,  297000,  297000,  594000,  594000,  594000,  594000,  594000,  594000,  594000,  594000,  594000,  594000,  594000,  594000,  594000,  594000),

	CORE_DVFS("i2s0",		-1, -1, 1, KHZ,	  24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576),
	CORE_DVFS("i2s1",		-1, -1, 1, KHZ,	  24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576),
	CORE_DVFS("i2s2",		-1, -1, 1, KHZ,	  24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576),
	CORE_DVFS("i2s3",		-1, -1, 1, KHZ,	  24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576),
	CORE_DVFS("i2s4",		-1, -1, 1, KHZ,	  24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576,   24576),

	CORE_DVFS("d_audio",		-1, -1, 1, KHZ,	 102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000),
	CORE_DVFS("spdif_out",		-1, -1, 1, KHZ,	  73728,   73728,   73728,   73728,   73728,   73728,   73728,   73728,   73728,   73728,   73728,   73728,   73728,   73728,   73728,   73728,   73728),
	CORE_DVFS("dmic1",		-1, -1, 1, KHZ,	  12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288),
	CORE_DVFS("dmic2",		-1, -1, 1, KHZ,	  12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288),
	CORE_DVFS("dmic3",		-1, -1, 1, KHZ,	  12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288,   12288),

	CORE_DVFS("hda",		-1, -1, 1, KHZ,	  51000,   51000,   51000,   51000,   51000,   51000,   51000,   51000,   51000,   51000,   51000,   51000,   51000,   51000,   51000,   51000,   51000),
	CORE_DVFS("hda2codec_2x",	-1, -1, 1, KHZ,	  48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000),
	CORE_DVFS("maud",		-1, -1, 1, KHZ,	 102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000,  102000),

	CORE_DVFS("sdmmc1",		-1, -1, 1, KHZ,	 204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),
	CORE_DVFS("sdmmc3",		-1, -1, 1, KHZ,	 204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),

	CORE_DVFS("sdmmc1_ddr",		-1, -1, 1, KHZ,	 204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),
	CORE_DVFS("sdmmc3_ddr",		-1, -1, 1, KHZ,	 204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),

	CORE_DVFS("sdmmc2",		-1, -1, 1, KHZ,	 204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),
	CORE_DVFS("sdmmc4",		-1, -1, 1, KHZ,	 204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000,  204000),

	CORE_DVFS("sdmmc_legacy",	-1, -1, 1, KHZ,	  12000,   12000,   12000,   12000,   12000,   12000,   12000,   12000,   12000,   12000,   12000,   12000,   12000,   12000,   12000,   12000,   12000),

	CORE_DVFS("xusb_falcon_src",	-1, -1, 1, KHZ,	 336000,  336000,  336000,  336000,  336000,  336000,  336000,  336000,  336000,  336000,  336000,  336000,  336000,  336000,  336000,  336000,  336000),
	CORE_DVFS("xusb_host_src",	-1, -1, 1, KHZ,	 112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000),
	CORE_DVFS("xusb_dev_src",	-1, -1, 1, KHZ,	 112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000,  112000),
	CORE_DVFS("xusb_ssp_src",	-1, -1, 1, KHZ,	 120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000),
	CORE_DVFS("xusb_fs_src",	-1, -1, 1, KHZ,	  48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000),
	CORE_DVFS("xusb_hs_src",	-1, -1, 1, KHZ,	 120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000,  120000),
	CORE_DVFS("usbd",		-1, -1, 1, KHZ,	 480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000),
	CORE_DVFS("usb2",		-1, -1, 1, KHZ,	 480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000,  480000),

	CORE_DVFS("pcie",		-1, -1, 1, KHZ,	 500000,  500000,  500000,  500000,  500000,  500000,  500000,  500000,  500000,  500000,  500000,  500000,  500000,  500000,  500000,  500000,  500000),

	CORE_DVFS("i2c1",		-1, -1, 1, KHZ,	 136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000),
	CORE_DVFS("i2c2",		-1, -1, 1, KHZ,	 136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000),
	CORE_DVFS("i2c3",		-1, -1, 1, KHZ,	 136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000),
	CORE_DVFS("i2c4",		-1, -1, 1, KHZ,	 136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000),
	CORE_DVFS("i2c5",		-1, -1, 1, KHZ,	 136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000),
	CORE_DVFS("i2c6",		-1, -1, 1, KHZ,	 136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000),
	CORE_DVFS("vii2c",		-1, -1, 1, KHZ,	 136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000),

	CORE_DVFS("i2cslow",		-1, -1, 1, KHZ,	     33,      33,      33,      33,      33,      33,      33,      33,      33,      33,      33,      33,      33,      33,      33,      33,      33),
	CORE_DVFS("pwm",		-1, -1, 1, KHZ,	  48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000,   48000),

	CORE_DVFS("extern1",		-1, -1, 1, KHZ,	  49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152),
	CORE_DVFS("extern2",		-1, -1, 1, KHZ,	  49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152),
	CORE_DVFS("extern3",		-1, -1, 1, KHZ,	  49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152,   49152),


	CORE_DVFS("soc_therm",		-1, -1, 1, KHZ,	 136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000,  136000),
	CORE_DVFS("tsensor",		-1, -1, 1, KHZ,	  19200,   19200,   19200,   19200,   19200,   19200,   19200,   19200,   19200,   19200,   19200,   19200,   19200,   19200,   19200,   19200,   19200),
};

static struct dvfs sor1_dpb01_dvfs_table[] = {
	CORE_DVFS("sor1",		-1, -1, 1, KHZ,	 270000,  270000,  270000,  270000,  540000,  540000,  540000,  540000,  540000,  540000,  540000,  540000,  540000,  540000,  540000,  540000,  540000),
};

static struct dvfs spib01_dvfs_table[] = {
	CORE_DVFS("sbc1",		-1, -1, 1, KHZ,	  12000,   35000,   50000,   50000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000),
	CORE_DVFS("sbc2",		-1, -1, 1, KHZ,	  12000,   35000,   50000,   50000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000),
	CORE_DVFS("sbc3",		-1, -1, 1, KHZ,	  12000,   35000,   50000,   50000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000),
	CORE_DVFS("sbc4",		-1, -1, 1, KHZ,	  12000,   35000,   50000,   50000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000,   65000),

};

static struct dvfs spi_slaveb01_dvfs_table[] = {
	CORE_DVFS("sbc1",		-1, -1, 1, KHZ,	  45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000),
	CORE_DVFS("sbc2",		-1, -1, 1, KHZ,	  45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000),
	CORE_DVFS("sbc3",		-1, -1, 1, KHZ,	  45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000),
	CORE_DVFS("sbc4",		-1, -1, 1, KHZ,	  45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000,   45000),

};

static struct dvfs qspi_sdrb01_dvfs_table[] = {
	CORE_DVFS("qspi",		-1, -1, 1, KHZ,	 116600,  116600,  116600,  116600,  116600,  116600,  116600,  116600,  116600,  116600,  116600,  116600,  116600,  116600,  116600,  116600,  116600),
};

static struct dvfs qspi_ddrb01_dvfs_table[] = {
	CORE_DVFS("qspi",		-1, -1, 1, KHZ,	 163200,  163200,  163200,  163200,  163200,  163200,  163200,  163200,  163200,  163200,  163200,  163200,  163200,  163200,  163200,  163200,  163200),
};

/* EMC DVB DVFS tables */
static struct dvb_dvfs emcb01_dvb_dvfs_table[] = {
	{
		.speedo_id = -1,
		.freqs_mult = KHZ,
		.dvb_table = {
			{  204000, {  800,  800,  800, } },
			{  408000, {  800,  800,  800, } },
			{  800000, {  800,  800,  800, } },
			{ 1065600, {  800,  800,  800, } },
			{ 1331200, {  800,  800,  800, } },
			{ 1600000, {  800,  800,  800, } },
			{ 0, { } },
		},
	},
};

static const char coreb01_dvfs_table_ver[] = "SOC - p4v1 SOC_IO - p4v1 SOC_EMC- p4v1";
