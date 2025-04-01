#include <Geode/Geode.hpp>
#include "ServerAPI.hpp"
#include "../include/ServerAPIEvents.hpp"
#include "Geode/cocos/support/zip_support/ZipUtils.h"
#include "LiteUtils.hpp"

using namespace ServerAPIEvents;
using namespace geode::prelude;

ServerAPI * ServerAPI::instance = nullptr;

int ServerAPI::registerURL(std::string url, int priority) {
    if (!url.ends_with("/")) url = url + "/";
    // <id, <url, prio>>
    this->overrides[this->nextId] = std::pair(url, priority);
    this->nextId++;
    return this->nextId - 1;
}

void ServerAPI::updateURLAndPrio(int id, std::string url, int priority) {
    if (!url.ends_with("/")) url = url + "/";
    // <id, <url, prio>>
    this->overrides[id] = std::pair(url, priority);
}

void ServerAPI::updateURL(int id, std::string url) {
    if (!url.ends_with("/")) url = url + "/";
    // <id, <url, prio>>
    int priority = this->overrides[id].second;
    this->overrides[id] = std::pair(url, priority);
}

void ServerAPI::updatePrio(int id, int priority) {
    // <id, <url, prio>>
    std::string url = this->overrides[id].first;
    this->overrides[id] = std::pair(url, priority);
}

std::string ServerAPI::getURLById(int id) {
    for (const auto& [uid, urlPrio] : overrides) {
        if (id == uid) {
            return urlPrio.first;
        }
    }
    return "";
}

int ServerAPI::getPrioById(int id) {
    for (const auto& [uid, urlPrio] : overrides) {
        if (id == uid) {
            return urlPrio.second;
        }
    }
    return 0;
}

std::string ServerAPI::getCurrentURL() {
    std::pair<std::string, int> highestPrioUrl = std::make_pair("NONE_REGISTERED", INT_MIN);
    
    for (const auto& [id, urlPrio] : overrides) {
        const auto& [url, prio] = urlPrio;
        if (prio > highestPrioUrl.second) {
            highestPrioUrl = urlPrio;
        }
    }
    
    return highestPrioUrl.first;
}

int ServerAPI::getCurrentPrio() {
    std::pair<std::string, int> highestPrioUrl = std::make_pair("NONE_REGISTERED", INT_MIN);
    
    for (const auto& [id, urlPrio] : overrides) {
        const auto& [url, prio] = urlPrio;
        if (prio > highestPrioUrl.second) {
            highestPrioUrl = urlPrio;
        }
    }
    
    return highestPrioUrl.second;
}

int ServerAPI::getCurrentId() {
    std::pair<std::string, int> highestPrioUrl = std::make_pair("NONE_REGISTERED", INT_MIN);
    int highestPrioId = 0;
    
    for (const auto& [id, urlPrio] : overrides) {
        const auto& [url, prio] = urlPrio;
        if (prio > highestPrioUrl.second) {
            highestPrioUrl = urlPrio;
            highestPrioId = id;
        }
    }
    
    return highestPrioId;
}

void ServerAPI::removeURL(int id) {
    this->overrides.erase(id);
}

std::map<int, std::pair<std::string, int>> ServerAPI::getAllServers() {
    return overrides;
}

std::string ServerAPI::getBaseUrl() {
    return baseUrl;
}

std::string ServerAPI::getSecondaryUrl() {
    return secondaryUrl;
}

ServerAPI *ServerAPI::get() {
            if (!instance) {
                instance = new ServerAPI();
            }
            if (geode::lite::isLite()) {
                #ifdef GEODE_IS_ANDROID64
                    static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                    instance->baseUrl = (char*)(geode::base::get() + 0xE9CF78);
                #elif defined(GEODE_IS_ANDROID32)
                    static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                    instance->baseUrl = (char*)(geode::base::get() + 0x95039f);
                #endif
            } else {
            #ifdef GEODE_IS_WINDOWS
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0x53ea48);
                instance->secondaryUrl = ZipUtils::base64URLDecode((char*)(geode::base::get() + 0x53ec80));
            #elif defined(GEODE_IS_ARM_MAC)
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0x7749fb);
                instance->secondaryUrl = ZipUtils::base64URLDecode((char*)(geode::base::get() + 0x774c73));
                // instance->secondaryUrl = (char*)(geode::base::get() + 0x488544);
            #elif defined(GEODE_IS_INTEL_MAC)
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0x8516bf);
                instance->secondaryUrl = ZipUtils::base64URLDecode((char*)(geode::base::get() + 0x851947));
                // instance->secondaryUrl = (char*)(geode::base::get() + 0x52D620);
            #elif defined(GEODE_IS_ANDROID64)
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0xEA2988);
                instance->secondaryUrl = ZipUtils::base64URLDecode((char *)(geode::base::get() + 0xEA1748));
            #elif defined(GEODE_IS_ANDROID32)
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0x952E9E);
                instance->secondaryUrl = ZipUtils::base64URLDecode((char *)(geode::base::get() + 0x951E11));
            #elif defined(GEODE_IS_IOS)
                static_assert(GEODE_COMP_GD_VERSION == 22074, "Unsupported GD version");
                instance->baseUrl = (char*)(geode::base::get() + 0x6AF51A);
                instance->secondaryUrl = ZipUtils::base64URLDecode((char*)(geode::base::get() + 0x6af7a0));
            #else
                static_assert(false, "Unsupported platform");
            #endif
            }
            if(instance->baseUrl.size() > 36) instance->baseUrl = instance->baseUrl.substr(0, 35);
            if(instance->secondaryUrl.size() > 35) instance->secondaryUrl = instance->secondaryUrl.substr(0, 34);
            return instance;
        };

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
