#include "ServerAPI.hpp"
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

class PopupSetting : public SettingV3 {
public:
  static Result<std::shared_ptr<PopupSetting>>
  parse(std::string const &key, std::string const &modID,
        matjson::Value const &json) {
    auto res = std::make_shared<PopupSetting>();
    auto root = checkJson(json, "PopupSetting");

    res->init(key, modID, root);
    res->parseNameAndDescription(root);
    res->parseEnableIf(root);

    root.checkUnknownKeys();
    return root.ok(res);
  }

  bool load(matjson::Value const &json) override { return true; }
  bool save(matjson::Value &json) const override { return true; }

  bool isDefaultValue() const override { return true; }
  void reset() override {}

  SettingNodeV3 *createNode(float width) override;
};

class PopupSettingNode : public SettingNodeV3 {
protected:
  ButtonSprite *m_buttonSprite;
  CCMenuItemSpriteExtra *m_button;

  bool init(std::shared_ptr<PopupSetting> setting, float width) {
    if (!SettingNodeV3::init(setting, width))
      return false;

    m_buttonSprite =
        ButtonSprite::create("Debug", "goldFont.fnt", "GJ_button_04.png", .8f);
    m_buttonSprite->setScale(.5f);
    m_button = CCMenuItemSpriteExtra::create(
        m_buttonSprite, this, menu_selector(PopupSettingNode::onButton));
    this->getButtonMenu()->addChildAtPosition(m_button, Anchor::Center);
    this->getButtonMenu()->setContentWidth(60);
    this->getButtonMenu()->updateLayout();

    this->updateState(nullptr);

    return true;
  }

  void updateState(CCNode *invoker) override {
    SettingNodeV3::updateState(invoker);

    auto shouldEnable = this->getSetting()->shouldEnable();
    m_button->setEnabled(shouldEnable);
    m_buttonSprite->setCascadeColorEnabled(true);
    m_buttonSprite->setCascadeOpacityEnabled(true);
    m_buttonSprite->setOpacity(shouldEnable ? 255 : 155);
    m_buttonSprite->setColor(shouldEnable ? ccWHITE : ccGRAY);
  }
  void onButton(CCObject *) {
    // DebugPopup::create()->show();
    std::string str = "";
    for (const auto &[uid, urlPrio] : ServerAPI::get()->getAllServers()) {
      str += fmt::format("{}: {}\n\n", urlPrio.first, urlPrio.second);
    }
    str += fmt::format("{}\n\nBase URL: {}\n\nSecondary URL: {}", ServerAPI::get()->isAmazon() ? "Amazon" : "", ServerAPI::get()->getBaseUrl(), ServerAPI::get()->getSecondaryUrl());
    geode::MDPopup::create("Server API Debug", str, "Close")->show();
  }

  void onCommit() override {}
  void onResetToDefault() override {}

public:
  static PopupSettingNode *create(std::shared_ptr<PopupSetting> setting,
                                  float width) {
    auto ret = new PopupSettingNode();
    if (ret && ret->init(setting, width)) {
      ret->autorelease();
      return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
  }

  bool hasUncommittedChanges() const override { return false; }
  bool hasNonDefaultValue() const override { return false; }

  std::shared_ptr<PopupSetting> getSetting() const {
    return std::static_pointer_cast<PopupSetting>(SettingNodeV3::getSetting());
  }
};

SettingNodeV3 *PopupSetting::createNode(float width) {
  return PopupSettingNode::create(
      std::static_pointer_cast<PopupSetting>(shared_from_this()), width);
}

auto changeToGenerator(auto &&function) {
  return [function](
             std::string const &key, std::string const &modID,
             matjson::Value const &json) -> Result<std::shared_ptr<SettingV3>> {
    return function(key, modID, json).map([](auto &&ptr) {
      return std::shared_ptr<SettingV3>(ptr);
    });
  };
}

$execute {
  (void)Mod::get()->registerCustomSettingType(
      "popup-setting", changeToGenerator(PopupSetting::parse));
}