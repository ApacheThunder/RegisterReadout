/*---------------------------------------------------------------------------------

	Simple eeprom example
	-- davr

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "nds/arm9/console.h"

// MBK Registers
// Latest libnds defines these, but not in a way that's compatible with this old code. :P
unsigned int * SCFG_MBK1=(unsigned int*)0x04004040; // WRAM_A 0..3
unsigned int * SCFG_MBK2=(unsigned int*)0x04004044; // WRAM_B 0..3
unsigned int * SCFG_MBK3=(unsigned int*)0x04004048; // WRAM_B 4..7
unsigned int * SCFG_MBK4=(unsigned int*)0x0400404C; // WRAM_C 0..3
unsigned int * SCFG_MBK5=(unsigned int*)0x04004050; // WRAM_C 4..7

unsigned int * SCFG_MBK6=(unsigned int*)0x04004054;
unsigned int * SCFG_MBK7=(unsigned int*)0x04004058;
unsigned int * SCFG_MBK8=(unsigned int*)0x0400405C;
unsigned int * SCFG_MBK9=(unsigned int*)0x04004060;

//---------------------------------------------------------------------------------
void dopause() {
//---------------------------------------------------------------------------------
	iprintf("Press Start for Arm7 readout...\n");
	while(1) {
		scanKeys();
		if(keysDown() & KEY_START) { break; }
		swiWaitForVBlank();
	}
	scanKeys();
}

//---------------------------------------------------------------------------------
void dopauseExit() {
//---------------------------------------------------------------------------------
	for (int i = 0; i < 30; i++) { swiWaitForVBlank(); }
	iprintf("Press Start to exit...\n");
	while(1) {
		scanKeys();
		if(keysDown() & KEY_START) {
			powerOn(PM_BACKLIGHT_TOP);
			break;
		}
		swiWaitForVBlank();
	}
	scanKeys();
}

void getMBK_ARM7() {
	
	iprintf( "MBK6:" );
	fifoSendValue32(FIFO_USER_01,(unsigned int)SCFG_MBK6);	
	iprintf( " \n" );
	iprintf( "MBK7:" );
	fifoSendValue32(FIFO_USER_01,(unsigned int)SCFG_MBK7);
	iprintf( " \n" );
	iprintf( "MBK8:" );
	fifoSendValue32(FIFO_USER_01,(unsigned int)SCFG_MBK8);
	iprintf( " \n" );
	iprintf( "MBK9:" );
	fifoSendValue32(FIFO_USER_01,(unsigned int)SCFG_MBK9);

}

static void myFIFOValue32Handler(u32 value,void* data) { iprintf( " %08x\n", value ); }

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	// Top screen not used for anything right now.
	powerOff(PM_BACKLIGHT_TOP);

	consoleDemoInit();
	
	defaultExceptionHandler();

	iprintf( "ARM9 MBK:\n" );
	iprintf( " \n" );
	iprintf( "MBK1: %08x\n", *SCFG_MBK1 );
	iprintf( " \n" );
	iprintf( "MBK2: %08x\n", *SCFG_MBK2 );
	iprintf( " \n" );
	iprintf( "MBK3: %08x\n", *SCFG_MBK3 );
	iprintf( " \n" );
	iprintf( "MBK4: %08x\n", *SCFG_MBK4 );
	iprintf( " \n" );
	iprintf( "MBK5: %08x\n", *SCFG_MBK5 );
	iprintf( " \n" );
	iprintf( "MBK6: %08x\n", *SCFG_MBK6 );
	iprintf( " \n" );
	iprintf( "MBK7: %08x\n", *SCFG_MBK7 );
	iprintf( " \n" );
	iprintf( "MBK8: %08x\n", *SCFG_MBK8 );
	iprintf( " \n" );
	iprintf( "MBK9: %08x\n", *SCFG_MBK9 );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	dopause();

	fifoSetValue32Handler(FIFO_USER_02,myFIFOValue32Handler,0);

	iprintf( "ARM7 MBK:\n" );
	iprintf( " \n" );
	getMBK_ARM7();
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	dopauseExit();
	return 0;
}

