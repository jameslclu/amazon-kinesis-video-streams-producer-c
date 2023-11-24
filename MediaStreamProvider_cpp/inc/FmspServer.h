/**
 * Copyright (c) 2022 D Group / PCEBG / SHPBU, Foxconn .
 * All rights reserved.
 */

/**
 * @ingroup FXN-MEDIA-STREAM-PROVIDER-SERVER
 * @file    FmspServer.h
 * @brief   Header files for Fxn media streaming provider Server APIs.
 */

#ifndef FMSP_SERVER_H
#define FMSP_SERVER_H

#ifdef __cplusplus
#if __cplusplus
   extern "C"{
#endif
#endif /* __cplusplus */

/* ---- Includes ----------------------------------------------------------- */
#include "LibFxnMediaProviderDef.h"

/* ---- Constants ---------------------------------------------------------- */

/* ---- Types -------------------------------------------------------------- */
typedef struct{
    bool bRun;
    bool bSuspend;
    bool bRenew;

    pthread_t         TID;
    pthread_cond_t    C;
    pthread_mutex_t   M;

    void * ( * ppFnLoop )( void * arg );
    void ( * pFnSuspend )( void );
    void ( * pFnWakup )( void );
}FmspServerThread_t;

typedef struct{
    FmspServerThread_t Main;
    FmspServerThread_t Recv;
    FmspServerThread_t Send;

    int32_t ServerSd;
    int32_t ClientSd[ 2 ];
    int8_t  ClientName[ FMSP_CMD_PROGRAM_NAME_SIZE ];

    int64_t TargetTs;
    int64_t ArrivalTs;
}FmspServer_t;

/* ---- Variables ---------------------------------------------------------- */

/* ---- Global Functions --------------------------------------------------- */
/**
 * @brief Server Create.
 */
int32_t FmspServerCreate( void );

/**
 * @brief Server Destroy
 */
void FmspServerDestroy( void );

/* ------------------------------------------------------------------------- */

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif
