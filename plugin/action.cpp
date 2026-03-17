
#include "global.h"
#include "utils/utils.h"
#include "dialog/TableDialog.h"

static MEMDUMP MemDump;

enum GuiType : int {
    TYPE_NONE = -1,
    TYPE_DUMP = GUI_DUMP,
    TYPE_MEMMAP = GUI_MEMMAP,
    TYPE_DISASSM = GUI_DISASSEMBLY,
};

static GuiType selGuiType(Action action)
{
    switch (action)
    {
        case MD_DUMP_TO_FILE:
        case MD_DUMP_TO_TABLE:
            return TYPE_DUMP;
        case MD_MEMMAP_TO_FILE:
        case MD_MEMMAP_TO_TABLE:
            return TYPE_MEMMAP;
        case MD_DISASSM_TO_FILE:
        case MD_DISASSM_TO_TABLE:
            return TYPE_DISASSM;
        default: return TYPE_NONE;
    }
}

static bool getMemoryDump(
    GuiType guiType,
    MEMDUMP* memDump)
{
    if (!DbgIsDebugging())
        return false;

    SELECTIONDATA selData{};
    if (!GuiSelectionGet((GUISELECTIONTYPE)guiType, &selData))
        return false;

    size_t memSize = (size_t)selData.end - selData.start;
    memSize++;

    void* memBase = VirtualAlloc(
        nullptr,
        memSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE);
    if (!memBase)
        return false;

    if (!DbgMemRead(selData.start, memBase, memSize))
    {
        VirtualFree(memBase, 0, MEM_RELEASE);
        return false;
    }
    memDump->memStart = (uchar*)selData.start;
    memDump->memEnd = (uchar*)selData.end;
    memDump->memBase = (uchar*)memBase;
    memDump->memSize = memSize;
    return true;
}

static bool writeBufferToFile(
    const wchar_t* fileName,
    void* fileBuffer,
    size_t fileSize)
{
    HANDLE fileHandle = CreateFileW(
        fileName,
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (fileHandle == INVALID_HANDLE_VALUE)
        return false;

    bool result = WriteFile(fileHandle, fileBuffer, (ulong)fileSize, nullptr, nullptr);

    CloseHandle(fileHandle);
    return result;
}

static bool toFile(MEMDUMP* memDump)
{
    wchar_t saveFileName[MAX_PATH];

    if (!_plugin_qt_getsavefilename(
        g_hWndDlg,
        L"dump_0x%1-0x%2",
        memDump->memStart,
        memDump->memEnd,
        saveFileName))
        return false;
    if (saveFileName[0] == 0)
        return true;
    return writeBufferToFile(saveFileName, memDump->memBase, memDump->memSize);
}

static bool toTable(MEMDUMP* memDump)
{
    return ShowTableDialog(g_hWndDlg, memDump);
}

void memoryDumpTo(Action action)
{
    GuiType gui = selGuiType(action);
    if (gui == TYPE_NONE)
        return;

    if (!getMemoryDump(gui, &MemDump))
    {
        _plugin_qt_lasterrormessage(g_hWndDlg, 0);
        return;
    }
    switch (action)
    {
        case MD_DUMP_TO_FILE:
        case MD_MEMMAP_TO_FILE:
        case MD_DISASSM_TO_FILE: {
            if (!toFile(&MemDump))
                _plugin_qt_lasterrormessage(g_hWndDlg, 0);
            break;
        }
        default: {
            if (!toTable(&MemDump))
                _plugin_qt_lasterrormessage(g_hWndDlg, 0);
            break;
        }
    }
    if (MemDump.memBase)
        VirtualFree(MemDump.memBase, 0, MEM_RELEASE);
}