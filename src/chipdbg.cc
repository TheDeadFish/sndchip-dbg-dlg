#include <stdshit.h>
#include <win32hlp.h>
#include "chipdbgInt.h"

HINSTANCE ghInstance;

void chnSelCombo_init(HWND hwnd, int nChnl)
{
	char buff[32];
	for(int i = 0; i < nChnl; i++) {
		sprintf(buff, "Channel %d", i);
		dlgCombo_addStr(hwnd, 1000, buff);
	}
}

void setDlgItemHex8(HWND hwnd, int ctrlID, byte data)
{
	char buff[32]; sprintf(buff, "%02X", data);
	SetDlgItemTextA(hwnd, ctrlID, buff);
}

void setDlgItemHex16(HWND hwnd, int ctrlID, u16 data)
{
	char buff[32]; sprintf(buff, "%04X", data);
	SetDlgItemTextA(hwnd, ctrlID, buff);
}


void setDlgChkBits(HWND hwnd, const WORD* lst, byte data)
{
	for(;*lst; lst++, data >>= 1) {
		CheckDlgButton(hwnd, *lst, !!(data & 1)); }
}

void chipdbg_create(ChipDebug** ctx, HWND hParent, decltype(CHIPDBG_C64SID) id)
{
	if(!chipdbg_alive(*ctx)) {	chipdbg_destroy(ctx);
		*ctx = id(hParent); }
}

void chipdbg_toggle(ChipDebug** ctx, HWND hParent, decltype(CHIPDBG_C64SID) id)
{
	if(chipdbg_alive(*ctx)) { chipdbg_destroy(ctx); }
	else { chipdbg_create(ctx, hParent, id); }
}

void chipdbg_destroy(ChipDebug** ctx)
{
	ChipDebug* dbg = *ctx;
	if(dbg) delete dbg; *ctx = 0;
}

int chipdbg_alive(ChipDebug* This)
{
	return This && This->hwnd;
}


void chipdbg_write(ChipDebug* This, int addr, int data)
{
	if(This) This->write(addr, data);
}

void chipdbg_select(ChipDebug* This, int chnl)
{
	if(This) This->select(chnl);
}


WCHAR* chipdbg_resName(WCHAR* str, int index)
{
	wsprintfW(str, L"CHIPDBG_%d", index);
	return str;
}

#if 0
extern "C" __declspec( dllexport )
int __stdcall DllMainCRTStartup(
	void* h, int r, void* s) { return 1; }
#endif
