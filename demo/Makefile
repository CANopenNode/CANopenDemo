# Makefile for application with CANopenNode and Linux


DRV_SRC = ../CANopenLinux
CANOPEN_SRC = ../CANopenLinux/CANopenNode
APPL_SRC = .
APPL_DRV_SRC = linux


LINK_TARGET = demoLinuxDevice
ID_FILE = linux/CO_ident_defs.h
GIT_VERSION = $(shell git describe --tags --long --dirty)


INCLUDE_DIRS = \
	-I$(DRV_SRC) \
	-I$(CANOPEN_SRC) \
	-I$(APPL_SRC) \
	-I$(APPL_DRV_SRC)


SOURCES = \
	$(DRV_SRC)/CO_driver.c \
	$(DRV_SRC)/CO_error.c \
	$(DRV_SRC)/CO_epoll_interface.c \
	$(DRV_SRC)/CO_storageLinux.c \
	$(CANOPEN_SRC)/301/CO_ODinterface.c \
	$(CANOPEN_SRC)/301/CO_NMT_Heartbeat.c \
	$(CANOPEN_SRC)/301/CO_HBconsumer.c \
	$(CANOPEN_SRC)/301/CO_Emergency.c \
	$(CANOPEN_SRC)/301/CO_SDOserver.c \
	$(CANOPEN_SRC)/301/CO_TIME.c \
	$(CANOPEN_SRC)/301/CO_SYNC.c \
	$(CANOPEN_SRC)/301/CO_PDO.c \
	$(CANOPEN_SRC)/301/crc16-ccitt.c \
	$(CANOPEN_SRC)/303/CO_LEDs.c \
	$(CANOPEN_SRC)/305/CO_LSSslave.c \
	$(CANOPEN_SRC)/storage/CO_storage.c \
	$(CANOPEN_SRC)/CANopen.c \
	$(APPL_SRC)/CO_application.c \
	$(APPL_SRC)/OD.c \
	$(APPL_SRC)/CO_identificators.c \
	$(APPL_SRC)/objectAccessOD.c \
	$(APPL_SRC)/domainDemo.c \
	$(DRV_SRC)/CO_main_basic.c


OBJS = $(SOURCES:%.c=%.o)
CC ?= gcc
OPT =
OPT += -g
#OPT += -O2
#OPT += -DCO_SINGLE_THREAD
#OPT += -DCO_CONFIG_DEBUG=0xFFFF
#OPT += -Wextra -Wshadow -pedantic -fanalyzer
#OPT += -DCO_USE_GLOBALS
#OPT += -DCO_MULTIPLE_OD
CFLAGS = -Wall -DCO_SW_VERSION=\"$(GIT_VERSION)\" -DCO_DRIVER_CUSTOM $(OPT) $(INCLUDE_DIRS)
LDFLAGS =
LDFLAGS += -g
LDFLAGS += -pthread

#Options can be also passed via make: 'make OPT="-g" LDFLAGS="-pthread"'


.PHONY: all clean

all: clean $(LINK_TARGET)

clean:
	rm -f $(OBJS) $(LINK_TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LINK_TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
