# devkitPro / libnds ARM9 Makefile
TARGET := ds_powerpoint
BUILD := build
SOURCES := .
DATA :=
INCLUDES :=

ARCH := -mthumb -mthumb-interwork
CFLAGS := -g -Wall -O2 $(ARCH) $(INCLUDE) -DARM9
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := $(ARCH)
LDFLAGS := -specs=ds_arm9.specs $(ARCH) -Wl,-Map,$(notdir $*).map
LIBS := -lfat -lnds9

include $(DEVKITPRO)/libnds/default_arm9/Makefile
