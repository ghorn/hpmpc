
IF ( ${HPMPC_TARGET} STREQUAL "X64_AVX2" )
    ADD_SUBDIRECTORY( avx )

ELSEIF ( ${HPMPC_TARGET} STREQUAL "X64_AVX" )
    ADD_SUBDIRECTORY( avx )

ELSEIF ( ${HPMPC_TARGET} STREQUAL "X64_SSE3" )
    ADD_SUBDIRECTORY( c99 )

ELSEIF ( ${HPMPC_TARGET} STREQUAL "CORTEX_A15" )
    ADD_SUBDIRECTORY( c99 )

ELSEIF ( ${HPMPC_TARGET} STREQUAL "CORTEX_A9" )
    ADD_SUBDIRECTORY( c99 )

ELSEIF ( ${HPMPC_TARGET} STREQUAL "CORTEX_A7" )
    ADD_SUBDIRECTORY( c99 )

ELSEIF ( ${HPMPC_TARGET} STREQUAL "C99_4X4" )
    ADD_SUBDIRECTORY( c99 )

ELSE( )
    MESSAGE( ERROR "Impossible happened!" )
ENDIF( )

HPMPC_ADD_SOURCE_FILES(
	# Double
	d_ip_hard.c d_ip2_hard.c d_res_ip_hard.c d_ip_soft.c d_ip2_soft.c d_res_ip_soft.c d_admm_box.c d_admm_soft.c
	# Single
	s_ip_box.c s_res_ip_box.c s_ip2_box.c s_admm_box.c s_admm_soft.c
)
