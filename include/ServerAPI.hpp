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
        std::map<int, std::pair<std::string, int>> overrides = {};
        int nextId = 0;
    protected: 
        static ServerAPI *instance;
	public:

        static ServerAPI *get() {
            if (!instance) {
                instance = new ServerAPI();
            }
            return instance;
        };
        std::string getServerURL();
        int setServerURL(std::string url, int priority = 0);
};