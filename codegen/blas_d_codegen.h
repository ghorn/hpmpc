/**************************************************************************************************
*                                                                                                 *
* This file is part of HPMPC.                                                                     *
*                                                                                                 *
* HPMPC -- Library for High-Performance implementation of solvers for MPC.                        *
* Copyright (C) 2014 by Technical University of Denmark. All rights reserved.                     *
*                                                                                                 *
* HPMPC is free software; you can redistribute it and/or                                          *
* modify it under the terms of the GNU Lesser General Public                                      *
* License as published by the Free Software Foundation; either                                    *
* version 2.1 of the License, or (at your option) any later version.                              *
*                                                                                                 *
* HPMPC is distributed in the hope that it will be useful,                                        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                                            *
* See the GNU Lesser General Public License for more details.                                     *
*                                                                                                 *
* You should have received a copy of the GNU Lesser General Public                                *
* License along with HPMPC; if not, write to the Free Software                                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA                  *
*                                                                                                 *
* Author: Gianluca Frison, giaf (at) dtu.dk                                                       *
*                                                                                                 *
**************************************************************************************************/

void dtrmm_code_generator(FILE *f, int m, int n);
void dsyrk_dpotrf_code_generator(FILE *f, int m, int k, int n);
void dgemv_n_code_generator(FILE *f, int m, int n, int alg);
void dgemv_t_code_generator(FILE *f, int m, int n, int offset, int sda, int alg);
void dtrmv_u_n_code_generator(FILE *f, int m, int alg);
void dtrmv_u_t_code_generator(FILE *f, int m, int alg);
void dsymv_code_generator(FILE *f, int m, int offset, int alg);
void dmvmv_code_generator(FILE *f, int m, int n, int offset, int alg);
void dtrsv_dgemv_n_code_generator(FILE *f, int m, int n);
void dtrsv_dgemv_t_code_generator(FILE *f, int m, int n);
void dtrtr_l_code_generator(FILE *f, int m, int offset);

