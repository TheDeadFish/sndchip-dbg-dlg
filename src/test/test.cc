#include <stdshit.h>
#include <win32hlp.h>
#include "../chipdbg.h"
#include "resource.h"
const char progName[] = "test";

ChipDebug* c64SidDbg;
ChipDebug* ym2612Dbg;
int seqCount;

void ym2612_write1(int addr, int data)
{
	if(inRng1(addr&255, 0x30, 0xA8)) {
		if((addr & 3) == 1) data = ~data;
		if((addr & 3) == 2) data ^= 0x55; }
	chipdbg_write(ym2612Dbg, addr, data);
}

void onTimerYm2612(HWND hwnd)
{
	int secs = seqCount / 20;

	// note on/off
	chipdbg_write(ym2612Dbg, 0x28, (0x10 << (seqCount & 3)) | 0);
	chipdbg_write(ym2612Dbg, 0x28, (0x10 << (seqCount/2 & 3)) | 1);
	chipdbg_write(ym2612Dbg, 0x28, (0x10 << (seqCount/3 & 3)) | 2);
	chipdbg_write(ym2612Dbg, 0x28, (0x10 << (seqCount & 3)) | 4);
	chipdbg_write(ym2612Dbg, 0x28, (0x10 << (seqCount/2 & 3)) | 5);
	chipdbg_write(ym2612Dbg, 0x28, (0x10 << (seqCount/3 & 3)) | 6);
	chipdbg_write(ym2612Dbg, 0x28, 0 | 4);
	chipdbg_write(ym2612Dbg, 0x28, 0 | 5);
	chipdbg_write(ym2612Dbg, 0x28, 0 | 6);
	
	
	// timers
	chipdbg_write(ym2612Dbg, 0x24, seqCount<<2);
	chipdbg_write(ym2612Dbg, 0x25, seqCount);
	chipdbg_write(ym2612Dbg, 0x26, secs);
	
	chipdbg_write(ym2612Dbg, 0x22, 1 << (seqCount/3 & 3));
	chipdbg_write(ym2612Dbg, 0x27, 1 << (seqCount/3 & 7));

	// dac
	chipdbg_write(ym2612Dbg, 0x2B, (secs&1)*128);
	for(int i = 0; i < 100; i++) {
		chipdbg_write(ym2612Dbg, 0x2A, rand()); }

		
	for(int i = 0x30; i < 512; i++) {
		ym2612_write1(i, 0xFF); }
		
	// 0x30 - 0x9F
	int val = 15; if(seqCount & 8) val = 0xF0;
	int idx = ((seqCount/16 % 7) * 0x10);
	idx += ((seqCount/16/7 % 4) * 4);
	ym2612_write1(idx+0x30, val);
	ym2612_write1(idx+0x31, val);
	ym2612_write1(idx+0x32, val);
	ym2612_write1(idx+0x130, val);
	ym2612_write1(idx+0x131, val);
	ym2612_write1(idx+0x132, val);
	
	
	
	
	
}


void onTimer(HWND hwnd)
{
	seqCount += 1;

	// update c64 sid registers
	if(chipdbg_alive(c64SidDbg)) {
		for(int i = 0; i < 25; i++) 
			chipdbg_write(c64SidDbg, i, i + (seqCount << 4));
		chipdbg_write(c64SidDbg, 0x04, 1 << (seqCount & 7));
		chipdbg_write(c64SidDbg, 0x0B, 1 << (seqCount/2 & 7));
		chipdbg_write(c64SidDbg, 0x12, 1 << (seqCount/3 & 7));
		chipdbg_write(c64SidDbg, 0x17, 1 << (seqCount/2 & 7));
		chipdbg_write(c64SidDbg, 0x18, 1 << (seqCount/3 & 7));
	}
	
	// update YM2612 registers
	if(chipdbg_alive(ym2612Dbg)) {
		onTimerYm2612(hwnd);
	}
}

INT_PTR WINAPI dlgProc( HWND hwnd,
    UINT uMsg, WPARAM wParam, LPARAM lParam
){
	DLGMSG_SWITCH(
	  ON_MESSAGE(WM_INITDIALOG, SetTimer(hwnd, 1, 50, 0))
	  ON_MESSAGE(WM_TIMER, onTimer(hwnd));
		ON_MESSAGE(WM_CLOSE, EndDialog(hwnd, 0));
		CASE_COMMAND(
			ON_COMMAND(IDC_C64SID,  chipdbg_toggle(
				&c64SidDbg, hwnd, CHIPDBG_C64SID))
			ON_COMMAND(IDC_YM2612,  chipdbg_toggle(
				&ym2612Dbg, hwnd, CHIPDBG_YM2612))
		,)
	,)		
}

int main()
{
	DialogBox(NULL, MAKEINTRESOURCE(
		IDD_DIALOG1), NULL, dlgProc);
}
