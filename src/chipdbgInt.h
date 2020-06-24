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

#define CHIPDBG_DEFCREATE(type, idd) CHIPDBG_CALL \
ChipDebug* CHIPDBG_##idd(HWND hParent) { type* dbg = new type(); \
	extern const WCHAR resn_CHIPDBG_##idd[]; \
	CreateDialogParam(ghInstance, resn_CHIPDBG_##idd, \
	hParent, dbg->cDlgProc, (LPARAM)dbg); return dbg; }

WCHAR* chipdbg_resName(WCHAR* str, int index);

// resource allocations
#define YM2612_ALGO 0
