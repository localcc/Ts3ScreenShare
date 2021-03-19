#pragma once


#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#define PLUGINS_EXPORTDLL __declspec(dllexport)
#else
#define PLUGINS_EXPORTDLL __attribute__ ((visibility("default")))
#endif

extern "C" {
    PLUGINS_EXPORTDLL const char* ts3plugin_name();
    PLUGINS_EXPORTDLL const char* ts3plugin_version();
    PLUGINS_EXPORTDLL int ts3plugin_apiVersion();
    PLUGINS_EXPORTDLL const char* ts3plugin_author();
    PLUGINS_EXPORTDLL const char* ts3plugin_description();
    PLUGINS_EXPORTDLL void ts3plugin_setFunctionPointers(const struct TS3Functions funcs);
    PLUGINS_EXPORTDLL int ts3plugin_init();
    PLUGINS_EXPORTDLL void ts3plugin_shutdown();

    PLUGINS_EXPORTDLL void ts3plugin_registerPluginID(const char* id);
}