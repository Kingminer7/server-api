#include <Geode/Geode.hpp>
#include "ServerAPI.hpp"
#include "../include/ServerAPIEvents.hpp"
#include "Geode/cocos/support/zip_support/ZipUtils.h"

constexpr int REQUIRED_GD_VERSION = 22074;

#ifdef GEODE_IS_ANDROID
    constexpr bool isSupportedPlatform() { return true; }
    bool evaluateAmazon() {
        return !((GJMoreGamesLayer *volatile)nullptr)->getMoreGamesList()->count();
    }
    #ifdef GEODE_IS_ANDROID64
        constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0xea27f8;
        constexpr std::uintptr_t BASE_URL_OFFSET             = 0xea2988;
        constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0xea15b8;
        constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0xea1748;
    #elif defined(GEODE_IS_ANDROID32)
        constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x952cce;
        constexpr std::uintptr_t BASE_URL_OFFSET             = 0x952e9e;
        constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x861c41;
        constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x951e11;
    #endif
#else
    bool evaluateAmazon() { return false; }
#endif
#ifdef GEODE_IS_WINDOWS
    constexpr bool isSupportedPlatform() { return true; }
    constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x0;
    constexpr std::uintptr_t BASE_URL_OFFSET             = 0x53ea48;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x53ec80;
#elif defined(GEODE_IS_ARM_MAC)
    constexpr bool isSupportedPlatform() { return true; }
    constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x0;
    constexpr std::uintptr_t BASE_URL_OFFSET             = 0x7749fb;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x774c73;
#elif defined(GEODE_IS_INTEL_MAC)
    constexpr bool isSupportedPlatform() { return true; }
    constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x0;
    constexpr std::uintptr_t BASE_URL_OFFSET             = 0x8516bf;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x851947;
#elif defined(GEODE_IS_IOS)
    constexpr bool isSupportedPlatform() { return true; }
    constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x0;
    constexpr std::uintptr_t BASE_URL_OFFSET             = 0x6af51a;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x6af7a0;
#elif not defined(GEODE_IS_ANDROID)
    constexpr bool isSupportedPlatform() { return false; }
    constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x0;
    constexpr std::uintptr_t BASE_URL_OFFSET             = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x0;
#endif

using namespace ServerAPIEvents;
using namespace geode::prelude;

ServerAPI * ServerAPI::instance = nullptr;

int ServerAPI::registerURL(std::string url, int priority) {
    if (!url.ends_with("/")) url = url + "/";
    // <id, <url, prio>>
    this->m_overrides[this->m_nextId] = std::pair(url, priority);
    this->m_nextId++;
    evaluateCache();
    return this->m_nextId - 1;
}

void ServerAPI::updateURLAndPrio(int id, std::string url, int priority) {
    if (!url.ends_with("/")) url = url + "/";
    // <id, <url, prio>>
    this->m_overrides[id] = std::pair(url, priority);
    evaluateCache();
}

void ServerAPI::updateURL(int id, std::string url) {
    if (!url.ends_with("/")) url = url + "/";
    // <id, <url, prio>>
    int priority = this->m_overrides[id].second;
    this->m_overrides[id] = std::pair(url, priority);
    evaluateCache(); // TODO: Maybe cache should use references to the servers?
}

void ServerAPI::updatePrio(int id, int priority) {
    // <id, <url, prio>>
    std::string url = this->m_overrides[id].first;
    this->m_overrides[id] = std::pair(url, priority);
    evaluateCache();
}

std::string ServerAPI::getURLById(int id) {
    for (const auto& [uid, urlPrio] : m_overrides) {
        if (id == uid) {
            return urlPrio.first;
        }
    }
    return "";
}

int ServerAPI::getPrioById(int id) {
    for (const auto& [uid, urlPrio] : m_overrides) {
        if (id == uid) {
            return urlPrio.second;
        }
    }
    return 0;
}

std::string ServerAPI::getCurrentURL() {
    return m_cache.URL;
}

int ServerAPI::getCurrentPrio() {
    return m_cache.Priority;
}

int ServerAPI::getCurrentId() {
    return m_cache.ID;
}

void ServerAPI::removeURL(int id) {
    this->m_overrides.erase(id);
}

std::map<int, std::pair<std::string, int>> ServerAPI::getAllServers() {
    return m_overrides;
}

std::string ServerAPI::getBaseUrl() {
    return m_baseUrl;
}

std::string ServerAPI::getSecondaryUrl() {
    return m_secondaryUrl;
}

void ServerAPI::init() {
    m_amazon = evaluateAmazon();
    static_assert(GEODE_COMP_GD_VERSION == REQUIRED_GD_VERSION, "Unsupported GD version");
    static_assert(isSupportedPlatform(), "Unsupported platform");

    this->m_baseUrl = (char*)(geode::base::get() + (m_amazon ? BASE_URL_OFFSET_AMAZON : BASE_URL_OFFSET));
    this->m_secondaryUrl = ZipUtils::base64URLDecode((char *)(geode::base::get() + (m_amazon ? SECONDARY_URL_OFFSET_AMAZON : SECONDARY_URL_OFFSET)));
    m_cache = {0, "NONE_REGISTERED", INT_MIN};

    // DO NOT FUCKING TOUCH THIS!
    // Kam wrote this forever ago and doesn't remember what it does.
    // The commit message when these lines were added was literally just
    // "gdps url support kinda." SO DONT FUCK WITH IT!!
    if(this->m_baseUrl.size() > 36) this->m_baseUrl = this->m_baseUrl.substr(0, 35);
    if(this->m_secondaryUrl.size() > 35) this->m_secondaryUrl = this->m_secondaryUrl.substr(0, 34);
}

void ServerAPI::evaluateCache() {
    std::pair<std::string, int> highestServer = std::make_pair("NONE_REGISTERED", INT_MIN);
    int highestServerId = 0;
    
    for (const auto& [id, server] : m_overrides) {
        const auto& [url, prio] = server;
        if (prio > highestServer.second) {
            highestServerId = id;
            highestServer = server;
        }
    }

    m_cache.ID = highestServerId;
    m_cache.URL = highestServer.first;
    m_cache.Priority = highestServer.second;
}

ServerAPI *ServerAPI::get() {
    if (!instance) {
        instance = new ServerAPI();
        instance->init();
    }
    return instance;
};

bool ServerAPI::isAmazon() {
    return m_amazon;
}

$on_mod(Loaded) {
    new EventListener<EventFilter<GetCurrentServerEvent>>(+[](GetCurrentServerEvent* e) {
        e->m_url = ServerAPI::get()->getCurrentURL();
        e->m_prio = ServerAPI::get()->getCurrentPrio();
        e->m_id = ServerAPI::get()->getCurrentId();
        return ListenerResult::Stop;
    });

    new EventListener<EventFilter<GetServerByIdEvent>>(+[](GetServerByIdEvent* e) {
        e->m_url = ServerAPI::get()->getURLById(e->m_id);
        e->m_prio = ServerAPI::get()->getPrioById(e->m_id);
        return ListenerResult::Stop;
    });

    new EventListener<EventFilter<RegisterServerEvent>>(+[](RegisterServerEvent* e) {
        e->m_id = ServerAPI::get()->registerURL(e->m_url, e->m_priority);
        return ListenerResult::Stop;
    });

    new EventListener<EventFilter<UpdateServerEvent>>(+[](UpdateServerEvent* e) {
        if (e->m_url != "") {
            if (e->m_priority != 0) {
                ServerAPI::get()->updateURLAndPrio(e->m_id, e->m_url, e->m_priority);
            } else {
                ServerAPI::get()->updateURL(e->m_id, e->m_url);
            }
        } else {
            ServerAPI::get()->updatePrio(e->m_id, e->m_priority);
        }
        return ListenerResult::Stop;
    });

    new EventListener<EventFilter<RemoveServerEvent>>(+[](RemoveServerEvent* e) {
        ServerAPI::get()->removeURL(e->m_id);
        return ListenerResult::Stop;
    });

    new EventListener<EventFilter<GetBaseUrlEvent>>(+[](GetBaseUrlEvent* e) {
        e->m_url = ServerAPI::get()->getBaseUrl();
        return ListenerResult::Stop;
    });

    new EventListener<EventFilter<GetSecondaryUrlEvent>>(+[](GetSecondaryUrlEvent* e) {
        e->m_url = ServerAPI::get()->getSecondaryUrl();
        return ListenerResult::Stop;
    });

    new EventListener<EventFilter<GetRegisteredServersEvent>>(+[](GetRegisteredServersEvent* e) {
        e->m_servers = ServerAPI::get()->getAllServers();
        return ListenerResult::Stop;
    });
}