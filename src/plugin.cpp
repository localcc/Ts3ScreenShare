//
// Created by localcc on 3/19/21.
//
#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#include <Windows.h>
#include <Winsock2.h>
#endif
#define __STDC_CONSTANT_MACROS
extern "C" {
#include <libavdevice/avdevice.h>
}
#include <plugin.h>
#include <iostream>
#include <cstring>

const int apiVersion = 23;

char* pluginID;

static struct TS3Functions ts3Functions;

const char* ts3plugin_name() {
    return "Ts3ScreenShare";
}

const char* ts3plugin_version() {
    return "0.1 Beta";
}

int ts3plugin_apiVersion() {
    return apiVersion;
}

const char* ts3plugin_author() {
    return "localcc";
}

const char* ts3plugin_description() {
    return "Plugin that enables screen sharing in teamspeak";
}

void ts3plugin_setFunctionPointers(const struct TS3Functions functions) {
    ts3Functions = functions;
}

int ts3plugin_init() {
#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
    WSAData wsaData {};
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(res != 0) {
        return res;
    }
#endif
    avdevice_register_all();

    return 0;
}

void ts3plugin_shutdown() {
    if(pluginID != nullptr) {
        delete[] pluginID;
    }
}

void ts3plugin_registerPluginID(const char* id) {
    const auto size = strlen(id) + 1;
    pluginID = new char[size];
    std::copy(id, id + size, pluginID);

    std::cout << "Plugin id: " << pluginID << std::endl;
}


static struct PluginMenuItem* createMenuItem(enum PluginMenuType type, int id, const char* text, const char* icon) {
    auto* const menuItem = new PluginMenuItem;

    menuItem->type = type;
    menuItem->id = id;

    std::copy_n(text, PLUGIN_MENU_BUFSZ, menuItem->text);
    std::copy_n(icon, PLUGIN_MENU_BUFSZ, menuItem->icon);

    return menuItem;
}


enum {
    MENU_ID_GLOBAL_START_SCREEN_SHARE,
    MENU_ID_GLOBAL_STOP_SCREEN_SHARE
};


void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, char** menuIcon) {
    const size_t itemsAmount = 3;
    *menuItems = new PluginMenuItem*[itemsAmount];

    int n = 0;
    (*menuItems)[n++] = createMenuItem(PLUGIN_MENU_TYPE_GLOBAL, MENU_ID_GLOBAL_START_SCREEN_SHARE,  "Start screen share", "");
    (*menuItems)[n++] = createMenuItem(PLUGIN_MENU_TYPE_GLOBAL, MENU_ID_GLOBAL_STOP_SCREEN_SHARE, "Stop screen share", "");

    (*menuItems)[n] = nullptr;
}

void ts3plugin_onMenuItemEvent(uint64_t serverConnectionHandlerID, enum PluginMenuType type, int menuItemId, uint64_t selectedItemId) {
    std::cout << "Connection handler: " << serverConnectionHandlerID << " type: " << type << " menuItemId: " << menuItemId << " selectedItemId: " << selectedItemId << std::endl;
}


void ts3plugin_freeMemory(void* data) {
    free(data);
}