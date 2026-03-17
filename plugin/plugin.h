#pragma once

#include "global.h"

#ifndef PLUGIN_EXPORT
#define PLUGIN_EXPORT __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    PLUGIN_EXPORT bool pluginit(PLUG_INITSTRUCT* initStruct);
    PLUGIN_EXPORT bool plugstop();
    PLUGIN_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct);
    PLUGIN_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info);

#ifdef __cplusplus
}
#endif

