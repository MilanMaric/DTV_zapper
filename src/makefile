CROSS_COMPILE=$(TOOLCHAIN_CROSS_COMPILE)

CC_PREFIX=$(CROSS_COMPILE)-
CC=$(CC_PREFIX)gcc
CXX=$(CC_PREFIX)g++
LD=$(CC_PREFIX)ld
ROOTFS_PATH=$(SDK_ROOTFS)

CFLAGS= -I. \
        -I./include/ \
        -I$(ROOTFS_PATH)/usr/include/ \
        -I$(ROOTFS_PATH)/usr/include/directfb/
        
CFLAGS += -D__LINUX__ -O0 -Wno-psabi 

CXXFLAGS = $(CFLAGS)

LIBS_PATH += -L$(ROOTFS_PATH)/home/galois/lib/directfb-1.4-6-libs

LIBS := $(LIBS_PATH) -ldirectfb -ldirect -lfusion -lrt

SRC= main.c
SRC+= remote.c
SRC+= drawing.c
SRC+= table_parser.c

all: dfb_example

dfb_example:
	$(CC) -o dtv_zapper $(SRC) $(CFLAGS) $(LIBS)
    
clean:
	rm -f dtv_zapper
