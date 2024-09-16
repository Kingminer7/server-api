
#include <Geode/Geode.hpp>
#include "../include/ServerAPI.hpp"

using namespace geode::prelude;

ServerAPI * ServerAPI::instance = nullptr;

void ServerAPI::setServerURL(std::string url) {
    if (!url.ends_with("/")) url = url + "/";
    this->server = url;
}

std::string ServerAPI::getServerURL() {
    return this->server;
}