#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_1960240961147461763);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_2311725942617102893);
void pose_H_mod_fun(double *state, double *out_3907531448917969471);
void pose_f_fun(double *state, double dt, double *out_322024657666628683);
void pose_F_fun(double *state, double dt, double *out_842370593129636931);
void pose_h_4(double *state, double *unused, double *out_2597862681562698782);
void pose_H_4(double *state, double *unused, double *out_7753124876646203718);
void pose_h_10(double *state, double *unused, double *out_9148864414269485495);
void pose_H_10(double *state, double *unused, double *out_2721278627709812660);
void pose_h_13(double *state, double *unused, double *out_4889610167345451391);
void pose_H_13(double *state, double *unused, double *out_7481345371731015097);
void pose_h_14(double *state, double *unused, double *out_4191467436296733347);
void pose_H_14(double *state, double *unused, double *out_4670336444350831422);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}