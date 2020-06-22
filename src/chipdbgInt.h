#pragma once
#include "chipdbg.h"

extern HINSTANCE ghInstance;

void chnSelCombo_init(HWND hwnd, int nChnl);
void setDlgItemHex8(HWND hwnd, int ctrlID, byte data);
void setDlgItemHex16(HWND hwnd, int ctrlID, u16 data);
void setDlgChkBits(HWND hwnd, const WORD* lst, byte data);

struct ChipDebug
{
	virtual ~ChipDebug(){};
	virtual void write(int addr, int data){};
	virtual void select(int chnl){};
	
	HWND hwnd;
	void destroy() { DestroyWindow(hwnd); hwnd =0; }
};
	
 
ChipDebug* c64sid_create(HWND);
ChipDebug* ym2612_create(HWND);

#define CHIPDBG_DEFCREATE(name, type, idd) \
ChipDebug* name(HWND hParent) { type* dbg = new type(); \
	CreateDialogParam(ghInstance, MAKEINTRESOURCE(idd), \
	hParent, dbg->cDlgProc, (LPARAM)dbg); return dbg; }
