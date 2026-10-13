#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

static bool noclipOn() {
    return Mod::get()->getSettingValue<bool>("noclip");
}

// ===== NoClip: evita que el jugador muera =====
class $modify(NCPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        // El anticheat spike se deja pasar para no romper el juego
        if (noclipOn() && obj != m_anticheatSpike) return;
        PlayLayer::destroyPlayer(player, obj);
    }
};

// ===== Botón ON/OFF en el menú de pausa =====
class $modify(NCPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto menu = this->getChildByID("right-button-menu");
        if (!menu) return;

        bool on = noclipOn();
        auto spr = ButtonSprite::create(
            on ? "NoClip: ON" : "NoClip: OFF",
            "goldFont.fnt",
            on ? "GJ_button_01.png" : "GJ_button_06.png",
            0.8f
        );
        spr->setScale(0.6f);

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(NCPauseLayer::onToggleNoclip)
        );
        btn->setID("noclip-toggle"_spr);

        menu->addChild(btn);
        menu->updateLayout();
    }

    void onToggleNoclip(CCObject* sender) {
        bool on = !noclipOn();
        Mod::get()->setSettingValue<bool>("noclip", on);

        auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        auto spr = static_cast<ButtonSprite*>(btn->getNormalImage());
        spr->setString(on ? "NoClip: ON" : "NoClip: OFF");
        spr->updateBGImage(on ? "GJ_button_01.png" : "GJ_button_06.png");

        Notification::create(
            on ? "NoClip activado" : "NoClip desactivado",
            on ? NotificationIcon::Success : NotificationIcon::Info
        )->show();
    }
};
