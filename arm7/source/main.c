/*---------------------------------------------------------------------------------

	default ARM7 core

		Copyright (C) 2005 - 2010
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>

// MBK Registers
unsigned int * SCFG_MBK1=(unsigned int*)0x04004040; // WRAM_A 0..3
unsigned int * SCFG_MBK2=(unsigned int*)0x04004044; // WRAM_B 0..3
unsigned int * SCFG_MBK3=(unsigned int*)0x04004048; // WRAM_B 4..7
unsigned int * SCFG_MBK4=(unsigned int*)0x0400404C; // WRAM_C 0..3
unsigned int * SCFG_MBK5=(unsigned int*)0x04004050; // WRAM_C 4..7
unsigned int * SCFG_MBK6=(unsigned int*)0x04004054;
unsigned int * SCFG_MBK7=(unsigned int*)0x04004058;
unsigned int * SCFG_MBK8=(unsigned int*)0x0400405C;
unsigned int * SCFG_MBK9=(unsigned int*)0x04004060;

void VblankHandler(void) { }
void VcountHandler() { inputGetAndSend(); }

volatile bool exitflag = false;

void powerButtonCB() { exitflag = true; }

static void myFIFOValue32Handler(u32 value,void* data) { fifoSendValue32(FIFO_USER_02,*((unsigned int*)value)); }

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------

	dmaFillWords(0, (void*)0x04000400, 0x100);

	REG_SOUNDCNT |= SOUND_ENABLE;
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	powerOn(POWER_SOUND);

	readUserSettings();
	ledBlink(0);

	irqInit();

	initClockIRQ();
	fifoInit();


	SetYtrigger(80);

	installSoundFIFO();

	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

	setPowerButtonCB(powerButtonCB);	
		
	fifoSetValue32Handler(FIFO_USER_01,myFIFOValue32Handler,0);
	
	// Keep the ARM7 mostly idle
	while (1) {
		swiWaitForVBlank();
	}
	return 0;
}

