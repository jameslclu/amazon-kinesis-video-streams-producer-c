/**
 * Copyright (c) 2022 D Group / PCEBG / SHPBU, Foxconn .
 * All rights reserved.
 */

/**
 * @ingroup FXN-MEDIA-STREAMING-PROVIDER-STREAMING
 * @file    FmspStreaming.h
 * @brief   Header files for Fxn streaming provider about media source control APIs.
 */

#ifndef ORYX_STREAMING_H
#define ORYX_STREAMING_H

#ifdef __cplusplus
#if __cplusplus
   extern "C"{
#endif
#endif /* __cplusplus */

/* ---- Includes ----------------------------------------------------------- */
#include <stdbool.h>

//#include "LibFxnMediaProviderDef.h"

/* ---- Constants ---------------------------------------------------------- */

/* ---- Types -------------------------------------------------------------- */

/* ---- Variables ---------------------------------------------------------- */

/* ---- Global Functions --------------------------------------------------- */
/**
 * @brief Create Streaming From Sytem Media Service Procedure.
 *
 * @return EXIT_SUCCESS if initialization successful.
 * @return EXIT_FAILURE if initialization failure.
 */
int32_t OryxStreamingCreate( void );

/**
 * @brief Destroy and Dis-connect Streaming Procedure.
 */
void OryxStreamingDestroy( void );

/**
 * @brief Get Frame Data and Info From RAM
 *
 * @param[out] pInfo  Pointer to specify frame structure
 *
 * @return EXIT_SUCCESS if get frame success
 * @return EXIT_FAILURE if get frame failure
 */
int32_t OryxStreamingGetFrame( FmspFramePlaybackInfo_u * pInfo );
int32_t OryxStreamingGetLiveFrame( FmspFramePlaybackInfo_u * pInfo );

/* ------------------------------------------------------------------------- */

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif
