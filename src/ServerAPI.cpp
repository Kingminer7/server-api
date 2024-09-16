
#include <Geode/Geode.hpp>
#include "../include/ServerAPI.hpp"

using namespace geode::prelude;

ServerAPI * ServerAPI::instance = nullptr;

int ServerAPI::setServerURL(std::string url, int priority) {
    if (!url.ends_with("/")) url = url + "/";
    // <id, <url, prio>>
    this->overrides[this->nextId] = std::pair(url, priority);
    this->nextId++;
    return this->nextId - 1;
}

std::string ServerAPI::getServerURL() {
    auto highestPrioUrl = std::make_pair("https://www.boomlings.com/database/", INT_MIN);
    
    for (const auto& [id, urlPrio] : overrides) {
        const auto& [url, prio] = urlPrio;
        if (prio > highestPrioUrl.second) {
            return url;
        }
    }
    
    return "https://www.boomlings.com/database/";
}