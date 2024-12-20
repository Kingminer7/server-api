#include <Geode/Geode.hpp>
#include "../include/ServerAPI.hpp"
#include "../include/ServerAPIEvents.hpp"

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
    std::pair<std::string, int> highestPrioUrl = std::make_pair("https://www.boomlings.com/database/", INT_MIN);
    
    for (const auto& [id, urlPrio] : overrides) {
        const auto& [url, prio] = urlPrio;
        if (prio > highestPrioUrl.second) {
            highestPrioUrl = urlPrio;
        }
    }
    
    return highestPrioUrl.first;
}

int ServerAPI::getCurrentPrio() {
    std::pair<std::string, int> highestPrioUrl = std::make_pair("https://www.boomlings.com/database/", INT_MIN);
    
    for (const auto& [id, urlPrio] : overrides) {
        const auto& [url, prio] = urlPrio;
        if (prio > highestPrioUrl.second) {
            highestPrioUrl = urlPrio;
        }
    }
    
    return highestPrioUrl.second;
}

int ServerAPI::getCurrentId() {
    std::pair<std::string, int> highestPrioUrl = std::make_pair("https://www.boomlings.com/database/", INT_MIN);
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

    new EventListener<EventFilter<GetRegisteredServersEvent>>(+[](GetRegisteredServersEvent* e) {
        e->m_servers = ServerAPI::get()->getAllServers();
        return ListenerResult::Stop;
    });
}