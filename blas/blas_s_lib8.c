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

#include "../include/kernel_s_lib8.h"
#include "../include/block_size.h"



/* preforms                                          */
/* C  = A * B' (alg== 0)                             */
/* C += A * B' (alg== 1)                             */
/* C -= A * B' (alg==-1)                             */
/* where A, B and C are packed with block size 4     */
void sgemm_nt_lib(int m, int n, int k, float *pA, int sda, float *pB, int sdb, float *pC, int sdc, int alg)
	{

	const int bs = 8;

	int i, j, jj;
	
	i = 0;
#if defined(TARGET_X64_AVX2)
	for(; i<m-16; i+=24)
		{
		j = 0;
		for(; j<n-7; j+=8)
			{
			kernel_sgemm_pp_nt_24x4_lib8(k, &pA[0+i*sda], &pA[0+(i+8)*sda], &pA[0+(i+16)*sda], &pB[0+j*sdb], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+(i+8)*sdc], &pC[0+(j+0)*bs+(i+16)*sdc], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+(i+8)*sdc], &pC[0+(j+0)*bs+(i+16)*sdc], bs, alg);
			kernel_sgemm_pp_nt_24x4_lib8(k, &pA[0+i*sda], &pA[0+(i+8)*sda], &pA[0+(i+16)*sda], &pB[4+j*sdb], &pC[0+(j+4)*bs+i*sdc], &pC[0+(j+4)*bs+(i+8)*sdc], &pC[0+(j+4)*bs+(i+16)*sdc], &pC[0+(j+4)*bs+i*sdc], &pC[0+(j+4)*bs+(i+8)*sdc], &pC[0+(j+4)*bs+(i+16)*sdc], bs, alg);
			}
		jj = 0;
		for(; jj<n-j-3; jj+=4)
			{
			kernel_sgemm_pp_nt_24x4_lib8(k, &pA[0+i*sda], &pA[0+(i+8)*sda], &pA[0+(i+16)*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], &pC[0+(j+jj)*bs+(i+8)*sdc], &pC[0+(j+jj)*bs+(i+16)*sdc], &pC[0+(j+jj)*bs+i*sdc], &pC[0+(j+jj)*bs+(i+8)*sdc], &pC[0+(j+jj)*bs+(i+16)*sdc], bs, alg);
			}
/*		for(; jj<n-j-1; jj+=2)*/
/*			{*/
/*			kernel_sgemm_pp_nt_16x2_lib8(k, &pA[0+i*sda], &pA[0+(i+8)*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], &pC[0+(j+jj)*bs+(i+8)*sdc], bs, alg);*/
/*			}*/
/*		for(; jj<n-j; jj++)*/
/*			{*/
/*			kernel_sgemm_pp_nt_16x1_lib8(k, &pA[0+i*sda], &pA[0+(i+8)*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], &pC[0+(j+jj)*bs+(i+8)*sdc], bs, alg);*/
/*			}*/
		}
#endif
	for(; i<m-8; i+=16)
		{
		j = 0;
		for(; j<n-7; j+=8)
			{
			kernel_sgemm_pp_nt_16x4_lib8(k, &pA[0+i*sda], &pA[0+(i+8)*sda], &pB[0+j*sdb], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+(i+8)*sdc], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+(i+8)*sdc], bs, alg);
			kernel_sgemm_pp_nt_16x4_lib8(k, &pA[0+i*sda], &pA[0+(i+8)*sda], &pB[4+j*sdb], &pC[0+(j+4)*bs+i*sdc], &pC[0+(j+4)*bs+(i+8)*sdc], &pC[0+(j+4)*bs+i*sdc], &pC[0+(j+4)*bs+(i+8)*sdc], bs, alg);
			}
		jj = 0;
		for(; jj<n-j-3; jj+=4)
			{
			kernel_sgemm_pp_nt_16x4_lib8(k, &pA[0+i*sda], &pA[0+(i+8)*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], &pC[0+(j+jj)*bs+(i+8)*sdc], &pC[0+(j+jj)*bs+i*sdc], &pC[0+(j+jj)*bs+(i+8)*sdc], bs, alg);
			}
/*		for(; jj<n-j-1; jj+=2)*/
/*			{*/
/*			kernel_sgemm_pp_nt_16x2_lib8(k, &pA[0+i*sda], &pA[0+(i+8)*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], &pC[0+(j+jj)*bs+(i+8)*sdc], bs, alg);*/
/*			}*/
/*		for(; jj<n-j; jj++)*/
/*			{*/
/*			kernel_sgemm_pp_nt_16x1_lib8(k, &pA[0+i*sda], &pA[0+(i+8)*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], &pC[0+(j+jj)*bs+(i+8)*sdc], bs, alg);*/
/*			}*/
		}
	for(; i<m-4; i+=8)
		{
		j = 0;
		for(; j<n-7; j+=8)
			{
			kernel_sgemm_pp_nt_8x8_lib8(k, &pA[0+i*sda], &pB[0+j*sdb], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+i*sdc], bs, alg);
/*			kernel_sgemm_pp_nt_8x4_lib8(k, &pA[0+i*sda], &pB[0+j*sdb], &pC[0+(j+0)*bs+i*sdc], bs, alg);*/
/*			kernel_sgemm_pp_nt_8x4_lib8(k, &pA[0+i*sda], &pB[4+j*sdb], &pC[0+(j+4)*bs+i*sdc], bs, alg);*/
			}
		jj = 0;
		for(; jj<n-j-3; jj+=4)
			{
			kernel_sgemm_pp_nt_8x4_lib8(k, &pA[0+i*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], &pC[0+(j+jj)*bs+i*sdc], bs, alg);
			}
/*		for(; jj<n-j-1; jj+=2)*/
/*			{*/
/*			kernel_sgemm_pp_nt_8x2_lib8(k, &pA[0+i*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], bs, alg);*/
/*			}*/
/*		for(; jj<n-j; jj++)*/
/*			{*/
/*			kernel_sgemm_pp_nt_8x1_lib8(k, &pA[0+i*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], bs, alg);*/
/*			}*/
		}
	for(; i<m; i+=4)
		{
		j = 0;
		for(; j<n-7; j+=8)
			{
			kernel_sgemm_pp_nt_4x8_lib8(k, &pA[0+i*sda], &pB[0+j*sdb], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+i*sdc], bs, alg);
/*			kernel_sgemm_pp_nt_4x4_lib8(k, &pA[0+i*sda], &pB[0+j*sdb], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+i*sdc], bs, alg);*/
/*			kernel_sgemm_pp_nt_4x4_lib8(k, &pA[0+i*sda], &pB[4+j*sdb], &pC[0+(j+4)*bs+i*sdc], &pC[0+(j+4)*bs+i*sdc], bs, alg);*/
			}
		jj = 0;
		for(; jj<n-j-3; jj+=4)
			{
			kernel_sgemm_pp_nt_4x4_lib8(k, &pA[0+i*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], &pC[0+(j+jj)*bs+i*sdc], bs, alg);
			}
/*		for(; jj<n-j-1; jj+=2)*/
/*			{*/
/*			kernel_sgemm_pp_nt_4x2_lib8(k, &pA[0+i*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], bs, alg);*/
/*			}*/
/*		for(; jj<n-j; jj++)*/
/*			{*/
/*			kernel_sgemm_pp_nt_4x1_lib8(k, &pA[0+i*sda], &pB[jj+j*sdb], &pC[0+(j+jj)*bs+i*sdc], bs, alg);*/
/*			}*/
		}
	
	}



/* preforms                                          */
/* C  = A * B'                                       */
/* where A, B and C are packed with block size 4,    */
/* and B is upper triangular                         */
void strmm_lib(int m, int n, float *pA, int sda, float *pB, int sdb, float *pC, int sdc)
	{
	
	const int bs = 8;
	
	int i, j;
	
	i = 0;
	for(; i<m-8; i+=16)
		{
		j = 0;
		for(; j<n-7; j+=8)
			{
			kernel_strmm_nt_16x4_lib8(n-j-0, &pA[0+(j+0)*bs+i*sda], &pA[0+(j+0)*bs+(i+8)*sda], &pB[0+(j+0)*bs+j*sdb], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+(i+8)*sdc], bs);
			kernel_strmm_nt_16x4_lib8(n-j-4, &pA[0+(j+0)*bs+i*sda], &pA[0+(j+0)*bs+(i+8)*sda], &pB[4+(j+4)*bs+j*sdb], &pC[0+(j+4)*bs+i*sdc], &pC[0+(j+4)*bs+(i+8)*sdc], bs);
			}
		if(n-j>3)
			{
			kernel_strmm_nt_16x4_lib8(n-j-0, &pA[0+(j+0)*bs+i*sda], &pA[0+(j+0)*bs+(i+8)*sda], &pB[0+(j+0)*bs+j*sdb], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+(i+8)*sdc], bs);
			j+=4;
			if(n-j==1)
				{
				corner_strmm_nt_16x1_lib8(&pA[0+(j+0)*bs+i*sda], &pA[0+(j+0)*bs+(i+8)*sda], &pB[4+(j+4)*bs+j*sdb], &pC[0+(j+4)*bs+i*sdc], &pC[0+(j+4)*bs+(i+8)*sdc], bs);
				}
			else if(n-j==2)
				{
				corner_strmm_nt_16x2_lib8(&pA[0+(j+0)*bs+i*sda], &pA[0+(j+0)*bs+(i+8)*sda], &pB[4+(j+4)*bs+j*sdb], &pC[0+(j+4)*bs+i*sdc], &pC[0+(j+4)*bs+(i+8)*sdc], bs);
				}
			else if(n-j==3)
				{
				corner_strmm_nt_16x3_lib8(&pA[0+(j+0)*bs+i*sda], &pA[0+(j+0)*bs+(i+8)*sda], &pB[4+(j+4)*bs+j*sdb], &pC[0+(j+4)*bs+i*sdc], &pC[0+(j+4)*bs+(i+8)*sdc], bs);
				}
			}
		else
			{
			if(n-j==1)
				{
				corner_strmm_nt_16x1_lib8(&pA[0+(j+0)*bs+i*sda], &pA[0+(j+0)*bs+(i+8)*sda], &pB[0+(j+0)*bs+j*sdb], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+(i+8)*sdc], bs);
				}
			else if(n-j==2)
				{
				corner_strmm_nt_16x2_lib8(&pA[0+(j+0)*bs+i*sda], &pA[0+(j+0)*bs+(i+8)*sda], &pB[0+(j+0)*bs+j*sdb], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+(i+8)*sdc], bs);
				}
			else if(n-j==3)
				{
				corner_strmm_nt_16x3_lib8(&pA[0+(j+0)*bs+i*sda], &pA[0+(j+0)*bs+(i+8)*sda], &pB[0+(j+0)*bs+j*sdb], &pC[0+(j+0)*bs+i*sdc], &pC[0+(j+0)*bs+(i+8)*sdc], bs);
				}
			}
		}
	for(; i<m; i+=8)
		{
		j = 0;
		for(; j<n-7; j+=8)
			{
			kernel_strmm_nt_8x4_lib8(n-j-0, &pA[0+(j+0)*bs+i*sda], &pB[0+(j+0)*bs+j*sdb], &pC[0+(j+0)*bs+i*sdc], bs);
			kernel_strmm_nt_8x4_lib8(n-j-4, &pA[0+(j+0)*bs+i*sda], &pB[4+(j+4)*bs+j*sdb], &pC[0+(j+4)*bs+i*sdc], bs);
			}
		if(n-j>3)
			{
			kernel_strmm_nt_8x4_lib8(n-j-0, &pA[0+(j+0)*bs+i*sda], &pB[0+(j+0)*bs+j*sdb], &pC[0+(j+0)*bs+i*sdc], bs);
			j+=4;
			if(n-j==1)
				{
				corner_strmm_nt_8x1_lib8(&pA[0+(j+0)*bs+i*sda], &pB[4+(j+4)*bs+j*sdb], &pC[0+(j+4)*bs+i*sdc], bs);
				}
			else if(n-j==2)
				{
				corner_strmm_nt_8x2_lib8(&pA[0+(j+0)*bs+i*sda], &pB[4+(j+4)*bs+j*sdb], &pC[0+(j+4)*bs+i*sdc], bs);
				}
			else if(n-j==3)
				{
				corner_strmm_nt_8x3_lib8(&pA[0+(j+0)*bs+i*sda], &pB[4+(j+4)*bs+j*sdb], &pC[0+(j+4)*bs+i*sdc], bs);
				}
			}
		else
			{
			if(n-j==1)
				{
				corner_strmm_nt_8x1_lib8(&pA[0+(j+0)*bs+i*sda], &pB[0+(j+0)*bs+j*sdb], &pC[0+(j+0)*bs+i*sdc], bs);
				}
			else if(n-j==2)
				{
				corner_strmm_nt_8x2_lib8(&pA[0+(j+0)*bs+i*sda], &pB[0+(j+0)*bs+j*sdb], &pC[0+(j+0)*bs+i*sdc], bs);
				}
			else if(n-j==3)
				{
				corner_strmm_nt_8x3_lib8(&pA[0+(j+0)*bs+i*sda], &pB[0+(j+0)*bs+j*sdb], &pC[0+(j+0)*bs+i*sdc], bs);
				}
			}
		}

	}



void ssyrk_spotrf_lib(int m, int k, int n, float *pA, int sda, float *pC, int sdc, float *diag)
	{
	const int bs = 8;
	const int d_ncl = S_NCL;
	const int k0 = (d_ncl-k%d_ncl)%d_ncl;
	
	int i, j;
	
/*	int n = m;*/
	
	float fact0[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	float fact1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
/*	j=0;*/
/*	i=0;*/
/*	kernel_spotrf_nt_16x4_lib8(k, j, &pA[i*sda], &pA[(i+8)*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pC[j*bs+(i+8)*sdc], &pA[(k0+k+j)*bs+i*sda], &pA[(k0+k+j)*bs+(i+8)*sda], bs, fact);*/
/*	kernel_spotrf_nt_12x4_lib8(k, j+4, &pA[i*sda], &pA[(i+8)*sda], &pA[4+j*sda], &pC[(j+4)*bs+i*sdc], &pC[(j+4)*bs+(i+8)*sdc], &pA[(k0+k+j+4)*bs+i*sda], &pA[(k0+k+j+4)*bs+(i+8)*sda], bs, fact);*/
/*	j=8;*/
/*	i = j;*/
/*	kernel_spotrf_nt_8x4_lib8(k, j, &pA[i*sda], &pA[j*sda], &pC[(j+0)*bs+i*sdc], &pA[(k0+k+j+0)*bs+i*sda], bs, fact);*/
/*	kernel_spotrf_nt_4x4_lib8(k, j+4, &pA[i*sda], &pA[4+j*sda], &pC[(j+4)*bs+i*sdc], &pA[(k0+k+j+4)*bs+i*sda], bs, fact);*/

	j = 0;
	for(; j<n-4; j+=8)
		{
		i = j;
		if(i<m-8)
			{
			kernel_spotrf_nt_16x4_lib8(k, j, &pA[i*sda], &pA[(i+8)*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pC[j*bs+(i+8)*sdc], &pA[(k0+k+j)*bs+i*sda], &pA[(k0+k+j)*bs+(i+8)*sda], bs, fact0);
			diag[j+0] = fact0[0];
			diag[j+1] = fact0[2];
			diag[j+2] = fact0[5];
			diag[j+3] = fact0[9];
			kernel_spotrf_nt_12x4_lib8(k, j+4, &pA[i*sda], &pA[(i+8)*sda], &pA[4+j*sda], &pC[(j+4)*bs+i*sdc], &pC[(j+4)*bs+(i+8)*sdc], &pA[(k0+k+j+4)*bs+i*sda], &pA[(k0+k+j+4)*bs+(i+8)*sda], bs, fact1);
			diag[j+4] = fact1[0];
			diag[j+5] = fact1[2];
			diag[j+6] = fact1[5];
			diag[j+7] = fact1[9];
			i += 16;
#if defined(TARGET_X64_AVX2)
			for(; i<m-16; i+=24)
				{
				kernel_strsm_nt_24x4_lib8(k, j, &pA[i*sda], &pA[(i+8)*sda], &pA[(i+16)*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pC[j*bs+(i+8)*sdc], &pC[j*bs+(i+16)*sdc], &pA[(k0+k+j)*bs+i*sda], &pA[(k0+k+j)*bs+(i+8)*sda], &pA[(k0+k+j)*bs+(i+16)*sda], bs, fact0);
				kernel_strsm_nt_24x4_lib8(k, j+4, &pA[i*sda], &pA[(i+8)*sda], &pA[(i+16)*sda], &pA[4+j*sda], &pC[(j+4)*bs+i*sdc], &pC[(j+4)*bs+(i+8)*sdc], &pC[(j+4)*bs+(i+16)*sdc], &pA[(k0+k+j+4)*bs+i*sda], &pA[(k0+k+j+4)*bs+(i+8)*sda], &pA[(k0+k+j+4)*bs+(i+16)*sda], bs, fact1);
				}
#endif
			for(; i<m-8; i+=16)
				{
				kernel_strsm_nt_16x4_lib8(k, j, &pA[i*sda], &pA[(i+8)*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pC[j*bs+(i+8)*sdc], &pA[(k0+k+j)*bs+i*sda], &pA[(k0+k+j)*bs+(i+8)*sda], bs, fact0);
				kernel_strsm_nt_16x4_lib8(k, j+4, &pA[i*sda], &pA[(i+8)*sda], &pA[4+j*sda], &pC[(j+4)*bs+i*sdc], &pC[(j+4)*bs+(i+8)*sdc], &pA[(k0+k+j+4)*bs+i*sda], &pA[(k0+k+j+4)*bs+(i+8)*sda], bs, fact1);
				}
			for(; i<m; i+=8)
				{
				kernel_strsm_nt_8x4_lib8(k, j, &pA[i*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pA[(k0+k+j)*bs+i*sda],bs, fact0);
				kernel_strsm_nt_8x4_lib8(k, j+4, &pA[i*sda], &pA[4+j*sda], &pC[(j+4)*bs+i*sdc], &pA[(k0+k+j+4)*bs+i*sda], bs, fact1);
				}
			}
		else //if(i<m-2)
			{
			kernel_spotrf_nt_8x4_lib8(k, j, &pA[i*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pA[(k0+k+j)*bs+i*sda], bs, fact0);
			diag[j+0] = fact0[0];
			diag[j+1] = fact0[2];
			diag[j+2] = fact0[5];
			diag[j+3] = fact0[9];
			kernel_spotrf_nt_4x4_lib8(k, j+4, &pA[i*sda], &pA[4+j*sda], &pC[4+(j+4)*bs+i*sdc], &pA[4+(k0+k+j+4)*bs+i*sda], bs, fact0);
			diag[j+0] = fact0[0];
			diag[j+1] = fact0[2];
			diag[j+2] = fact0[5];
			diag[j+3] = fact0[9];
			}
		}
	if(j<n)
		{
		i = j;
		kernel_spotrf_nt_4x4_lib8(k, j, &pA[i*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pA[(k0+k+j)*bs+i*sda], bs, fact0);
		diag[j+0] = fact0[0];
		diag[j+1] = fact0[2];
		diag[j+2] = fact0[5];
		diag[j+3] = fact0[9];
		}



/*	for(; j<n; j+=2)*/
/*		{*/
/*		i = j;*/
/*		if(i<m-2)*/
/*			{*/
/*			kernel_spotrf_pp_nt_4x2_lib4(k, j, &pA[i*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pA[(k0+k+j)*bs+i*sda], bs, fact);*/
/*			diag[j+0] = fact[0];*/
/*			diag[j+1] = fact[2];*/
/*			i += 4;*/
/*#if defined(TARGET_X64_AVX) || defined(TARGET_X64_AVX2)*/
/*			for(; i<m-4; i+=8)*/
/*				{*/
/*				kernel_strsm_pp_nt_8x2_lib4(k, j, &pA[i*sda], &pA[(i+4)*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pC[j*bs+(i+4)*sdc], &pA[(k0+k+j)*bs+i*sda], &pA[(k0+k+j)*bs+(i+4)*sda], bs, fact);*/
/*				}*/
/*#endif*/
/*			for(; i<m-2; i+=4)*/
/*				{*/
/*				kernel_strsm_pp_nt_4x2_lib4(k, j, &pA[i*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pA[(k0+k+j)*bs+i*sda], bs, fact);*/
/*				}*/
/*			for(; i<m; i+=2)*/
/*				{*/
/*				kernel_strsm_pp_nt_2x2_lib4(k, j, &pA[i*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pA[(k0+k+j)*bs+i*sda], bs, fact);*/
/*				}*/
/*			}*/
/*		else //if(i<m)*/
/*			{*/
/*			kernel_spotrf_pp_nt_2x2_lib4(k, j, &pA[i*sda], &pA[j*sda], &pC[j*bs+i*sdc], &pA[(k0+k+j)*bs+i*sda], bs, fact);*/
/*			diag[j+0] = fact[0];*/
/*			diag[j+1] = fact[2];*/
/*			}*/
/*		}*/

	}



// transpose & align lower triangular matrix
void strtr_l_lib(int m, int offset, float *pA, int sda, float *pC, int sdc)
	{
	
	const int bs = 8;
	
	int mna = (bs-offset%bs)%bs;
	
	int j;
	
	j=0;
	for(; j<m; j+=8)
		{
		kernel_stran_8_lib8(m, mna, pA, sda, pC);
		pA += bs*bs;
		pC += bs*sdc;
		}

/*	j=0;*/
/*	for(; j<m-3; j+=4)*/
/*		{*/
/*		kernel_dtran_pp_4_lib4(m-j, mna, pA, sda, pC);*/
/*		pA += bs*(sda+bs);*/
/*		pC += bs*(sdc+bs);*/
/*		}*/
/*	if(j==m)*/
/*		{*/
/*		return;*/
/*		}*/
/*	else if(m-j==1)*/
/*		{*/
/*		pC[0] = pA[0];*/
/*		}*/
/*	else if(m-j==2)*/
/*		{*/
/*		corner_dtran_pp_2_lib4(mna, pA, sda, pC);*/
/*		}*/
/*	else // if(m-j==3)*/
/*		{*/
/*		corner_dtran_pp_3_lib4(mna, pA, sda, pC);*/
/*		}*/
	
	}
