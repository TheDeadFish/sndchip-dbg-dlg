#include <stdshit.h>
#include <win32hlp.h>
#include "resource.h"
#include "../chipdbgInt.h"
#include "vgx/vgxutil.h"

double ym2612_freq(int clock, int A0, int A4)
{
	int fNum = A0 + ((A4 & 7) << 8); int Block = (A4 >> 3) & 7;
	return (fNum * (clock / 144.0) * (1 << (Block))) / 2097152;
}

char* ym2612_freqToStr(char* buff, int clock, int A0, int A4)
{
	return vgx_midiNoteFmt2F(buff, 
		vgx_getMidiNote(ym2612_freq(clock, A0, A4)));
}

struct YM2612Debug : ChipDebug
{

	// vtable functions
	~YM2612Debug() { DestroyWindow(hwnd); }
	void write(int addr, int data);
	void select(int sel) { setDlgTabPage(
		hwnd, IDC_YM2612_TAB, sel); }
	
//private:
	
	int chnSel; 
	
	u32 notesMask;
	u32 notesChng;
	
	
	
	HBITMAP algoBm[8];
	
	int dacCount;
	byte regs[512];
	
	
	void onTimer(HWND hwnd);
	
	void initDialog(HWND hwnd);
	MEMBER_DLGPROC(YM2612Debug, 
		DlgProc, This->initDialog(hwnd))	
};

void YM2612Debug::initDialog(HWND hwnd)
{
	this->hwnd = hwnd;
	
	for(int i = 0; i < 8; i++) {
		algoBm[i] = LoadBitmap( ghInstance, 
			MAKEINTRESOURCE( IDB_ALGO0 + i) ); }
	
	// init tab control
	for(int i = 0; i < 6; i++) {
		char buff[32]; sprintf(buff, "Channel %d", i+1);
		addDlgTabPage(hwnd, IDC_YM2612_TAB, i, buff); }
		
	SetTimer(hwnd, 1, 10, 0);
	select(0);
}

#define FMT_CAT(fmt, ...) curPos += wsprintfA(curPos, fmt, __VA_ARGS__)

void YM2612Debug::onTimer(HWND hwnd)
{
	char buff[1024];
	const char* str;
	
	// get channel
	int chnl = getDlgTabPage(hwnd, IDC_YM2612_TAB);
	int addr = chnl;
	if(addr >= 3) addr += (256-3);
	byte* vReg = regs+addr;
	
	// Operators
	for(int op = 0; op < 4; op++) {
		byte* oReg = vReg + op*4;
		char* curPos = buff;
		
		FMT_CAT("0x%0.2X\n", oReg[0x30] >> 4);
		FMT_CAT("0x%0.2X\n", oReg[0x30] & 15);
		FMT_CAT("0x%0.2X\n", oReg[0x40] & 0x7F);
		FMT_CAT("0x%0.2X\n", oReg[0x50] >> 6);
		FMT_CAT("0x%0.2X\n", oReg[0x50] & 0x1F);
		FMT_CAT(oReg[0x60] & 0x80 ? "Yes\n" : "No\n", 0);
		FMT_CAT("0x%0.2X\n", oReg[0x60] & 0x1F);
		FMT_CAT("0x%0.2X\n", oReg[0x70] & 0x1F);
		FMT_CAT("0x%0.2X\n", oReg[0x80] >> 4);	
		FMT_CAT("0x%0.2X\n", oReg[0x80] & 15);
		FMT_CAT("0x%0.2X\n", oReg[0x90]);
		
		SetDlgItemTextA(hwnd, IDC_YM2612_OP1+op, buff);
	}
	
	// Channel
	char* curPos = ym2612_freqToStr(buff, 
		7670454, vReg[0xA0], vReg[0xA4]);
	if((chnl == 2) && (regs[0x27]&0x40)) {
		*curPos++ = '\n'; curPos = ym2612_freqToStr(
			curPos, 7670454, regs[0xA8], regs[0xAC]);
		*curPos++ = '\n'; curPos = ym2612_freqToStr(
			curPos, 7670454, regs[0xA9], regs[0xAD]);
		*curPos++ = '\n'; curPos = ym2612_freqToStr(
			curPos, 7670454, regs[0xAA], regs[0xAE]);
	}
		
	SetDlgItemTextA(hwnd, IDC_YM2612_FREQ, buff);
	
	wsprintfA(buff,"0x%0.2X\n0x%0.2X\n", (vReg[0xB4]>>4)&7,  vReg[0xB4]&0x03);
	SetDlgItemTextA(hwnd, IDC_YM2612_MOD, buff);
	CheckDlgButton(hwnd, IDC_YM2612_LEFT, !!(vReg[0xB4]&0x80));
	CheckDlgButton(hwnd, IDC_YM2612_RIGHT, !!(vReg[0xB4]&0x40));
	wsprintfA(buff,"0x%0.2X", (vReg[0xB0]&0x38)>>3);
	SetDlgItemTextA(hwnd, IDC_YM2612_FBACK, buff);
	SendDlgItemMessage(hwnd, IDC_YM2612_ALGO, STM_SETIMAGE,
		IMAGE_BITMAP,  (LPARAM) algoBm[vReg[0xB0] & 0x7]);

	// update key state
	u32 tmp = notesMask | release(notesChng);
	for(int i = 0; i < 24; i++, tmp >>= 1) {
		CheckDlgButton(hwnd, IDC_YM2612_KEY_1_1+i, tmp & 1); }
		
	// LFO state
	wsprintfA(buff,"LFO 0x%0.2X", regs[0x22]&0x07 );
	CheckDlgButton(hwnd, IDC_YM2612_LFO, !!(regs[0x22] & 0x08));
	SetDlgItemTextA(hwnd, IDC_YM2612_LFO, buff);

	// timer value
	int timer = (regs[0x24]<<2) | (regs[0x25]&3);
	wsprintfA(buff,"0x%0.4X\n0x%0.2X", timer, regs[0x26]);
	SetDlgItemTextA(hwnd, IDC_YM2612_TIMER, buff);
		
	// timer state
	static const WORD timerChk[] = {
		IDC_YM2612_TIMER_A1, IDC_YM2612_TIMER_B1, 
		IDC_YM2612_TIMER_A2, IDC_YM2612_TIMER_B2, 
		IDC_YM2612_TIMER_A3, IDC_YM2612_TIMER_B3, 0};
	setDlgChkBits(hwnd, timerChk, regs[0x27]);
		
	// Ch3 mode
	str = "Normal";
	if(regs[0x27]&0x40) str = "Special";
	if(regs[0x27]&0x80) str = "Illegal";
	SetDlgItemTextA(hwnd, IDC_YM2612_C3MODE, str);
	
	// dac state
	int rate = 1234;
	wsprintfA(buff,"DAC 0x%0.2X at %0.5dHz", regs[0x2A], rate );
	CheckDlgButton(hwnd, IDC_YM2612_DAC, !!(regs[0x2B] & 0x80));
	SetDlgItemTextA(hwnd, IDC_YM2612_DAC, buff);
}


void YM2612Debug::write(int addr, int data)
{
	regs[addr] = data;
	if(addr == 0x2A) dacCount ++;
	
	if(addr == 0x28) {
		// get op number
		int opn = data & 7;
		if(opn & 4) opn--; 
		opn <<= 2;
		
		// get value
		data = ((data>>4) & 15);
		notesMask &= ~(15 << opn);
		notesMask |= (data << opn);
		notesChng |= notesMask;
	}
}

INT_PTR YM2612Debug::DlgProc( HWND hwnd,
    UINT uMsg, WPARAM wParam, LPARAM lParam
){
	DLGMSG_SWITCH(
		ON_MESSAGE(WM_NCDESTROY, 
			for(auto x : algoBm) DeleteObject(x))
	
		ON_MESSAGE(WM_CLOSE, destroy());
		ON_MESSAGE(WM_TIMER, onTimer(hwnd));
	,)
}


CHIPDBG_DEFCREATE(ym2612_create, 
	YM2612Debug, IDD_DEBUGYM2612)
