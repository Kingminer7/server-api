#include <Geode/modify/CCHttpClient.hpp>
#include "../../include/ServerAPI.hpp"

using namespace geode::prelude;

class $modify(CCHttpClient) {
	void send(CCHttpRequest* req)
    {
	    log::info("Http Request");
		std::string url = req->getUrl();
		log::info("{}", ServerAPI::get()->getBaseUrl());
		auto newUrl = ServerAPI::get()->getCurrentURL();
		if(url.starts_with("https://www.newgrounds.com/audio/download/") && !Mod::get()->getSettingValue<bool>("audio-fix")) {
			if (ServerAPI::get()->getCurrentURL() != "https://www.boomlings.com/database/") {
				req->setUrl(url.replace(0, 41, fmt::format("{}/music/", newUrl)).c_str());
			}
		}
		if (url.starts_with(ServerAPI::get()->getBaseUrl())) {
			req->setUrl(url.replace(0, ServerAPI::get()->getBaseUrl().length() - 1, newUrl).c_str());
		} else if (url.starts_with("http://www.boomlings.com/database/")) {
			// robtop please
			// just replace this one with https
			req->setUrl(url.replace(0, 34, newUrl).c_str());
		}
        CCHttpClient::send(req);
    }
};
