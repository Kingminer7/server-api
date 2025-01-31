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
        static ServerAPI *get();
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
        std::string getSecondaryUrl();
        std::map<int, std::pair<std::string, int>> getAllServers();
        bool firstML = true;
};
