#pragma once
#include "AbstractObject.h"
#include "ItemObject.h"
#include "src/Helper.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <cmath>

class GameObject : public AbstractObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 Id MEMBER m_Id)
    Q_PROPERTY(quint32 Hash MEMBER m_Hash)
    Q_PROPERTY(QPoint Pos MEMBER m_Pos)

public:
    quint32 m_Id;
    quint32 m_Hash;
    QPoint  m_Pos;
    QPoint  m_TargetPos;

    static QJsonObject GameComponents;

public:
    explicit GameObject(quint32 id, quint32 hash, QByteArray &data, QObject *parent = nullptr);
    virtual ~GameObject();
    bool Init();

    Q_INVOKABLE int size(QPoint target)
    {
        QPoint pos = m_Pos - target;
        return std::sqrt(QPoint::dotProduct(pos, pos));
    }

public slots:
    // 解析数据段虚函数
    void readHead();
    void Positioned();
    void Stats();
    void Pathfinding(){/*空*/};
    void WorldItem();
    void Buffs();
    void Life();
    void Animated();
    void Player();
    void AreaTransition();
    void PlayerClass(){/*空*/};
    void Inventories();
    void Actor();
    void LimitedLifespan(){/*空*/};
    void Render(){/*空*/};
    void ObjectMagicProperties();
    void BaseEvents(){/*空*/};
    void Targetable(){/*空*/};
    void Monster(){/*空*/};
    void Transitionable();
    void TriggerableBlockage();
    void Counter(){/*空*/};
    void Timer(){/*空*/};
    void ProximityTrigger(){/*空*/};
    void InteractionAction(){/*空*/};
    void NPC();
    void MinimapIcon();
    void Chest();

    //
    bool fs_componentPlayerUnknown(unsigned char *buffer, int len, unsigned __int64 a2);
    void fs_ActorA0(QJsonObject &json);
    void fs_ActorA0_0();
    void fs_ActorA0_1();
    void fs_ActiveSkills(QJsonObject &json);
    void fs_ActiveSkills1(quint8 size, QJsonArray &json);
    void fs_ActiveSkills3(QJsonArray &json);
    void fs_ActiveSkills_0(QJsonObject &json);
    //每个级别的授权效果
    QJsonObject fs_GrantedEffectsPerLevel();
    void        fs_AlternateQualityTypes();
    void        fs_Data_Mods();
};
