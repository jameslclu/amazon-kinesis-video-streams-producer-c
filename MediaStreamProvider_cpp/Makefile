#
# vendor/aws/kvs/producer/Telus/Makefile
#
# History:
#    2023/05/25 - [PCEBG / SHPBU, Foxconn] Created file
#
# Copyright 2022(c) Hon Hai Precision Industry Co., Ltd.
#
# All rights reserved. No Part of this file may be reproduced, stored
# in a retrieval system, or transmitted, in any form, or by any means,
# electronic, mechanical, photocopying, recording, or otherwise,
# without the prior consent of Hon Hai Precision Industry Co., Ltd.
#

# -------------------------------------------------------------------------
#   Definitions of the project files
# -------------------------------------------------------------------------
EXEC_NAME := fxnMediaStreamProvider
CONF_NAME := ./config/fxn-camera-MediaProvider.service

# -------------------------------------------------------------------------
#   Put suffix on all c srcs files and obj files
# -------------------------------------------------------------------------

# -------------------------------------------------------------------------
#   Predefines, Include Paths and Compile Options
# -------------------------------------------------------------------------
ROOT = $(word 1, $(subst /vendor/, ,$(shell pwd)))
include $(ROOT)/vendor/aws/config/config.mk
include $(ROOT)/vendor/aws/config/kvsp_config.mk

DIR_TARGET := $(ROOT)/vendor/aws/bin/utils

# -------------------------------------------------------------------------
#   Setup CFLAGS for compile command line
# -------------------------------------------------------------------------
CFLAGS += -I./inc \
	-I../inc \
	$(CFLAG_INC_GSTREAMER) \
	$(CFLAG_INC_ORYX_V2) \
	$(CFLAG_INC_DBUS) \
	$(CFLAG_INC_GLIB2) \
	$(CFLAG_INC_JSONC)

CXXFLAGS += -I./inc  -I../inc \
	$(CFLAG_AWS_KVS_PRODUCER) \
	$(CFLAG_INC_GSTREAMER) \
	$(CFLAG_INC_ORYX_V2) \
	$(CFLAG_INC_DBUS) \
	$(CFLAG_INC_GLIB2) -fpermissive

# -------------------------------------------------------------------------
#   Libs, Lib Paths, and Link Options
# -------------------------------------------------------------------------
LIBS := -L../libs -lFxnDBus -lFxnUtils \
	$(LDFLAG_GSTREAMER) \
	$(LDFLAG_GLIB2) \
	$(LDFLAG_DBUS) \
	$(LDFLAG_ZLIB) \
	$(LDFLAG_LUA) \
	$(LDFLAG_UNWIND) \
	$(LDFLAG_ORYX_V2) \
	$(LDFLAG_JSONC)

# -------------------------------------------------------------------------
#   Setup LDFLAGS for link command line
# -------------------------------------------------------------------------
LDFLAGS += -lstdc++ -ldl 

# -------------------------------------------------------------------------
#   Put suffix on all c srcs files and obj files
# -------------------------------------------------------------------------
MODULE_C_SRCS := $(wildcard *.c)

MODULE_CXX_SRCS := $(wildcard *.cpp)

MODULE_C_OBJS := $(MODULE_C_SRCS:%.c=%.o)

MODULE_CXX_OBJS := $(MODULE_CXX_SRCS:%.cpp=%.o)

# -------------------------------------------------------------------------
#   Build rules
# -------------------------------------------------------------------------
.PHONY: clean all

all: $(MODULE_CXX_OBJS) $(MODULE_C_OBJS)
	$(RM) $(DIR_TARGET)
	$(GXX) $(MODULE_CXX_OBJS) $(MODULE_C_OBJS) -o $(EXEC_NAME) $(LDFLAGS) $(LIBS)
	$(STRIP) $(EXEC_NAME)
	$(RM) $(MODULE_CXX_OBJS) $(MODULE_C_OBJS)
	$(MKDIR) $(DIR_TARGET)
	$(MV) $(EXEC_NAME) $(DIR_TARGET)
	$(CP) $(CONF_NAME) $(DIR_TARGET)

clean:
	$(RM) $(MODULE_CXX_OBJS) $(MODULE_C_OBJS)
	$(RM) $(DIR_TARGET)

$(MODULE_CXX_OBJS) :
	$(GXX) -c $(subst .o,.cpp,$(notdir $(subst \,/,$@))) $(CXXFLAGS) -o $@

$(MODULE_C_OBJS) :
	$(CC) -c $(subst .o,.c,$(notdir $(subst \,/,$@))) $(CFLAGS) -o $@
