
IF ( ${HPMPC_TARGET} STREQUAL "CORTEX_A15" )
    HPMPC_ADD_SOURCE_FILES(
		# Double
		kernel_dgemm_vfpv3_lib4.c kernel_dtrmm_vfpv3_lib4.c kernel_dtrsm_vfpv3_lib4.c kernel_dsyrk_vfpv3_lib4.o kernel_dpotrf_vfpv3_lib4.c kernel_dgemv_c99_lib4.c kernel_dtrmv_c99_lib4.c kernel_dtrsv_c99_lib4.c 
		# Single
    	kernel_sgemm_neon_lib4.c kernel_strmm_neon_lib4.c kernel_sgemm_strsm_neon_lib4.c kernel_ssyrk_spotrf_neon_lib4.c kernel_sgemv_neon_lib4.c kernel_strmv_neon_lib4.c kernel_strsv_neon_lib4.c kernel_ssymv_c99_lib4.c kernel_stran_neon_lib4.c
	)

ELSEIF ( ${HPMPC_TARGET} STREQUAL "CORTEX_A9" )
    HPMPC_ADD_SOURCE_FILES(
		# Double
	    kernel_dgemm_vfpv3_lib4.c kernel_dtrmm_vfpv3_lib4.c kernel_dtrsm_vfpv3_lib4.c kernel_dsyrk_vfpv3_lib4.o kernel_dpotrf_vfpv3_lib4.c kernel_dgemv_c99_lib4.c kernel_dtrmv_c99_lib4.c kernel_dtrsv_c99_lib4.c 
		# Single
    	kernel_sgemm_neon_lib4.c kernel_strmm_neon_lib4.c kernel_sgemm_strsm_neon_lib4.c kernel_ssyrk_spotrf_neon_lib4.c kernel_sgemv_neon_lib4.c kernel_strmv_neon_lib4.c kernel_strsv_neon_lib4.c kernel_ssymv_c99_lib4.c kernel_stran_neon_lib4.c
	)

ELSEIF ( ${HPMPC_TARGET} STREQUAL "CORTEX_A7" )
    HPMPC_ADD_SOURCE_FILES(
		# Double
	    kernel_dgemm_vfpv3_lib4.c kernel_dtrmm_vfpv3_lib4.c kernel_dtrsm_vfpv3_lib4.c kernel_dsyrk_vfpv3_lib4.o kernel_dpotrf_vfpv3_lib4.c kernel_dgemv_c99_lib4.c kernel_dtrmv_c99_lib4.c kernel_dtrsv_c99_lib4.c 
		# Single
    	kernel_sgemm_neon_lib4.c kernel_strmm_neon_lib4.c kernel_sgemm_strsm_neon_lib4.c kernel_ssyrk_spotrf_neon_lib4.c kernel_sgemv_neon_lib4.c kernel_strmv_neon_lib4.c kernel_strsv_neon_lib4.c kernel_ssymv_c99_lib4.c kernel_stran_neon_lib4.c
	)

ELSE( )
    MESSAGE( ERROR "Impossible happened!" )
ENDIF( )
