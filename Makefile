#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TARGET	:=	RegisterReadout
export TOPDIR	:=	$(CURDIR)

# specify a directory which contains the nitro filesystem
# this is relative to the Makefile
NITRO_FILES	:=

include $(DEVKITARM)/ds_rules

.PHONY: checkarm7 checkarm9 clean

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: checkarm7 checkarm9 $(TARGET).nds

#---------------------------------------------------------------------------------
checkarm7:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
checkarm9:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
$(TARGET).nds	: $(NITRO_FILES) arm7/$(TARGET).elf arm9/$(TARGET).elf
	ndstool	-c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf \
	-g REGE 01 "REGREADER" -z 80040000 -u 00030004 -a 00000138 -p 0001 \
	-b icon.bmp "View SCFG/MBK Registers;Created by ahezard; Upgraded by ApacheThunder"
	$(_ADDFILES)

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).nds $(TARGET).arm7 $(TARGET).arm9
