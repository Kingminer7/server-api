#pragma once

#include <Geode/Geode.hpp>

#ifdef GEODE_IS_WINDOWS
	#ifdef KM7DEV_SERVER_API_EXPORTING
		#define KM7DEV_SERVER_API_DLL __declspec(dllexport)
	#else
		#define KM7DEV_SERVER_API_DLL __declspec(dllimport)
	#endif
	#else
		#define KM7DEV_SERVER_API_DLL
#endif

class KM7DEV_SERVER_API_DLL ServerAPI {
    private: 
        static ServerAPI *instance;
        std::string server = "https://www.boomlings.com/database/"
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