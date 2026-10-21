#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_9110105061904509225) {
   out_9110105061904509225[0] = delta_x[0] + nom_x[0];
   out_9110105061904509225[1] = delta_x[1] + nom_x[1];
   out_9110105061904509225[2] = delta_x[2] + nom_x[2];
   out_9110105061904509225[3] = delta_x[3] + nom_x[3];
   out_9110105061904509225[4] = delta_x[4] + nom_x[4];
   out_9110105061904509225[5] = delta_x[5] + nom_x[5];
   out_9110105061904509225[6] = delta_x[6] + nom_x[6];
   out_9110105061904509225[7] = delta_x[7] + nom_x[7];
   out_9110105061904509225[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_3235904000805918128) {
   out_3235904000805918128[0] = -nom_x[0] + true_x[0];
   out_3235904000805918128[1] = -nom_x[1] + true_x[1];
   out_3235904000805918128[2] = -nom_x[2] + true_x[2];
   out_3235904000805918128[3] = -nom_x[3] + true_x[3];
   out_3235904000805918128[4] = -nom_x[4] + true_x[4];
   out_3235904000805918128[5] = -nom_x[5] + true_x[5];
   out_3235904000805918128[6] = -nom_x[6] + true_x[6];
   out_3235904000805918128[7] = -nom_x[7] + true_x[7];
   out_3235904000805918128[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_6378495611849220177) {
   out_6378495611849220177[0] = 1.0;
   out_6378495611849220177[1] = 0.0;
   out_6378495611849220177[2] = 0.0;
   out_6378495611849220177[3] = 0.0;
   out_6378495611849220177[4] = 0.0;
   out_6378495611849220177[5] = 0.0;
   out_6378495611849220177[6] = 0.0;
   out_6378495611849220177[7] = 0.0;
   out_6378495611849220177[8] = 0.0;
   out_6378495611849220177[9] = 0.0;
   out_6378495611849220177[10] = 1.0;
   out_6378495611849220177[11] = 0.0;
   out_6378495611849220177[12] = 0.0;
   out_6378495611849220177[13] = 0.0;
   out_6378495611849220177[14] = 0.0;
   out_6378495611849220177[15] = 0.0;
   out_6378495611849220177[16] = 0.0;
   out_6378495611849220177[17] = 0.0;
   out_6378495611849220177[18] = 0.0;
   out_6378495611849220177[19] = 0.0;
   out_6378495611849220177[20] = 1.0;
   out_6378495611849220177[21] = 0.0;
   out_6378495611849220177[22] = 0.0;
   out_6378495611849220177[23] = 0.0;
   out_6378495611849220177[24] = 0.0;
   out_6378495611849220177[25] = 0.0;
   out_6378495611849220177[26] = 0.0;
   out_6378495611849220177[27] = 0.0;
   out_6378495611849220177[28] = 0.0;
   out_6378495611849220177[29] = 0.0;
   out_6378495611849220177[30] = 1.0;
   out_6378495611849220177[31] = 0.0;
   out_6378495611849220177[32] = 0.0;
   out_6378495611849220177[33] = 0.0;
   out_6378495611849220177[34] = 0.0;
   out_6378495611849220177[35] = 0.0;
   out_6378495611849220177[36] = 0.0;
   out_6378495611849220177[37] = 0.0;
   out_6378495611849220177[38] = 0.0;
   out_6378495611849220177[39] = 0.0;
   out_6378495611849220177[40] = 1.0;
   out_6378495611849220177[41] = 0.0;
   out_6378495611849220177[42] = 0.0;
   out_6378495611849220177[43] = 0.0;
   out_6378495611849220177[44] = 0.0;
   out_6378495611849220177[45] = 0.0;
   out_6378495611849220177[46] = 0.0;
   out_6378495611849220177[47] = 0.0;
   out_6378495611849220177[48] = 0.0;
   out_6378495611849220177[49] = 0.0;
   out_6378495611849220177[50] = 1.0;
   out_6378495611849220177[51] = 0.0;
   out_6378495611849220177[52] = 0.0;
   out_6378495611849220177[53] = 0.0;
   out_6378495611849220177[54] = 0.0;
   out_6378495611849220177[55] = 0.0;
   out_6378495611849220177[56] = 0.0;
   out_6378495611849220177[57] = 0.0;
   out_6378495611849220177[58] = 0.0;
   out_6378495611849220177[59] = 0.0;
   out_6378495611849220177[60] = 1.0;
   out_6378495611849220177[61] = 0.0;
   out_6378495611849220177[62] = 0.0;
   out_6378495611849220177[63] = 0.0;
   out_6378495611849220177[64] = 0.0;
   out_6378495611849220177[65] = 0.0;
   out_6378495611849220177[66] = 0.0;
   out_6378495611849220177[67] = 0.0;
   out_6378495611849220177[68] = 0.0;
   out_6378495611849220177[69] = 0.0;
   out_6378495611849220177[70] = 1.0;
   out_6378495611849220177[71] = 0.0;
   out_6378495611849220177[72] = 0.0;
   out_6378495611849220177[73] = 0.0;
   out_6378495611849220177[74] = 0.0;
   out_6378495611849220177[75] = 0.0;
   out_6378495611849220177[76] = 0.0;
   out_6378495611849220177[77] = 0.0;
   out_6378495611849220177[78] = 0.0;
   out_6378495611849220177[79] = 0.0;
   out_6378495611849220177[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_6959068607126581523) {
   out_6959068607126581523[0] = state[0];
   out_6959068607126581523[1] = state[1];
   out_6959068607126581523[2] = state[2];
   out_6959068607126581523[3] = state[3];
   out_6959068607126581523[4] = state[4];
   out_6959068607126581523[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8100000000000005*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_6959068607126581523[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_6959068607126581523[7] = state[7];
   out_6959068607126581523[8] = state[8];
}
void F_fun(double *state, double dt, double *out_321314453042049537) {
   out_321314453042049537[0] = 1;
   out_321314453042049537[1] = 0;
   out_321314453042049537[2] = 0;
   out_321314453042049537[3] = 0;
   out_321314453042049537[4] = 0;
   out_321314453042049537[5] = 0;
   out_321314453042049537[6] = 0;
   out_321314453042049537[7] = 0;
   out_321314453042049537[8] = 0;
   out_321314453042049537[9] = 0;
   out_321314453042049537[10] = 1;
   out_321314453042049537[11] = 0;
   out_321314453042049537[12] = 0;
   out_321314453042049537[13] = 0;
   out_321314453042049537[14] = 0;
   out_321314453042049537[15] = 0;
   out_321314453042049537[16] = 0;
   out_321314453042049537[17] = 0;
   out_321314453042049537[18] = 0;
   out_321314453042049537[19] = 0;
   out_321314453042049537[20] = 1;
   out_321314453042049537[21] = 0;
   out_321314453042049537[22] = 0;
   out_321314453042049537[23] = 0;
   out_321314453042049537[24] = 0;
   out_321314453042049537[25] = 0;
   out_321314453042049537[26] = 0;
   out_321314453042049537[27] = 0;
   out_321314453042049537[28] = 0;
   out_321314453042049537[29] = 0;
   out_321314453042049537[30] = 1;
   out_321314453042049537[31] = 0;
   out_321314453042049537[32] = 0;
   out_321314453042049537[33] = 0;
   out_321314453042049537[34] = 0;
   out_321314453042049537[35] = 0;
   out_321314453042049537[36] = 0;
   out_321314453042049537[37] = 0;
   out_321314453042049537[38] = 0;
   out_321314453042049537[39] = 0;
   out_321314453042049537[40] = 1;
   out_321314453042049537[41] = 0;
   out_321314453042049537[42] = 0;
   out_321314453042049537[43] = 0;
   out_321314453042049537[44] = 0;
   out_321314453042049537[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_321314453042049537[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_321314453042049537[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_321314453042049537[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_321314453042049537[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_321314453042049537[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_321314453042049537[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_321314453042049537[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_321314453042049537[53] = -9.8100000000000005*dt;
   out_321314453042049537[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_321314453042049537[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_321314453042049537[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_321314453042049537[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_321314453042049537[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_321314453042049537[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_321314453042049537[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_321314453042049537[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_321314453042049537[62] = 0;
   out_321314453042049537[63] = 0;
   out_321314453042049537[64] = 0;
   out_321314453042049537[65] = 0;
   out_321314453042049537[66] = 0;
   out_321314453042049537[67] = 0;
   out_321314453042049537[68] = 0;
   out_321314453042049537[69] = 0;
   out_321314453042049537[70] = 1;
   out_321314453042049537[71] = 0;
   out_321314453042049537[72] = 0;
   out_321314453042049537[73] = 0;
   out_321314453042049537[74] = 0;
   out_321314453042049537[75] = 0;
   out_321314453042049537[76] = 0;
   out_321314453042049537[77] = 0;
   out_321314453042049537[78] = 0;
   out_321314453042049537[79] = 0;
   out_321314453042049537[80] = 1;
}
void h_25(double *state, double *unused, double *out_8974464392655101919) {
   out_8974464392655101919[0] = state[6];
}
void H_25(double *state, double *unused, double *out_3393338321313619400) {
   out_3393338321313619400[0] = 0;
   out_3393338321313619400[1] = 0;
   out_3393338321313619400[2] = 0;
   out_3393338321313619400[3] = 0;
   out_3393338321313619400[4] = 0;
   out_3393338321313619400[5] = 0;
   out_3393338321313619400[6] = 1;
   out_3393338321313619400[7] = 0;
   out_3393338321313619400[8] = 0;
}
void h_24(double *state, double *unused, double *out_1301839241695337441) {
   out_1301839241695337441[0] = state[4];
   out_1301839241695337441[1] = state[5];
}
void H_24(double *state, double *unused, double *out_5081154757182723591) {
   out_5081154757182723591[0] = 0;
   out_5081154757182723591[1] = 0;
   out_5081154757182723591[2] = 0;
   out_5081154757182723591[3] = 0;
   out_5081154757182723591[4] = 1;
   out_5081154757182723591[5] = 0;
   out_5081154757182723591[6] = 0;
   out_5081154757182723591[7] = 0;
   out_5081154757182723591[8] = 0;
   out_5081154757182723591[9] = 0;
   out_5081154757182723591[10] = 0;
   out_5081154757182723591[11] = 0;
   out_5081154757182723591[12] = 0;
   out_5081154757182723591[13] = 0;
   out_5081154757182723591[14] = 1;
   out_5081154757182723591[15] = 0;
   out_5081154757182723591[16] = 0;
   out_5081154757182723591[17] = 0;
}
void h_30(double *state, double *unused, double *out_8870861523441086143) {
   out_8870861523441086143[0] = state[4];
}
void H_30(double *state, double *unused, double *out_5911671279820868027) {
   out_5911671279820868027[0] = 0;
   out_5911671279820868027[1] = 0;
   out_5911671279820868027[2] = 0;
   out_5911671279820868027[3] = 0;
   out_5911671279820868027[4] = 1;
   out_5911671279820868027[5] = 0;
   out_5911671279820868027[6] = 0;
   out_5911671279820868027[7] = 0;
   out_5911671279820868027[8] = 0;
}
void h_26(double *state, double *unused, double *out_8267545638764608626) {
   out_8267545638764608626[0] = state[7];
}
void H_26(double *state, double *unused, double *out_348164997560436824) {
   out_348164997560436824[0] = 0;
   out_348164997560436824[1] = 0;
   out_348164997560436824[2] = 0;
   out_348164997560436824[3] = 0;
   out_348164997560436824[4] = 0;
   out_348164997560436824[5] = 0;
   out_348164997560436824[6] = 0;
   out_348164997560436824[7] = 1;
   out_348164997560436824[8] = 0;
}
void h_27(double *state, double *unused, double *out_1339523744063128545) {
   out_1339523744063128545[0] = state[3];
}
void H_27(double *state, double *unused, double *out_3736907968020443116) {
   out_3736907968020443116[0] = 0;
   out_3736907968020443116[1] = 0;
   out_3736907968020443116[2] = 0;
   out_3736907968020443116[3] = 1;
   out_3736907968020443116[4] = 0;
   out_3736907968020443116[5] = 0;
   out_3736907968020443116[6] = 0;
   out_3736907968020443116[7] = 0;
   out_3736907968020443116[8] = 0;
}
void h_29(double *state, double *unused, double *out_111107648565754622) {
   out_111107648565754622[0] = state[1];
}
void H_29(double *state, double *unused, double *out_6421902624135260211) {
   out_6421902624135260211[0] = 0;
   out_6421902624135260211[1] = 1;
   out_6421902624135260211[2] = 0;
   out_6421902624135260211[3] = 0;
   out_6421902624135260211[4] = 0;
   out_6421902624135260211[5] = 0;
   out_6421902624135260211[6] = 0;
   out_6421902624135260211[7] = 0;
   out_6421902624135260211[8] = 0;
}
void h_28(double *state, double *unused, double *out_1253116090841068673) {
   out_1253116090841068673[0] = state[0];
}
void H_28(double *state, double *unused, double *out_1339503607065729637) {
   out_1339503607065729637[0] = 1;
   out_1339503607065729637[1] = 0;
   out_1339503607065729637[2] = 0;
   out_1339503607065729637[3] = 0;
   out_1339503607065729637[4] = 0;
   out_1339503607065729637[5] = 0;
   out_1339503607065729637[6] = 0;
   out_1339503607065729637[7] = 0;
   out_1339503607065729637[8] = 0;
}
void h_31(double *state, double *unused, double *out_1211199135834429478) {
   out_1211199135834429478[0] = state[8];
}
void H_31(double *state, double *unused, double *out_974373099793788300) {
   out_974373099793788300[0] = 0;
   out_974373099793788300[1] = 0;
   out_974373099793788300[2] = 0;
   out_974373099793788300[3] = 0;
   out_974373099793788300[4] = 0;
   out_974373099793788300[5] = 0;
   out_974373099793788300[6] = 0;
   out_974373099793788300[7] = 0;
   out_974373099793788300[8] = 1;
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

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_9110105061904509225) {
  err_fun(nom_x, delta_x, out_9110105061904509225);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_3235904000805918128) {
  inv_err_fun(nom_x, true_x, out_3235904000805918128);
}
void car_H_mod_fun(double *state, double *out_6378495611849220177) {
  H_mod_fun(state, out_6378495611849220177);
}
void car_f_fun(double *state, double dt, double *out_6959068607126581523) {
  f_fun(state,  dt, out_6959068607126581523);
}
void car_F_fun(double *state, double dt, double *out_321314453042049537) {
  F_fun(state,  dt, out_321314453042049537);
}
void car_h_25(double *state, double *unused, double *out_8974464392655101919) {
  h_25(state, unused, out_8974464392655101919);
}
void car_H_25(double *state, double *unused, double *out_3393338321313619400) {
  H_25(state, unused, out_3393338321313619400);
}
void car_h_24(double *state, double *unused, double *out_1301839241695337441) {
  h_24(state, unused, out_1301839241695337441);
}
void car_H_24(double *state, double *unused, double *out_5081154757182723591) {
  H_24(state, unused, out_5081154757182723591);
}
void car_h_30(double *state, double *unused, double *out_8870861523441086143) {
  h_30(state, unused, out_8870861523441086143);
}
void car_H_30(double *state, double *unused, double *out_5911671279820868027) {
  H_30(state, unused, out_5911671279820868027);
}
void car_h_26(double *state, double *unused, double *out_8267545638764608626) {
  h_26(state, unused, out_8267545638764608626);
}
void car_H_26(double *state, double *unused, double *out_348164997560436824) {
  H_26(state, unused, out_348164997560436824);
}
void car_h_27(double *state, double *unused, double *out_1339523744063128545) {
  h_27(state, unused, out_1339523744063128545);
}
void car_H_27(double *state, double *unused, double *out_3736907968020443116) {
  H_27(state, unused, out_3736907968020443116);
}
void car_h_29(double *state, double *unused, double *out_111107648565754622) {
  h_29(state, unused, out_111107648565754622);
}
void car_H_29(double *state, double *unused, double *out_6421902624135260211) {
  H_29(state, unused, out_6421902624135260211);
}
void car_h_28(double *state, double *unused, double *out_1253116090841068673) {
  h_28(state, unused, out_1253116090841068673);
}
void car_H_28(double *state, double *unused, double *out_1339503607065729637) {
  H_28(state, unused, out_1339503607065729637);
}
void car_h_31(double *state, double *unused, double *out_1211199135834429478) {
  h_31(state, unused, out_1211199135834429478);
}
void car_H_31(double *state, double *unused, double *out_974373099793788300) {
  H_31(state, unused, out_974373099793788300);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
