#pragma once
#define CHIPDBG_CALL extern "C" __stdcall

struct ChipDebug;

// define basic types
#ifndef _WINDOWS_
 typedef void* HWND;
#endif


CHIPDBG_CALL ChipDebug* CHIPDBG_C64SID(HWND);
CHIPDBG_CALL ChipDebug* CHIPDBG_YM2612(HWND);

// creation api
CHIPDBG_CALL
void chipdbg_create(ChipDebug**, HWND, decltype(CHIPDBG_C64SID));
CHIPDBG_CALL
void chipdbg_toggle(ChipDebug**, HWND, decltype(CHIPDBG_C64SID));
CHIPDBG_CALL
void chipdbg_destroy(ChipDebug**);

CHIPDBG_CALL
int chipdbg_alive(ChipDebug*); 
CHIPDBG_CALL
void chipdbg_write(ChipDebug*, int addr, int data);
CHIPDBG_CALL
void chipdbg_select(ChipDebug*, int chnl);
