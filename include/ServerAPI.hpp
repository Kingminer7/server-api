#pragma once

#include <Geode/Geode.hpp>
#ifdef GEODE_IS_WINDOWS
    #ifdef KM7DEV_SERVER_API_EXPORTING
        #define SERVER_API_DLL __declspec(dllexport)
    #else
        #define SERVER_API_DLL __declspec(dllimport)
    #endif
#else
    #define SERVER_API_DLL __attribute__((visibility("default")))
#endif

class SERVER_API_DLL ServerAPI {
    private:
        // <id, <url, prio>>
        int nextId = 0;
        std::string baseUrl;
        std::string secondaryUrl;
    protected: 
        static ServerAPI *instance;
        std::map<int, std::pair<std::string, int>> overrides = {};
	public:
        static ServerAPI *get() {
            if (!instance) {
                instance = new ServerAPI();
            }
            geode::log::info("{}", defined(GEODE_IS_LITE));
            #ifdef GEODE_IS_WINDOWS
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0x53ea48);
            #elif defined(GEODE_IS_ARM_MAC)
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0x7749fb);
            #elif defined(GEODE_IS_INTEL_MAC)
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0x8516bf);
            #elif defined(GEODE_IS_ANDROID64)
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0xEA2988);
            #elif defined(GEODE_IS_ANDROID32)
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0x952E9E);
            #else
                static_assert(false, "Unsupported platform");
            #endif
            if(instance->baseUrl.size() > 36) instance->baseUrl = instance->baseUrl.substr(0, 35);
            return instance;
        };
        std::string getCurrentURL();
        int getCurrentPrio();
        int getCurrentId();
        std::string getURLById(int id);
        int getPrioById(int id);
        int registerURL(std::string url, int priority = 0);
        void removeURL(int id);
        void updateURLAndPrio(int id, std::string url, int priority);
        void updatePrio(int id, int priority);
        void updateURL(int id, std::string url);
        std::string getBaseUrl();
        std::map<int, std::pair<std::string, int>> getAllServers();
        bool firstML = true;
};
