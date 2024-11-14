#include "../../include/ServerAPI.hpp"
#include "Geode/binding/FLAlertLayer.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
class $modify(GDPSSwitchMenuLayer, MenuLayer){

    bool init(){
        if (!MenuLayer::init()){
            return false;
        }

        if (ServerAPI::get()->firstML == true) {
        ServerAPI::get()->firstML = false;
        log::info("{}", ServerAPI::get()->getBaseUrl());
        if (ServerAPI::get()->getBaseUrl() != "https://www.boomlings.com/database/") {
        Loader::get()->queueInMainThread([this] {
            FLAlertLayer::create(
                "Modified URL Warning",
                "You seem to have a modified Geometry Dash app, such as an app with GDPS URLs pre-set. Some issues may occur.",
                "OK")->show();
            });
        }
        }

        return true;
    }
}
;