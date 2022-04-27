#pragma once
#include "AbstractObject.h"
#include "src/Helper.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class GameObject : public AbstractObject
{
    Q_OBJECT
    Q_PROPERTY(qint32 x MEMBER m_X)
    Q_PROPERTY(qint32 y MEMBER m_Y)

private:
    quint32 m_Hash;
    quint32 m_hp;
    qint32  m_X;
    qint32  m_Y;

public:
    explicit GameObject(quint32 hash, QDataStream *dataStream, QObject *parent = nullptr);
    virtual ~GameObject();

public slots:
    //解析数据段虚函数
    void readHead();
    void Positioned();
    void Stats();
    void Pathfinding(){};
    void WorldItem(){};
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
    void BaseEvents(){};

    //
    bool fs_componentPlayerUnknown(unsigned char *buffer, int len, unsigned __int64 a2);
    void fs_ActorA0(QJsonObject& json);
    void fs_ActiveSkills(QJsonObject& json);
    void fs_ActiveSkills1(quint8 size , QJsonObject& json);
    void fs_ActiveSkills3(QJsonObject& json);
    void fs_ActiveSkills_0(QJsonObject& json);
    void fs_GrantedEffectsPerLevel(QJsonObject& json);
    void fs_AlternateQualityTypes();
    void fs_Data_Mods();
};
