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
#include <map>
#include <functional>
#include <memory>
#include <network/udp_client.h>
#include <capture/screen_capture.h>
#include <thread>

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
    MENU_ID_GLOBAL_STOP_SCREEN_SHARE,
    MENU_ID_CLIENT_VIEW_SCREEN_SHARE
};


void ts3plugin_initMenus(struct PluginMenuItem*** menuItems, [[maybe_unused]] char** menuIcon) {
    const size_t itemsAmount = 4;
    *menuItems = new PluginMenuItem*[itemsAmount];

    int n = 0;
    (*menuItems)[n++] = createMenuItem(PLUGIN_MENU_TYPE_GLOBAL, MENU_ID_GLOBAL_START_SCREEN_SHARE,  "Start screen share", "");
    (*menuItems)[n++] = createMenuItem(PLUGIN_MENU_TYPE_GLOBAL, MENU_ID_GLOBAL_STOP_SCREEN_SHARE, "Stop screen share", "");
    (*menuItems)[n++] = createMenuItem(PLUGIN_MENU_TYPE_CLIENT, MENU_ID_CLIENT_VIEW_SCREEN_SHARE, "View screen share", "");

    (*menuItems)[n] = nullptr;
}

std::shared_ptr<udp_client> client;
std::unique_ptr<screen_capture> capture;

void startScreenShare([[maybe_unused]] uint64_t serverConnectionHandlerID) {
    if(capture == nullptr) {
        capture = std::make_unique<screen_capture>(client, 1680, 1050);
    }

    int32_t res = capture->start();
    if(res < 0) {
        std::cout << "Failed to start screen sharing!" << std::endl;
        return;
    }
    std::cout << "Started capture!" << std::endl;
}

void stopScreenShare([[maybe_unused]] uint64_t serverConnectionHandlerID) {
    if(capture == nullptr) return;

    capture->stop();
}

void ts3plugin_onConnectStatusChangeEvent([[maybe_unused]] uint64 serverConnectionHandlerID, int newStatus, [[maybe_unused]] unsigned int errorNumber) {
    if(newStatus == STATUS_CONNECTION_ESTABLISHED) {
        std::cout << "Connected to server!" << std::endl;

        client = std::make_shared<udp_client>("127.0.0.1", 30004);
    }else if(newStatus == STATUS_DISCONNECTED) {
        std::cout << "Disconnected from server!" << std::endl;
    }
}

const std::map<int, std::function<void(uint64_t)>> menuEventMap = {
    {0, &startScreenShare},
    {1, &stopScreenShare}
};

void ts3plugin_onMenuItemEvent(uint64_t serverConnectionHandlerID, [[maybe_unused]] enum PluginMenuType type, int menuItemId, [[maybe_unused]] uint64_t selectedItemId) {
    const auto search = menuEventMap.find(menuItemId);
    if(search == menuEventMap.end()) return;
    search->second(serverConnectionHandlerID);
}


void ts3plugin_freeMemory(void* data) {
    free(data);
}
