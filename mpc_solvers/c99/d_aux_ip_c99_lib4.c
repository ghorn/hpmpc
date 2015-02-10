/**************************************************************************************************
*                                                                                                 *
* This file is part of HPMPC.                                                                     *
*                                                                                                 *
* HPMPC -- Library for High-Performance implementation of solvers for MPC.                        *
* Copyright (C) 2014-2015 by Technical University of Denmark. All rights reserved.                *
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

#include "../../include/block_size.h"



void d_init_var_hard_mpc(int N, int nx, int nu, int nb, int ng, double **ux, double **pi, double **db, double **t, double **lam, double mu0, int warm_start)
	{

	const int nbu = nu<nb ? nu : nb ;
	
	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	//const int anb = nal*((nb+nal-1)/nal); // cache aligned number of box constraints
	const int pnb = bs*((nb+bs-1)/bs); // simd aligned number of box constraints
	//const int ang = nal*((ng+nal-1)/nal); // cache aligned number of general constraints
	const int png = bs*((ng+bs-1)/bs); // cache aligned number of general constraints

	int jj, ll, ii;
	
	double thr0 = 0.1; // minimum distance from a constraint

	if(warm_start==1)
		{
		for(ll=0; ll<nbu; ll++)
			{
			t[0][ll]     =  - db[0][ll]     + ux[0][ll];
			t[0][pnb+ll] =  - db[0][pnb+ll] - ux[0][ll];
			if(t[0][ll] < thr0)
				{
				if(t[0][pnb+ll] < thr0)
					{
					ux[0][ll] = ( - db[0][pnb+ll] + db[0][ll])*0.5;
					t[0][ll]     = - db[0][ll]     + ux[0][ll] ;
					t[0][pnb+ll] = - db[0][pnb+ll] - ux[0][ll];
					}
				else
					{
					t[0][ll] = thr0;
					ux[0][ll] = db[0][ll] + thr0;
					}
				}
			else if(t[0][pnb+ll] < thr0)
				{
				t[0][pnb+ll] = thr0;
				ux[0][ll] = - db[0][pnb+ll] - thr0;
				}
			lam[0][ll]     = mu0/t[0][ll];
			lam[0][pnb+ll] = mu0/t[0][pnb+ll];
			}
		for(; ll<nb; ll++)
			{
			t[0][ll]       = 1.0; // this has to be strictly positive !!!
			lam[0][ll]     = 1.0; // this has to be strictly positive !!!
			t[0][pnb+ll]   = 1.0; // this has to be strictly positive !!!
			lam[0][pnb+ll] = 1.0; // this has to be strictly positive !!!
			}
		for(jj=1; jj<N; jj++)
			{
			for(ll=0; ll<nb; ll++)
				{
				t[jj][ll]     = - db[jj][ll]     + ux[jj][ll];
				t[jj][pnb+ll] = - db[jj][pnb+ll] - ux[jj][ll];
				if(t[jj][ll] < thr0)
					{
					if(t[jj][pnb+ll] < thr0)
						{
						ux[jj][ll] = ( - db[jj][pnb+ll] + db[jj][ll])*0.5;
						t[jj][ll]     = - db[jj][ll]     + ux[jj][ll];
						t[jj][pnb+ll] = - db[jj][pnb+ll] - ux[jj][ll];
						}
					else
						{
						t[jj][ll] = thr0;
						ux[jj][ll] = db[jj][ll] + thr0;
						}
					}
				else if(t[jj][pnb+ll] < thr0)
					{
					t[jj][pnb+ll] = thr0;
					ux[jj][ll] = - db[jj][pnb+ll] - thr0;
					}
				lam[jj][ll]     = mu0/t[jj][ll];
				lam[jj][pnb+ll] = mu0/t[jj][pnb+ll];
				}
			}
		for(ll=0; ll<nbu; ll++) // this has to be strictly positive !!!
			{
			t[N][ll]       = 1.0;
			lam[N][ll]     = 1.0;
			t[N][pnb+ll]   = 1.0;
			lam[N][pnb+ll] = 1.0;
			}
		for(ll=nu; ll<nb; ll++)
			{
			t[N][ll]     = - db[N][ll]     + ux[N][ll];
			t[N][pnb+ll] = - db[N][pnb+ll] - ux[N][ll];
			if(t[N][ll] < thr0)
				{
				if(t[N][pnb+ll] < thr0)
					{
					ux[N][ll] = ( - db[N][pnb+ll] + db[N][ll])*0.5;
					t[N][ll]     = - db[N][ll]     + ux[N][ll];
					t[N][pnb+ll] = - db[N][pnb+ll] - ux[N][ll];
					}
				else
					{
					t[N][ll] = thr0;
					ux[N][ll] = db[N][ll] + thr0;
					}
				}
			else if(t[N][pnb+ll] < thr0)
				{
				t[N][pnb+ll] = thr0;
				ux[N][ll] = - db[N][pnb+ll] - thr0;
				}
			lam[N][ll] = mu0/t[N][ll];
			lam[N][pnb+ll] = mu0/t[N][pnb+ll];
			}
		}
	else // cold start
		{
		for(ll=0; ll<nbu; ll++)
			{
			ux[0][ll] = 0.0;
			//ux[0][ll] = 0.5*( - db[0][pnb+ll] + db[0][ll] );
/*			t[0][ll] = 1.0;*/
/*			t[0][pnb+ll] = 1.0;*/
			t[0][ll]     = - db[0][ll]     + ux[0][ll];
			t[0][pnb+ll] = - db[0][pnb+ll] - ux[0][ll];
			if(t[0][ll] < thr0)
				{
				if(t[0][pnb+ll] < thr0)
					{
					ux[0][ll] = ( - db[0][pnb+ll] + db[0][ll])*0.5;
					t[0][ll]     = - db[0][ll]     + ux[0][ll];
					t[0][pnb+ll] = - db[0][pnb+ll] - ux[0][ll];
					}
				else
					{
					t[0][ll] = thr0;
					ux[0][ll] = db[0][ll] + thr0;
					}
				}
			else if(t[0][pnb+ll] < thr0)
				{
				t[0][pnb+ll] = thr0;
				ux[0][ll] = - db[0][pnb+ll] - thr0;
				}
			lam[0][ll] = mu0/t[0][ll];
			lam[0][pnb+ll] = mu0/t[0][pnb+ll];
			}
		for(ii=ll; ii<nu; ii++)
			ux[0][ii] = 0.0; // initialize remaining components of u to zero
		for(; ll<nb; ll++)
			{
			t[0][ll]       = 1.0; // this has to be strictly positive !!!
			lam[0][ll]     = 1.0; // this has to be strictly positive !!!
			t[0][pnb+ll]   = 1.0; // this has to be strictly positive !!!
			lam[0][pnb+ll] = 1.0; // this has to be strictly positive !!!
			}
		for(jj=1; jj<N; jj++)
			{
			for(ll=0; ll<nb; ll++)
				{
				ux[jj][ll] = 0.0;
				//ux[jj][ll] = 0.5*( - db[jj][pnb+ll] + db[jj][ll] );
/*				t[jj][ll] = 1.0;*/
/*				t[jj][pnb+ll] = 1.0;*/
				t[jj][ll]     = - db[jj][ll]     + ux[jj][ll];
				t[jj][pnb+ll] = - db[jj][pnb+ll] - ux[jj][ll];
				if(t[jj][ll] < thr0)
					{
					if(t[jj][pnb+ll] < thr0)
						{
						ux[jj][ll] = ( - db[jj][pnb+ll] + db[jj][ll])*0.5;
						t[jj][ll]     = - db[jj][ll]     + ux[jj][ll];
						t[jj][pnb+ll] = - db[jj][pnb+ll] - ux[jj][ll];
						}
					else
						{
						t[jj][ll] = thr0;
						ux[jj][ll] = db[jj][ll] + thr0;
						}
					}
				else if(t[jj][pnb+ll] < thr0)
					{
					t[jj][pnb+ll] = thr0;
					ux[jj][ll] = - db[jj][pnb+ll] - thr0;
					}
				lam[jj][ll] = mu0/t[jj][ll];
				lam[jj][pnb+ll] = mu0/t[jj][pnb+ll];
				}
			for(ii=ll; ii<nx+nu; ii++)
				ux[jj][ii] = 0.0; // initialize remaining components of u and x to zero
			}
		for(ll=0; ll<nbu; ll++)
			{
			t[N][ll]       = 1.0; // this has to be strictly positive !!!
			lam[N][ll]     = 1.0; // this has to be strictly positive !!!
			t[N][pnb+ll]   = 1.0; // this has to be strictly positive !!!
			lam[N][pnb+ll] = 1.0; // this has to be strictly positive !!!
			}
		for(ll=nu; ll<nb; ll++)
			{
			ux[N][ll] = 0.0;
			//ux[N][ll] = 0.5*( - db[N][pnb+ll] + db[N][ll] );
/*			t[N][ll] = 1.0;*/
/*			t[N][pnb+ll] = 1.0;*/
			t[N][ll]     = - db[N][ll]     + ux[N][ll];
			t[N][pnb+ll] = - db[N][pnb+ll] - ux[N][ll];
			if(t[N][ll] < thr0)
				{
				if(t[N][pnb+ll] < thr0)
					{
					ux[N][ll] = ( - db[N][pnb+ll] + db[N][ll])*0.5;
					t[N][ll]     = - db[N][ll]     + ux[N][ll];
					t[N][pnb+ll] = - db[N][pnb+ll] - ux[N][ll];
					}
				else
					{
					t[N][ll] = thr0;
					ux[N][ll] = db[N][ll] + thr0;
					}
				}
			else if(t[N][pnb+ll] < thr0)
				{
				t[N][pnb+ll] = thr0;
				ux[N][ll] = - db[N][pnb+ll] - thr0;
				}
			lam[N][ll] = mu0/t[N][ll];
			lam[N][pnb+ll] = mu0/t[N][pnb+ll];
			}
		for(ii=ll; ii<nx+nu; ii++)
			ux[N][ii] = 0.0; // initialize remaining components of x to zero

		// initialize pi
		for(jj=0; jj<=N; jj++)
			for(ll=0; ll<nx; ll++)
				pi[jj][ll] = 0.0; // initialize multipliers to zero

		// TODO find a better way to initialize general constraints
		if(ng>0)
			{
			for(jj=0; jj<=N; jj++)
				for(ll=0; ll<ng; ll++)
					{
					t[jj][2*pnb+ll] = 1.0;
					lam[jj][2*pnb+ll] = mu0; // /t[jj][pnb+ll];
					t[jj][2*pnb+png+ll] = 1.0;
					lam[jj][2*pnb+png+ll] = mu0; // /t[jj][pnb+ll];
					}
			}

		}
	}



void d_init_var_soft_mpc(int N, int nx, int nu, int nh, int ns, double **ux, double **pi, double **db, double **t, double **lam, double mu0, int warm_start)
	{

	int nb = nh + ns;
	
	const int nhu = nu<nh ? nu : nh ;

	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	const int anb = nal*((2*nb+nal-1)/nal); // cache aligned number of box and soft constraints // !!!!! doubled to include soft constraints !!!!!

	int jj, ll, ii;
	
	double thr0 = 0.1; // minimum distance from a constraint

	// warm start: user-provided guess as starting point
	if(warm_start==1)
		{
		// first stage
		for(ll=0; ll<2*nhu; ll+=2)
			{
			t[0][ll+0] =   ux[0][ll/2] - db[0][ll+0];
			t[0][ll+1] = - db[0][ll+1] - ux[0][ll/2];
			if(t[0][ll+0] < thr0)
				{
				if(t[0][ll+1] < thr0)
					{
					ux[0][ll/2] = ( - db[0][ll+1] + db[0][ll+0])*0.5;
					t[0][ll+0] =   ux[0][ll/2] - db[0][ll+0];
					t[0][ll+1] = - db[0][ll+1] - ux[0][ll/2];
					}
				else
					{
					t[0][ll+0] = thr0;
					ux[0][ll/2] = db[0][ll+0] + thr0;
					}
				}
			else if(t[0][ll+1] < thr0)
				{
				t[0][ll+1] = thr0;
				ux[0][ll/2] = - db[0][ll+1] - thr0;
				}

			lam[0][ll+0] = mu0/t[0][ll+0];
			lam[0][ll+1] = mu0/t[0][ll+1];
			}
		for(; ll<2*nb; ll++)
			{
			t[0][ll] = 1.0; // this has to be strictly positive !!!
			lam[0][ll] = 1.0;
			}
		// middle stages
		for(jj=1; jj<N; jj++)
			{
			for(ll=0; ll<2*nb; ll+=2)
				{
				t[jj][ll+0] =   ux[jj][ll/2] - db[jj][ll+0];
				t[jj][ll+1] = - db[jj][ll+1] - ux[jj][ll/2];
				if(t[jj][ll+0] < thr0)
					{
					if(t[jj][ll+1] < thr0)
						{
						ux[jj][ll/2] = ( - db[jj][ll+1] + db[jj][ll+0])*0.5;
						t[jj][ll+0] =   ux[jj][ll/2] - db[jj][ll+0];
						t[jj][ll+1] = - db[jj][ll+1] - ux[jj][ll/2];
						}
					else
						{
						t[jj][ll+0] = thr0;
						ux[jj][ll/2] = db[jj][ll+0] + thr0;
						}
					}
				else if(t[jj][ll+1] < thr0)
					{
					t[jj][ll+1] = thr0;
					ux[jj][ll/2] = - db[jj][ll+1] - thr0;
					}
				lam[jj][ll+0] = mu0/t[jj][ll+0];
				lam[jj][ll+1] = mu0/t[jj][ll+1];
				}
			}
		// last stage
		for(ll=0; ll<2*nhu; ll++) // this has to be strictly positive !!!
			{
			t[N][ll] = 1.0;
			lam[N][ll] = 1.0;
			}
		for(ll=2*nu; ll<2*nb; ll+=2)
			{
			t[N][ll+0] =   ux[N][ll/2] - db[N][ll+0];
			t[N][ll+1] = - db[N][ll+1] - ux[N][ll/2];
			if(t[N][ll+0] < thr0)
				{
				if(t[N][ll+1] < thr0)
					{
					ux[N][ll/2] = ( - db[N][ll+1] + db[N][ll+0])*0.5;
					t[N][ll+0] =   ux[N][ll/2] - db[N][ll+0];
					t[N][ll+1] = - db[N][ll+1] - ux[N][ll/2];
					}
				else
					{
					t[N][ll+0] = thr0;
					ux[N][ll/2] = db[N][ll+0] + thr0;
					}
				}
			else if(t[N][ll+1] < thr0)
				{
				t[N][ll+1] = thr0;
				ux[N][ll/2] = - db[N][ll+1] - thr0;
				}
			lam[N][ll+0] = mu0/t[N][ll+0];
			lam[N][ll+1] = mu0/t[N][ll+1];
			}

		}
	else // cold start
		{
		// first stage
		for(ll=0; ll<2*nhu; ll+=2)
			{
			ux[0][ll/2] = 0.0;
/*			t[0][ll+0] = 1.0;*/
/*			t[0][ll+1] = 1.0;*/
			t[0][ll+0] =   ux[0][ll/2] - db[0][ll+0];
			t[0][ll+1] = - db[0][ll+1] - ux[0][ll/2];
			if(t[0][ll+0] < thr0)
				{
				if(t[0][ll+1] < thr0)
					{
					ux[0][ll/2] = ( - db[0][ll+1] + db[0][ll+0])*0.5;
					t[0][ll+0] =   ux[0][ll/2] - db[0][ll+0];
					t[0][ll+1] = - db[0][ll+1] - ux[0][ll/2];
					}
				else
					{
					t[0][ll+0] = thr0;
					ux[0][ll/2] = db[0][ll+0] + thr0;
					}
				}
			else if(t[0][ll+1] < thr0)
				{
				t[0][ll+1] = thr0;
				ux[0][ll/2] = - db[0][ll+1] - thr0;
				}
			lam[0][ll+0] = mu0/t[0][ll+0];
			lam[0][ll+1] = mu0/t[0][ll+1];
			}
		for(ii=ll/2; ii<nu; ii++)
			ux[0][ii] = 0.0; // initialize remaining components of u to zero
		for(; ll<2*nb; ll++)
			{
			t[0][ll] = 1.0; // this has to be strictly positive !!!
			lam[0][ll] = 1.0;
			}
		// middle stages
		for(jj=1; jj<N; jj++)
			{
			for(ll=0; ll<2*nb; ll+=2)
				{
				ux[jj][ll/2] = 0.0;
/*				t[jj][ll+0] = 1.0;*/
/*				t[jj][ll+1] = 1.0;*/
				t[jj][ll+0] =   ux[jj][ll/2] - db[jj][ll+0];
				t[jj][ll+1] = - db[jj][ll+1] - ux[jj][ll/2];
				if(t[jj][ll+0] < thr0)
					{
					if(t[jj][ll+1] < thr0)
						{
						ux[jj][ll/2] = ( - db[jj][ll+1] + db[jj][ll+0])*0.5;
						t[jj][ll+0] =   ux[jj][ll/2] - db[jj][ll+0];
						t[jj][ll+1] = - db[jj][ll+1] - ux[jj][ll/2];
						}
					else
						{
						t[jj][ll+0] = thr0;
						ux[jj][ll/2] = db[jj][ll+0] + thr0;
						}
					}
				else if(t[jj][ll+1] < thr0)
					{
					t[jj][ll+1] = thr0;
					ux[jj][ll/2] = - db[jj][ll+1] - thr0;
					}
				lam[jj][ll+0] = mu0/t[jj][ll+0];
				lam[jj][ll+1] = mu0/t[jj][ll+1];
				}
			for(ii=ll/2; ii<nx+nu; ii++)
				ux[jj][ii] = 0.0; // initialize remaining components of u and x to zero
			}
		// last stage
		for(ll=0; ll<2*nhu; ll++)
			{
			t[N][ll] = 1.0; // this has to be strictly positive !!!
			lam[N][ll] = 1.0;
			}
		for(ll=2*nu; ll<2*nb; ll+=2)
			{
			ux[N][ll/2] = 0.0;
/*			t[N][ll+0] = 1.0;*/
/*			t[N][ll+1] = 1.0;*/
			t[N][ll+0] =   ux[N][ll/2] - db[N][ll+0];
			t[N][ll+1] = - db[N][ll+1] - ux[N][ll/2];
			if(t[N][ll+0] < thr0)
				{
				if(t[N][ll+1] < thr0)
					{
					ux[N][ll/2] = ( - db[N][ll+1] + db[N][ll+0])*0.5;
					t[N][ll+0] =   ux[N][ll/2] - db[N][ll+0];
					t[N][ll+1] = - db[N][ll+1] - ux[N][ll/2];
					}
				else
					{
					t[N][ll+0] = thr0;
					ux[N][ll/2] = db[N][ll+0] + thr0;
					}
				}
			else if(t[N][ll+1] < thr0)
				{
				t[N][ll+1] = thr0;
				ux[N][ll/2] = - db[N][ll+1] - thr0;
				}
			lam[N][ll+0] = mu0/t[N][ll+0];
			lam[N][ll+1] = mu0/t[N][ll+1];
			}
		for(ii=ll/2; ii<nx+nu; ii++)
			ux[N][ii] = 0.0; // initialize remaining components of x to zero

		// inizialize t_theta and lam_theta (cold start only for the moment)
		for(jj=0; jj<=N; jj++)
			for(ll=2*nh; ll<2*nb; ll++)
				{
				t[jj][anb+ll] = 1.0;
				lam[jj][anb+ll] = mu0; // /t[jj][anb+ll]; // TODO restore division if needed
				}

		// initialize pi
		for(jj=0; jj<=N; jj++)
			for(ll=0; ll<nx; ll++)
				pi[jj][ll] = 0.0; // initialize multipliers to zero

		}
	
	}



void d_update_hessian_hard_mpc(int N, int nx, int nu, int nb, int ng, int cnz, double sigma_mu, double **t, double **t_inv, double **lam, double **lamt, double **dlam, double **qx, double **bd, double **bl, double **pd, double **pl, double **db)

/*void d_update_hessian_box(int k0, int kmax, int nb, int cnz, double sigma_mu, double *t, double *lam, double *lamt, double *dlam, double *bd, double *bl, double *pd, double *pl, double *lb, double *ub)*/
	{
	// TODO remove k1 !!! and use nx nu nb instead of k0 k1 kmax
	
	const int nbu = nu<nb ? nu : nb ;

	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	//const int anb = nal*((nb+nal-1)/nal); // cache aligned number of box and soft constraints
	const int pnb = bs*((nb+bs-1)/bs); // simd aligned number of box constraints
	const int png = bs*((ng+bs-1)/bs); // simd aligned number of general constraints

	//const int k0 = nbu;
	//const int k1 = (nu/bs)*bs;
	//const int kmax = nb;
	
	double temp0, temp1;
	
	double 
		*ptr_pd, *ptr_pl, *ptr_bd, *ptr_bl, *ptr_db, *ptr_qx,
		*ptr_t, *ptr_lam, *ptr_lamt, *ptr_dlam, *ptr_tinv;
	
	int ii, jj, bs0;
	
	// first stage
	jj = 0;
	
	ptr_t     = t[jj];
	ptr_lam   = lam[jj];
	ptr_lamt  = lamt[jj];
	ptr_dlam  = dlam[jj];
	ptr_tinv  = t_inv[jj];
	ptr_db    = db[jj];
	ptr_bd    = bd[jj];
	ptr_bl    = bl[jj];
	ptr_pd    = pd[jj];
	ptr_pl    = pl[jj];

	ii = 0;
	for(; ii<nbu-3; ii+=4)
		{

		ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
		ptr_tinv[ii+pnb+0] = 1.0/ptr_t[ii+pnb+0];
		ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
		ptr_lamt[ii+pnb+0] = ptr_lam[ii+pnb+0]*ptr_tinv[ii+pnb+0];
		ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
		ptr_dlam[ii+pnb+0] = ptr_tinv[ii+pnb+0]*sigma_mu; // !!!!!
		ptr_pd[ii+0] = ptr_bd[ii+0] + ptr_lamt[ii+0] + ptr_lamt[ii+pnb+0];
		ptr_pl[ii+0] = ptr_bl[ii+0] + ptr_lam[ii+pnb+0] + ptr_lamt[ii+pnb+0]*ptr_db[ii+pnb+0] + ptr_dlam[ii+pnb+0] - ptr_lam[ii+0] - ptr_lamt[ii+0]*ptr_db[ii+0] - ptr_dlam[ii+0];

		ptr_tinv[ii+1] = 1.0/ptr_t[ii+1];
		ptr_tinv[ii+pnb+1] = 1.0/ptr_t[ii+pnb+1];
		ptr_lamt[ii+1] = ptr_lam[ii+1]*ptr_tinv[ii+1];
		ptr_lamt[ii+pnb+1] = ptr_lam[ii+pnb+1]*ptr_tinv[ii+pnb+1];
		ptr_dlam[ii+1] = ptr_tinv[ii+1]*sigma_mu; // !!!!!
		ptr_dlam[ii+pnb+1] = ptr_tinv[ii+pnb+1]*sigma_mu; // !!!!!
		ptr_pd[ii+1] = ptr_bd[ii+1] + ptr_lamt[ii+1] + ptr_lamt[ii+pnb+1];
		ptr_pl[ii+1] = ptr_bl[ii+1] + ptr_lam[ii+pnb+1] + ptr_lamt[ii+pnb+1]*ptr_db[ii+pnb+1] + ptr_dlam[ii+pnb+1] - ptr_lam[ii+1] - ptr_lamt[ii+1]*ptr_db[ii+1] - ptr_dlam[ii+1];

		ptr_tinv[ii+2] = 1.0/ptr_t[ii+2];
		ptr_tinv[ii+pnb+2] = 1.0/ptr_t[ii+pnb+2];
		ptr_lamt[ii+2] = ptr_lam[ii+2]*ptr_tinv[ii+2];
		ptr_lamt[ii+pnb+2] = ptr_lam[ii+pnb+2]*ptr_tinv[ii+pnb+2];
		ptr_dlam[ii+2] = ptr_tinv[ii+2]*sigma_mu; // !!!!!
		ptr_dlam[ii+pnb+2] = ptr_tinv[ii+pnb+2]*sigma_mu; // !!!!!
		ptr_pd[ii+2] = ptr_bd[ii+2] + ptr_lamt[ii+2] + ptr_lamt[ii+pnb+2];
		ptr_pl[ii+2] = ptr_bl[ii+2] + ptr_lam[ii+pnb+2] + ptr_lamt[ii+pnb+2]*ptr_db[ii+pnb+2] + ptr_dlam[ii+pnb+2] - ptr_lam[ii+2] - ptr_lamt[ii+2]*ptr_db[ii+2] - ptr_dlam[ii+2];

		ptr_tinv[ii+3] = 1.0/ptr_t[ii+3];
		ptr_tinv[ii+pnb+3] = 1.0/ptr_t[ii+pnb+3];
		ptr_lamt[ii+3] = ptr_lam[ii+3]*ptr_tinv[ii+3];
		ptr_lamt[ii+pnb+3] = ptr_lam[ii+pnb+3]*ptr_tinv[ii+pnb+3];
		ptr_dlam[ii+3] = ptr_tinv[ii+3]*sigma_mu; // !!!!!
		ptr_dlam[ii+pnb+3] = ptr_tinv[ii+pnb+3]*sigma_mu; // !!!!!
		ptr_pd[ii+3] = ptr_bd[ii+3] + ptr_lamt[ii+3] + ptr_lamt[ii+pnb+3];
		ptr_pl[ii+3] = ptr_bl[ii+3] + ptr_lam[ii+pnb+3] + ptr_lamt[ii+pnb+3]*ptr_db[ii+pnb+3] + ptr_dlam[ii+pnb+3] - ptr_lam[ii+3] - ptr_lamt[ii+3]*ptr_db[ii+3] - ptr_dlam[ii+3];

		}
	for(; ii<nbu; ii++)
		{

		ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
		ptr_tinv[ii+pnb+0] = 1.0/ptr_t[ii+pnb+0];
		ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
		ptr_lamt[ii+pnb+0] = ptr_lam[ii+pnb+0]*ptr_tinv[ii+pnb+0];
		ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
		ptr_dlam[ii+pnb+0] = ptr_tinv[ii+pnb+0]*sigma_mu; // !!!!!
		ptr_pd[ii] = ptr_bd[ii] + ptr_lamt[ii+0] + ptr_lamt[ii+pnb+0];
		ptr_pl[ii] = ptr_bl[ii] + ptr_lam[ii+pnb+0] + ptr_lamt[ii+pnb+0]*ptr_db[ii+pnb+0] + ptr_dlam[ii+pnb+0] - ptr_lam[ii+0] - ptr_lamt[ii+0]*ptr_db[ii+0] - ptr_dlam[ii+0];

		}
	for( ; ii<nu; ii++)
		{
		ptr_pd[ii] = ptr_bd[ii];
		ptr_pl[ii] = ptr_bl[ii];
		}
	if(ng>0) // there are general constraints
		{

		ptr_qx    = qx[jj];

		ii = 2*pnb;
		for(; ii<2*pnb+ng-3; ii+=4)
			{

			ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
			ptr_tinv[ii+png+0] = 1.0/ptr_t[ii+png+0];
			ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
			ptr_lamt[ii+png+0] = ptr_lam[ii+png+0]*ptr_tinv[ii+png+0];
			ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
			ptr_dlam[ii+png+0] = ptr_tinv[ii+png+0]*sigma_mu; // !!!!!
			ptr_qx[ii+0] =  ptr_lam[ii+0] + ptr_lamt[ii+0]*ptr_db[ii+0] + ptr_dlam[ii+0];
			ptr_qx[ii+png+0] = ptr_lam[ii+png+0] + ptr_lamt[ii+png+0]*ptr_db[ii+png+0] + ptr_dlam[ii+png+0];

			ptr_tinv[ii+1] = 1.0/ptr_t[ii+1];
			ptr_tinv[ii+png+1] = 1.0/ptr_t[ii+png+1];
			ptr_lamt[ii+1] = ptr_lam[ii+1]*ptr_tinv[ii+1];
			ptr_lamt[ii+png+1] = ptr_lam[ii+png+1]*ptr_tinv[ii+png+1];
			ptr_dlam[ii+1] = ptr_tinv[ii+1]*sigma_mu; // !!!!!
			ptr_dlam[ii+png+1] = ptr_tinv[ii+png+1]*sigma_mu; // !!!!!
			ptr_qx[ii+1] =  ptr_lam[ii+1] + ptr_lamt[ii+1]*ptr_db[ii+1] + ptr_dlam[ii+1];
			ptr_qx[ii+png+1] = ptr_lam[ii+png+1] + ptr_lamt[ii+png+1]*ptr_db[ii+png+1] + ptr_dlam[ii+png+1];

			ptr_tinv[ii+2] = 1.0/ptr_t[ii+2];
			ptr_tinv[ii+png+2] = 1.0/ptr_t[ii+png+2];
			ptr_lamt[ii+2] = ptr_lam[ii+2]*ptr_tinv[ii+2];
			ptr_lamt[ii+png+2] = ptr_lam[ii+png+2]*ptr_tinv[ii+png+2];
			ptr_dlam[ii+2] = ptr_tinv[ii+2]*sigma_mu; // !!!!!
			ptr_dlam[ii+png+2] = ptr_tinv[ii+png+2]*sigma_mu; // !!!!!
			ptr_qx[ii+2] =  ptr_lam[ii+2] + ptr_lamt[ii+2]*ptr_db[ii+2] + ptr_dlam[ii+2];
			ptr_qx[ii+png+2] = ptr_lam[ii+png+2] + ptr_lamt[ii+png+2]*ptr_db[ii+png+2] + ptr_dlam[ii+png+2];

			ptr_tinv[ii+3] = 1.0/ptr_t[ii+3];
			ptr_tinv[ii+png+3] = 1.0/ptr_t[ii+png+3];
			ptr_lamt[ii+3] = ptr_lam[ii+3]*ptr_tinv[ii+3];
			ptr_lamt[ii+png+3] = ptr_lam[ii+png+3]*ptr_tinv[ii+png+3];
			ptr_dlam[ii+3] = ptr_tinv[ii+3]*sigma_mu; // !!!!!
			ptr_dlam[ii+png+3] = ptr_tinv[ii+png+3]*sigma_mu; // !!!!!
			ptr_qx[ii+3] =  ptr_lam[ii+3] + ptr_lamt[ii+3]*ptr_db[ii+3] + ptr_dlam[ii+3];
			ptr_qx[ii+png+3] = ptr_lam[ii+png+3] + ptr_lamt[ii+png+3]*ptr_db[ii+png+3] + ptr_dlam[ii+png+3];

			}
		for(; ii<2*pnb+ng-3; ii+=4)
			{

			ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
			ptr_tinv[ii+png+0] = 1.0/ptr_t[ii+png+0];
			ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
			ptr_lamt[ii+png+0] = ptr_lam[ii+png+0]*ptr_tinv[ii+png+0];
			ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
			ptr_dlam[ii+png+0] = ptr_tinv[ii+png+0]*sigma_mu; // !!!!!
			ptr_qx[ii+0] =  ptr_lam[ii+0] + ptr_lamt[ii+0]*ptr_db[ii+0] + ptr_dlam[ii+0];
			ptr_qx[ii+png+0] = ptr_lam[ii+png+0] + ptr_lamt[ii+png+0]*ptr_db[ii+png+0] + ptr_dlam[ii+png+0];

			}

		}


	// middle stages

	for(jj=1; jj<N; jj++)
		{
		
		ptr_t     = t[jj];
		ptr_lam   = lam[jj];
		ptr_lamt  = lamt[jj];
		ptr_dlam  = dlam[jj];
		ptr_tinv  = t_inv[jj];
		ptr_db    = db[jj];
		ptr_bd    = bd[jj];
		ptr_bl    = bl[jj];
		ptr_pd    = pd[jj];
		ptr_pl    = pl[jj];

		ii = 0;
		for(; ii<nb-3; ii+=4)
			{

			ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
			ptr_tinv[ii+pnb+0] = 1.0/ptr_t[ii+pnb+0];
			ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
			ptr_lamt[ii+pnb+0] = ptr_lam[ii+pnb+0]*ptr_tinv[ii+pnb+0];
			ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
			ptr_dlam[ii+pnb+0] = ptr_tinv[ii+pnb+0]*sigma_mu; // !!!!!
			ptr_pd[ii+0] = ptr_bd[ii+0] + ptr_lamt[ii+0] + ptr_lamt[ii+pnb+0];
			ptr_pl[ii+0] = ptr_bl[ii+0] + ptr_lam[ii+pnb+0] + ptr_lamt[ii+pnb+0]*ptr_db[ii+pnb+0] + ptr_dlam[ii+pnb+0] - ptr_lam[ii+0] - ptr_lamt[ii+0]*ptr_db[ii+0] - ptr_dlam[ii+0];

			ptr_tinv[ii+1] = 1.0/ptr_t[ii+1];
			ptr_tinv[ii+pnb+1] = 1.0/ptr_t[ii+pnb+1];
			ptr_lamt[ii+1] = ptr_lam[ii+1]*ptr_tinv[ii+1];
			ptr_lamt[ii+pnb+1] = ptr_lam[ii+pnb+1]*ptr_tinv[ii+pnb+1];
			ptr_dlam[ii+1] = ptr_tinv[ii+1]*sigma_mu; // !!!!!
			ptr_dlam[ii+pnb+1] = ptr_tinv[ii+pnb+1]*sigma_mu; // !!!!!
			ptr_pd[ii+1] = ptr_bd[ii+1] + ptr_lamt[ii+1] + ptr_lamt[ii+pnb+1];
			ptr_pl[ii+1] = ptr_bl[ii+1] + ptr_lam[ii+pnb+1] + ptr_lamt[ii+pnb+1]*ptr_db[ii+pnb+1] + ptr_dlam[ii+pnb+1] - ptr_lam[ii+1] - ptr_lamt[ii+1]*ptr_db[ii+1] - ptr_dlam[ii+1];

			ptr_tinv[ii+2] = 1.0/ptr_t[ii+2];
			ptr_tinv[ii+pnb+2] = 1.0/ptr_t[ii+pnb+2];
			ptr_lamt[ii+2] = ptr_lam[ii+2]*ptr_tinv[ii+2];
			ptr_lamt[ii+pnb+2] = ptr_lam[ii+pnb+2]*ptr_tinv[ii+pnb+2];
			ptr_dlam[ii+2] = ptr_tinv[ii+2]*sigma_mu; // !!!!!
			ptr_dlam[ii+pnb+2] = ptr_tinv[ii+pnb+2]*sigma_mu; // !!!!!
			ptr_pd[ii+2] = ptr_bd[ii+2] + ptr_lamt[ii+2] + ptr_lamt[ii+pnb+2];
			ptr_pl[ii+2] = ptr_bl[ii+2] + ptr_lam[ii+pnb+2] + ptr_lamt[ii+pnb+2]*ptr_db[ii+pnb+2] + ptr_dlam[ii+pnb+2] - ptr_lam[ii+2] - ptr_lamt[ii+2]*ptr_db[ii+2] - ptr_dlam[ii+2];

			ptr_tinv[ii+3] = 1.0/ptr_t[ii+3];
			ptr_tinv[ii+pnb+3] = 1.0/ptr_t[ii+pnb+3];
			ptr_lamt[ii+3] = ptr_lam[ii+3]*ptr_tinv[ii+3];
			ptr_lamt[ii+pnb+3] = ptr_lam[ii+pnb+3]*ptr_tinv[ii+pnb+3];
			ptr_dlam[ii+3] = ptr_tinv[ii+3]*sigma_mu; // !!!!!
			ptr_dlam[ii+pnb+3] = ptr_tinv[ii+pnb+3]*sigma_mu; // !!!!!
			ptr_pd[ii+3] = ptr_bd[ii+3] + ptr_lamt[ii+3] + ptr_lamt[ii+pnb+3];
			ptr_pl[ii+3] = ptr_bl[ii+3] + ptr_lam[ii+pnb+3] + ptr_lamt[ii+pnb+3]*ptr_db[ii+pnb+3] + ptr_dlam[ii+pnb+3] - ptr_lam[ii+3] - ptr_lamt[ii+3]*ptr_db[ii+3] - ptr_dlam[ii+3];

			}
		for(; ii<nb; ii++)
			{

			ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
			ptr_tinv[ii+pnb+0] = 1.0/ptr_t[ii+pnb+0];
			ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
			ptr_lamt[ii+pnb+0] = ptr_lam[ii+pnb+0]*ptr_tinv[ii+pnb+0];
			ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
			ptr_dlam[ii+pnb+0] = ptr_tinv[ii+pnb+0]*sigma_mu; // !!!!!
			ptr_pd[ii] = ptr_bd[ii] + ptr_lamt[ii+0] + ptr_lamt[ii+pnb+0];
			ptr_pl[ii] = ptr_bl[ii] + ptr_lam[ii+pnb+0] + ptr_lamt[ii+pnb+0]*ptr_db[ii+pnb+0] + ptr_dlam[ii+pnb+0] - ptr_lam[ii+0] - ptr_lamt[ii+0]*ptr_db[ii+0] - ptr_dlam[ii+0];

			}
		for( ; ii<nu+nx; ii++)
			{
			ptr_pd[ii] = ptr_bd[ii];
			ptr_pl[ii] = ptr_bl[ii];
			}
		if(ng>0) // there are general constraints
			{

			ptr_qx    = qx[jj];

			ii = 2*pnb;
			for(; ii<2*pnb+ng-3; ii+=4)
				{

				ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
				ptr_tinv[ii+png+0] = 1.0/ptr_t[ii+png+0];
				ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
				ptr_lamt[ii+png+0] = ptr_lam[ii+png+0]*ptr_tinv[ii+png+0];
				ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
				ptr_dlam[ii+png+0] = ptr_tinv[ii+png+0]*sigma_mu; // !!!!!
				ptr_qx[ii+0] =  ptr_lam[ii+0] + ptr_lamt[ii+0]*ptr_db[ii+0] + ptr_dlam[ii+0];
				ptr_qx[ii+png+0] = ptr_lam[ii+png+0] + ptr_lamt[ii+png+0]*ptr_db[ii+png+0] + ptr_dlam[ii+png+0];

				ptr_tinv[ii+1] = 1.0/ptr_t[ii+1];
				ptr_tinv[ii+png+1] = 1.0/ptr_t[ii+png+1];
				ptr_lamt[ii+1] = ptr_lam[ii+1]*ptr_tinv[ii+1];
				ptr_lamt[ii+png+1] = ptr_lam[ii+png+1]*ptr_tinv[ii+png+1];
				ptr_dlam[ii+1] = ptr_tinv[ii+1]*sigma_mu; // !!!!!
				ptr_dlam[ii+png+1] = ptr_tinv[ii+png+1]*sigma_mu; // !!!!!
				ptr_qx[ii+1] =  ptr_lam[ii+1] + ptr_lamt[ii+1]*ptr_db[ii+1] + ptr_dlam[ii+1];
				ptr_qx[ii+png+1] = ptr_lam[ii+png+1] + ptr_lamt[ii+png+1]*ptr_db[ii+png+1] + ptr_dlam[ii+png+1];

				ptr_tinv[ii+2] = 1.0/ptr_t[ii+2];
				ptr_tinv[ii+png+2] = 1.0/ptr_t[ii+png+2];
				ptr_lamt[ii+2] = ptr_lam[ii+2]*ptr_tinv[ii+2];
				ptr_lamt[ii+png+2] = ptr_lam[ii+png+2]*ptr_tinv[ii+png+2];
				ptr_dlam[ii+2] = ptr_tinv[ii+2]*sigma_mu; // !!!!!
				ptr_dlam[ii+png+2] = ptr_tinv[ii+png+2]*sigma_mu; // !!!!!
				ptr_qx[ii+2] =  ptr_lam[ii+2] + ptr_lamt[ii+2]*ptr_db[ii+2] + ptr_dlam[ii+2];
				ptr_qx[ii+png+2] = ptr_lam[ii+png+2] + ptr_lamt[ii+png+2]*ptr_db[ii+png+2] + ptr_dlam[ii+png+2];

				ptr_tinv[ii+3] = 1.0/ptr_t[ii+3];
				ptr_tinv[ii+png+3] = 1.0/ptr_t[ii+png+3];
				ptr_lamt[ii+3] = ptr_lam[ii+3]*ptr_tinv[ii+3];
				ptr_lamt[ii+png+3] = ptr_lam[ii+png+3]*ptr_tinv[ii+png+3];
				ptr_dlam[ii+3] = ptr_tinv[ii+3]*sigma_mu; // !!!!!
				ptr_dlam[ii+png+3] = ptr_tinv[ii+png+3]*sigma_mu; // !!!!!
				ptr_qx[ii+3] =  ptr_lam[ii+3] + ptr_lamt[ii+3]*ptr_db[ii+3] + ptr_dlam[ii+3];
				ptr_qx[ii+png+3] = ptr_lam[ii+png+3] + ptr_lamt[ii+png+3]*ptr_db[ii+png+3] + ptr_dlam[ii+png+3];

				}
			for(; ii<2*pnb+ng-3; ii+=4)
				{

				ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
				ptr_tinv[ii+png+0] = 1.0/ptr_t[ii+png+0];
				ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
				ptr_lamt[ii+png+0] = ptr_lam[ii+png+0]*ptr_tinv[ii+png+0];
				ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
				ptr_dlam[ii+png+0] = ptr_tinv[ii+png+0]*sigma_mu; // !!!!!
				ptr_qx[ii+0] =  ptr_lam[ii+0] + ptr_lamt[ii+0]*ptr_db[ii+0] + ptr_dlam[ii+0];
				ptr_qx[ii+png+0] = ptr_lam[ii+png+0] + ptr_lamt[ii+png+0]*ptr_db[ii+png+0] + ptr_dlam[ii+png+0];

				}

			}
	
		}

	// last stage
	jj = N;

	ptr_t     = t[jj];
	ptr_lam   = lam[jj];
	ptr_lamt  = lamt[jj];
	ptr_dlam  = dlam[jj];
	ptr_tinv  = t_inv[jj];
	ptr_db    = db[jj];
	ptr_bd    = bd[jj];
	ptr_bl    = bl[jj];
	ptr_pd    = pd[jj];
	ptr_pl    = pl[jj];

	ii=nu;
	for(; ii<nb-3; ii+=4)
		{

		ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
		ptr_tinv[ii+pnb+0] = 1.0/ptr_t[ii+pnb+0];
		ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
		ptr_lamt[ii+pnb+0] = ptr_lam[ii+pnb+0]*ptr_tinv[ii+pnb+0];
		ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
		ptr_dlam[ii+pnb+0] = ptr_tinv[ii+pnb+0]*sigma_mu; // !!!!!
		ptr_pd[ii+0] = ptr_bd[ii+0] + ptr_lamt[ii+0] + ptr_lamt[ii+pnb+0];
		ptr_pl[ii+0] = ptr_bl[ii+0] + ptr_lam[ii+pnb+0] + ptr_lamt[ii+pnb+0]*ptr_db[ii+pnb+0] + ptr_dlam[ii+pnb+0] - ptr_lam[ii+0] - ptr_lamt[ii+0]*ptr_db[ii+0] - ptr_dlam[ii+0];

		ptr_tinv[ii+1] = 1.0/ptr_t[ii+1];
		ptr_tinv[ii+pnb+1] = 1.0/ptr_t[ii+pnb+1];
		ptr_lamt[ii+1] = ptr_lam[ii+1]*ptr_tinv[ii+1];
		ptr_lamt[ii+pnb+1] = ptr_lam[ii+pnb+1]*ptr_tinv[ii+pnb+1];
		ptr_dlam[ii+1] = ptr_tinv[ii+1]*sigma_mu; // !!!!!
		ptr_dlam[ii+pnb+1] = ptr_tinv[ii+pnb+1]*sigma_mu; // !!!!!
		ptr_pd[ii+1] = ptr_bd[ii+1] + ptr_lamt[ii+1] + ptr_lamt[ii+pnb+1];
		ptr_pl[ii+1] = ptr_bl[ii+1] + ptr_lam[ii+pnb+1] + ptr_lamt[ii+pnb+1]*ptr_db[ii+pnb+1] + ptr_dlam[ii+pnb+1] - ptr_lam[ii+1] - ptr_lamt[ii+1]*ptr_db[ii+1] - ptr_dlam[ii+1];

		ptr_tinv[ii+2] = 1.0/ptr_t[ii+2];
		ptr_tinv[ii+pnb+2] = 1.0/ptr_t[ii+pnb+2];
		ptr_lamt[ii+2] = ptr_lam[ii+2]*ptr_tinv[ii+2];
		ptr_lamt[ii+pnb+2] = ptr_lam[ii+pnb+2]*ptr_tinv[ii+pnb+2];
		ptr_dlam[ii+2] = ptr_tinv[ii+2]*sigma_mu; // !!!!!
		ptr_dlam[ii+pnb+2] = ptr_tinv[ii+pnb+2]*sigma_mu; // !!!!!
		ptr_pd[ii+2] = ptr_bd[ii+2] + ptr_lamt[ii+2] + ptr_lamt[ii+pnb+2];
		ptr_pl[ii+2] = ptr_bl[ii+2] + ptr_lam[ii+pnb+2] + ptr_lamt[ii+pnb+2]*ptr_db[ii+pnb+2] + ptr_dlam[ii+pnb+2] - ptr_lam[ii+2] - ptr_lamt[ii+2]*ptr_db[ii+2] - ptr_dlam[ii+2];

		ptr_tinv[ii+3] = 1.0/ptr_t[ii+3];
		ptr_tinv[ii+pnb+3] = 1.0/ptr_t[ii+pnb+3];
		ptr_lamt[ii+3] = ptr_lam[ii+3]*ptr_tinv[ii+3];
		ptr_lamt[ii+pnb+3] = ptr_lam[ii+pnb+3]*ptr_tinv[ii+pnb+3];
		ptr_dlam[ii+3] = ptr_tinv[ii+3]*sigma_mu; // !!!!!
		ptr_dlam[ii+pnb+3] = ptr_tinv[ii+pnb+3]*sigma_mu; // !!!!!
		ptr_pd[ii+3] = ptr_bd[ii+3] + ptr_lamt[ii+3] + ptr_lamt[ii+pnb+3];
		ptr_pl[ii+3] = ptr_bl[ii+3] + ptr_lam[ii+pnb+3] + ptr_lamt[ii+pnb+3]*ptr_db[ii+pnb+3] + ptr_dlam[ii+pnb+3] - ptr_lam[ii+3] - ptr_lamt[ii+3]*ptr_db[ii+3] - ptr_dlam[ii+3];

		}
	for(; ii<nb; ii++)
		{

		ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
		ptr_tinv[ii+pnb+0] = 1.0/ptr_t[ii+pnb+0];
		ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
		ptr_lamt[ii+pnb+0] = ptr_lam[ii+pnb+0]*ptr_tinv[ii+pnb+0];
		ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
		ptr_dlam[ii+pnb+0] = ptr_tinv[ii+pnb+0]*sigma_mu; // !!!!!
		ptr_pd[ii] = ptr_bd[ii] + ptr_lamt[ii+0] + ptr_lamt[ii+pnb+0];
		ptr_pl[ii] = ptr_bl[ii] + ptr_lam[ii+pnb+0] + ptr_lamt[ii+pnb+0]*ptr_db[ii+pnb+0] + ptr_dlam[ii+pnb+0] - ptr_lam[ii+0] - ptr_lamt[ii+0]*ptr_db[ii+0] - ptr_dlam[ii+0];

		}
	for( ; ii<nu+nx; ii++)
		{
		ptr_pd[ii] = ptr_bd[ii];
		ptr_pl[ii] = ptr_bl[ii];
		}
	if(ng>0) // there are general constraints
		{

		ptr_qx    = qx[jj];

		ii = 2*pnb;
		for(; ii<2*pnb+ng-3; ii+=4)
			{

			ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
			ptr_tinv[ii+png+0] = 1.0/ptr_t[ii+png+0];
			ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
			ptr_lamt[ii+png+0] = ptr_lam[ii+png+0]*ptr_tinv[ii+png+0];
			ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
			ptr_dlam[ii+png+0] = ptr_tinv[ii+png+0]*sigma_mu; // !!!!!
			ptr_qx[ii+0] =  ptr_lam[ii+0] + ptr_lamt[ii+0]*ptr_db[ii+0] + ptr_dlam[ii+0];
			ptr_qx[ii+png+0] = ptr_lam[ii+png+0] + ptr_lamt[ii+png+0]*ptr_db[ii+png+0] + ptr_dlam[ii+png+0];

			ptr_tinv[ii+1] = 1.0/ptr_t[ii+1];
			ptr_tinv[ii+png+1] = 1.0/ptr_t[ii+png+1];
			ptr_lamt[ii+1] = ptr_lam[ii+1]*ptr_tinv[ii+1];
			ptr_lamt[ii+png+1] = ptr_lam[ii+png+1]*ptr_tinv[ii+png+1];
			ptr_dlam[ii+1] = ptr_tinv[ii+1]*sigma_mu; // !!!!!
			ptr_dlam[ii+png+1] = ptr_tinv[ii+png+1]*sigma_mu; // !!!!!
			ptr_qx[ii+1] =  ptr_lam[ii+1] + ptr_lamt[ii+1]*ptr_db[ii+1] + ptr_dlam[ii+1];
			ptr_qx[ii+png+1] = ptr_lam[ii+png+1] + ptr_lamt[ii+png+1]*ptr_db[ii+png+1] + ptr_dlam[ii+png+1];

			ptr_tinv[ii+2] = 1.0/ptr_t[ii+2];
			ptr_tinv[ii+png+2] = 1.0/ptr_t[ii+png+2];
			ptr_lamt[ii+2] = ptr_lam[ii+2]*ptr_tinv[ii+2];
			ptr_lamt[ii+png+2] = ptr_lam[ii+png+2]*ptr_tinv[ii+png+2];
			ptr_dlam[ii+2] = ptr_tinv[ii+2]*sigma_mu; // !!!!!
			ptr_dlam[ii+png+2] = ptr_tinv[ii+png+2]*sigma_mu; // !!!!!
			ptr_qx[ii+2] =  ptr_lam[ii+2] + ptr_lamt[ii+2]*ptr_db[ii+2] + ptr_dlam[ii+2];
			ptr_qx[ii+png+2] = ptr_lam[ii+png+2] + ptr_lamt[ii+png+2]*ptr_db[ii+png+2] + ptr_dlam[ii+png+2];

			ptr_tinv[ii+3] = 1.0/ptr_t[ii+3];
			ptr_tinv[ii+png+3] = 1.0/ptr_t[ii+png+3];
			ptr_lamt[ii+3] = ptr_lam[ii+3]*ptr_tinv[ii+3];
			ptr_lamt[ii+png+3] = ptr_lam[ii+png+3]*ptr_tinv[ii+png+3];
			ptr_dlam[ii+3] = ptr_tinv[ii+3]*sigma_mu; // !!!!!
			ptr_dlam[ii+png+3] = ptr_tinv[ii+png+3]*sigma_mu; // !!!!!
			ptr_qx[ii+3] =  ptr_lam[ii+3] + ptr_lamt[ii+3]*ptr_db[ii+3] + ptr_dlam[ii+3];
			ptr_qx[ii+png+3] = ptr_lam[ii+png+3] + ptr_lamt[ii+png+3]*ptr_db[ii+png+3] + ptr_dlam[ii+png+3];

			}
		for(; ii<2*pnb+ng-3; ii+=4)
			{

			ptr_tinv[ii+0] = 1.0/ptr_t[ii+0];
			ptr_tinv[ii+png+0] = 1.0/ptr_t[ii+png+0];
			ptr_lamt[ii+0] = ptr_lam[ii+0]*ptr_tinv[ii+0];
			ptr_lamt[ii+png+0] = ptr_lam[ii+png+0]*ptr_tinv[ii+png+0];
			ptr_dlam[ii+0] = ptr_tinv[ii+0]*sigma_mu; // !!!!!
			ptr_dlam[ii+png+0] = ptr_tinv[ii+png+0]*sigma_mu; // !!!!!
			ptr_qx[ii+0] =  ptr_lam[ii+0] + ptr_lamt[ii+0]*ptr_db[ii+0] + ptr_dlam[ii+0];
			ptr_qx[ii+png+0] = ptr_lam[ii+png+0] + ptr_lamt[ii+png+0]*ptr_db[ii+png+0] + ptr_dlam[ii+png+0];

			}

		}


	}



void d_update_hessian_soft_mpc(int N, int nx, int nu, int nh, int ns, int cnz, double sigma_mu, double **t, double **t_inv, double **lam, double **lamt, double **dlam, double **bd, double **bl, double **pd, double **pl, double **db, double **Z, double **z, double **Zl, double **zl)

/*void d_update_hessian_box(int k0, int kmax, int nb, int cnz, double sigma_mu, double *t, double *lam, double *lamt, double *dlam, double *bd, double *bl, double *pd, double *pl, double *lb, double *ub)*/
	{

	int nb = nh + ns;

	int nhu = nu<nh ? nu : nh ;

	// constants
	const int bs = 4; //D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	const int anb = nal*((2*nb+nal-1)/nal); // cache aligned number of box and soft constraints // !!!!! doubled to include soft constraints !!!!!

	//const int k0 = nbu;
	//const int k1 = (nu/bs)*bs;
	//const int kmax = nb;
	
	
	double temp0, temp1;
	
	double *ptr_t, *ptr_lam, *ptr_lamt, *ptr_dlam, *ptr_tinv, *ptr_pd, *ptr_pl, *ptr_bd, *ptr_bl, *ptr_db, *ptr_Z, *ptr_z, *ptr_Zl, *ptr_zl;

	static double Qx[8] = {};
	static double qx[8] = {};
	
	int ii, jj, bs0;
	
	// first stage
	jj = 0;
	
	ptr_t     = t[0];
	ptr_lam   = lam[0];
	ptr_lamt  = lamt[0];
	ptr_dlam  = dlam[0];
	ptr_tinv  = t_inv[0];
	ptr_pd    = pd[0];
	ptr_pl    = pl[0];
	ptr_bd    = bd[0];
	ptr_bl    = bl[0];
	ptr_db    = db[0];
	//ptr_Z     = Z[0];
	//ptr_z     = z[0];
	//ptr_Zl    = Zl[0];
	//ptr_zl    = zl[0];
	
	ii = 0;
	// hard constraints on u only
	for(; ii<nhu-3; ii+=4)
		{

		ptr_tinv[0] = 1.0/ptr_t[0];
		ptr_tinv[1] = 1.0/ptr_t[1];
		ptr_lamt[0] = ptr_lam[0]*ptr_tinv[0];
		ptr_lamt[1] = ptr_lam[1]*ptr_tinv[1];
		ptr_dlam[0] = ptr_tinv[0]*sigma_mu; // !!!!!
		ptr_dlam[1] = ptr_tinv[1]*sigma_mu; // !!!!!
		ptr_pd[ii+0] = ptr_bd[ii+0] + ptr_lamt[0] + ptr_lamt[1];
		ptr_pl[ii+0] = ptr_bl[ii+0] + ptr_lam[1] + ptr_lamt[1]*ptr_db[1] + ptr_dlam[1] - ptr_lam[0] - ptr_lamt[0]*ptr_db[0] - ptr_dlam[0];

		ptr_tinv[2] = 1.0/ptr_t[2];
		ptr_tinv[3] = 1.0/ptr_t[3];
		ptr_lamt[2] = ptr_lam[2]*ptr_tinv[2];
		ptr_lamt[3] = ptr_lam[3]*ptr_tinv[3];
		ptr_dlam[2] = ptr_tinv[2]*sigma_mu; // !!!!!
		ptr_dlam[3] = ptr_tinv[3]*sigma_mu; // !!!!!
		ptr_pd[ii+1] = ptr_bd[ii+1] + ptr_lamt[2] + ptr_lamt[3];
		ptr_pl[ii+1] = ptr_bl[ii+1] + ptr_lam[3] + ptr_lamt[3]*ptr_db[3] + ptr_dlam[3] - ptr_lam[2] - ptr_lamt[2]*ptr_db[2] - ptr_dlam[2];

		ptr_tinv[4] = 1.0/ptr_t[4];
		ptr_tinv[5] = 1.0/ptr_t[5];
		ptr_lamt[4] = ptr_lam[4]*ptr_tinv[4];
		ptr_lamt[5] = ptr_lam[5]*ptr_tinv[5];
		ptr_dlam[4] = ptr_tinv[4]*sigma_mu; // !!!!!
		ptr_dlam[5] = ptr_tinv[5]*sigma_mu; // !!!!!
		ptr_pd[ii+2] = ptr_bd[ii+2] + ptr_lamt[4] + ptr_lamt[5];
		ptr_pl[ii+2] = ptr_bl[ii+2] + ptr_lam[5] + ptr_lamt[5]*ptr_db[5] + ptr_dlam[5] - ptr_lam[4] - ptr_lamt[4]*ptr_db[4] - ptr_dlam[4];

		ptr_tinv[6] = 1.0/ptr_t[6];
		ptr_tinv[7] = 1.0/ptr_t[7];
		ptr_lamt[6] = ptr_lam[6]*ptr_tinv[6];
		ptr_lamt[7] = ptr_lam[7]*ptr_tinv[7];
		ptr_dlam[6] = ptr_tinv[6]*sigma_mu; // !!!!!
		ptr_dlam[7] = ptr_tinv[7]*sigma_mu; // !!!!!
		ptr_pd[ii+3] = ptr_bd[ii+3] + ptr_lamt[6] + ptr_lamt[7];
		ptr_pl[ii+3] = ptr_bl[ii+3] + ptr_lam[7] + ptr_lamt[7]*ptr_db[7] + ptr_dlam[7] - ptr_lam[6] - ptr_lamt[6]*ptr_db[6] - ptr_dlam[6];

		ptr_t     += 8;
		ptr_lam   += 8;
		ptr_lamt  += 8;
		ptr_dlam  += 8;
		ptr_tinv  += 8;
		ptr_db    += 8;

		}
	for(; ii<nhu; ii++)
		{
		ptr_tinv[0] = 1.0/ptr_t[0];
		ptr_tinv[1] = 1.0/ptr_t[1];
		ptr_lamt[0] = ptr_lam[0]*ptr_tinv[0];
		ptr_lamt[1] = ptr_lam[1]*ptr_tinv[1];
		ptr_dlam[0] = ptr_tinv[0]*sigma_mu; // !!!!!
		ptr_dlam[1] = ptr_tinv[1]*sigma_mu; // !!!!!
		ptr_pd[ii] = ptr_bd[ii] + ptr_lamt[0] + ptr_lamt[1];
		ptr_pl[ii] = ptr_bl[ii] + ptr_lam[1] + ptr_lamt[1]*ptr_db[1] + ptr_dlam[1] - ptr_lam[0] - ptr_lamt[0]*ptr_db[0] - ptr_dlam[0];

		ptr_t     += 2;
		ptr_lam   += 2;
		ptr_lamt  += 2;
		ptr_dlam  += 2;
		ptr_tinv  += 2;
		ptr_db    += 2;
		}
	for( ; ii<nu; ii++)
		{
		pd[jj][ii] = bd[jj][ii];
		pl[jj][ii] = bl[jj][ii];
		}

	// middle stages

	for(jj=1; jj<N; jj++)
		{
		
		ptr_t     = t[jj];
		ptr_lam   = lam[jj];
		ptr_lamt  = lamt[jj];
		ptr_dlam  = dlam[jj];
		ptr_tinv  = t_inv[jj];
		ptr_pd    = pd[jj];
		ptr_pl    = pl[jj];
		ptr_bd    = bd[jj];
		ptr_bl    = bl[jj];
		ptr_db    = db[jj];
		ptr_Z     = Z[jj];
		ptr_z     = z[jj];
		ptr_Zl    = Zl[jj];
		ptr_zl    = zl[jj];

		ii = 0;
		// hard constraints on u and x
		for(; ii<nh-3; ii+=4)
			{

			ptr_tinv[0] = 1.0/ptr_t[0];
			ptr_tinv[1] = 1.0/ptr_t[1];
			ptr_lamt[0] = ptr_lam[0]*ptr_tinv[0];
			ptr_lamt[1] = ptr_lam[1]*ptr_tinv[1];
			ptr_dlam[0] = ptr_tinv[0]*sigma_mu; // !!!!!
			ptr_dlam[1] = ptr_tinv[1]*sigma_mu; // !!!!!
			ptr_pd[ii+0] = ptr_bd[ii+0] + ptr_lamt[0] + ptr_lamt[1];
			ptr_pl[ii+0] = ptr_bl[ii+0] + ptr_lam[1] + ptr_lamt[1]*ptr_db[1] + ptr_dlam[1] - ptr_lam[0] - ptr_lamt[0]*ptr_db[0] - ptr_dlam[0];

			ptr_tinv[2] = 1.0/ptr_t[2];
			ptr_tinv[3] = 1.0/ptr_t[3];
			ptr_lamt[2] = ptr_lam[2]*ptr_tinv[2];
			ptr_lamt[3] = ptr_lam[3]*ptr_tinv[3];
			ptr_dlam[2] = ptr_tinv[2]*sigma_mu; // !!!!!
			ptr_dlam[3] = ptr_tinv[3]*sigma_mu; // !!!!!
			ptr_pd[ii+1] = ptr_bd[ii+1] + ptr_lamt[2] + ptr_lamt[3];
			ptr_pl[ii+1] = ptr_bl[ii+1] + ptr_lam[3] + ptr_lamt[3]*ptr_db[3] + ptr_dlam[3] - ptr_lam[2] - ptr_lamt[2]*ptr_db[2] - ptr_dlam[2];

			ptr_tinv[4] = 1.0/ptr_t[4];
			ptr_tinv[5] = 1.0/ptr_t[5];
			ptr_lamt[4] = ptr_lam[4]*ptr_tinv[4];
			ptr_lamt[5] = ptr_lam[5]*ptr_tinv[5];
			ptr_dlam[4] = ptr_tinv[4]*sigma_mu; // !!!!!
			ptr_dlam[5] = ptr_tinv[5]*sigma_mu; // !!!!!
			ptr_pd[ii+2] = ptr_bd[ii+2] + ptr_lamt[4] + ptr_lamt[5];
			ptr_pl[ii+2] = ptr_bl[ii+2] + ptr_lam[5] + ptr_lamt[5]*ptr_db[5] + ptr_dlam[5] - ptr_lam[4] - ptr_lamt[4]*ptr_db[4] - ptr_dlam[4];

			ptr_tinv[6] = 1.0/ptr_t[6];
			ptr_tinv[7] = 1.0/ptr_t[7];
			ptr_lamt[6] = ptr_lam[6]*ptr_tinv[6];
			ptr_lamt[7] = ptr_lam[7]*ptr_tinv[7];
			ptr_dlam[6] = ptr_tinv[6]*sigma_mu; // !!!!!
			ptr_dlam[7] = ptr_tinv[7]*sigma_mu; // !!!!!
			ptr_pd[ii+3] = ptr_bd[ii+3] + ptr_lamt[6] + ptr_lamt[7];
			ptr_pl[ii+3] = ptr_bl[ii+3] + ptr_lam[7] + ptr_lamt[7]*ptr_db[7] + ptr_dlam[7] - ptr_lam[6] - ptr_lamt[6]*ptr_db[6] - ptr_dlam[6];

			ptr_t     += 8;
			ptr_lam   += 8;
			ptr_lamt  += 8;
			ptr_dlam  += 8;
			ptr_tinv  += 8;
			ptr_db    += 8;
			ptr_Z     += 8;
			ptr_z     += 8;
			ptr_Zl    += 8;
			ptr_zl    += 8;

			}
		for(; ii<nh; ii++)
			{
			ptr_tinv[0] = 1.0/ptr_t[0];
			ptr_tinv[1] = 1.0/ptr_t[1];
			ptr_lamt[0] = ptr_lam[0]*ptr_tinv[0];
			ptr_lamt[1] = ptr_lam[1]*ptr_tinv[1];
			ptr_dlam[0] = ptr_tinv[0]*sigma_mu; // !!!!!
			ptr_dlam[1] = ptr_tinv[1]*sigma_mu; // !!!!!
			ptr_pd[ii] = ptr_bd[ii] + ptr_lamt[0] + ptr_lamt[1];
			ptr_pl[ii] = ptr_bl[ii] + ptr_lam[1] + ptr_lamt[1]*ptr_db[1] + ptr_dlam[1] - ptr_lam[0] - ptr_lamt[0]*ptr_db[0] - ptr_dlam[0];

			ptr_t     += 2;
			ptr_lam   += 2;
			ptr_lamt  += 2;
			ptr_dlam  += 2;
			ptr_tinv  += 2;
			ptr_db    += 2;
			ptr_Z     += 2;
			ptr_z     += 2;
			ptr_Zl    += 2;
			ptr_zl    += 2;
			}

		// soft constraints on states
		for(; ii<nb-3; ii+=4)
			{
			ptr_tinv[0] = 1.0/ptr_t[0];
			ptr_tinv[1] = 1.0/ptr_t[1];
			ptr_tinv[anb+0] = 1.0/ptr_t[anb+0];
			ptr_tinv[anb+1] = 1.0/ptr_t[anb+1];
			ptr_lamt[0] = ptr_lam[0]*ptr_tinv[0];
			ptr_lamt[1] = ptr_lam[1]*ptr_tinv[1];
			ptr_lamt[anb+0] = ptr_lam[anb+0]*ptr_tinv[anb+0];
			ptr_lamt[anb+1] = ptr_lam[anb+1]*ptr_tinv[anb+1];
			ptr_dlam[0] = ptr_tinv[0]*sigma_mu; // !!!!!
			ptr_dlam[1] = ptr_tinv[1]*sigma_mu; // !!!!!
			ptr_dlam[anb+0] = ptr_tinv[anb+0]*sigma_mu; // !!!!!
			ptr_dlam[anb+1] = ptr_tinv[anb+1]*sigma_mu; // !!!!!
			Qx[0] = ptr_lamt[0];
			Qx[1] = ptr_lamt[1];
			qx[0] = ptr_lam[0] + ptr_dlam[0] + ptr_lamt[0]*ptr_db[0];
			qx[1] = ptr_lam[1] + ptr_dlam[1] + ptr_lamt[1]*ptr_db[1];
			ptr_Zl[0] = 1.0 / (ptr_Z[0] + Qx[0] + ptr_lamt[anb+0]); // inverted of updated diagonal !!!
			ptr_Zl[1] = 1.0 / (ptr_Z[1] + Qx[1] + ptr_lamt[anb+1]); // inverted of updated diagonal !!!
			ptr_zl[0] = - ptr_z[0] + qx[0] + ptr_lam[anb+0] + ptr_dlam[anb+0];
			ptr_zl[1] = - ptr_z[1] + qx[1] + ptr_lam[anb+1] + ptr_dlam[anb+1];
			qx[0] = qx[0] - Qx[0]*ptr_zl[0]*ptr_Zl[0]; // update this before Qx !!!!!!!!!!!
			qx[1] = qx[1] - Qx[1]*ptr_zl[1]*ptr_Zl[1]; // update this before Qx !!!!!!!!!!!
			Qx[0] = Qx[0] - Qx[0]*Qx[0]*ptr_Zl[0];
			Qx[1] = Qx[1] - Qx[1]*Qx[1]*ptr_Zl[1];
			ptr_pd[ii+0] = ptr_bd[ii+0] + Qx[1] + Qx[0];
			ptr_pl[ii+0] = ptr_bl[ii+0] + qx[1] - qx[0];

			ptr_tinv[2] = 1.0/ptr_t[2];
			ptr_tinv[3] = 1.0/ptr_t[3];
			ptr_tinv[anb+2] = 1.0/ptr_t[anb+2];
			ptr_tinv[anb+3] = 1.0/ptr_t[anb+3];
			ptr_lamt[2] = ptr_lam[2]*ptr_tinv[2];
			ptr_lamt[3] = ptr_lam[3]*ptr_tinv[3];
			ptr_lamt[anb+2] = ptr_lam[anb+2]*ptr_tinv[anb+2];
			ptr_lamt[anb+3] = ptr_lam[anb+3]*ptr_tinv[anb+3];
			ptr_dlam[2] = ptr_tinv[2]*sigma_mu; // !!!!!
			ptr_dlam[3] = ptr_tinv[3]*sigma_mu; // !!!!!
			ptr_dlam[anb+2] = ptr_tinv[anb+2]*sigma_mu; // !!!!!
			ptr_dlam[anb+3] = ptr_tinv[anb+3]*sigma_mu; // !!!!!
			Qx[2] = ptr_lamt[2];
			Qx[3] = ptr_lamt[3];
			qx[2] = ptr_lam[2] + ptr_dlam[2] + ptr_lamt[2]*ptr_db[2];
			qx[3] = ptr_lam[3] + ptr_dlam[3] + ptr_lamt[3]*ptr_db[3];
			ptr_Zl[2] = 1.0 / (ptr_Z[2] + Qx[2] + ptr_lamt[anb+2]); // inverted of updated diagonal !!!
			ptr_Zl[3] = 1.0 / (ptr_Z[3] + Qx[3] + ptr_lamt[anb+3]); // inverted of updated diagonal !!!
			ptr_zl[2] = - ptr_z[2] + qx[2] + ptr_lam[anb+2] + ptr_dlam[anb+2];
			ptr_zl[3] = - ptr_z[3] + qx[3] + ptr_lam[anb+3] + ptr_dlam[anb+3];
			qx[2] = qx[2] - Qx[2]*ptr_zl[2]*ptr_Zl[2]; // update this before Qx !!!!!!!!!!!
			qx[3] = qx[3] - Qx[3]*ptr_zl[3]*ptr_Zl[3]; // update this before Qx !!!!!!!!!!!
			Qx[2] = Qx[2] - Qx[2]*Qx[2]*ptr_Zl[2];
			Qx[3] = Qx[3] - Qx[3]*Qx[3]*ptr_Zl[3];
			ptr_pd[ii+1] = ptr_bd[ii+1] + Qx[3] + Qx[2];
			ptr_pl[ii+1] = ptr_bl[ii+1] + qx[3] - qx[2];

			ptr_tinv[4] = 1.0/ptr_t[4];
			ptr_tinv[5] = 1.0/ptr_t[5];
			ptr_tinv[anb+4] = 1.0/ptr_t[anb+4];
			ptr_tinv[anb+5] = 1.0/ptr_t[anb+5];
			ptr_lamt[4] = ptr_lam[4]*ptr_tinv[4];
			ptr_lamt[5] = ptr_lam[5]*ptr_tinv[5];
			ptr_lamt[anb+4] = ptr_lam[anb+4]*ptr_tinv[anb+4];
			ptr_lamt[anb+5] = ptr_lam[anb+5]*ptr_tinv[anb+5];
			ptr_dlam[4] = ptr_tinv[4]*sigma_mu; // !!!!!
			ptr_dlam[5] = ptr_tinv[5]*sigma_mu; // !!!!!
			ptr_dlam[anb+4] = ptr_tinv[anb+4]*sigma_mu; // !!!!!
			ptr_dlam[anb+5] = ptr_tinv[anb+5]*sigma_mu; // !!!!!
			Qx[4] = ptr_lamt[4];
			Qx[5] = ptr_lamt[5];
			qx[4] = ptr_lam[4] + ptr_dlam[4] + ptr_lamt[4]*ptr_db[4];
			qx[5] = ptr_lam[5] + ptr_dlam[5] + ptr_lamt[5]*ptr_db[5];
			ptr_Zl[4] = 1.0 / (ptr_Z[4] + Qx[4] + ptr_lamt[anb+4]); // inverted of updated diagonal !!!
			ptr_Zl[5] = 1.0 / (ptr_Z[5] + Qx[5] + ptr_lamt[anb+5]); // inverted of updated diagonal !!!
			ptr_zl[4] = - ptr_z[4] + qx[4] + ptr_lam[anb+4] + ptr_dlam[anb+4];
			ptr_zl[5] = - ptr_z[5] + qx[5] + ptr_lam[anb+5] + ptr_dlam[anb+5];
			qx[4] = qx[4] - Qx[4]*ptr_zl[4]*ptr_Zl[4]; // update this before Qx !!!!!!!!!!!
			qx[5] = qx[5] - Qx[5]*ptr_zl[5]*ptr_Zl[5]; // update this before Qx !!!!!!!!!!!
			Qx[4] = Qx[4] - Qx[4]*Qx[4]*ptr_Zl[4];
			Qx[5] = Qx[5] - Qx[5]*Qx[5]*ptr_Zl[5];
			ptr_pd[ii+2] = ptr_bd[ii+2] + Qx[5] + Qx[4];
			ptr_pl[ii+2] = ptr_bl[ii+2] + qx[5] - qx[4];

			ptr_tinv[6] = 1.0/ptr_t[6];
			ptr_tinv[7] = 1.0/ptr_t[7];
			ptr_tinv[anb+6] = 1.0/ptr_t[anb+6];
			ptr_tinv[anb+7] = 1.0/ptr_t[anb+7];
			ptr_lamt[6] = ptr_lam[6]*ptr_tinv[6];
			ptr_lamt[7] = ptr_lam[7]*ptr_tinv[7];
			ptr_lamt[anb+6] = ptr_lam[anb+6]*ptr_tinv[anb+6];
			ptr_lamt[anb+7] = ptr_lam[anb+7]*ptr_tinv[anb+7];
			ptr_dlam[6] = ptr_tinv[6]*sigma_mu; // !!!!!
			ptr_dlam[7] = ptr_tinv[7]*sigma_mu; // !!!!!
			ptr_dlam[anb+6] = ptr_tinv[anb+6]*sigma_mu; // !!!!!
			ptr_dlam[anb+7] = ptr_tinv[anb+7]*sigma_mu; // !!!!!
			Qx[6] = ptr_lamt[6];
			Qx[7] = ptr_lamt[7];
			qx[6] = ptr_lam[6] + ptr_dlam[6] + ptr_lamt[6]*ptr_db[6];
			qx[7] = ptr_lam[7] + ptr_dlam[7] + ptr_lamt[7]*ptr_db[7];
			ptr_Zl[6] = 1.0 / (ptr_Z[6] + Qx[6] + ptr_lamt[anb+6]); // inverted of updated diagonal !!!
			ptr_Zl[7] = 1.0 / (ptr_Z[7] + Qx[7] + ptr_lamt[anb+7]); // inverted of updated diagonal !!!
			ptr_zl[6] = - ptr_z[6] + qx[6] + ptr_lam[anb+6] + ptr_dlam[anb+6];
			ptr_zl[7] = - ptr_z[7] + qx[7] + ptr_lam[anb+7] + ptr_dlam[anb+7];
			qx[6] = qx[6] - Qx[6]*ptr_zl[6]*ptr_Zl[6]; // update this before Qx !!!!!!!!!!!
			qx[7] = qx[7] - Qx[7]*ptr_zl[7]*ptr_Zl[7]; // update this before Qx !!!!!!!!!!!
			Qx[6] = Qx[6] - Qx[6]*Qx[6]*ptr_Zl[6];
			Qx[7] = Qx[7] - Qx[7]*Qx[7]*ptr_Zl[7];
			ptr_pd[ii+3] = ptr_bd[ii+3] + Qx[7] + Qx[6];
			ptr_pl[ii+3] = ptr_bl[ii+3] + qx[7] - qx[6];

			ptr_t     += 8;
			ptr_lam   += 8;
			ptr_lamt  += 8;
			ptr_dlam  += 8;
			ptr_tinv  += 8;
			ptr_db    += 8;
			ptr_Z     += 8;
			ptr_z     += 8;
			ptr_Zl    += 8;
			ptr_zl    += 8;

			}
		for(; ii<nb; ii++)
			{
			ptr_tinv[0] = 1.0/ptr_t[0];
			ptr_tinv[1] = 1.0/ptr_t[1];
			ptr_tinv[anb+0] = 1.0/ptr_t[anb+0];
			ptr_tinv[anb+1] = 1.0/ptr_t[anb+1];
			ptr_lamt[0] = ptr_lam[0]*ptr_tinv[0];
			ptr_lamt[1] = ptr_lam[1]*ptr_tinv[1];
			ptr_lamt[anb+0] = ptr_lam[anb+0]*ptr_tinv[anb+0];
			ptr_lamt[anb+1] = ptr_lam[anb+1]*ptr_tinv[anb+1];
			ptr_dlam[0] = ptr_tinv[0]*sigma_mu; // !!!!!
			ptr_dlam[1] = ptr_tinv[1]*sigma_mu; // !!!!!
			ptr_dlam[anb+0] = ptr_tinv[anb+0]*sigma_mu; // !!!!!
			ptr_dlam[anb+1] = ptr_tinv[anb+1]*sigma_mu; // !!!!!
			Qx[0] = ptr_lamt[0];
			Qx[1] = ptr_lamt[1];
			qx[0] = ptr_lam[0] + ptr_dlam[0] + ptr_lamt[0]*ptr_db[0];
			qx[1] = ptr_lam[1] + ptr_dlam[1] + ptr_lamt[1]*ptr_db[1];
			ptr_Zl[0] = 1.0 / (ptr_Z[0] + Qx[0] + ptr_lamt[anb+0]); // inverted of updated diagonal !!!
			ptr_Zl[1] = 1.0 / (ptr_Z[1] + Qx[1] + ptr_lamt[anb+1]); // inverted of updated diagonal !!!
			ptr_zl[0] = - ptr_z[0] + qx[0] + ptr_lam[anb+0] + ptr_dlam[anb+0];
			ptr_zl[1] = - ptr_z[1] + qx[1] + ptr_lam[anb+1] + ptr_dlam[anb+1];
			qx[0] = qx[0] - Qx[0]*ptr_zl[0]*ptr_Zl[0]; // update this before Qx !!!!!!!!!!!
			qx[1] = qx[1] - Qx[1]*ptr_zl[1]*ptr_Zl[1]; // update this before Qx !!!!!!!!!!!
			Qx[0] = Qx[0] - Qx[0]*Qx[0]*ptr_Zl[0];
			Qx[1] = Qx[1] - Qx[1]*Qx[1]*ptr_Zl[1];
			ptr_pd[ii] = ptr_bd[ii] + Qx[1] + Qx[0];
			ptr_pl[ii] = ptr_bl[ii] + qx[1] - qx[0];

			ptr_t     += 2;
			ptr_lam   += 2;
			ptr_lamt  += 2;
			ptr_dlam  += 2;
			ptr_tinv  += 2;
			ptr_db    += 2;
			ptr_Z     += 2;
			ptr_z     += 2;
			ptr_Zl    += 2;
			ptr_zl    += 2;
			}
		for( ; ii<nu+nx; ii++)
			{
			pd[jj][ii] = bd[jj][ii];
			pl[jj][ii] = bl[jj][ii];
			}
	
		}

	// last stage
	jj = N;

	ptr_t     = t[N]     + 2*nu;
	ptr_lam   = lam[N]   + 2*nu;
	ptr_lamt  = lamt[N]  + 2*nu;
	ptr_dlam  = dlam[N]  + 2*nu;
	ptr_tinv  = t_inv[N] + 2*nu;
	ptr_db    = db[N]    + 2*nu;
	ptr_Z     = Z[N]     + 2*nu;
	ptr_z     = z[N]     + 2*nu;
	ptr_Zl    = Zl[N]    + 2*nu;
	ptr_zl    = zl[N]    + 2*nu;
	ptr_pd    = pd[N];
	ptr_pl    = pl[N];
	ptr_bd    = bd[N];
	ptr_bl    = bl[N];

	ii=nu;
	for(; ii<nh-3; ii+=4)
		{
		ptr_tinv[0] = 1.0/ptr_t[0];
		ptr_tinv[1] = 1.0/ptr_t[1];
		ptr_lamt[0] = ptr_lam[0]*ptr_tinv[0];
		ptr_lamt[1] = ptr_lam[1]*ptr_tinv[1];
		ptr_dlam[0] = ptr_tinv[0]*sigma_mu; // !!!!!
		ptr_dlam[1] = ptr_tinv[1]*sigma_mu; // !!!!!
		ptr_pd[ii+0] = ptr_bd[ii+0] + ptr_lamt[0] + ptr_lamt[1];
		ptr_pl[ii+0] = ptr_bl[ii+0] + ptr_lam[1] + ptr_lamt[1]*ptr_db[1] + ptr_dlam[1] - ptr_lam[0] - ptr_lamt[0]*ptr_db[0] - ptr_dlam[0];

		ptr_tinv[2] = 1.0/ptr_t[2];
		ptr_tinv[3] = 1.0/ptr_t[3];
		ptr_lamt[2] = ptr_lam[2]*ptr_tinv[2];
		ptr_lamt[3] = ptr_lam[3]*ptr_tinv[3];
		ptr_dlam[2] = ptr_tinv[2]*sigma_mu; // !!!!!
		ptr_dlam[3] = ptr_tinv[3]*sigma_mu; // !!!!!
		ptr_pd[ii+1] = ptr_bd[ii+1] + ptr_lamt[2] + ptr_lamt[3];
		ptr_pl[ii+1] = ptr_bl[ii+1] + ptr_lam[3] + ptr_lamt[3]*ptr_db[3] + ptr_dlam[3] - ptr_lam[2] - ptr_lamt[2]*ptr_db[2] - ptr_dlam[2];

		ptr_tinv[4] = 1.0/ptr_t[4];
		ptr_tinv[5] = 1.0/ptr_t[5];
		ptr_lamt[4] = ptr_lam[4]*ptr_tinv[4];
		ptr_lamt[5] = ptr_lam[5]*ptr_tinv[5];
		ptr_dlam[4] = ptr_tinv[4]*sigma_mu; // !!!!!
		ptr_dlam[5] = ptr_tinv[5]*sigma_mu; // !!!!!
		ptr_pd[ii+2] = ptr_bd[ii+2] + ptr_lamt[4] + ptr_lamt[5];
		ptr_pl[ii+2] = ptr_bl[ii+2] + ptr_lam[5] + ptr_lamt[5]*ptr_db[5] + ptr_dlam[5] - ptr_lam[4] - ptr_lamt[4]*ptr_db[4] - ptr_dlam[4];

		ptr_tinv[6] = 1.0/ptr_t[6];
		ptr_tinv[7] = 1.0/ptr_t[7];
		ptr_lamt[6] = ptr_lam[6]*ptr_tinv[6];
		ptr_lamt[7] = ptr_lam[7]*ptr_tinv[7];
		ptr_dlam[6] = ptr_tinv[6]*sigma_mu; // !!!!!
		ptr_dlam[7] = ptr_tinv[7]*sigma_mu; // !!!!!
		ptr_pd[ii+3] = ptr_bd[ii+3] + ptr_lamt[6] + ptr_lamt[7];
		ptr_pl[ii+3] = ptr_bl[ii+3] + ptr_lam[7] + ptr_lamt[7]*ptr_db[7] + ptr_dlam[7] - ptr_lam[6] - ptr_lamt[6]*ptr_db[6] - ptr_dlam[6];

		ptr_t     += 8;
		ptr_lam   += 8;
		ptr_lamt  += 8;
		ptr_dlam  += 8;
		ptr_tinv  += 8;
		ptr_db    += 8;
		ptr_Z     += 8;
		ptr_z     += 8;
		ptr_Zl    += 8;
		ptr_zl    += 8;

		}
	// cleanup hard constraints
	for(; ii<nh; ii++)
		{
		ptr_tinv[0] = 1.0/ptr_t[0];
		ptr_tinv[1] = 1.0/ptr_t[1];
		ptr_lamt[0] = ptr_lam[0]*ptr_tinv[0];
		ptr_lamt[1] = ptr_lam[1]*ptr_tinv[1];
		ptr_dlam[0] = ptr_tinv[0]*sigma_mu; // !!!!!
		ptr_dlam[1] = ptr_tinv[1]*sigma_mu; // !!!!!
		ptr_pd[ii] = ptr_bd[ii] + ptr_lamt[0] + ptr_lamt[1];
		ptr_pl[ii] = ptr_bl[ii] + ptr_lam[1] + ptr_lamt[1]*ptr_db[1] + ptr_dlam[1] - ptr_lam[0] - ptr_lamt[0]*ptr_db[0] - ptr_dlam[0];

		ptr_t     += 2;
		ptr_lam   += 2;
		ptr_lamt  += 2;
		ptr_dlam  += 2;
		ptr_tinv  += 2;
		ptr_db    += 2;
		ptr_Z     += 2;
		ptr_z     += 2;
		ptr_Zl    += 2;
		ptr_zl    += 2;
		}

	// soft constraints main loop
	for(; ii<nb-3; ii+=4)
		{
		ptr_tinv[0] = 1.0/ptr_t[0];
		ptr_tinv[1] = 1.0/ptr_t[1];
		ptr_tinv[anb+0] = 1.0/ptr_t[anb+0];
		ptr_tinv[anb+1] = 1.0/ptr_t[anb+1];
		ptr_lamt[0] = ptr_lam[0]*ptr_tinv[0];
		ptr_lamt[1] = ptr_lam[1]*ptr_tinv[1];
		ptr_lamt[anb+0] = ptr_lam[anb+0]*ptr_tinv[anb+0];
		ptr_lamt[anb+1] = ptr_lam[anb+1]*ptr_tinv[anb+1];
		ptr_dlam[0] = ptr_tinv[0]*sigma_mu; // !!!!!
		ptr_dlam[1] = ptr_tinv[1]*sigma_mu; // !!!!!
		ptr_dlam[anb+0] = ptr_tinv[anb+0]*sigma_mu; // !!!!!
		ptr_dlam[anb+1] = ptr_tinv[anb+1]*sigma_mu; // !!!!!
		Qx[0] = ptr_lamt[0];
		Qx[1] = ptr_lamt[1];
		qx[0] = ptr_lam[0] + ptr_dlam[0] + ptr_lamt[0]*ptr_db[0];
		qx[1] = ptr_lam[1] + ptr_dlam[1] + ptr_lamt[1]*ptr_db[1];
		ptr_Zl[0] = 1.0 / (ptr_Z[0] + Qx[0] + ptr_lamt[anb+0]); // inverted of updated diagonal !!!
		ptr_Zl[1] = 1.0 / (ptr_Z[1] + Qx[1] + ptr_lamt[anb+1]); // inverted of updated diagonal !!!
		ptr_zl[0] = - ptr_z[0] + qx[0] + ptr_lam[anb+0] + ptr_dlam[anb+0];
		ptr_zl[1] = - ptr_z[1] + qx[1] + ptr_lam[anb+1] + ptr_dlam[anb+1];
		qx[0] = qx[0] - Qx[0]*ptr_zl[0]*ptr_Zl[0]; // update this before Qx !!!!!!!!!!!
		qx[1] = qx[1] - Qx[1]*ptr_zl[1]*ptr_Zl[1]; // update this before Qx !!!!!!!!!!!
		Qx[0] = Qx[0] - Qx[0]*Qx[0]*ptr_Zl[0];
		Qx[1] = Qx[1] - Qx[1]*Qx[1]*ptr_Zl[1];
		ptr_pd[ii+0] = ptr_bd[ii+0] + Qx[1] + Qx[0];
		ptr_pl[ii+0] = ptr_bl[ii+0] + qx[1] - qx[0];

		ptr_tinv[2] = 1.0/ptr_t[2];
		ptr_tinv[3] = 1.0/ptr_t[3];
		ptr_tinv[anb+2] = 1.0/ptr_t[anb+2];
		ptr_tinv[anb+3] = 1.0/ptr_t[anb+3];
		ptr_lamt[2] = ptr_lam[2]*ptr_tinv[2];
		ptr_lamt[3] = ptr_lam[3]*ptr_tinv[3];
		ptr_lamt[anb+2] = ptr_lam[anb+2]*ptr_tinv[anb+2];
		ptr_lamt[anb+3] = ptr_lam[anb+3]*ptr_tinv[anb+3];
		ptr_dlam[2] = ptr_tinv[2]*sigma_mu; // !!!!!
		ptr_dlam[3] = ptr_tinv[3]*sigma_mu; // !!!!!
		ptr_dlam[anb+2] = ptr_tinv[anb+2]*sigma_mu; // !!!!!
		ptr_dlam[anb+3] = ptr_tinv[anb+3]*sigma_mu; // !!!!!
		Qx[2] = ptr_lamt[2];
		Qx[3] = ptr_lamt[3];
		qx[2] = ptr_lam[2] + ptr_dlam[2] + ptr_lamt[2]*ptr_db[2];
		qx[3] = ptr_lam[3] + ptr_dlam[3] + ptr_lamt[3]*ptr_db[3];
		ptr_Zl[2] = 1.0 / (ptr_Z[2] + Qx[2] + ptr_lamt[anb+2]); // inverted of updated diagonal !!!
		ptr_Zl[3] = 1.0 / (ptr_Z[3] + Qx[3] + ptr_lamt[anb+3]); // inverted of updated diagonal !!!
		ptr_zl[2] = - ptr_z[2] + qx[2] + ptr_lam[anb+2] + ptr_dlam[anb+2];
		ptr_zl[3] = - ptr_z[3] + qx[3] + ptr_lam[anb+3] + ptr_dlam[anb+3];
		qx[2] = qx[2] - Qx[2]*ptr_zl[2]*ptr_Zl[2]; // update this before Qx !!!!!!!!!!!
		qx[3] = qx[3] - Qx[3]*ptr_zl[3]*ptr_Zl[3]; // update this before Qx !!!!!!!!!!!
		Qx[2] = Qx[2] - Qx[2]*Qx[2]*ptr_Zl[2];
		Qx[3] = Qx[3] - Qx[3]*Qx[3]*ptr_Zl[3];
		ptr_pd[ii+1] = ptr_bd[ii+1] + Qx[3] + Qx[2];
		ptr_pl[ii+1] = ptr_bl[ii+1] + qx[3] - qx[2];

		ptr_tinv[4] = 1.0/ptr_t[4];
		ptr_tinv[5] = 1.0/ptr_t[5];
		ptr_tinv[anb+4] = 1.0/ptr_t[anb+4];
		ptr_tinv[anb+5] = 1.0/ptr_t[anb+5];
		ptr_lamt[4] = ptr_lam[4]*ptr_tinv[4];
		ptr_lamt[5] = ptr_lam[5]*ptr_tinv[5];
		ptr_lamt[anb+4] = ptr_lam[anb+4]*ptr_tinv[anb+4];
		ptr_lamt[anb+5] = ptr_lam[anb+5]*ptr_tinv[anb+5];
		ptr_dlam[4] = ptr_tinv[4]*sigma_mu; // !!!!!
		ptr_dlam[5] = ptr_tinv[5]*sigma_mu; // !!!!!
		ptr_dlam[anb+4] = ptr_tinv[anb+4]*sigma_mu; // !!!!!
		ptr_dlam[anb+5] = ptr_tinv[anb+5]*sigma_mu; // !!!!!
		Qx[4] = ptr_lamt[4];
		Qx[5] = ptr_lamt[5];
		qx[4] = ptr_lam[4] + ptr_dlam[4] + ptr_lamt[4]*ptr_db[4];
		qx[5] = ptr_lam[5] + ptr_dlam[5] + ptr_lamt[5]*ptr_db[5];
		ptr_Zl[4] = 1.0 / (ptr_Z[4] + Qx[4] + ptr_lamt[anb+4]); // inverted of updated diagonal !!!
		ptr_Zl[5] = 1.0 / (ptr_Z[5] + Qx[5] + ptr_lamt[anb+5]); // inverted of updated diagonal !!!
		ptr_zl[4] = - ptr_z[4] + qx[4] + ptr_lam[anb+4] + ptr_dlam[anb+4];
		ptr_zl[5] = - ptr_z[5] + qx[5] + ptr_lam[anb+5] + ptr_dlam[anb+5];
		qx[4] = qx[4] - Qx[4]*ptr_zl[4]*ptr_Zl[4]; // update this before Qx !!!!!!!!!!!
		qx[5] = qx[5] - Qx[5]*ptr_zl[5]*ptr_Zl[5]; // update this before Qx !!!!!!!!!!!
		Qx[4] = Qx[4] - Qx[4]*Qx[4]*ptr_Zl[4];
		Qx[5] = Qx[5] - Qx[5]*Qx[5]*ptr_Zl[5];
		ptr_pd[ii+2] = ptr_bd[ii+2] + Qx[5] + Qx[4];
		ptr_pl[ii+2] = ptr_bl[ii+2] + qx[5] - qx[4];

		ptr_tinv[6] = 1.0/ptr_t[6];
		ptr_tinv[7] = 1.0/ptr_t[7];
		ptr_tinv[anb+6] = 1.0/ptr_t[anb+6];
		ptr_tinv[anb+7] = 1.0/ptr_t[anb+7];
		ptr_lamt[6] = ptr_lam[6]*ptr_tinv[6];
		ptr_lamt[7] = ptr_lam[7]*ptr_tinv[7];
		ptr_lamt[anb+6] = ptr_lam[anb+6]*ptr_tinv[anb+6];
		ptr_lamt[anb+7] = ptr_lam[anb+7]*ptr_tinv[anb+7];
		ptr_dlam[6] = ptr_tinv[6]*sigma_mu; // !!!!!
		ptr_dlam[7] = ptr_tinv[7]*sigma_mu; // !!!!!
		ptr_dlam[anb+6] = ptr_tinv[anb+6]*sigma_mu; // !!!!!
		ptr_dlam[anb+7] = ptr_tinv[anb+7]*sigma_mu; // !!!!!
		Qx[6] = ptr_lamt[6];
		Qx[7] = ptr_lamt[7];
		qx[6] = ptr_lam[6] + ptr_dlam[6] + ptr_lamt[6]*ptr_db[6];
		qx[7] = ptr_lam[7] + ptr_dlam[7] + ptr_lamt[7]*ptr_db[7];
		ptr_Zl[6] = 1.0 / (ptr_Z[6] + Qx[6] + ptr_lamt[anb+6]); // inverted of updated diagonal !!!
		ptr_Zl[7] = 1.0 / (ptr_Z[7] + Qx[7] + ptr_lamt[anb+7]); // inverted of updated diagonal !!!
		ptr_zl[6] = - ptr_z[6] + qx[6] + ptr_lam[anb+6] + ptr_dlam[anb+6];
		ptr_zl[7] = - ptr_z[7] + qx[7] + ptr_lam[anb+7] + ptr_dlam[anb+7];
		qx[6] = qx[6] - Qx[6]*ptr_zl[6]*ptr_Zl[6]; // update this before Qx !!!!!!!!!!!
		qx[7] = qx[7] - Qx[7]*ptr_zl[7]*ptr_Zl[7]; // update this before Qx !!!!!!!!!!!
		Qx[6] = Qx[6] - Qx[6]*Qx[6]*ptr_Zl[6];
		Qx[7] = Qx[7] - Qx[7]*Qx[7]*ptr_Zl[7];
		ptr_pd[ii+3] = ptr_bd[ii+3] + Qx[7] + Qx[6];
		ptr_pl[ii+3] = ptr_bl[ii+3] + qx[7] - qx[6];

		ptr_t     += 8;
		ptr_lam   += 8;
		ptr_lamt  += 8;
		ptr_dlam  += 8;
		ptr_tinv  += 8;
		ptr_db    += 8;
		ptr_Z     += 8;
		ptr_z     += 8;
		ptr_Zl    += 8;
		ptr_zl    += 8;

		}
	for(; ii<nb; ii++)
		{
		ptr_tinv[0] = 1.0/ptr_t[0];
		ptr_tinv[1] = 1.0/ptr_t[1];
		ptr_tinv[anb+0] = 1.0/ptr_t[anb+0];
		ptr_tinv[anb+1] = 1.0/ptr_t[anb+1];
		ptr_lamt[0] = ptr_lam[0]*ptr_tinv[0];
		ptr_lamt[1] = ptr_lam[1]*ptr_tinv[1];
		ptr_lamt[anb+0] = ptr_lam[anb+0]*ptr_tinv[anb+0];
		ptr_lamt[anb+1] = ptr_lam[anb+1]*ptr_tinv[anb+1];
		ptr_dlam[0] = ptr_tinv[0]*sigma_mu; // !!!!!
		ptr_dlam[1] = ptr_tinv[1]*sigma_mu; // !!!!!
		ptr_dlam[anb+0] = ptr_tinv[anb+0]*sigma_mu; // !!!!!
		ptr_dlam[anb+1] = ptr_tinv[anb+1]*sigma_mu; // !!!!!
		Qx[0] = ptr_lamt[0];
		Qx[1] = ptr_lamt[1];
		qx[0] = ptr_lam[0] + ptr_dlam[0] + ptr_lamt[0]*ptr_db[0];
		qx[1] = ptr_lam[1] + ptr_dlam[1] + ptr_lamt[1]*ptr_db[1];
		ptr_Zl[0] = 1.0 / (ptr_Z[0] + Qx[0] + ptr_lamt[anb+0]); // inverted of updated diagonal !!!
		ptr_Zl[1] = 1.0 / (ptr_Z[1] + Qx[1] + ptr_lamt[anb+1]); // inverted of updated diagonal !!!
		ptr_zl[0] = - ptr_z[0] + qx[0] + ptr_lam[anb+0] + ptr_dlam[anb+0];
		ptr_zl[1] = - ptr_z[1] + qx[1] + ptr_lam[anb+1] + ptr_dlam[anb+1];
		qx[0] = qx[0] - Qx[0]*ptr_zl[0]*ptr_Zl[0]; // update this before Qx !!!!!!!!!!!
		qx[1] = qx[1] - Qx[1]*ptr_zl[1]*ptr_Zl[1]; // update this before Qx !!!!!!!!!!!
		Qx[0] = Qx[0] - Qx[0]*Qx[0]*ptr_Zl[0];
		Qx[1] = Qx[1] - Qx[1]*Qx[1]*ptr_Zl[1];
		ptr_pd[ii] = ptr_bd[ii] + Qx[1] + Qx[0];
		ptr_pl[ii] = ptr_bl[ii] + qx[1] - qx[0];

		ptr_t     += 2;
		ptr_lam   += 2;
		ptr_lamt  += 2;
		ptr_dlam  += 2;
		ptr_tinv  += 2;
		ptr_db    += 2;
		ptr_Z     += 2;
		ptr_z     += 2;
		ptr_Zl    += 2;
		ptr_zl    += 2;
		}
	for( ; ii<nu+nx; ii++)
		{
		pd[jj][ii] = bd[jj][ii];
		pl[jj][ii] = bl[jj][ii];
		}


	}



void d_update_gradient_hard_mpc(int N, int nx, int nu, int nb, double sigma_mu, double **dt, double **dlam, double **t_inv, double **pl2)
	{

	const int nbu = nu<nb ? nu : nb ;

	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	//const int anb = nal*((nb+nal-1)/nal); // cache aligned number of box and soft constraints
	const int pnb = bs*((nb+bs-1)/bs); // simd aligned number of box and soft constraints

	int ii, jj;

	// first stage
	for(ii=0; ii<nbu; ii++)
		{
		dlam[0][ii]     = t_inv[0][ii]    *(sigma_mu - dlam[0][ii]*dt[0][ii]); // !!!!!
		dlam[0][pnb+ii] = t_inv[0][pnb+ii]*(sigma_mu - dlam[0][pnb+ii]*dt[0][pnb+ii]); // !!!!!
		pl2[0][ii] += dlam[0][pnb+ii] - dlam[0][ii];
		}

	// middle stages
	for(jj=1; jj<N; jj++)
		{
		for(ii=0; ii<nb; ii++)
			{
			dlam[jj][ii]     = t_inv[jj][ii]    *(sigma_mu - dlam[jj][ii]*dt[jj][ii]); // !!!!!
			dlam[jj][pnb+ii] = t_inv[jj][pnb+ii]*(sigma_mu - dlam[jj][pnb+ii]*dt[jj][pnb+ii]); // !!!!!
			pl2[jj][ii] += dlam[jj][pnb+ii] - dlam[jj][ii];
			}
		}

	// last stages
	for(ii=nu; ii<nb; ii++)
		{
		dlam[jj][ii]     = t_inv[jj][ii]    *(sigma_mu - dlam[jj][ii]*dt[jj][ii]); // !!!!!
		dlam[jj][pnb+ii] = t_inv[jj][pnb+ii]*(sigma_mu - dlam[jj][pnb+ii]*dt[jj][pnb+ii]); // !!!!!
		pl2[jj][ii] += dlam[jj][pnb+ii] - dlam[jj][ii];
		}

	}



void d_update_gradient_soft_mpc(int N, int nx, int nu, int nh, int ns, double sigma_mu, double **dt, double **dlam, double **t_inv, double **lamt, double **pl2, double **Zl, double **zl)
	{

	int nb = nh + ns;

	int nhu = nu<nh ? nu : nh ;

	// constants
	const int bs = 4; //D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	const int anb = nal*((2*nb+nal-1)/nal); // cache aligned number of box and soft constraints // !!!!! doubled to include soft constraints !!!!!

	int ii, jj;
	
	static double Qx[2] = {};
	static double qx[2] = {};


	// first stage
	jj = 0;
	for(ii=0; ii<2*nhu; ii+=2)
		{
		dlam[0][ii+0] = t_inv[0][ii+0]*(sigma_mu - dlam[0][ii+0]*dt[0][ii+0]); // !!!!!
		dlam[0][ii+1] = t_inv[0][ii+1]*(sigma_mu - dlam[0][ii+1]*dt[0][ii+1]); // !!!!!
		pl2[0][ii/2] += dlam[0][ii+1] - dlam[0][ii+0];
		}

	// middle stages
	for(jj=1; jj<N; jj++)
		{
		ii=0;
		for(; ii<2*nh; ii+=2)
			{
			dlam[jj][ii+0] = t_inv[jj][ii+0]*(sigma_mu - dlam[jj][ii+0]*dt[jj][ii+0]); // !!!!!
			dlam[jj][ii+1] = t_inv[jj][ii+1]*(sigma_mu - dlam[jj][ii+1]*dt[jj][ii+1]); // !!!!!
			pl2[jj][ii/2] += dlam[jj][ii+1] - dlam[jj][ii+0];
			}
		for(; ii<2*nb; ii+=2)
			{
			dlam[jj][ii+0] = t_inv[jj][ii+0]*(sigma_mu - dlam[jj][ii+0]*dt[jj][ii+0]); // !!!!!
			dlam[jj][ii+1] = t_inv[jj][ii+1]*(sigma_mu - dlam[jj][ii+1]*dt[jj][ii+1]); // !!!!!
			dlam[jj][anb+ii+0] = t_inv[jj][anb+ii+0]*(sigma_mu - dlam[jj][anb+ii+0]*dt[jj][anb+ii+0]); // !!!!!
			dlam[jj][anb+ii+1] = t_inv[jj][anb+ii+1]*(sigma_mu - dlam[jj][anb+ii+1]*dt[jj][anb+ii+1]); // !!!!!
			Qx[0] = lamt[jj][ii+0];
			Qx[1] = lamt[jj][ii+1];
			//qx[0] = ptr_lam[0] + ptr_dlam[0] + ptr_lamt[0]*ptr_db[0];
			//qx[1] = ptr_lam[1] + ptr_dlam[1] + ptr_lamt[1]*ptr_db[1];
			qx[0] = dlam[jj][ii+0];
			qx[1] = dlam[jj][ii+1];
			//ptr_zl[0] = ptr_z[0] + qx[0] + ptr_lam[anb+0] + ptr_dlam[anb+0];
			//ptr_zl[1] = ptr_z[1] + qx[1] + ptr_lam[anb+1] + ptr_dlam[anb+1];
			zl[jj][ii+0] += qx[0] + dlam[jj][anb+ii+0];
			zl[jj][ii+1] += qx[1] + dlam[jj][anb+ii+1];
			//qx[0] = qx[0] - Qx[0]*ptr_zl[0]*ptr_Zl[0]; // update this before Qx !!!!!!!!!!!
			//qx[1] = qx[1] - Qx[1]*ptr_zl[1]*ptr_Zl[1]; // update this before Qx !!!!!!!!!!!
			qx[0] = qx[0] - Qx[0]*(qx[0] + dlam[jj][anb+ii+0])*Zl[jj][ii+0]; // update this before Qx !!!!!!!!!!!
			qx[1] = qx[1] - Qx[1]*(qx[1] + dlam[jj][anb+ii+1])*Zl[jj][ii+1]; // update this before Qx !!!!!!!!!!!
			pl2[jj][ii/2] += qx[1] - qx[0];
			}
		}

	// last stages
	jj = N;
	ii=2*nu;
	for(; ii<2*nh; ii+=2)
		{
		dlam[jj][ii+0] = t_inv[jj][ii+0]*(sigma_mu - dlam[jj][ii+0]*dt[jj][ii+0]); // !!!!!
		dlam[jj][ii+1] = t_inv[jj][ii+1]*(sigma_mu - dlam[jj][ii+1]*dt[jj][ii+1]); // !!!!!
		pl2[jj][ii/2] += dlam[jj][ii+1] - dlam[jj][ii+0];
		}
	for(; ii<2*nb; ii+=2)
		{
		dlam[N][ii+0] = t_inv[N][ii+0]*(sigma_mu - dlam[N][ii+0]*dt[N][ii+0]); // !!!!!
		dlam[N][ii+1] = t_inv[N][ii+1]*(sigma_mu - dlam[N][ii+1]*dt[N][ii+1]); // !!!!!
		dlam[N][anb+ii+0] = t_inv[N][anb+ii+0]*(sigma_mu - dlam[N][anb+ii+0]*dt[N][anb+ii+0]); // !!!!!
		dlam[N][anb+ii+1] = t_inv[N][anb+ii+1]*(sigma_mu - dlam[N][anb+ii+1]*dt[N][anb+ii+1]); // !!!!!
		Qx[0] = lamt[N][ii+0];
		Qx[1] = lamt[N][ii+1];
		//qx[0] = ptr_lam[0] + ptr_dlam[0] + ptr_lamt[0]*ptr_db[0];
		//qx[1] = ptr_lam[1] + ptr_dlam[1] + ptr_lamt[1]*ptr_db[1];
		qx[0] = dlam[N][ii+0];
		qx[1] = dlam[N][ii+1];
		//ptr_zl[0] = ptr_z[0] + qx[0] + ptr_lam[anb+0] + ptr_dlam[anb+0];
		//ptr_zl[1] = ptr_z[1] + qx[1] + ptr_lam[anb+1] + ptr_dlam[anb+1];
		zl[N][ii+0] += qx[0] + dlam[N][anb+ii+0];
		zl[N][ii+1] += qx[1] + dlam[N][anb+ii+1];
		//qx[0] = qx[0] - Qx[0]*ptr_zl[0]*ptr_Zl[0]; // update this before Qx !!!!!!!!!!!
		//qx[1] = qx[1] - Qx[1]*ptr_zl[1]*ptr_Zl[1]; // update this before Qx !!!!!!!!!!!
		qx[0] = qx[0] - Qx[0]*(qx[0] + dlam[N][anb+ii+0])*Zl[N][ii+0]; // update this before Qx !!!!!!!!!!!
		qx[1] = qx[1] - Qx[1]*(qx[1] + dlam[N][anb+ii+1])*Zl[N][ii+1]; // update this before Qx !!!!!!!!!!!
		pl2[N][ii/2] += qx[1] - qx[0];
		}

	}



void d_compute_alpha_hard_mpc(int N, int nx, int nu, int nb, double *ptr_alpha, double **t, double **dt, double **lam, double **dlam, double **lamt, double **dux, double **db)
	{
	
/*	const int bs = 4; //d_get_mr();*/

	const int nbu = nu<nb ? nu : nb ;
	
	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	//const int anb = nal*((nb+nal-1)/nal); // cache aligned number of box and soft constraints
	const int pnb = bs*((nb+bs-1)/bs); // simd aligned number of box and soft constraints

	double alpha = ptr_alpha[0];
	
/*	int kna = ((k1+bs-1)/bs)*bs;*/

	int jj, ll;


	// first stage

	ll = 0;
	for(; ll<nbu; ll++)
		{

		dt[0][ll+0]   =   dux[0][ll] - db[0][ll+0]   - t[0][ll+0];
		dt[0][ll+pnb] = - dux[0][ll] - db[0][ll+pnb] - t[0][ll+pnb];
		dlam[0][ll+0]   -= lamt[0][ll+0]   * dt[0][ll+0]   + lam[0][ll+0];
		dlam[0][ll+pnb] -= lamt[0][ll+pnb] * dt[0][ll+pnb] + lam[0][ll+pnb];
		if( -alpha*dlam[0][ll+0]>lam[0][ll+0] )
			{
			alpha = - lam[0][ll+0] / dlam[0][ll+0];
			}
		if( -alpha*dlam[0][ll+pnb]>lam[0][ll+pnb] )
			{
			alpha = - lam[0][ll+pnb] / dlam[0][ll+pnb];
			}
		if( -alpha*dt[0][ll+0]>t[0][ll+0] )
			{
			alpha = - t[0][ll+0] / dt[0][ll+0];
			}
		if( -alpha*dt[0][ll+pnb]>t[0][ll+pnb] )
			{
			alpha = - t[0][ll+pnb] / dt[0][ll+pnb];
			}

		}

	// middle stages
	for(jj=1; jj<N; jj++)
		{

		ll = 0;
		for(; ll<nb; ll++)
			{

			dt[jj][ll+0]   =   dux[jj][ll] - db[jj][ll+0]   - t[jj][ll+0];
			dt[jj][ll+pnb] = - dux[jj][ll] - db[jj][ll+pnb] - t[jj][ll+pnb];
			dlam[jj][ll+0]   -= lamt[jj][ll+0]   * dt[jj][ll+0]   + lam[jj][ll+0];
			dlam[jj][ll+pnb] -= lamt[jj][ll+pnb] * dt[jj][ll+pnb] + lam[jj][ll+pnb];
			if( -alpha*dlam[jj][ll+0]>lam[jj][ll+0] )
				{
				alpha = - lam[jj][ll+0] / dlam[jj][ll+0];
				}
			if( -alpha*dlam[jj][ll+pnb]>lam[jj][ll+pnb] )
				{
				alpha = - lam[jj][ll+pnb] / dlam[jj][ll+pnb];
				}
			if( -alpha*dt[jj][ll+0]>t[jj][ll+0] )
				{
				alpha = - t[jj][ll+0] / dt[jj][ll+0];
				}
			if( -alpha*dt[jj][ll+pnb]>t[jj][ll+pnb] )
				{
				alpha = - t[jj][ll+pnb] / dt[jj][ll+pnb];
				}

			}

		}		

	// last stage
	ll = nu;
	for(; ll<nb; ll++)
		{

		dt[N][ll+0]   =   dux[N][ll] - db[N][ll+0]   - t[N][ll+0];
		dt[N][ll+pnb] = - dux[N][ll] - db[N][ll+pnb] - t[N][ll+pnb];
		dlam[N][ll+0]   -= lamt[N][ll+0]   * dt[N][ll+0]   + lam[N][ll+0];
		dlam[N][ll+pnb] -= lamt[N][ll+pnb] * dt[N][ll+pnb] + lam[N][ll+pnb];
		if( -alpha*dlam[N][ll+0]>lam[N][ll+0] )
			{
			alpha = - lam[N][ll+0] / dlam[N][ll+0];
			}
		if( -alpha*dlam[N][ll+pnb]>lam[N][ll+pnb] )
			{
			alpha = - lam[N][ll+pnb] / dlam[N][ll+pnb];
			}
		if( -alpha*dt[N][ll+0]>t[N][ll+0] )
			{
			alpha = - t[N][ll+0] / dt[N][ll+0];
			}
		if( -alpha*dt[N][ll+pnb]>t[N][ll+pnb] )
			{
			alpha = - t[N][ll+pnb] / dt[N][ll+pnb];
			}

		}
	
	ptr_alpha[0] = alpha;

	return;
	
	}



void d_compute_alpha_soft_mpc(int N, int nx, int nu, int nh, int ns, double *ptr_alpha, double **t, double **dt, double **lam, double **dlam, double **lamt, double **dux, double **db, double **Zl, double **zl)
	{
	
/*	const int bs = 4; //d_get_mr();*/

	int nb = nh + ns;

	int nhu = nu<nh ? nu : nh ;

	// constants
	const int bs = 4; //D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	const int anb = nal*((2*nb+nal-1)/nal); // cache aligned number of box and soft constraints // !!!!! doubled to include soft constraints !!!!!

	
	double alpha = ptr_alpha[0];
	
/*	int kna = ((k1+bs-1)/bs)*bs;*/

	int jj, ll;


	// first stage

	ll = 0;
	// hard input constraints
	for(; ll<nhu; ll++)
		{

		dt[0][2*ll+0] =   dux[0][ll] - db[0][2*ll+0] - t[0][2*ll+0];
		dt[0][2*ll+1] = - dux[0][ll] - db[0][2*ll+1] - t[0][2*ll+1];
		dlam[0][2*ll+0] -= lamt[0][2*ll+0] * dt[0][2*ll+0] + lam[0][2*ll+0];
		dlam[0][2*ll+1] -= lamt[0][2*ll+1] * dt[0][2*ll+1] + lam[0][2*ll+1];
		if( -alpha*dlam[0][2*ll+0]>lam[0][2*ll+0] )
			{
			alpha = - lam[0][2*ll+0] / dlam[0][2*ll+0];
			}
		if( -alpha*dlam[0][2*ll+1]>lam[0][2*ll+1] )
			{
			alpha = - lam[0][2*ll+1] / dlam[0][2*ll+1];
			}
		if( -alpha*dt[0][2*ll+0]>t[0][2*ll+0] )
			{
			alpha = - t[0][2*ll+0] / dt[0][2*ll+0];
			}
		if( -alpha*dt[0][2*ll+1]>t[0][2*ll+1] )
			{
			alpha = - t[0][2*ll+1] / dt[0][2*ll+1];
			}

		}

	// middle stages
	for(jj=1; jj<N; jj++)
		{

		ll = 0;
		// hard input and state constraints
		for(; ll<nh; ll++)
			{

			dt[jj][2*ll+0] =   dux[jj][ll] - db[jj][2*ll+0] - t[jj][2*ll+0];
			dt[jj][2*ll+1] = - dux[jj][ll] - db[jj][2*ll+1] - t[jj][2*ll+1];
			dlam[jj][2*ll+0] -= lamt[jj][2*ll+0] * dt[jj][2*ll+0] + lam[jj][2*ll+0];
			dlam[jj][2*ll+1] -= lamt[jj][2*ll+1] * dt[jj][2*ll+1] + lam[jj][2*ll+1];
			if( -alpha*dlam[jj][2*ll+0]>lam[jj][2*ll+0] )
				{
				alpha = - lam[jj][2*ll+0] / dlam[jj][2*ll+0];
				}
			if( -alpha*dlam[jj][2*ll+1]>lam[jj][2*ll+1] )
				{
				alpha = - lam[jj][2*ll+1] / dlam[jj][2*ll+1];
				}
			if( -alpha*dt[jj][2*ll+0]>t[jj][2*ll+0] )
				{
				alpha = - t[jj][2*ll+0] / dt[jj][2*ll+0];
				}
			if( -alpha*dt[jj][2*ll+1]>t[jj][2*ll+1] )
				{
				alpha = - t[jj][2*ll+1] / dt[jj][2*ll+1];
				}

			}
		// soft state constraints
		for(; ll<nb; ll++)
			{

			dt[jj][anb+2*ll+0] = ( zl[jj][2*ll+0] - lamt[jj][2*ll+0]*dux[jj][ll] ) * Zl[jj][2*ll+0];
			dt[jj][anb+2*ll+1] = ( zl[jj][2*ll+1] + lamt[jj][2*ll+1]*dux[jj][ll] ) * Zl[jj][2*ll+1];
			dt[jj][2*ll+0] = dt[jj][anb+2*ll+0] + dux[jj][ll] - db[jj][2*ll+0] - t[jj][2*ll+0];
			dt[jj][2*ll+1] = dt[jj][anb+2*ll+1] - dux[jj][ll] - db[jj][2*ll+1] - t[jj][2*ll+1];
			dt[jj][anb+2*ll+0] -= t[jj][anb+2*ll+0];
			dt[jj][anb+2*ll+1] -= t[jj][anb+2*ll+1];
			dlam[jj][2*ll+0] -= lamt[jj][2*ll+0] * dt[jj][2*ll+0] + lam[jj][2*ll+0];
			dlam[jj][2*ll+1] -= lamt[jj][2*ll+1] * dt[jj][2*ll+1] + lam[jj][2*ll+1];
			dlam[jj][anb+2*ll+0] -= lamt[jj][anb+2*ll+0] * dt[jj][anb+2*ll+0] + lam[jj][anb+2*ll+0];
			dlam[jj][anb+2*ll+1] -= lamt[jj][anb+2*ll+1] * dt[jj][anb+2*ll+1] + lam[jj][anb+2*ll+1];
			if( -alpha*dlam[jj][2*ll+0]>lam[jj][2*ll+0] )
				{
				alpha = - lam[jj][2*ll+0] / dlam[jj][2*ll+0];
				}
			if( -alpha*dlam[jj][2*ll+1]>lam[jj][2*ll+1] )
				{
				alpha = - lam[jj][2*ll+1] / dlam[jj][2*ll+1];
				}
			if( -alpha*dt[jj][2*ll+0]>t[jj][2*ll+0] )
				{
				alpha = - t[jj][2*ll+0] / dt[jj][2*ll+0];
				}
			if( -alpha*dt[jj][2*ll+1]>t[jj][2*ll+1] )
				{
				alpha = - t[jj][2*ll+1] / dt[jj][2*ll+1];
				}
			if( -alpha*dlam[jj][anb+2*ll+0]>lam[jj][anb+2*ll+0] )
				{
				alpha = - lam[jj][anb+2*ll+0] / dlam[jj][anb+2*ll+0];
				}
			if( -alpha*dlam[jj][anb+2*ll+1]>lam[jj][anb+2*ll+1] )
				{
				alpha = - lam[jj][anb+2*ll+1] / dlam[jj][anb+2*ll+1];
				}
			if( -alpha*dt[jj][anb+2*ll+0]>t[jj][anb+2*ll+0] )
				{
				alpha = - t[jj][anb+2*ll+0] / dt[jj][anb+2*ll+0];
				}
			if( -alpha*dt[jj][anb+2*ll+1]>t[jj][anb+2*ll+1] )
				{
				alpha = - t[jj][anb+2*ll+1] / dt[jj][anb+2*ll+1];
				}

			}

		}		

	// last stage
	jj = N;
	ll = nu;
	// hard state constraints
	for(; ll<nh; ll++)
		{

		dt[jj][2*ll+0] =   dux[jj][ll] - db[jj][2*ll+0] - t[jj][2*ll+0];
		dt[jj][2*ll+1] = - dux[jj][ll] - db[jj][2*ll+1] - t[jj][2*ll+1];
		dlam[jj][2*ll+0] -= lamt[jj][2*ll+0] * dt[jj][2*ll+0] + lam[jj][2*ll+0];
		dlam[jj][2*ll+1] -= lamt[jj][2*ll+1] * dt[jj][2*ll+1] + lam[jj][2*ll+1];
		if( -alpha*dlam[jj][2*ll+0]>lam[jj][2*ll+0] )
			{
			alpha = - lam[jj][2*ll+0] / dlam[jj][2*ll+0];
			}
		if( -alpha*dlam[jj][2*ll+1]>lam[jj][2*ll+1] )
			{
			alpha = - lam[jj][2*ll+1] / dlam[jj][2*ll+1];
			}
		if( -alpha*dt[jj][2*ll+0]>t[jj][2*ll+0] )
			{
			alpha = - t[jj][2*ll+0] / dt[jj][2*ll+0];
			}
		if( -alpha*dt[jj][2*ll+1]>t[jj][2*ll+1] )
			{
			alpha = - t[jj][2*ll+1] / dt[jj][2*ll+1];
			}

		}
	// soft state constraints
	for(; ll<nb; ll++)
		{

		dt[N][anb+2*ll+0] = ( zl[N][2*ll+0] - lamt[N][2*ll+0]*dux[N][ll] ) * Zl[N][2*ll+0];
		dt[N][anb+2*ll+1] = ( zl[N][2*ll+1] + lamt[N][2*ll+1]*dux[N][ll] ) * Zl[N][2*ll+1];
		dt[N][2*ll+0] = dt[N][anb+2*ll+0] + dux[N][ll] - db[N][2*ll+0] - t[N][2*ll+0];
		dt[N][2*ll+1] = dt[N][anb+2*ll+1] - dux[N][ll] - db[N][2*ll+1] - t[N][2*ll+1];
		dt[N][anb+2*ll+0] -= t[N][anb+2*ll+0];
		dt[N][anb+2*ll+1] -= t[N][anb+2*ll+1];
		dlam[N][2*ll+0] -= lamt[N][2*ll+0] * dt[N][2*ll+0] + lam[N][2*ll+0];
		dlam[N][2*ll+1] -= lamt[N][2*ll+1] * dt[N][2*ll+1] + lam[N][2*ll+1];
		dlam[N][anb+2*ll+0] -= lamt[N][anb+2*ll+0] * dt[N][anb+2*ll+0] + lam[N][anb+2*ll+0];
		dlam[N][anb+2*ll+1] -= lamt[N][anb+2*ll+1] * dt[N][anb+2*ll+1] + lam[N][anb+2*ll+1];
		if( -alpha*dlam[N][2*ll+0]>lam[N][2*ll+0] )
			{
			alpha = - lam[N][2*ll+0] / dlam[N][2*ll+0];
			}
		if( -alpha*dlam[N][2*ll+1]>lam[N][2*ll+1] )
			{
			alpha = - lam[N][2*ll+1] / dlam[N][2*ll+1];
			}
		if( -alpha*dt[N][2*ll+0]>t[N][2*ll+0] )
			{
			alpha = - t[N][2*ll+0] / dt[N][2*ll+0];
			}
		if( -alpha*dt[N][2*ll+1]>t[N][2*ll+1] )
			{
			alpha = - t[N][2*ll+1] / dt[N][2*ll+1];
			}
		if( -alpha*dlam[N][anb+2*ll+0]>lam[N][anb+2*ll+0] )
			{
			alpha = - lam[N][anb+2*ll+0] / dlam[N][anb+2*ll+0];
			}
		if( -alpha*dlam[N][anb+2*ll+1]>lam[N][anb+2*ll+1] )
			{
			alpha = - lam[N][anb+2*ll+1] / dlam[N][anb+2*ll+1];
			}
		if( -alpha*dt[N][anb+2*ll+0]>t[N][anb+2*ll+0] )
			{
			alpha = - t[N][anb+2*ll+0] / dt[N][anb+2*ll+0];
			}
		if( -alpha*dt[N][anb+2*ll+1]>t[N][anb+2*ll+1] )
			{
			alpha = - t[N][anb+2*ll+1] / dt[N][anb+2*ll+1];
			}


		}
	
	ptr_alpha[0] = alpha;

	return;
	
	}



void d_update_var_hard_mpc(int N, int nx, int nu, int nb, double *ptr_mu, double mu_scal, double alpha, double **ux, double **dux, double **t, double **dt, double **lam, double **dlam, double **pi, double **dpi)
	{

	const int nbu = nu<nb ? nu : nb ;
	
	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	//const int anb = nal*((nb+nal-1)/nal); // cache aligned number of box and soft constraints
	const int pnb = bs*((nb+bs-1)/bs); // cache aligned number of box and soft constraints

	int jj, ll;
	
	double mu = 0;

	// update inputs
	for(ll=0; ll<nu; ll++)
		ux[0][ll] += alpha*(dux[0][ll] - ux[0][ll]);
	// box constraints
	for(ll=0; ll<nbu; ll++)
		{
		lam[0][ll+0]   += alpha*dlam[0][ll+0];
		lam[0][ll+pnb] += alpha*dlam[0][ll+pnb];
		t[0][ll+0]   += alpha*dt[0][ll+0];
		t[0][ll+pnb] += alpha*dt[0][ll+pnb];
		mu += lam[0][ll+0] * t[0][ll+0] + lam[0][ll+pnb] * t[0][ll+pnb];
		}

	for(jj=1; jj<N; jj++)
		{
		// update inputs
		for(ll=0; ll<nu; ll++)
			ux[jj][ll] += alpha*(dux[jj][ll] - ux[jj][ll]);
		// update states
		for(ll=0; ll<nx; ll++)
			ux[jj][nu+ll] += alpha*(dux[jj][nu+ll] - ux[jj][nu+ll]);
		// update equality constrained multipliers
		for(ll=0; ll<nx; ll++)
			pi[jj][ll] += alpha*(dpi[jj][ll] - pi[jj][ll]);
		// box constraints
		for(ll=0; ll<nb; ll++)
			{
			lam[jj][ll+0] += alpha*dlam[jj][ll+0];
			lam[jj][ll+pnb] += alpha*dlam[jj][ll+pnb];
			t[jj][ll+0] += alpha*dt[jj][ll+0];
			t[jj][ll+pnb] += alpha*dt[jj][ll+pnb];
			mu += lam[jj][ll+0] * t[jj][ll+0] + lam[jj][ll+pnb] * t[jj][ll+pnb];
			}
		}

	// update states
	for(ll=0; ll<nx; ll++)
		ux[N][nu+ll] += alpha*(dux[N][nu+ll] - ux[N][nu+ll]);
	// update equality constrained multipliers
	for(ll=0; ll<nx; ll++)
		pi[N][ll] += alpha*(dpi[N][ll] - pi[N][ll]);
	// box constraints
	for(ll=nu; ll<nb; ll++)
		{
		lam[N][ll+0] += alpha*dlam[N][ll+0];
		lam[N][ll+pnb] += alpha*dlam[N][ll+pnb];
		t[N][ll+0] += alpha*dt[N][ll+0];
		t[N][ll+pnb] += alpha*dt[N][ll+pnb];
		mu += lam[N][ll+0] * t[N][ll+0] + lam[N][ll+pnb] * t[N][ll+pnb];
		}
	mu *= mu_scal;

	ptr_mu[0] = mu;

	return;
	
	}



void d_update_var_soft_mpc(int N, int nx, int nu, int nh, int ns, double *ptr_mu, double mu_scal, double alpha, double **ux, double **dux, double **t, double **dt, double **lam, double **dlam, double **pi, double **dpi)
	{

	int nb = nh + ns;

	int nhu = nu<nh ? nu : nh ;

	// constants
	const int bs = 4; //D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	const int anb = nal*((2*nb+nal-1)/nal); // cache aligned number of box and soft constraints // !!!!! doubled to include soft constraints !!!!!

	int jj, ll;
	
	double mu = 0;

	// initial stage
	// update inputs
	for(ll=0; ll<nu; ll++)
		ux[0][ll] += alpha*(dux[0][ll] - ux[0][ll]);
	// box constraints on inputs
	for(ll=0; ll<2*nhu; ll+=2)
		{
		lam[0][ll+0] += alpha*dlam[0][ll+0];
		lam[0][ll+1] += alpha*dlam[0][ll+1];
		t[0][ll+0] += alpha*dt[0][ll+0];
		t[0][ll+1] += alpha*dt[0][ll+1];
		mu += lam[0][ll+0] * t[0][ll+0] + lam[0][ll+1] * t[0][ll+1];
		}

	// middle stages
	for(jj=1; jj<N; jj++)
		{
		// update inputs
		for(ll=0; ll<nu; ll++)
			ux[jj][ll] += alpha*(dux[jj][ll] - ux[jj][ll]);
		// update states
		for(ll=0; ll<nx; ll++)
			ux[jj][nu+ll] += alpha*(dux[jj][nu+ll] - ux[jj][nu+ll]);
		// update equality constrained multipliers
		for(ll=0; ll<nx; ll++)
			pi[jj][ll] += alpha*(dpi[jj][ll] - pi[jj][ll]);
		// box constraints on inputs and states
		ll = 0;
		for(; ll<2*nh; ll+=2)
			{
			lam[jj][ll+0] += alpha*dlam[jj][ll+0];
			lam[jj][ll+1] += alpha*dlam[jj][ll+1];
			t[jj][ll+0] += alpha*dt[jj][ll+0];
			t[jj][ll+1] += alpha*dt[jj][ll+1];
			mu += lam[jj][ll+0] * t[jj][ll+0] + lam[jj][ll+1] * t[jj][ll+1];
			}
		// soft constraints on states
		for(; ll<2*nb; ll+=2)
			{
			lam[jj][ll+0] += alpha*dlam[jj][ll+0];
			lam[jj][ll+1] += alpha*dlam[jj][ll+1];
			t[jj][ll+0] += alpha*dt[jj][ll+0];
			t[jj][ll+1] += alpha*dt[jj][ll+1];
			lam[jj][anb+ll+0] += alpha*dlam[jj][anb+ll+0];
			lam[jj][anb+ll+1] += alpha*dlam[jj][anb+ll+1];
			t[jj][anb+ll+0] += alpha*dt[jj][anb+ll+0];
			t[jj][anb+ll+1] += alpha*dt[jj][anb+ll+1];
			mu += lam[jj][ll+0] * t[jj][ll+0] + lam[jj][ll+1] * t[jj][ll+1] + lam[jj][anb+ll+0] * t[jj][anb+ll+0] + lam[jj][anb+ll+1] * t[jj][anb+ll+1];
			}
		}

	// final stage
	// update states
	for(ll=0; ll<nx; ll++)
		ux[N][nu+ll] += alpha*(dux[N][nu+ll] - ux[N][nu+ll]);
	// update equality constrained multipliers
	for(ll=0; ll<nx; ll++)
		pi[N][ll] += alpha*(dpi[N][ll] - pi[N][ll]);
	ll=2*nu;
	// box constraints on states
	for(; ll<2*nh; ll+=2)
		{
		lam[jj][ll+0] += alpha*dlam[jj][ll+0];
		lam[jj][ll+1] += alpha*dlam[jj][ll+1];
		t[jj][ll+0] += alpha*dt[jj][ll+0];
		t[jj][ll+1] += alpha*dt[jj][ll+1];
		mu += lam[jj][ll+0] * t[jj][ll+0] + lam[jj][ll+1] * t[jj][ll+1];
		}
	// soft constraints on states
	for(; ll<2*nb; ll+=2)
		{
		lam[N][ll+0] += alpha*dlam[N][ll+0];
		lam[N][ll+1] += alpha*dlam[N][ll+1];
		t[N][ll+0] += alpha*dt[N][ll+0];
		t[N][ll+1] += alpha*dt[N][ll+1];
		lam[N][anb+ll+0] += alpha*dlam[N][anb+ll+0];
		lam[N][anb+ll+1] += alpha*dlam[N][anb+ll+1];
		t[N][anb+ll+0] += alpha*dt[N][anb+ll+0];
		t[N][anb+ll+1] += alpha*dt[N][anb+ll+1];
		mu += lam[N][ll+0] * t[N][ll+0] + lam[N][ll+1] * t[N][ll+1] + lam[N][anb+ll+0] * t[N][anb+ll+0] + lam[N][anb+ll+1] * t[N][anb+ll+1];
		}
	mu *= mu_scal;

	ptr_mu[0] = mu;

	return;
	
	}



void d_compute_mu_hard_mpc(int N, int nx, int nu, int nb, double *ptr_mu, double mu_scal, double alpha, double **lam, double **dlam, double **t, double **dt)
	{
	
	const int nbu = nu<nb ? nu : nb ;

	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	//const int anb = nal*((nb+nal-1)/nal); // cache aligned number of box and soft constraints
	const int pnb = bs*((nb+bs-1)/bs); // simd aligned number of box and soft constraints

	int jj, ll;
	
	double mu = 0;
	
	for(ll=0 ; ll<nbu; ll++)
		{
		mu += (lam[0][ll+0] + alpha*dlam[0][ll+0]) * (t[0][ll+0] + alpha*dt[0][ll+0]) + (lam[0][ll+pnb] + alpha*dlam[0][ll+pnb]) * (t[0][ll+pnb] + alpha*dt[0][ll+pnb]);
		}


	for(jj=1; jj<N; jj++)
		for(ll=0 ; ll<nb; ll++)
			mu += (lam[jj][ll+0] + alpha*dlam[jj][ll+0]) * (t[jj][ll+0] + alpha*dt[jj][ll+0]) + (lam[jj][ll+pnb] + alpha*dlam[jj][ll+pnb]) * (t[jj][ll+pnb] + alpha*dt[jj][ll+pnb]);

	for(ll=nu ; ll<nb; ll++)
		mu += (lam[N][ll+0] + alpha*dlam[N][ll+0]) * (t[N][ll+0] + alpha*dt[N][ll+0]) + (lam[N][ll+pnb] + alpha*dlam[N][ll+pnb]) * (t[N][ll+pnb] + alpha*dt[N][ll+pnb]);

	mu *= mu_scal;
		
	ptr_mu[0] = mu;

	return;

	}



void d_compute_mu_soft_mpc(int N, int nx, int nu, int nh, int ns, double *ptr_mu, double mu_scal, double alpha, double **lam, double **dlam, double **t, double **dt)
	{

	int nb = nh + ns;
	
	int nhu = nu<nh ? nu : nh ;

	// constants
	const int bs = D_MR;
	const int ncl = D_NCL;
	const int nal = bs*ncl; // number of doubles per cache line

	const int anb = nal*((2*nb+nal-1)/nal); // cache aligned number of box and soft constraints // !!!!! doubled to include soft constraints !!!!!

	int jj, ll;
	
	double mu = 0;
	
	// fist stage: bounds on u only
	for(ll=0; ll<2*nhu; ll+=2)
		{
		mu += (lam[0][ll+0] + alpha*dlam[0][ll+0]) * (t[0][ll+0] + alpha*dt[0][ll+0]) + (lam[0][ll+1] + alpha*dlam[0][ll+1]) * (t[0][ll+1] + alpha*dt[0][ll+1]);
		}

	// middle stages: bounds on both u and x
	for(jj=1; jj<N; jj++)
		{
		for(ll=0; ll<2*nb; ll+=2)
			mu += (lam[jj][ll+0] + alpha*dlam[jj][ll+0]) * (t[jj][ll+0] + alpha*dt[jj][ll+0]) + (lam[jj][ll+1] + alpha*dlam[jj][ll+1]) * (t[jj][ll+1] + alpha*dt[jj][ll+1]);
		for(ll=anb+2*nh; ll<anb+2*nb; ll+=2)
			mu += (lam[jj][ll+0] + alpha*dlam[jj][ll+0]) * (t[jj][ll+0] + alpha*dt[jj][ll+0]) + (lam[jj][ll+1] + alpha*dlam[jj][ll+1]) * (t[jj][ll+1] + alpha*dt[jj][ll+1]);
		}	

	// last stage: bounds on x only
	for(ll=2*nu; ll<2*nb; ll+=2)
		mu += (lam[N][ll+0] + alpha*dlam[N][ll+0]) * (t[N][ll+0] + alpha*dt[N][ll+0]) + (lam[N][ll+1] + alpha*dlam[N][ll+1]) * (t[N][ll+1] + alpha*dt[N][ll+1]);
	for(ll=anb+2*nh; ll<anb+2*nb; ll+=2)
		mu += (lam[N][ll+0] + alpha*dlam[N][ll+0]) * (t[N][ll+0] + alpha*dt[N][ll+0]) + (lam[N][ll+1] + alpha*dlam[N][ll+1]) * (t[N][ll+1] + alpha*dt[N][ll+1]);

	mu *= mu_scal;
		
	ptr_mu[0] = mu;

	return;

	}


