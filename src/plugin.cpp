//
// Created by localcc on 3/19/21.
//
#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#include <Windows.h>
#endif
#include <plugin.h>
#include <cstddef>
#include <ts3_functions.h>
#include <memory>
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

