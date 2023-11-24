/**
 * Copyright (c) 2022 D Group / PCEBG / SHPBU, Foxconn .
 * All rights reserved.
 */

/**
 * @ingroup FXN-MEDIA-STREAMING-PROVIDER-MAIN
 * @file    FmspMain.c
 * @brief   Source files for Fxn Media Source Provider main entry.
 */

/* ---- Includes ----------------------------------------------------------- */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <json-c/json.h>

#include "LibFxnDBus.h"
#include "LibFxnUtils.h"

#include "FmspCommon.h"
#include "FmspServer.h"
#include "FmspStreaming.h"
#include "FmspConfig.h"

/* ---- Constants ---------------------------------------------------------- */

/* ---- Types -------------------------------------------------------------- */

/* ---- Private Function Prototypes ---------------------------------------- */

/* ---- Variables ---------------------------------------------------------- */

/* ---- Global Functions --------------------------------------------------- */
int32_t main( int32_t argc, char * argv[] )
{
    int32_t retStatus;

    FXNLOGD( "------------------------------------------------" );
    FXNLOGD( "   Application Name:    %s", FXN_PROGRAM_NAME );
    FXNLOGD( "   Application Version: %s", FXN_PROGRAM_VERSION );
    FXNLOGD( "------------------------------------------------" );

    retStatus = EXIT_FAILURE;

    if( LIB_UTILS_PROGRAM_ALREADY_EXECUTE ==
                LibUtilProgramIsExecute( FXN_PROGRAM_NAME ) )
        FXNLOGD( "%s Already Execute ..", FXN_PROGRAM_NAME );
    else
    {
        LibUtilSystemSignalReg( &LibDBusClientLoopTerminate );

        ( void )LibUtilInit();
        ( void )LibUtilHWTimerOpen();

        LibDBusClientInit();

        retStatus = FmspStreamingCreate();
    }

    if( EXIT_SUCCESS == retStatus )
        retStatus = FmspServerCreate();

    if( EXIT_SUCCESS == retStatus )
        LibDBusClientLoop();

    FmspServerDestroy();
    FmspStreamingDestroy();

    LibDBusClientDeInit();
    LibUtilDeInit();

    return EXIT_SUCCESS;
}

/* ---- Private Function --------------------------------------------------- */

/* ---- Call-back Functions ------------------------------------------------ */

/* ------------------------------------------------------------------------- */
