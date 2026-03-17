
#include "plugin.h"
#include "utils/utils.h"
#include "dialog/AboutDialog.h"

HMODULE g_hModule = NULL;
HWND    g_hWndDlg = NULL;

int g_hMenu = 0;
int g_hMenuDump = 0;
int g_hMenuStack = 0;
int g_hMenuMemmap = 0;
int g_hMenuDisasm = 0;
int g_hPlugin = 0;

BOOL APIENTRY DllMain(
    HMODULE hModule,
    ULONG   fdwReason,
    PVOID   pReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            g_hModule = hModule;
            DisableThreadLibraryCalls(hModule);
            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

static void initMenu()
{
    do {
        ICONDATA iconData{};
        if (!_plugin_qt_getimagerefromsource(":/png/images/main.png", (void**)&iconData.data, (size_t*)&iconData.size))
            break;
        _plugin_menuseticon(g_hMenu, &iconData);
        _plugin_menuseticon(g_hMenuDump, &iconData);
        _plugin_menuseticon(g_hMenuStack, &iconData);
        _plugin_menuseticon(g_hMenuMemmap, &iconData);
        _plugin_menuseticon(g_hMenuDisasm, &iconData);

        /* About */
        if (!_plugin_menuaddentry(g_hMenu, MD_ABOUT, "About"))
            break;
        if (!_plugin_qt_getimagerefromsource(":/png/images/about.png", (void**)&iconData.data, (size_t*)&iconData.size))
            break;
        _plugin_menuentryseticon(g_hPlugin, MD_ABOUT, &iconData);
        /* Dump to file */
        if (!_plugin_menuaddentry(g_hMenuDump, MD_DUMP_TO_FILE, "Save to bin file..."))
            break;
        if (!_plugin_qt_getimagerefromsource(":/png/images/save.png", (void**)&iconData.data, (size_t*)&iconData.size))
            break;
        _plugin_menuentryseticon(g_hPlugin, MD_DUMP_TO_FILE, &iconData);
        /* Dump to Table */
        if (!_plugin_menuaddentry(g_hMenuDump, MD_DUMP_TO_TABLE, "Convert to table..."))
            break;
        if (!_plugin_qt_getimagerefromsource(":/png/images/conv.png", (void**)&iconData.data, (size_t*)&iconData.size))
            break;
        _plugin_menuentryseticon(g_hPlugin, MD_DUMP_TO_TABLE, &iconData);
        if (!_plugin_menuaddentry(g_hMenuMemmap, MD_MEMMAP_TO_FILE, "Save to bin file..."))
            break;
        if (!_plugin_qt_getimagerefromsource(":/png/images/save.png", (void**)&iconData.data, (size_t*)&iconData.size))
            break;
        _plugin_menuentryseticon(g_hPlugin, MD_MEMMAP_TO_FILE, &iconData);
        /* Memmap to Table */
        if (!_plugin_menuaddentry(g_hMenuMemmap, MD_MEMMAP_TO_TABLE, "Convert to table..."))
            break;
        if (!_plugin_qt_getimagerefromsource(":/png/images/conv.png", (void**)&iconData.data, (size_t*)&iconData.size))
            break;
        _plugin_menuentryseticon(g_hPlugin, MD_MEMMAP_TO_TABLE, &iconData);
        /* Disasm to file */
        if (!_plugin_menuaddentry(g_hMenuDisasm, MD_DISASSM_TO_FILE, "Save to bin file..."))
            break;
        if (!_plugin_qt_getimagerefromsource(":/png/images/save.png", (void**)&iconData.data, (size_t*)&iconData.size))
            break;
        _plugin_menuentryseticon(g_hPlugin, MD_DISASSM_TO_FILE, &iconData);
        /* Disasm to Table */
        if (!_plugin_menuaddentry(g_hMenuDisasm, MD_DISASSM_TO_TABLE, "Convert to table..."))
            break;
        if (!_plugin_qt_getimagerefromsource(":/png/images/conv.png", (void**)&iconData.data, (size_t*)&iconData.size))
            break;
        _plugin_menuentryseticon(g_hPlugin, MD_DISASSM_TO_TABLE, &iconData);

    } while (0);
}

PLUGIN_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
    g_hWndDlg = setupStruct->hwndDlg;
    g_hMenu = setupStruct->hMenu;
    g_hMenuDump = setupStruct->hMenuDump;
    g_hMenuStack = setupStruct->hMenuStack;
    g_hMenuMemmap = setupStruct->hMenuMemmap;
    g_hMenuDisasm = setupStruct->hMenuDisasm;
    initMenu();
}

PLUGIN_EXPORT bool pluginit(PLUG_INITSTRUCT* initStruct)
{
    initStruct->pluginVersion = (int)PLUGIN_VERSION;
    initStruct->sdkVersion = PLUG_SDKVERSION;
    g_hPlugin = initStruct->pluginHandle;
    return true;
}

void memoryDumpTo(Action action);

PLUGIN_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    Action action = (Action)info->hEntry;
    switch (action)
    {
        case MD_ABOUT: {

            ShowAboutDialog(g_hWndDlg);
            break;
        }
        default: {
            memoryDumpTo(action);
            break;
        }
    }
}

PLUGIN_EXPORT bool plugstop()
{
    return true;
}
