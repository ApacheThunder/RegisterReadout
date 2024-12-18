#include <nds.h>
#include <fat.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "nds/arm9/console.h"
#include "tonccpy.h"
#include "font.h"

static PrintConsole topConsole;
static PrintConsole bottomConsole;

static int bg;
static int bgSub;


DTCM_DATA static volatile uint arm7Data = 0;
DTCM_DATA static volatile bool valueUpdated = false;

DTCM_DATA static u8 cartHeaderBuffer[0x200];

// SCFG Registers
// Latest libnds defines these, but not in a way that's compatible with this old code. :P
uint * SCFG_ROM		= (uint*)0x4004000;
uint * SCFG_CLK		= (uint*)0x4004004; 
uint * SCFG_EXT		= (uint*)0x4004008;
uint * SCFG_MC		= (uint*)0x4004010;

u16 * SCFG_MC_INSERT_DELAY = (u16*)0x4004012;
u16 * SCFG_MC_PWROFF_DELAY = (u16*)0x4004014;

u16 * SCFG_CPUID	= (u16*)0x4004D04;
u16 * SCFG_CPUID2	= (u16*)0x4004D00;

// MBK Registers
// Latest libnds defines these, but not in a way that's compatible with this old code. :P
uint * SCFG_MBK1	= (uint*)0x04004040; // WRAM_A 0..3
uint * SCFG_MBK2	= (uint*)0x04004044; // WRAM_B 0..3
uint * SCFG_MBK3	= (uint*)0x04004048; // WRAM_B 4..7
uint * SCFG_MBK4	= (uint*)0x0400404C; // WRAM_C 0..3
uint * SCFG_MBK5	= (uint*)0x04004050; // WRAM_C 4..7

uint * SCFG_MBK6	= (uint*)0x04004054;
uint * SCFG_MBK7	= (uint*)0x04004058;
uint * SCFG_MBK8	= (uint*)0x0400405C;
uint * SCFG_MBK9	= (uint*)0x04004060;


ITCM_CODE uint readArm7Data(uint data) {
	fifoSendValue32(FIFO_USER_01, data);
	while(!valueUpdated)swiWaitForVBlank();
	valueUpdated = false;
	return arm7Data;
}

ITCM_CODE void CustomConsoleInit() {
	videoSetMode(MODE_0_2D);
	videoSetModeSub(MODE_0_2D);
	vramSetBankA (VRAM_A_MAIN_BG);
	vramSetBankC (VRAM_C_SUB_BG);

	bg = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 1, 0);
	bgSub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 1, 0);

	consoleInit(&bottomConsole, 3, BgType_Text4bpp, BgSize_T_256x256, 20, 0, false, false);
	consoleInit(&topConsole, 3, BgType_Text4bpp, BgSize_T_256x256, 20, 0, true, false);

	ConsoleFont font;
	font.gfx = (u16*)fontTiles;
	font.pal = (u16*)fontPal;
	font.numChars = 95;
	font.numColors =  fontPalLen / 2;
	font.bpp = 4;
	font.asciiOffset = 32;
	font.convertSingleColor = true;
	consoleSetFont(&bottomConsole, &font);
	consoleSetFont(&topConsole, &font);
	consoleSetWindow(&topConsole, 1, 0, 31, 28);
	consoleSetWindow(&bottomConsole, 1, 0, 31, 28);
}


ITCM_CODE void dopause(const char* text) {
	while(1) {
		scanKeys();
		if(!keysUp())break;
		swiWaitForVBlank();
	}
	printf(text);
	while(1) {
		scanKeys();
		if(keysDown())break;
		swiWaitForVBlank();
	}
}

static void myFIFOValue32Handler(u32 value,void* data) { 
	arm7Data = value;
	valueUpdated = true;
}

ITCM_CODE void getSCFG_ARM9(bool readMBK) {
	consoleSelect(&topConsole);
	consoleClear();
	if (readMBK) {
		printf("\nARM9 MBK:\n\n" );
		iprintf("MBK1: %08x\n\n", *SCFG_MBK1);
		iprintf("MBK2: %08x\n\n", *SCFG_MBK2);
		iprintf("MBK3: %08x\n\n", *SCFG_MBK3);
		iprintf("MBK4: %08x\n\n", *SCFG_MBK4);
		iprintf("MBK5: %08x\n\n", *SCFG_MBK5);
		iprintf("MBK6: %08x\n\n", *SCFG_MBK6);
		iprintf("MBK7: %08x\n\n", *SCFG_MBK7);
		iprintf("MBK8: %08x\n\n", *SCFG_MBK8);
		iprintf("MBK9: %08x\n\n\n", *SCFG_MBK9);
	} else {
		printf("\nARM9 SCFG:\n\n");
		iprintf("SCFG_CLK: 0x%08x\n", *SCFG_CLK );
		iprintf("SCFG_EXT: 0x%08x\n", *SCFG_EXT );
		// printf(" \n\n\n\n\n\n\n");
		printf("\n\n\n\nSlot-1 Status: \n\n");
		iprintf("SCFG_MC: 0x%04x\n\n", *SCFG_MC);
		iprintf("HEADER ROMCTRL1: 0x%08x\n", *(uint*)0x02FFFE60);
		iprintf("HEADER ROMCTRL2: 0x%08x\n\n\n", *(uint*)0x02FFFE64);
	}
}


ITCM_CODE void getSFCG_ARM7(bool readMBK) {
	consoleSelect(&bottomConsole);
	consoleClear();
	valueUpdated = false;
	if (readMBK) {
		printf("\nARM7 MBK:\n\n");
		iprintf("\nMBK6:  0x%08x", readArm7Data((uint)SCFG_MBK6));
		iprintf("\n\nMBK7:  0x%08x", readArm7Data((uint)SCFG_MBK7));
		iprintf("\n\nMBK8:  0x%08x", readArm7Data((uint)SCFG_MBK8));
		iprintf("\n\nMBK9:  0x%08x", readArm7Data((uint)SCFG_MBK9));
		for (int i = 0; i < 12; i++)printf("\n");
	} else {
		printf("\nARM7 SCFG:\n\n");
		iprintf("\nSCFG_ROM:  0x%08x", readArm7Data((uint)SCFG_ROM));
		iprintf("\nSCFG_CLK:  0x%08x", readArm7Data((uint)SCFG_CLK));
		iprintf("\nSCFG_EXT:  0x%08x", readArm7Data((uint)SCFG_EXT));
		iprintf("\nConsoleID: 0x%08x\n           ", readArm7Data((uint)SCFG_CPUID));
		iprintf("0x%08x\n\n\n", readArm7Data((uint)SCFG_CPUID2));
		
		printf("SCFG_MC Delay Settings:\n\n");
		iprintf("INSERT DELAY: 0x%04x\n", (u16)readArm7Data((uint)SCFG_MC_INSERT_DELAY));
		iprintf("PWROFF DELAY: 0x%04x\n", (u16)readArm7Data((uint)SCFG_MC_PWROFF_DELAY));
		
		
		for (int i = 0; i < 6; i++)printf("\n");
	}
}

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	CustomConsoleInit();
	
	defaultExceptionHandler();
	
	fifoSetValue32Handler(FIFO_USER_02, myFIFOValue32Handler, 0);
	
	if (fatInitDefault()) {
		FILE *src;
		src = fopen("/headerDump.bin", "wb");
		if (src != NULL) {
			tonccpy(cartHeaderBuffer, (u8*)0x02FFFE00, 0x200);
			fwrite(cartHeaderBuffer, 0x200, 1, src);
		}
	}
	getSCFG_ARM9(false);
	getSFCG_ARM7(false);
	dopause("Press any button to \ndisplay MBK Registers...\n");
	getSCFG_ARM9(true);	
	getSFCG_ARM7(true);	
	dopause("Press any button to exit...");
	return 0;
}

