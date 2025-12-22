#include <Geode/modify/CCHttpClient.hpp>
#include "../ServerAPI.hpp"

using namespace geode::prelude;

class $modify(CCHttpClient) {
	void send(CCHttpRequest* req)
    {
        std::string url = req->getUrl();
        const auto& baseUrl = ServerAPI::get()->getBaseUrl();
        const auto& secondaryUrl = ServerAPI::get()->getSecondaryUrl();
        auto newUrl = ServerAPI::get()->getCurrentURL();

        if (newUrl == "NONE_REGISTERED") {
            return CCHttpClient::send(req);
        }
        if (url.starts_with("https://www.newgrounds.com/audio/download/")
            && !Mod::get()->getSettingValue<bool>("audio-fix")
        ) {
            req->setUrl(url.replace(0, 41, fmt::format("{}/music/", newUrl)).c_str());
            return CCHttpClient::send(req);
        }
        if (url.starts_with(baseUrl)) {
            req->setUrl(url.replace(0, baseUrl.length(), newUrl).c_str());
        } else if (url.starts_with(secondaryUrl)) {
            // robtop please
            // just replace this one with https
            req->setUrl(url.replace(0, 34, newUrl).c_str());
        }
        CCHttpClient::send(req);
    }
};