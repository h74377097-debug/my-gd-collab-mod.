/**
 * Geometry Dash Collaborative Editor Mod
 * Создано автоматически в Geode GD Collab Creator
 * Автор: Malarew
 */

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <queue>
#include <string>

// Пакетная структура для синхронизации сетевых действий
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

// Кастомный класс для рендеринга курсора на Cocos2d
class PlayerCursorNode : public CCNode {
public:
    std::string m_username;
    ccColor3B m_color;
    CCLabelBMFont* m_label;
    CCSprite* m_cursorSprite;

    static PlayerCursorNode* create(std::string name, ccColor3B color) {
        auto ret = new PlayerCursorNode();
        if (ret && ret->init(name, color)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(std::string name, ccColor3B color) {
        m_username = name;
        m_color = color;

        // Создаем спрайт курсора (стрелочка)
        m_cursorSprite = CCSprite::createWithSpriteFrameName("edit_select_001.png");
        m_cursorSprite->setColor(m_color);
        m_cursorSprite->setScale(0.8f);
        m_cursorSprite->setAnchorPoint({0.0f, 1.0f});
        this->addChild(m_cursorSprite);

        // Текст с никнеймом игрока
        m_label = CCLabelBMFont::create(m_username.c_str(), "chatFont.fnt");
        m_label->setScale(0.4f);
        m_label->setPosition({20.0f, 10.0f});
        m_label->setAnchorPoint({0.0f, 0.5f});
        this->addChild(m_label);

        return true;
    }

    void updatePosition(CCPoint point) {
        this->setPosition(point);
    }
};

// Хранилище курсоров и стейта сети
static std::map<std::string, PlayerCursorNode*> s_activeCursors;
static bool s_connected = false;

// Хук на LevelEditorLayer для отслеживания установки объектов
class $modify(MyLevelEditorLayer, LevelEditorLayer) {
    bool init(GJGameLevel* level, bool p1) {
        if (!LevelEditorLayer::init(level, p1)) return false;
        
        log::info("Подключение к лобби совместной работы на порту 8080...");
        
        // В реальном моде здесь запускается WebSocket-клиент во втором потоке
        s_connected = true;
        
        return true;
    }

    // Хукаем установку объекта
    void addObject(GameObject* obj) {
        LevelEditorLayer::addObject(obj);
        
        // Избегаем бесконечного цикла рекурсии при получении пакетов от сервера
        if (obj->m_fields->m_isNetworkObject) return;

        // Получаем метаданные объекта (ID, X, Y, поворот)
        int objectID = obj->m_objectID;
        CCPoint pos = obj->getPosition();
        float rot = obj->getRotation();

        log::info("Установлен объект: ID {}, X {}, Y {}", objectID, pos.x, pos.y);

        // Отправка пакета на WebSocket-сервер
        // Пример отправки JSON строки:
        // WebSocket::send("ADD:" + std::to_string(objectID) + ":" + std::to_string(pos.x) + ":" + std::to_string(pos.y));
    }

    // Хукаем удаление объекта
    void removeObject(GameObject* obj, bool p1) {
        LevelEditorLayer::removeObject(obj, p1);

        // Синхронизация удаления объекта по уникальной координате/индексу
        // WebSocket::send("REMOVE:" + std::to_string(obj->getPosition().x) + ":" + std::to_string(obj->getPosition().y));
    }
};

// Хук на EditorUI для отслеживания движения мыши и осей курсора
class $modify(MyEditorUI, EditorUI) {
    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        EditorUI::ccTouchMoved(touch, event);
        
        if (s_connected) {
            CCPoint pos = touch->getLocation();
            // Отправляем наши координаты курсора на сервер раз в 50мс
            // WebSocket::send("CURSOR:" + std::to_string(pos.x) + ":" + std::to_string(pos.y));
        }
    }

    void showCustomChatOverlay() {
        // Отрисовка кастомного чата внутри редактора Geometry Dash
        auto alert = FLAlertLayer::create("Чат лобби", "Добро пожаловать в совместное создание!", "ОК");
        alert->show();
    }
};
