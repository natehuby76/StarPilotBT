#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_9110105061904509225);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_3235904000805918128);
void car_H_mod_fun(double *state, double *out_6378495611849220177);
void car_f_fun(double *state, double dt, double *out_6959068607126581523);
void car_F_fun(double *state, double dt, double *out_321314453042049537);
void car_h_25(double *state, double *unused, double *out_8974464392655101919);
void car_H_25(double *state, double *unused, double *out_3393338321313619400);
void car_h_24(double *state, double *unused, double *out_1301839241695337441);
void car_H_24(double *state, double *unused, double *out_5081154757182723591);
void car_h_30(double *state, double *unused, double *out_8870861523441086143);
void car_H_30(double *state, double *unused, double *out_5911671279820868027);
void car_h_26(double *state, double *unused, double *out_8267545638764608626);
void car_H_26(double *state, double *unused, double *out_348164997560436824);
void car_h_27(double *state, double *unused, double *out_1339523744063128545);
void car_H_27(double *state, double *unused, double *out_3736907968020443116);
void car_h_29(double *state, double *unused, double *out_111107648565754622);
void car_H_29(double *state, double *unused, double *out_6421902624135260211);
void car_h_28(double *state, double *unused, double *out_1253116090841068673);
void car_H_28(double *state, double *unused, double *out_1339503607065729637);
void car_h_31(double *state, double *unused, double *out_1211199135834429478);
void car_H_31(double *state, double *unused, double *out_974373099793788300);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}