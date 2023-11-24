/**
 * Copyright (c) 2022 D Group / PCEBG / SHPBU, Foxconn .
 * All rights reserved.
 */
#ifndef FMSP_COMMON_H
#define FMSP_COMMON_H

#ifdef __cplusplus
#if __cplusplus
   extern "C"{
#endif
#endif /* __cplusplus */

/* ---- Includes ----------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "LibFxnUtils.h"

/* ---- Constants ---------------------------------------------------------- */
#define FMSP_LOG_VERBOSE_ENABLE           ( 1 )
#define FMSP_LOG_INFO_ENABLE              ( 1 )
#define FMSP_LOG_DBG_ENABLE               ( 1 )
#define FMSP_LOG_WRN_ENABLE               ( 1 )
#define FMSP_LOG_ERR_ENABLE               ( 1 )

#define FMSP_LOG_CONSOLE_MODE             ( 0 )

#define NONE         "\033[m"
#define YELLOW       "\033[1;33m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

#define FMSP_LOG_DISABLE   do{ }while(0)

#define FmspGetFormattedTime  LibUtilDateTimeGetLocalFormatted

#if FMSP_LOG_VERBOSE_ENABLE == 1
  #if FMSP_LOG_CONSOLE_MODE == 1
    #define FXNLOGV(fmt,...) do{ printf( LIGHT_GRAY"[%s %s:%s %d] INFO: "fmt"\n"NONE, FmspGetFormattedTime(),__FILE__,__func__, __LINE__, ##__VA_ARGS__ ); }while(0)
  #else
    #define FXNLOGV(fmt,...) FMSP_LOG_DISABLE
  #endif
#else
  #define FXNLOGV(fmt,...) FMSP_LOG_DISABLE
#endif

#if FMSP_LOG_INFO_ENABLE == 1
  #if FMSP_LOG_CONSOLE_MODE == 1
    #define FXNLOGI(fmt,...) do{ printf( LIGHT_CYAN"[%s %s:%s %d] I: "fmt"\n"NONE, FmspGetFormattedTime(),__FILE__,__func__, __LINE__, ##__VA_ARGS__ ); }while(0)
  #else
    #define FXNLOGI(fmt,...) do{ syslog( LOG_USER | LOG_INFO, " I: "fmt"\n", ##__VA_ARGS__ ); }while(0)
  #endif
#else
  #define FXNLOGI(fmt,...) FMSP_LOG_DISABLE
#endif

#if FMSP_LOG_DBG_ENABLE == 1
  #if FMSP_LOG_CONSOLE_MODE == 1
    #define FXNLOGD(fmt,...) do{ printf( WHITE"[%s %s:%s %d] D: "fmt"\n"NONE, FmspGetFormattedTime(),__FILE__,__func__, __LINE__, ##__VA_ARGS__ ); }while(0)
  #else
    #define FXNLOGD(fmt,...) do{ syslog( LOG_USER | LOG_DEBUG, " D: "fmt"\n", ##__VA_ARGS__ ); }while(0)
  #endif
#else
  #define FXNLOGD(fmt,...) FMSP_LOG_DISABLE
#endif

#if FMSP_LOG_WRN_ENABLE == 1
  #if FMSP_LOG_CONSOLE_MODE == 1
    #define FXNLOGW(fmt,...) do{ printf( YELLOW"[%s %s:%s %d] W: "fmt"\n"NONE, FmspGetFormattedTime(),__FILE__,__func__, __LINE__, ##__VA_ARGS__ ); }while(0)
  #else
    #define FXNLOGW(fmt,...) do{ syslog( LOG_USER | LOG_WARNING, " W: "fmt"\n", ##__VA_ARGS__ ); }while(0)
  #endif
#else
  #define FXNLOGW(fmt,...) FMSP_LOG_DISABLE
#endif

#if FMSP_LOG_ERR_ENABLE == 1
  #if FMSP_LOG_CONSOLE_MODE == 1
    #define FXNLOGE(fmt,...) do{ printf( LIGHT_RED"[%s %s:%s %d] E: "fmt"\n"NONE, FmspGetFormattedTime(),__FILE__,__func__, __LINE__, ##__VA_ARGS__ ); }while(0)
  #else
    #define FXNLOGE(fmt,...) do{ syslog( LOG_USER | LOG_ERR, " E: "fmt"\n", ##__VA_ARGS__ ); }while(0)
  #endif
#else
  #define FXNLOGE(fmt,...) FMSP_LOG_DISABLE
#endif

/* ---- Types -------------------------------------------------------------- */

/* ---- Variables ---------------------------------------------------------- */

/* ---- Global Functions --------------------------------------------------- */


/* ------------------------------------------------------------------------- */

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */

#endif
