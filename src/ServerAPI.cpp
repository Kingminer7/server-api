
#include <Geode/Geode.hpp>
#include "../include/ServerAPI.hpp"

using namespace geode::prelude;

ServerAPI * ServerAPI::instance = nullptr;

void ServerAPI::setServerURL(std::string url) {
    if (!url.ends_with("/")) url = url + "/";
    this->server = url;
    Mod::get()->getSavedValue<std::string>("server", url);
}

std::string ServerAPI::getServerURL() {
    return this->server;
}

$on_mod(Loaded) {
    std::string url = Mod::get()->getSavedValue<std::string>("server");
    if (url == "") {
        url = "https://www.boomlings.com/database/";
    }
    ServerAPI::get()->setServerURL(url);
}