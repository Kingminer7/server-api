#include <Geode/modify/CCHttpClient.hpp>
#include "../../include/ServerAPI.hpp"

using namespace geode::prelude;

class $modify(CCHttpClient) {
	void send(CCHttpRequest* req)
    {
		std::string url = req->getUrl();
		auto newUrl = ServerAPI::get()->getServerURL();
		if (url.starts_with("https://www.boomlings.com/database/")) {
			req->setUrl(url.replace(0, 35, newUrl).c_str());
		} else if (url.starts_with("http://www.boomlings.com/database/")) {
			req->setUrl(url.replace(0, 34, newUrl).c_str());
		}
        CCHttpClient::send(req);
    }
};