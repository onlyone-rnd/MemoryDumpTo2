#pragma once

#include <windows.h>
#include <QtGlobal>
#include "res/version.h"
#include "res/resource.h"
#include "pluginsdk/_plugins.h"

extern HMODULE g_hModule;
extern HWND    g_hWndDlg;

enum Action : int {
    MD_ABOUT,
    MD_DUMP_TO_FILE,
    MD_DUMP_TO_TABLE,
    MD_MEMMAP_TO_FILE,
    MD_MEMMAP_TO_TABLE,
    MD_DISASSM_TO_FILE,
    MD_DISASSM_TO_TABLE,
};

typedef struct {
    uchar* memStart;
    uchar* memEnd;
    uchar* memBase;
    size_t memSize;
} MEMDUMP;
