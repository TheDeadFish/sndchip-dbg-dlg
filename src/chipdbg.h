#pragma once

struct ChipDebug;

// creation api
extern "C" __declspec(dllexport)
ChipDebug* chipdbg_create_(void* hParent, long long id);
extern "C" __declspec(dllexport)
void chipdbg_create(ChipDebug**, void* hParent, long long id);
extern "C" __declspec(dllexport)
void chipdbg_toggle(ChipDebug**, void* hParent, long long id);
extern "C" __declspec(dllexport)
void chipdbg_destroy(ChipDebug**);

extern "C" __declspec(dllexport) __thiscall
int chipdbg_alive(ChipDebug*); 
extern "C" __declspec(dllexport) __thiscall
void chipdbg_write(ChipDebug*, int addr, int data);
extern "C" __declspec(dllexport) __thiscall
void chipdbg_select(ChipDebug*, int chnl);

#define CHIPDBG_C64SID 0x444953343643LL
#define CHIPDBG_YM2612 0x323136324d59LL
