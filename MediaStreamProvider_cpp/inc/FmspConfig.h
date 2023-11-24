/**
 * Copyright (c) 2022 D Group / PCEBG / SHPBU, Foxconn .
 * All rights reserved.
 */

/**
 * @ingroup FXN-MEDIA-STREAM-PROVIDER-CONF
 * @file    KvsConfig.h
 * @brief   Header files for KVS-Producer Load IoT Credential infomation APIs.
 */

#ifndef KVSCONFIG_H
#define KVSCONFIG_H

#ifdef __cplusplus
#if __cplusplus
   extern "C"{
#endif
#endif /* __cplusplus */

/* ---- Includes ----------------------------------------------------------- */

/* ---- Constants ---------------------------------------------------------- */
#define FXN_PROGRAM_VERSION         "0.0.4"
#define FXN_PROGRAM_NAME            "fxnMediaStreamProvider"

/* ---- Types -------------------------------------------------------------- */

/* ---- Variables ---------------------------------------------------------- */

/* ---- Global Functions --------------------------------------------------- */
/**
 * @brief Load Configuration From File.
 *
 * @param[in] pFilename  Pointer to specify file name.
 *
 * @return EXIT_FAILURE if load failure.
 * @return EXIT_SUCCESS if load successful.
 */
int32_t KvsConfigLoadInfo( const char * pFilename );

/**
 * @brief Unload and Clear Configuration Buffer
 */
void KvsConfigUnloadInfo( void );

/* ------------------------------------------------------------------------- */

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif
