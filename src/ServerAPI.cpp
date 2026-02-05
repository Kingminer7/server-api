#include <Geode/Geode.hpp>
#include "ServerAPI.hpp"
#include "../include/ServerAPIEvents.hpp"
#include "Geode/cocos/support/zip_support/ZipUtils.h"
#include "PlatformOffsets.hpp"
#include "VoidCast.hpp"

constexpr int REQUIRED_GD_VERSION = 22081;

const std::unordered_map<std::string, ServerAPITrust::TrustLevel> ServerAPI::m_trustedModsLUT = {
    {"lblazen.gdps_hub", ServerAPITrust::TrustLevel::HighlyTrusted},
    {"km7dev.gdps-switcher", ServerAPITrust::TrustLevel::HighlyTrusted} // Putting my own mod here because I have an ego and fuck you.
};

using namespace ServerAPIEvents;
using namespace geode::prelude;

ServerAPI* ServerAPI::instance = nullptr;

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

const std::unordered_map<std::string, ServerAPITrust::TrustLevel>& ServerAPI::getTrustedMods() {
    return m_trustedModsLUT;
}

void ServerAPI::removeURL(int id) {
    this->m_overrides.erase(id);
    if (m_overrides.empty()) {
        m_cache = ServerCache::none();
    }
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
    static_assert(IS_SUPPORTED_PLATFORM, "Unsupported platform");

    this->m_baseUrl = (char*)(geode::base::get() + (m_amazon ? BASE_URL_OFFSET_AMAZON : BASE_URL_OFFSET));
    this->m_secondaryUrl = ZipUtils::base64URLDecode((char *)(geode::base::get() + (m_amazon ? SECONDARY_URL_OFFSET_AMAZON : SECONDARY_URL_OFFSET)));
    m_cache = ServerCache::none();

    // Trims endpoint off of URL.
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

ServerAPI* ServerAPI::get() {
    if (!instance) {
        instance = new ServerAPI();
        instance->init();
    }
    return instance;
};

template <typename CFunc, typename... TArgs>
requires std::invocable<CFunc, ServerAPI*, TArgs...> // Must be method of ServerAPI
decltype(auto) ServerAPI::doAndNotifyIfServerUpdate(CFunc&& func, Mod* updater, TArgs&&... args) {
    int oldID = m_cache.ID;

    // Void is an incomplete type, so we have to hack our way around it
    using TRet = VoidCastT<std::invoke_result_t<CFunc, ServerAPI*, TArgs...>>;
    using TCastBack = std::invoke_result_t<CFunc, ServerAPI*, TArgs...>;
    TRet ret = safeInvoke(
        std::forward<CFunc>(func),
        this,
        std::forward<TArgs>(args)...
    );

    // Server has updated
    if (oldID != m_cache.ID) {
        ServerUpdatingEventData event(updater, m_cache.URL);
        ServerUpdatingEvent().send(&event);
    }

    // Cast back to void if original result type is void
    return static_cast<TCastBack>(ret);
}

bool ServerAPI::isAmazon() {
    return m_amazon;
}

$on_mod(Loaded) {
    GetCurrentServerEvent().listen([](GetCurrentServerEventData* e) {
        e->m_url = ServerAPI::get()->getCurrentURL();
        e->m_prio = ServerAPI::get()->getCurrentPrio();
        e->m_id = ServerAPI::get()->getCurrentId();
        return ListenerResult::Stop;
    }).leak();

    GetServerByIdEvent().listen([](GetServerByIdEventData* e) {
        e->m_url = ServerAPI::get()->getURLById(e->m_id);
        e->m_prio = ServerAPI::get()->getPrioById(e->m_id);
        return ListenerResult::Stop;
    }).leak();

    RegisterServerEvent().listen([](RegisterServerEventData* e) {
        //e->m_id = ServerAPI::get()->registerURL(e->m_url, e->m_priority);
        e->m_id = ServerAPI::get()->doAndNotifyIfServerUpdate(&ServerAPI::registerURL, e->sender, e->m_url, e->m_priority);
        return ListenerResult::Stop;
    }).leak();

    UpdateServerEvent().listen([](UpdateServerEventData* e) {
        if (e->m_url != "") {
            if (e->m_priority != 0) {
                //ServerAPI::get()->updateURLAndPrio(e->m_id, e->m_url, e->m_priority);
                ServerAPI::get()->doAndNotifyIfServerUpdate(&ServerAPI::updateURLAndPrio, e->sender, e->m_id, e->m_url, e->m_priority);
            } else {
                //ServerAPI::get()->updateURL(e->m_id, e->m_url);
                ServerAPI::get()->doAndNotifyIfServerUpdate(&ServerAPI::updateURL, e->sender, e->m_id, e->m_url);
            }
        } else {
            //ServerAPI::get()->updatePrio(e->m_id, e->m_priority);
            ServerAPI::get()->doAndNotifyIfServerUpdate(&ServerAPI::updatePrio, e->sender, e->m_id, e->m_priority);
        }
        return ListenerResult::Stop;
    }).leak();

    RemoveServerEvent().listen([](RemoveServerEventData* e) {
        //ServerAPI::get()->removeURL(e->m_id);
        ServerAPI::get()->doAndNotifyIfServerUpdate(&ServerAPI::removeURL, e->sender, e->m_id);
        return ListenerResult::Stop;
    }).leak();

    GetBaseUrlEvent().listen([](GetBaseUrlEventData* e) {
        e->m_url = ServerAPI::get()->getBaseUrl();
        return ListenerResult::Stop;
    }).leak();

    GetSecondaryUrlEvent().listen([](GetSecondaryUrlEventData* e) {
        e->m_url = ServerAPI::get()->getSecondaryUrl();
        return ListenerResult::Stop;
    }).leak();

    GetRegisteredServersEvent().listen([](GetRegisteredServersEventData* e) {
        e->m_servers = ServerAPI::get()->getAllServers();
        return ListenerResult::Stop;
    }).leak();

    GetTrustedModsEvent().listen([](GetTrustedModsEventData* e) {
        e->trustedMods = ServerAPI::get()->getTrustedMods();
        return ListenerResult::Stop;
    }).leak();
}
