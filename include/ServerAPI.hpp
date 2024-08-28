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
    protected: 
        static ServerAPI *instance;
        std::string server = "https://www.boomlings.com/database/";
	public:

        static ServerAPI *get() {
            if (!instance) {
                instance = new ServerAPI();
            }
            return instance;
        };
        std::string getServerURL();
        void setServerURL(std::string url);
};