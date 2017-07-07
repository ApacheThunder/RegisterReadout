#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

include $(DEVKITARM)/ds_rules

export TARGET		:=	RegisterReadout_MBK
export TOPDIR		:=	$(CURDIR)

export VERSION_MAJOR	:= 1
export VERSION_MINOR	:= 0
export VERSTRING	:=	$(VERSION_MAJOR).$(VERSION_MINOR)

#---------------------------------------------------------------------------------
# path to tools - this can be deleted if you set the path in windows
#---------------------------------------------------------------------------------
export PATH		:=	$(DEVKITARM)/bin:$(PATH)

.PHONY: $(TARGET).arm7 $(TARGET).arm9

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds

$(TARGET).nds	:	$(TARGET).arm7 $(TARGET).arm9
	cp arm7/$(TARGET).elf $(TARGET).arm7.elf
	cp arm9/$(TARGET).elf $(TARGET).arm9.elf
	ndstool	-c $(TARGET).nds -7 $(TARGET).arm7.elf -9 $(TARGET).arm9.elf -b icon.bmp "View MBK Registers;Unlocked Arm7 SCFG Recommended;Made by ahezard"

#---------------------------------------------------------------------------------
$(TARGET).arm7	: arm7/$(TARGET).elf
$(TARGET).arm9	: arm9/$(TARGET).elf

#---------------------------------------------------------------------------------
arm9/source/version.h : Makefile
	@echo "#ifndef VERSION_H" > $@
	@echo "#define VERSION_H" >> $@
	@echo >> $@
	@echo '#define VERSION_STRING "v'$(VERSION_MAJOR).$(VERSION_MINOR)'"' >> $@
	@echo >> $@
	@echo "#endif // VERSION_H" >> $@

#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
dist-bin	:	$(TARGET).txt $(TARGET).nds License.txt
	zip -X -9 $(TARGET)_v$(VERSTRING).zip $(TARGET).txt $(TARGET).nds License.txt

dist-src	:
	tar --exclude=*~ -cvjf $(TARGET)_src_v$(VERSTRING).tar.bz2 \
	--transform 's,^,/'$(TARGET)'/,' \
	Makefile icon.bmp License.txt Launcher.txt \
	arm7/Makefile arm7/source \
	arm9/Makefile arm9/source

dist-legal	:	BootLoader/load.bin
	tar --exclude=*~ --exclude=read_card.c -cvjf $(TARGET)_src_v$(VERSTRING).tar.bz2 \
	--transform 's,^,/'$(TARGET)'/,' \
	Makefile icon.bmp License.txt Launcher.txt \
	arm7/Makefile arm7/source \
	arm9/Makefile arm9/source

dist	:	dist-bin dist-src

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET).ds.gba $(TARGET).nds $(TARGET).arm7.elf $(TARGET).arm9.elf
