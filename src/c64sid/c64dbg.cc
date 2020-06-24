#include <stdshit.h>
#include <win32hlp.h>
#include "resource.h"
#include "../chipdbgInt.h"

struct C64Debug : ChipDebug
{

	// vtable functions
	~C64Debug() { DestroyWindow(hwnd); }
	void write(int addr, int data) {
		regs[addr] = data; regUpdate(addr); }
	void select(int sel) { dlgCombo_setSel(
		hwnd, IDC_CHNSEL, sel); chnUpdate(); }

//private:
	
	int chnSel; 
	byte regs[25];
	double freqScale;
	
	void regUpdate(int addr);
	void chnUpdate(void);
	
	void initDialog(HWND hwnd);
	MEMBER_DLGPROC(C64Debug, 
		DlgProc, This->initDialog(hwnd))	
};

void C64Debug::initDialog(HWND hwnd)
{
	this->hwnd = hwnd; 
	chnSelCombo_init(hwnd, 3); select(0);
	for(int i = 0x15; i <= 0x18; i++)
		regUpdate(i);
}

void C64Debug::chnUpdate(void)
{	
	chnSel = dlgCombo_getSel(hwnd, IDC_CHNSEL);
	for(int i = 0; i < 7; i++) 
		regUpdate(chnSel*7 + i);
}

void C64Debug::regUpdate(int addr)
{
	static const WORD ctrlChk[] = {
		IDC_GATE, IDC_SYNC, IDC_RING, IDC_TEST,
		IDC_TRI, IDC_SAW, IDC_PULSE, IDC_NOISE, 0};
	static const WORD filtChk[] = { IDC_FILT1, 
		IDC_FILT2, IDC_FILT3, IDC_FILTEX, 0};
	static const WORD modeChk[] = { IDC_LOW, 
		IDC_BAND, IDC_HIGH, IDC_CH3OFF, 0};
		
	// get channel
	int iChnl = addr / 7; addr %= 7;
	byte* vRegs = &regs[iChnl*7];
	if(iChnl >= 3) goto GLOBAL_REGS;
	if(iChnl != chnSel) return;
	
	// channel registers
	switch(addr) {
	case 0: case 1:	setDlgItemHex16(hwnd, IDC_FREQ, 
			RW(vRegs,0) & 0xffff); break;		
	case 2: case 3: setDlgItemHex16(hwnd, IDC_DUTY, 
			RW(vRegs,2) & 0xfff); break;
			
			
	case 4: setDlgChkBits(hwnd, ctrlChk, vRegs[4]); break;
	case 5: setDlgItemHex8(hwnd, IDC_AD, vRegs[5]); break;
	case 6: setDlgItemHex8(hwnd, IDC_SR, vRegs[6]); break;
	} return;
	
	// global registers
GLOBAL_REGS:
	switch(addr) {
	case 0: case 1: {
		int value = ((vRegs[0] & 3) | (vRegs[1] << 3)) & 0x7FF;
		SetDlgItemInt(hwnd, IDC_CUTOFF, value, 0); break; }
	case 2:	
		setDlgChkBits(hwnd, filtChk, vRegs[2]);
		SetDlgItemInt(hwnd, IDC_RES, vRegs[2] >> 4, 0);
		break;
	case 3:
		setDlgChkBits(hwnd, modeChk, vRegs[3] >> 4);
		setDlgItemHex8(hwnd, IDC_VOLUME, vRegs[3] & 15);
		break;
	}
	

}

INT_PTR C64Debug::DlgProc( HWND hwnd,
    UINT uMsg, WPARAM wParam, LPARAM lParam
){
	DLGMSG_SWITCH(
		ON_MESSAGE(WM_CLOSE, destroy());
		CASE_COMMAND(
			ON_CONTROL(CBN_SELCHANGE, IDC_CHNSEL, chnUpdate())
		,)
	,)
}


CHIPDBG_DEFCREATE(C64Debug, C64SID)
