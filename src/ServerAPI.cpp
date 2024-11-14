
#include <Geode/Geode.hpp>
#include "../include/ServerAPI.hpp"

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

void ServerAPI::removeURL(int id) {
    this->overrides.erase(id);
}

std::map<int, std::pair<std::string, int>> ServerAPI::getAllServers() {
    return overrides;
}

std::string ServerAPI::getBaseUrl() {
    return baseUrl;
}