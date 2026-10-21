#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_1960240961147461763) {
   out_1960240961147461763[0] = delta_x[0] + nom_x[0];
   out_1960240961147461763[1] = delta_x[1] + nom_x[1];
   out_1960240961147461763[2] = delta_x[2] + nom_x[2];
   out_1960240961147461763[3] = delta_x[3] + nom_x[3];
   out_1960240961147461763[4] = delta_x[4] + nom_x[4];
   out_1960240961147461763[5] = delta_x[5] + nom_x[5];
   out_1960240961147461763[6] = delta_x[6] + nom_x[6];
   out_1960240961147461763[7] = delta_x[7] + nom_x[7];
   out_1960240961147461763[8] = delta_x[8] + nom_x[8];
   out_1960240961147461763[9] = delta_x[9] + nom_x[9];
   out_1960240961147461763[10] = delta_x[10] + nom_x[10];
   out_1960240961147461763[11] = delta_x[11] + nom_x[11];
   out_1960240961147461763[12] = delta_x[12] + nom_x[12];
   out_1960240961147461763[13] = delta_x[13] + nom_x[13];
   out_1960240961147461763[14] = delta_x[14] + nom_x[14];
   out_1960240961147461763[15] = delta_x[15] + nom_x[15];
   out_1960240961147461763[16] = delta_x[16] + nom_x[16];
   out_1960240961147461763[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_2311725942617102893) {
   out_2311725942617102893[0] = -nom_x[0] + true_x[0];
   out_2311725942617102893[1] = -nom_x[1] + true_x[1];
   out_2311725942617102893[2] = -nom_x[2] + true_x[2];
   out_2311725942617102893[3] = -nom_x[3] + true_x[3];
   out_2311725942617102893[4] = -nom_x[4] + true_x[4];
   out_2311725942617102893[5] = -nom_x[5] + true_x[5];
   out_2311725942617102893[6] = -nom_x[6] + true_x[6];
   out_2311725942617102893[7] = -nom_x[7] + true_x[7];
   out_2311725942617102893[8] = -nom_x[8] + true_x[8];
   out_2311725942617102893[9] = -nom_x[9] + true_x[9];
   out_2311725942617102893[10] = -nom_x[10] + true_x[10];
   out_2311725942617102893[11] = -nom_x[11] + true_x[11];
   out_2311725942617102893[12] = -nom_x[12] + true_x[12];
   out_2311725942617102893[13] = -nom_x[13] + true_x[13];
   out_2311725942617102893[14] = -nom_x[14] + true_x[14];
   out_2311725942617102893[15] = -nom_x[15] + true_x[15];
   out_2311725942617102893[16] = -nom_x[16] + true_x[16];
   out_2311725942617102893[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_3907531448917969471) {
   out_3907531448917969471[0] = 1.0;
   out_3907531448917969471[1] = 0.0;
   out_3907531448917969471[2] = 0.0;
   out_3907531448917969471[3] = 0.0;
   out_3907531448917969471[4] = 0.0;
   out_3907531448917969471[5] = 0.0;
   out_3907531448917969471[6] = 0.0;
   out_3907531448917969471[7] = 0.0;
   out_3907531448917969471[8] = 0.0;
   out_3907531448917969471[9] = 0.0;
   out_3907531448917969471[10] = 0.0;
   out_3907531448917969471[11] = 0.0;
   out_3907531448917969471[12] = 0.0;
   out_3907531448917969471[13] = 0.0;
   out_3907531448917969471[14] = 0.0;
   out_3907531448917969471[15] = 0.0;
   out_3907531448917969471[16] = 0.0;
   out_3907531448917969471[17] = 0.0;
   out_3907531448917969471[18] = 0.0;
   out_3907531448917969471[19] = 1.0;
   out_3907531448917969471[20] = 0.0;
   out_3907531448917969471[21] = 0.0;
   out_3907531448917969471[22] = 0.0;
   out_3907531448917969471[23] = 0.0;
   out_3907531448917969471[24] = 0.0;
   out_3907531448917969471[25] = 0.0;
   out_3907531448917969471[26] = 0.0;
   out_3907531448917969471[27] = 0.0;
   out_3907531448917969471[28] = 0.0;
   out_3907531448917969471[29] = 0.0;
   out_3907531448917969471[30] = 0.0;
   out_3907531448917969471[31] = 0.0;
   out_3907531448917969471[32] = 0.0;
   out_3907531448917969471[33] = 0.0;
   out_3907531448917969471[34] = 0.0;
   out_3907531448917969471[35] = 0.0;
   out_3907531448917969471[36] = 0.0;
   out_3907531448917969471[37] = 0.0;
   out_3907531448917969471[38] = 1.0;
   out_3907531448917969471[39] = 0.0;
   out_3907531448917969471[40] = 0.0;
   out_3907531448917969471[41] = 0.0;
   out_3907531448917969471[42] = 0.0;
   out_3907531448917969471[43] = 0.0;
   out_3907531448917969471[44] = 0.0;
   out_3907531448917969471[45] = 0.0;
   out_3907531448917969471[46] = 0.0;
   out_3907531448917969471[47] = 0.0;
   out_3907531448917969471[48] = 0.0;
   out_3907531448917969471[49] = 0.0;
   out_3907531448917969471[50] = 0.0;
   out_3907531448917969471[51] = 0.0;
   out_3907531448917969471[52] = 0.0;
   out_3907531448917969471[53] = 0.0;
   out_3907531448917969471[54] = 0.0;
   out_3907531448917969471[55] = 0.0;
   out_3907531448917969471[56] = 0.0;
   out_3907531448917969471[57] = 1.0;
   out_3907531448917969471[58] = 0.0;
   out_3907531448917969471[59] = 0.0;
   out_3907531448917969471[60] = 0.0;
   out_3907531448917969471[61] = 0.0;
   out_3907531448917969471[62] = 0.0;
   out_3907531448917969471[63] = 0.0;
   out_3907531448917969471[64] = 0.0;
   out_3907531448917969471[65] = 0.0;
   out_3907531448917969471[66] = 0.0;
   out_3907531448917969471[67] = 0.0;
   out_3907531448917969471[68] = 0.0;
   out_3907531448917969471[69] = 0.0;
   out_3907531448917969471[70] = 0.0;
   out_3907531448917969471[71] = 0.0;
   out_3907531448917969471[72] = 0.0;
   out_3907531448917969471[73] = 0.0;
   out_3907531448917969471[74] = 0.0;
   out_3907531448917969471[75] = 0.0;
   out_3907531448917969471[76] = 1.0;
   out_3907531448917969471[77] = 0.0;
   out_3907531448917969471[78] = 0.0;
   out_3907531448917969471[79] = 0.0;
   out_3907531448917969471[80] = 0.0;
   out_3907531448917969471[81] = 0.0;
   out_3907531448917969471[82] = 0.0;
   out_3907531448917969471[83] = 0.0;
   out_3907531448917969471[84] = 0.0;
   out_3907531448917969471[85] = 0.0;
   out_3907531448917969471[86] = 0.0;
   out_3907531448917969471[87] = 0.0;
   out_3907531448917969471[88] = 0.0;
   out_3907531448917969471[89] = 0.0;
   out_3907531448917969471[90] = 0.0;
   out_3907531448917969471[91] = 0.0;
   out_3907531448917969471[92] = 0.0;
   out_3907531448917969471[93] = 0.0;
   out_3907531448917969471[94] = 0.0;
   out_3907531448917969471[95] = 1.0;
   out_3907531448917969471[96] = 0.0;
   out_3907531448917969471[97] = 0.0;
   out_3907531448917969471[98] = 0.0;
   out_3907531448917969471[99] = 0.0;
   out_3907531448917969471[100] = 0.0;
   out_3907531448917969471[101] = 0.0;
   out_3907531448917969471[102] = 0.0;
   out_3907531448917969471[103] = 0.0;
   out_3907531448917969471[104] = 0.0;
   out_3907531448917969471[105] = 0.0;
   out_3907531448917969471[106] = 0.0;
   out_3907531448917969471[107] = 0.0;
   out_3907531448917969471[108] = 0.0;
   out_3907531448917969471[109] = 0.0;
   out_3907531448917969471[110] = 0.0;
   out_3907531448917969471[111] = 0.0;
   out_3907531448917969471[112] = 0.0;
   out_3907531448917969471[113] = 0.0;
   out_3907531448917969471[114] = 1.0;
   out_3907531448917969471[115] = 0.0;
   out_3907531448917969471[116] = 0.0;
   out_3907531448917969471[117] = 0.0;
   out_3907531448917969471[118] = 0.0;
   out_3907531448917969471[119] = 0.0;
   out_3907531448917969471[120] = 0.0;
   out_3907531448917969471[121] = 0.0;
   out_3907531448917969471[122] = 0.0;
   out_3907531448917969471[123] = 0.0;
   out_3907531448917969471[124] = 0.0;
   out_3907531448917969471[125] = 0.0;
   out_3907531448917969471[126] = 0.0;
   out_3907531448917969471[127] = 0.0;
   out_3907531448917969471[128] = 0.0;
   out_3907531448917969471[129] = 0.0;
   out_3907531448917969471[130] = 0.0;
   out_3907531448917969471[131] = 0.0;
   out_3907531448917969471[132] = 0.0;
   out_3907531448917969471[133] = 1.0;
   out_3907531448917969471[134] = 0.0;
   out_3907531448917969471[135] = 0.0;
   out_3907531448917969471[136] = 0.0;
   out_3907531448917969471[137] = 0.0;
   out_3907531448917969471[138] = 0.0;
   out_3907531448917969471[139] = 0.0;
   out_3907531448917969471[140] = 0.0;
   out_3907531448917969471[141] = 0.0;
   out_3907531448917969471[142] = 0.0;
   out_3907531448917969471[143] = 0.0;
   out_3907531448917969471[144] = 0.0;
   out_3907531448917969471[145] = 0.0;
   out_3907531448917969471[146] = 0.0;
   out_3907531448917969471[147] = 0.0;
   out_3907531448917969471[148] = 0.0;
   out_3907531448917969471[149] = 0.0;
   out_3907531448917969471[150] = 0.0;
   out_3907531448917969471[151] = 0.0;
   out_3907531448917969471[152] = 1.0;
   out_3907531448917969471[153] = 0.0;
   out_3907531448917969471[154] = 0.0;
   out_3907531448917969471[155] = 0.0;
   out_3907531448917969471[156] = 0.0;
   out_3907531448917969471[157] = 0.0;
   out_3907531448917969471[158] = 0.0;
   out_3907531448917969471[159] = 0.0;
   out_3907531448917969471[160] = 0.0;
   out_3907531448917969471[161] = 0.0;
   out_3907531448917969471[162] = 0.0;
   out_3907531448917969471[163] = 0.0;
   out_3907531448917969471[164] = 0.0;
   out_3907531448917969471[165] = 0.0;
   out_3907531448917969471[166] = 0.0;
   out_3907531448917969471[167] = 0.0;
   out_3907531448917969471[168] = 0.0;
   out_3907531448917969471[169] = 0.0;
   out_3907531448917969471[170] = 0.0;
   out_3907531448917969471[171] = 1.0;
   out_3907531448917969471[172] = 0.0;
   out_3907531448917969471[173] = 0.0;
   out_3907531448917969471[174] = 0.0;
   out_3907531448917969471[175] = 0.0;
   out_3907531448917969471[176] = 0.0;
   out_3907531448917969471[177] = 0.0;
   out_3907531448917969471[178] = 0.0;
   out_3907531448917969471[179] = 0.0;
   out_3907531448917969471[180] = 0.0;
   out_3907531448917969471[181] = 0.0;
   out_3907531448917969471[182] = 0.0;
   out_3907531448917969471[183] = 0.0;
   out_3907531448917969471[184] = 0.0;
   out_3907531448917969471[185] = 0.0;
   out_3907531448917969471[186] = 0.0;
   out_3907531448917969471[187] = 0.0;
   out_3907531448917969471[188] = 0.0;
   out_3907531448917969471[189] = 0.0;
   out_3907531448917969471[190] = 1.0;
   out_3907531448917969471[191] = 0.0;
   out_3907531448917969471[192] = 0.0;
   out_3907531448917969471[193] = 0.0;
   out_3907531448917969471[194] = 0.0;
   out_3907531448917969471[195] = 0.0;
   out_3907531448917969471[196] = 0.0;
   out_3907531448917969471[197] = 0.0;
   out_3907531448917969471[198] = 0.0;
   out_3907531448917969471[199] = 0.0;
   out_3907531448917969471[200] = 0.0;
   out_3907531448917969471[201] = 0.0;
   out_3907531448917969471[202] = 0.0;
   out_3907531448917969471[203] = 0.0;
   out_3907531448917969471[204] = 0.0;
   out_3907531448917969471[205] = 0.0;
   out_3907531448917969471[206] = 0.0;
   out_3907531448917969471[207] = 0.0;
   out_3907531448917969471[208] = 0.0;
   out_3907531448917969471[209] = 1.0;
   out_3907531448917969471[210] = 0.0;
   out_3907531448917969471[211] = 0.0;
   out_3907531448917969471[212] = 0.0;
   out_3907531448917969471[213] = 0.0;
   out_3907531448917969471[214] = 0.0;
   out_3907531448917969471[215] = 0.0;
   out_3907531448917969471[216] = 0.0;
   out_3907531448917969471[217] = 0.0;
   out_3907531448917969471[218] = 0.0;
   out_3907531448917969471[219] = 0.0;
   out_3907531448917969471[220] = 0.0;
   out_3907531448917969471[221] = 0.0;
   out_3907531448917969471[222] = 0.0;
   out_3907531448917969471[223] = 0.0;
   out_3907531448917969471[224] = 0.0;
   out_3907531448917969471[225] = 0.0;
   out_3907531448917969471[226] = 0.0;
   out_3907531448917969471[227] = 0.0;
   out_3907531448917969471[228] = 1.0;
   out_3907531448917969471[229] = 0.0;
   out_3907531448917969471[230] = 0.0;
   out_3907531448917969471[231] = 0.0;
   out_3907531448917969471[232] = 0.0;
   out_3907531448917969471[233] = 0.0;
   out_3907531448917969471[234] = 0.0;
   out_3907531448917969471[235] = 0.0;
   out_3907531448917969471[236] = 0.0;
   out_3907531448917969471[237] = 0.0;
   out_3907531448917969471[238] = 0.0;
   out_3907531448917969471[239] = 0.0;
   out_3907531448917969471[240] = 0.0;
   out_3907531448917969471[241] = 0.0;
   out_3907531448917969471[242] = 0.0;
   out_3907531448917969471[243] = 0.0;
   out_3907531448917969471[244] = 0.0;
   out_3907531448917969471[245] = 0.0;
   out_3907531448917969471[246] = 0.0;
   out_3907531448917969471[247] = 1.0;
   out_3907531448917969471[248] = 0.0;
   out_3907531448917969471[249] = 0.0;
   out_3907531448917969471[250] = 0.0;
   out_3907531448917969471[251] = 0.0;
   out_3907531448917969471[252] = 0.0;
   out_3907531448917969471[253] = 0.0;
   out_3907531448917969471[254] = 0.0;
   out_3907531448917969471[255] = 0.0;
   out_3907531448917969471[256] = 0.0;
   out_3907531448917969471[257] = 0.0;
   out_3907531448917969471[258] = 0.0;
   out_3907531448917969471[259] = 0.0;
   out_3907531448917969471[260] = 0.0;
   out_3907531448917969471[261] = 0.0;
   out_3907531448917969471[262] = 0.0;
   out_3907531448917969471[263] = 0.0;
   out_3907531448917969471[264] = 0.0;
   out_3907531448917969471[265] = 0.0;
   out_3907531448917969471[266] = 1.0;
   out_3907531448917969471[267] = 0.0;
   out_3907531448917969471[268] = 0.0;
   out_3907531448917969471[269] = 0.0;
   out_3907531448917969471[270] = 0.0;
   out_3907531448917969471[271] = 0.0;
   out_3907531448917969471[272] = 0.0;
   out_3907531448917969471[273] = 0.0;
   out_3907531448917969471[274] = 0.0;
   out_3907531448917969471[275] = 0.0;
   out_3907531448917969471[276] = 0.0;
   out_3907531448917969471[277] = 0.0;
   out_3907531448917969471[278] = 0.0;
   out_3907531448917969471[279] = 0.0;
   out_3907531448917969471[280] = 0.0;
   out_3907531448917969471[281] = 0.0;
   out_3907531448917969471[282] = 0.0;
   out_3907531448917969471[283] = 0.0;
   out_3907531448917969471[284] = 0.0;
   out_3907531448917969471[285] = 1.0;
   out_3907531448917969471[286] = 0.0;
   out_3907531448917969471[287] = 0.0;
   out_3907531448917969471[288] = 0.0;
   out_3907531448917969471[289] = 0.0;
   out_3907531448917969471[290] = 0.0;
   out_3907531448917969471[291] = 0.0;
   out_3907531448917969471[292] = 0.0;
   out_3907531448917969471[293] = 0.0;
   out_3907531448917969471[294] = 0.0;
   out_3907531448917969471[295] = 0.0;
   out_3907531448917969471[296] = 0.0;
   out_3907531448917969471[297] = 0.0;
   out_3907531448917969471[298] = 0.0;
   out_3907531448917969471[299] = 0.0;
   out_3907531448917969471[300] = 0.0;
   out_3907531448917969471[301] = 0.0;
   out_3907531448917969471[302] = 0.0;
   out_3907531448917969471[303] = 0.0;
   out_3907531448917969471[304] = 1.0;
   out_3907531448917969471[305] = 0.0;
   out_3907531448917969471[306] = 0.0;
   out_3907531448917969471[307] = 0.0;
   out_3907531448917969471[308] = 0.0;
   out_3907531448917969471[309] = 0.0;
   out_3907531448917969471[310] = 0.0;
   out_3907531448917969471[311] = 0.0;
   out_3907531448917969471[312] = 0.0;
   out_3907531448917969471[313] = 0.0;
   out_3907531448917969471[314] = 0.0;
   out_3907531448917969471[315] = 0.0;
   out_3907531448917969471[316] = 0.0;
   out_3907531448917969471[317] = 0.0;
   out_3907531448917969471[318] = 0.0;
   out_3907531448917969471[319] = 0.0;
   out_3907531448917969471[320] = 0.0;
   out_3907531448917969471[321] = 0.0;
   out_3907531448917969471[322] = 0.0;
   out_3907531448917969471[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_322024657666628683) {
   out_322024657666628683[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_322024657666628683[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_322024657666628683[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_322024657666628683[3] = dt*state[12] + state[3];
   out_322024657666628683[4] = dt*state[13] + state[4];
   out_322024657666628683[5] = dt*state[14] + state[5];
   out_322024657666628683[6] = state[6];
   out_322024657666628683[7] = state[7];
   out_322024657666628683[8] = state[8];
   out_322024657666628683[9] = state[9];
   out_322024657666628683[10] = state[10];
   out_322024657666628683[11] = state[11];
   out_322024657666628683[12] = state[12];
   out_322024657666628683[13] = state[13];
   out_322024657666628683[14] = state[14];
   out_322024657666628683[15] = state[15];
   out_322024657666628683[16] = state[16];
   out_322024657666628683[17] = state[17];
}
void F_fun(double *state, double dt, double *out_842370593129636931) {
   out_842370593129636931[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_842370593129636931[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_842370593129636931[2] = 0;
   out_842370593129636931[3] = 0;
   out_842370593129636931[4] = 0;
   out_842370593129636931[5] = 0;
   out_842370593129636931[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_842370593129636931[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_842370593129636931[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_842370593129636931[9] = 0;
   out_842370593129636931[10] = 0;
   out_842370593129636931[11] = 0;
   out_842370593129636931[12] = 0;
   out_842370593129636931[13] = 0;
   out_842370593129636931[14] = 0;
   out_842370593129636931[15] = 0;
   out_842370593129636931[16] = 0;
   out_842370593129636931[17] = 0;
   out_842370593129636931[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_842370593129636931[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_842370593129636931[20] = 0;
   out_842370593129636931[21] = 0;
   out_842370593129636931[22] = 0;
   out_842370593129636931[23] = 0;
   out_842370593129636931[24] = 0;
   out_842370593129636931[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_842370593129636931[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_842370593129636931[27] = 0;
   out_842370593129636931[28] = 0;
   out_842370593129636931[29] = 0;
   out_842370593129636931[30] = 0;
   out_842370593129636931[31] = 0;
   out_842370593129636931[32] = 0;
   out_842370593129636931[33] = 0;
   out_842370593129636931[34] = 0;
   out_842370593129636931[35] = 0;
   out_842370593129636931[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_842370593129636931[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_842370593129636931[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_842370593129636931[39] = 0;
   out_842370593129636931[40] = 0;
   out_842370593129636931[41] = 0;
   out_842370593129636931[42] = 0;
   out_842370593129636931[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_842370593129636931[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_842370593129636931[45] = 0;
   out_842370593129636931[46] = 0;
   out_842370593129636931[47] = 0;
   out_842370593129636931[48] = 0;
   out_842370593129636931[49] = 0;
   out_842370593129636931[50] = 0;
   out_842370593129636931[51] = 0;
   out_842370593129636931[52] = 0;
   out_842370593129636931[53] = 0;
   out_842370593129636931[54] = 0;
   out_842370593129636931[55] = 0;
   out_842370593129636931[56] = 0;
   out_842370593129636931[57] = 1;
   out_842370593129636931[58] = 0;
   out_842370593129636931[59] = 0;
   out_842370593129636931[60] = 0;
   out_842370593129636931[61] = 0;
   out_842370593129636931[62] = 0;
   out_842370593129636931[63] = 0;
   out_842370593129636931[64] = 0;
   out_842370593129636931[65] = 0;
   out_842370593129636931[66] = dt;
   out_842370593129636931[67] = 0;
   out_842370593129636931[68] = 0;
   out_842370593129636931[69] = 0;
   out_842370593129636931[70] = 0;
   out_842370593129636931[71] = 0;
   out_842370593129636931[72] = 0;
   out_842370593129636931[73] = 0;
   out_842370593129636931[74] = 0;
   out_842370593129636931[75] = 0;
   out_842370593129636931[76] = 1;
   out_842370593129636931[77] = 0;
   out_842370593129636931[78] = 0;
   out_842370593129636931[79] = 0;
   out_842370593129636931[80] = 0;
   out_842370593129636931[81] = 0;
   out_842370593129636931[82] = 0;
   out_842370593129636931[83] = 0;
   out_842370593129636931[84] = 0;
   out_842370593129636931[85] = dt;
   out_842370593129636931[86] = 0;
   out_842370593129636931[87] = 0;
   out_842370593129636931[88] = 0;
   out_842370593129636931[89] = 0;
   out_842370593129636931[90] = 0;
   out_842370593129636931[91] = 0;
   out_842370593129636931[92] = 0;
   out_842370593129636931[93] = 0;
   out_842370593129636931[94] = 0;
   out_842370593129636931[95] = 1;
   out_842370593129636931[96] = 0;
   out_842370593129636931[97] = 0;
   out_842370593129636931[98] = 0;
   out_842370593129636931[99] = 0;
   out_842370593129636931[100] = 0;
   out_842370593129636931[101] = 0;
   out_842370593129636931[102] = 0;
   out_842370593129636931[103] = 0;
   out_842370593129636931[104] = dt;
   out_842370593129636931[105] = 0;
   out_842370593129636931[106] = 0;
   out_842370593129636931[107] = 0;
   out_842370593129636931[108] = 0;
   out_842370593129636931[109] = 0;
   out_842370593129636931[110] = 0;
   out_842370593129636931[111] = 0;
   out_842370593129636931[112] = 0;
   out_842370593129636931[113] = 0;
   out_842370593129636931[114] = 1;
   out_842370593129636931[115] = 0;
   out_842370593129636931[116] = 0;
   out_842370593129636931[117] = 0;
   out_842370593129636931[118] = 0;
   out_842370593129636931[119] = 0;
   out_842370593129636931[120] = 0;
   out_842370593129636931[121] = 0;
   out_842370593129636931[122] = 0;
   out_842370593129636931[123] = 0;
   out_842370593129636931[124] = 0;
   out_842370593129636931[125] = 0;
   out_842370593129636931[126] = 0;
   out_842370593129636931[127] = 0;
   out_842370593129636931[128] = 0;
   out_842370593129636931[129] = 0;
   out_842370593129636931[130] = 0;
   out_842370593129636931[131] = 0;
   out_842370593129636931[132] = 0;
   out_842370593129636931[133] = 1;
   out_842370593129636931[134] = 0;
   out_842370593129636931[135] = 0;
   out_842370593129636931[136] = 0;
   out_842370593129636931[137] = 0;
   out_842370593129636931[138] = 0;
   out_842370593129636931[139] = 0;
   out_842370593129636931[140] = 0;
   out_842370593129636931[141] = 0;
   out_842370593129636931[142] = 0;
   out_842370593129636931[143] = 0;
   out_842370593129636931[144] = 0;
   out_842370593129636931[145] = 0;
   out_842370593129636931[146] = 0;
   out_842370593129636931[147] = 0;
   out_842370593129636931[148] = 0;
   out_842370593129636931[149] = 0;
   out_842370593129636931[150] = 0;
   out_842370593129636931[151] = 0;
   out_842370593129636931[152] = 1;
   out_842370593129636931[153] = 0;
   out_842370593129636931[154] = 0;
   out_842370593129636931[155] = 0;
   out_842370593129636931[156] = 0;
   out_842370593129636931[157] = 0;
   out_842370593129636931[158] = 0;
   out_842370593129636931[159] = 0;
   out_842370593129636931[160] = 0;
   out_842370593129636931[161] = 0;
   out_842370593129636931[162] = 0;
   out_842370593129636931[163] = 0;
   out_842370593129636931[164] = 0;
   out_842370593129636931[165] = 0;
   out_842370593129636931[166] = 0;
   out_842370593129636931[167] = 0;
   out_842370593129636931[168] = 0;
   out_842370593129636931[169] = 0;
   out_842370593129636931[170] = 0;
   out_842370593129636931[171] = 1;
   out_842370593129636931[172] = 0;
   out_842370593129636931[173] = 0;
   out_842370593129636931[174] = 0;
   out_842370593129636931[175] = 0;
   out_842370593129636931[176] = 0;
   out_842370593129636931[177] = 0;
   out_842370593129636931[178] = 0;
   out_842370593129636931[179] = 0;
   out_842370593129636931[180] = 0;
   out_842370593129636931[181] = 0;
   out_842370593129636931[182] = 0;
   out_842370593129636931[183] = 0;
   out_842370593129636931[184] = 0;
   out_842370593129636931[185] = 0;
   out_842370593129636931[186] = 0;
   out_842370593129636931[187] = 0;
   out_842370593129636931[188] = 0;
   out_842370593129636931[189] = 0;
   out_842370593129636931[190] = 1;
   out_842370593129636931[191] = 0;
   out_842370593129636931[192] = 0;
   out_842370593129636931[193] = 0;
   out_842370593129636931[194] = 0;
   out_842370593129636931[195] = 0;
   out_842370593129636931[196] = 0;
   out_842370593129636931[197] = 0;
   out_842370593129636931[198] = 0;
   out_842370593129636931[199] = 0;
   out_842370593129636931[200] = 0;
   out_842370593129636931[201] = 0;
   out_842370593129636931[202] = 0;
   out_842370593129636931[203] = 0;
   out_842370593129636931[204] = 0;
   out_842370593129636931[205] = 0;
   out_842370593129636931[206] = 0;
   out_842370593129636931[207] = 0;
   out_842370593129636931[208] = 0;
   out_842370593129636931[209] = 1;
   out_842370593129636931[210] = 0;
   out_842370593129636931[211] = 0;
   out_842370593129636931[212] = 0;
   out_842370593129636931[213] = 0;
   out_842370593129636931[214] = 0;
   out_842370593129636931[215] = 0;
   out_842370593129636931[216] = 0;
   out_842370593129636931[217] = 0;
   out_842370593129636931[218] = 0;
   out_842370593129636931[219] = 0;
   out_842370593129636931[220] = 0;
   out_842370593129636931[221] = 0;
   out_842370593129636931[222] = 0;
   out_842370593129636931[223] = 0;
   out_842370593129636931[224] = 0;
   out_842370593129636931[225] = 0;
   out_842370593129636931[226] = 0;
   out_842370593129636931[227] = 0;
   out_842370593129636931[228] = 1;
   out_842370593129636931[229] = 0;
   out_842370593129636931[230] = 0;
   out_842370593129636931[231] = 0;
   out_842370593129636931[232] = 0;
   out_842370593129636931[233] = 0;
   out_842370593129636931[234] = 0;
   out_842370593129636931[235] = 0;
   out_842370593129636931[236] = 0;
   out_842370593129636931[237] = 0;
   out_842370593129636931[238] = 0;
   out_842370593129636931[239] = 0;
   out_842370593129636931[240] = 0;
   out_842370593129636931[241] = 0;
   out_842370593129636931[242] = 0;
   out_842370593129636931[243] = 0;
   out_842370593129636931[244] = 0;
   out_842370593129636931[245] = 0;
   out_842370593129636931[246] = 0;
   out_842370593129636931[247] = 1;
   out_842370593129636931[248] = 0;
   out_842370593129636931[249] = 0;
   out_842370593129636931[250] = 0;
   out_842370593129636931[251] = 0;
   out_842370593129636931[252] = 0;
   out_842370593129636931[253] = 0;
   out_842370593129636931[254] = 0;
   out_842370593129636931[255] = 0;
   out_842370593129636931[256] = 0;
   out_842370593129636931[257] = 0;
   out_842370593129636931[258] = 0;
   out_842370593129636931[259] = 0;
   out_842370593129636931[260] = 0;
   out_842370593129636931[261] = 0;
   out_842370593129636931[262] = 0;
   out_842370593129636931[263] = 0;
   out_842370593129636931[264] = 0;
   out_842370593129636931[265] = 0;
   out_842370593129636931[266] = 1;
   out_842370593129636931[267] = 0;
   out_842370593129636931[268] = 0;
   out_842370593129636931[269] = 0;
   out_842370593129636931[270] = 0;
   out_842370593129636931[271] = 0;
   out_842370593129636931[272] = 0;
   out_842370593129636931[273] = 0;
   out_842370593129636931[274] = 0;
   out_842370593129636931[275] = 0;
   out_842370593129636931[276] = 0;
   out_842370593129636931[277] = 0;
   out_842370593129636931[278] = 0;
   out_842370593129636931[279] = 0;
   out_842370593129636931[280] = 0;
   out_842370593129636931[281] = 0;
   out_842370593129636931[282] = 0;
   out_842370593129636931[283] = 0;
   out_842370593129636931[284] = 0;
   out_842370593129636931[285] = 1;
   out_842370593129636931[286] = 0;
   out_842370593129636931[287] = 0;
   out_842370593129636931[288] = 0;
   out_842370593129636931[289] = 0;
   out_842370593129636931[290] = 0;
   out_842370593129636931[291] = 0;
   out_842370593129636931[292] = 0;
   out_842370593129636931[293] = 0;
   out_842370593129636931[294] = 0;
   out_842370593129636931[295] = 0;
   out_842370593129636931[296] = 0;
   out_842370593129636931[297] = 0;
   out_842370593129636931[298] = 0;
   out_842370593129636931[299] = 0;
   out_842370593129636931[300] = 0;
   out_842370593129636931[301] = 0;
   out_842370593129636931[302] = 0;
   out_842370593129636931[303] = 0;
   out_842370593129636931[304] = 1;
   out_842370593129636931[305] = 0;
   out_842370593129636931[306] = 0;
   out_842370593129636931[307] = 0;
   out_842370593129636931[308] = 0;
   out_842370593129636931[309] = 0;
   out_842370593129636931[310] = 0;
   out_842370593129636931[311] = 0;
   out_842370593129636931[312] = 0;
   out_842370593129636931[313] = 0;
   out_842370593129636931[314] = 0;
   out_842370593129636931[315] = 0;
   out_842370593129636931[316] = 0;
   out_842370593129636931[317] = 0;
   out_842370593129636931[318] = 0;
   out_842370593129636931[319] = 0;
   out_842370593129636931[320] = 0;
   out_842370593129636931[321] = 0;
   out_842370593129636931[322] = 0;
   out_842370593129636931[323] = 1;
}
void h_4(double *state, double *unused, double *out_2597862681562698782) {
   out_2597862681562698782[0] = state[6] + state[9];
   out_2597862681562698782[1] = state[7] + state[10];
   out_2597862681562698782[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_7753124876646203718) {
   out_7753124876646203718[0] = 0;
   out_7753124876646203718[1] = 0;
   out_7753124876646203718[2] = 0;
   out_7753124876646203718[3] = 0;
   out_7753124876646203718[4] = 0;
   out_7753124876646203718[5] = 0;
   out_7753124876646203718[6] = 1;
   out_7753124876646203718[7] = 0;
   out_7753124876646203718[8] = 0;
   out_7753124876646203718[9] = 1;
   out_7753124876646203718[10] = 0;
   out_7753124876646203718[11] = 0;
   out_7753124876646203718[12] = 0;
   out_7753124876646203718[13] = 0;
   out_7753124876646203718[14] = 0;
   out_7753124876646203718[15] = 0;
   out_7753124876646203718[16] = 0;
   out_7753124876646203718[17] = 0;
   out_7753124876646203718[18] = 0;
   out_7753124876646203718[19] = 0;
   out_7753124876646203718[20] = 0;
   out_7753124876646203718[21] = 0;
   out_7753124876646203718[22] = 0;
   out_7753124876646203718[23] = 0;
   out_7753124876646203718[24] = 0;
   out_7753124876646203718[25] = 1;
   out_7753124876646203718[26] = 0;
   out_7753124876646203718[27] = 0;
   out_7753124876646203718[28] = 1;
   out_7753124876646203718[29] = 0;
   out_7753124876646203718[30] = 0;
   out_7753124876646203718[31] = 0;
   out_7753124876646203718[32] = 0;
   out_7753124876646203718[33] = 0;
   out_7753124876646203718[34] = 0;
   out_7753124876646203718[35] = 0;
   out_7753124876646203718[36] = 0;
   out_7753124876646203718[37] = 0;
   out_7753124876646203718[38] = 0;
   out_7753124876646203718[39] = 0;
   out_7753124876646203718[40] = 0;
   out_7753124876646203718[41] = 0;
   out_7753124876646203718[42] = 0;
   out_7753124876646203718[43] = 0;
   out_7753124876646203718[44] = 1;
   out_7753124876646203718[45] = 0;
   out_7753124876646203718[46] = 0;
   out_7753124876646203718[47] = 1;
   out_7753124876646203718[48] = 0;
   out_7753124876646203718[49] = 0;
   out_7753124876646203718[50] = 0;
   out_7753124876646203718[51] = 0;
   out_7753124876646203718[52] = 0;
   out_7753124876646203718[53] = 0;
}
void h_10(double *state, double *unused, double *out_9148864414269485495) {
   out_9148864414269485495[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_9148864414269485495[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_9148864414269485495[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_2721278627709812660) {
   out_2721278627709812660[0] = 0;
   out_2721278627709812660[1] = 9.8100000000000005*cos(state[1]);
   out_2721278627709812660[2] = 0;
   out_2721278627709812660[3] = 0;
   out_2721278627709812660[4] = -state[8];
   out_2721278627709812660[5] = state[7];
   out_2721278627709812660[6] = 0;
   out_2721278627709812660[7] = state[5];
   out_2721278627709812660[8] = -state[4];
   out_2721278627709812660[9] = 0;
   out_2721278627709812660[10] = 0;
   out_2721278627709812660[11] = 0;
   out_2721278627709812660[12] = 1;
   out_2721278627709812660[13] = 0;
   out_2721278627709812660[14] = 0;
   out_2721278627709812660[15] = 1;
   out_2721278627709812660[16] = 0;
   out_2721278627709812660[17] = 0;
   out_2721278627709812660[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_2721278627709812660[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_2721278627709812660[20] = 0;
   out_2721278627709812660[21] = state[8];
   out_2721278627709812660[22] = 0;
   out_2721278627709812660[23] = -state[6];
   out_2721278627709812660[24] = -state[5];
   out_2721278627709812660[25] = 0;
   out_2721278627709812660[26] = state[3];
   out_2721278627709812660[27] = 0;
   out_2721278627709812660[28] = 0;
   out_2721278627709812660[29] = 0;
   out_2721278627709812660[30] = 0;
   out_2721278627709812660[31] = 1;
   out_2721278627709812660[32] = 0;
   out_2721278627709812660[33] = 0;
   out_2721278627709812660[34] = 1;
   out_2721278627709812660[35] = 0;
   out_2721278627709812660[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_2721278627709812660[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_2721278627709812660[38] = 0;
   out_2721278627709812660[39] = -state[7];
   out_2721278627709812660[40] = state[6];
   out_2721278627709812660[41] = 0;
   out_2721278627709812660[42] = state[4];
   out_2721278627709812660[43] = -state[3];
   out_2721278627709812660[44] = 0;
   out_2721278627709812660[45] = 0;
   out_2721278627709812660[46] = 0;
   out_2721278627709812660[47] = 0;
   out_2721278627709812660[48] = 0;
   out_2721278627709812660[49] = 0;
   out_2721278627709812660[50] = 1;
   out_2721278627709812660[51] = 0;
   out_2721278627709812660[52] = 0;
   out_2721278627709812660[53] = 1;
}
void h_13(double *state, double *unused, double *out_4889610167345451391) {
   out_4889610167345451391[0] = state[3];
   out_4889610167345451391[1] = state[4];
   out_4889610167345451391[2] = state[5];
}
void H_13(double *state, double *unused, double *out_7481345371731015097) {
   out_7481345371731015097[0] = 0;
   out_7481345371731015097[1] = 0;
   out_7481345371731015097[2] = 0;
   out_7481345371731015097[3] = 1;
   out_7481345371731015097[4] = 0;
   out_7481345371731015097[5] = 0;
   out_7481345371731015097[6] = 0;
   out_7481345371731015097[7] = 0;
   out_7481345371731015097[8] = 0;
   out_7481345371731015097[9] = 0;
   out_7481345371731015097[10] = 0;
   out_7481345371731015097[11] = 0;
   out_7481345371731015097[12] = 0;
   out_7481345371731015097[13] = 0;
   out_7481345371731015097[14] = 0;
   out_7481345371731015097[15] = 0;
   out_7481345371731015097[16] = 0;
   out_7481345371731015097[17] = 0;
   out_7481345371731015097[18] = 0;
   out_7481345371731015097[19] = 0;
   out_7481345371731015097[20] = 0;
   out_7481345371731015097[21] = 0;
   out_7481345371731015097[22] = 1;
   out_7481345371731015097[23] = 0;
   out_7481345371731015097[24] = 0;
   out_7481345371731015097[25] = 0;
   out_7481345371731015097[26] = 0;
   out_7481345371731015097[27] = 0;
   out_7481345371731015097[28] = 0;
   out_7481345371731015097[29] = 0;
   out_7481345371731015097[30] = 0;
   out_7481345371731015097[31] = 0;
   out_7481345371731015097[32] = 0;
   out_7481345371731015097[33] = 0;
   out_7481345371731015097[34] = 0;
   out_7481345371731015097[35] = 0;
   out_7481345371731015097[36] = 0;
   out_7481345371731015097[37] = 0;
   out_7481345371731015097[38] = 0;
   out_7481345371731015097[39] = 0;
   out_7481345371731015097[40] = 0;
   out_7481345371731015097[41] = 1;
   out_7481345371731015097[42] = 0;
   out_7481345371731015097[43] = 0;
   out_7481345371731015097[44] = 0;
   out_7481345371731015097[45] = 0;
   out_7481345371731015097[46] = 0;
   out_7481345371731015097[47] = 0;
   out_7481345371731015097[48] = 0;
   out_7481345371731015097[49] = 0;
   out_7481345371731015097[50] = 0;
   out_7481345371731015097[51] = 0;
   out_7481345371731015097[52] = 0;
   out_7481345371731015097[53] = 0;
}
void h_14(double *state, double *unused, double *out_4191467436296733347) {
   out_4191467436296733347[0] = state[6];
   out_4191467436296733347[1] = state[7];
   out_4191467436296733347[2] = state[8];
}
void H_14(double *state, double *unused, double *out_4670336444350831422) {
   out_4670336444350831422[0] = 0;
   out_4670336444350831422[1] = 0;
   out_4670336444350831422[2] = 0;
   out_4670336444350831422[3] = 0;
   out_4670336444350831422[4] = 0;
   out_4670336444350831422[5] = 0;
   out_4670336444350831422[6] = 1;
   out_4670336444350831422[7] = 0;
   out_4670336444350831422[8] = 0;
   out_4670336444350831422[9] = 0;
   out_4670336444350831422[10] = 0;
   out_4670336444350831422[11] = 0;
   out_4670336444350831422[12] = 0;
   out_4670336444350831422[13] = 0;
   out_4670336444350831422[14] = 0;
   out_4670336444350831422[15] = 0;
   out_4670336444350831422[16] = 0;
   out_4670336444350831422[17] = 0;
   out_4670336444350831422[18] = 0;
   out_4670336444350831422[19] = 0;
   out_4670336444350831422[20] = 0;
   out_4670336444350831422[21] = 0;
   out_4670336444350831422[22] = 0;
   out_4670336444350831422[23] = 0;
   out_4670336444350831422[24] = 0;
   out_4670336444350831422[25] = 1;
   out_4670336444350831422[26] = 0;
   out_4670336444350831422[27] = 0;
   out_4670336444350831422[28] = 0;
   out_4670336444350831422[29] = 0;
   out_4670336444350831422[30] = 0;
   out_4670336444350831422[31] = 0;
   out_4670336444350831422[32] = 0;
   out_4670336444350831422[33] = 0;
   out_4670336444350831422[34] = 0;
   out_4670336444350831422[35] = 0;
   out_4670336444350831422[36] = 0;
   out_4670336444350831422[37] = 0;
   out_4670336444350831422[38] = 0;
   out_4670336444350831422[39] = 0;
   out_4670336444350831422[40] = 0;
   out_4670336444350831422[41] = 0;
   out_4670336444350831422[42] = 0;
   out_4670336444350831422[43] = 0;
   out_4670336444350831422[44] = 1;
   out_4670336444350831422[45] = 0;
   out_4670336444350831422[46] = 0;
   out_4670336444350831422[47] = 0;
   out_4670336444350831422[48] = 0;
   out_4670336444350831422[49] = 0;
   out_4670336444350831422[50] = 0;
   out_4670336444350831422[51] = 0;
   out_4670336444350831422[52] = 0;
   out_4670336444350831422[53] = 0;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_1960240961147461763) {
  err_fun(nom_x, delta_x, out_1960240961147461763);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_2311725942617102893) {
  inv_err_fun(nom_x, true_x, out_2311725942617102893);
}
void pose_H_mod_fun(double *state, double *out_3907531448917969471) {
  H_mod_fun(state, out_3907531448917969471);
}
void pose_f_fun(double *state, double dt, double *out_322024657666628683) {
  f_fun(state,  dt, out_322024657666628683);
}
void pose_F_fun(double *state, double dt, double *out_842370593129636931) {
  F_fun(state,  dt, out_842370593129636931);
}
void pose_h_4(double *state, double *unused, double *out_2597862681562698782) {
  h_4(state, unused, out_2597862681562698782);
}
void pose_H_4(double *state, double *unused, double *out_7753124876646203718) {
  H_4(state, unused, out_7753124876646203718);
}
void pose_h_10(double *state, double *unused, double *out_9148864414269485495) {
  h_10(state, unused, out_9148864414269485495);
}
void pose_H_10(double *state, double *unused, double *out_2721278627709812660) {
  H_10(state, unused, out_2721278627709812660);
}
void pose_h_13(double *state, double *unused, double *out_4889610167345451391) {
  h_13(state, unused, out_4889610167345451391);
}
void pose_H_13(double *state, double *unused, double *out_7481345371731015097) {
  H_13(state, unused, out_7481345371731015097);
}
void pose_h_14(double *state, double *unused, double *out_4191467436296733347) {
  h_14(state, unused, out_4191467436296733347);
}
void pose_H_14(double *state, double *unused, double *out_4670336444350831422) {
  H_14(state, unused, out_4670336444350831422);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
