#include <Geode/modify/CCHttpClient.hpp>
#include "../ServerAPI.hpp"

using namespace geode::prelude;

class $modify(CCHttpClient) {
	void send(CCHttpRequest* req)
    {
		std::string url = req->getUrl();
		auto newUrl = ServerAPI::get()->getCurrentURL();
		if (ServerAPI::get()->getCurrentURL() != "NONE_REGISTERED") {
			if(url.starts_with("https://www.newgrounds.com/audio/download/") && !Mod::get()->getSettingValue<bool>("audio-fix")) {
				req->setUrl(url.replace(0, 41, fmt::format("{}/music/", newUrl)).c_str());
			}
			if (url.starts_with(ServerAPI::get()->getBaseUrl())) {
				req->setUrl(url.replace(0, ServerAPI::get()->getBaseUrl().length(), newUrl).c_str());
			} else if (url.starts_with(ServerAPI::get()->getSecondaryUrl())) {
				// robtop please
				// just replace this one with https
				req->setUrl(url.replace(0, 34, newUrl).c_str());
			}
		}
    CCHttpClient::send(req);
  }
};