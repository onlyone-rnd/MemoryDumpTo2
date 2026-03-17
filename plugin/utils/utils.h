#pragma once

#include "global.h"

bool _plugin_qt_getimagerefromsource(
    _In_ const char* url,
    _Out_ void** data,
    _Out_ size_t* size);

bool _plugin_qt_getsavefilename(
    _In_ HWND hParent,
    _In_z_ const wchar_t* defaultName,
    _In_ uchar* memStart,
    _In_ uchar* memEnd,
    _Out_writes_z_(MAX_PATH) wchar_t* fileName);

void _plugin_qt_lasterrormessage(
    _In_ HWND hParent,
    _In_opt_ ulong error);

bool isDark();

