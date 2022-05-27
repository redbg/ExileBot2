#pragma once
#include "AbstractObject.h"
#include "src/Helper.h"

class ItemObject : public AbstractObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 InventoryId MEMBER m_InventoryId)
    Q_PROPERTY(QString InventoryName MEMBER m_InventoryName)
    Q_PROPERTY(quint32 Id MEMBER m_Id)
    Q_PROPERTY(QPoint Pos MEMBER m_Pos)
    Q_PROPERTY(QJsonObject BaseItemType MEMBER m_BaseItemType)

public:
    quint32     m_InventoryId;
    QString     m_InventoryName;
    quint32     m_Id;
    QPoint      m_Pos;
    QJsonObject m_BaseItemType;

public:
    explicit ItemObject(quint32 inventoryId, quint32 id, QPoint pos, QByteArray &data, QObject *parent = nullptr);
    explicit ItemObject(QDataStream *dataStream, QObject *parent = nullptr);
    virtual ~ItemObject();

    void Init();

    QJsonObject toJsonObject();

public slots:
    void Base();
    void Mods();
    void HeistEquipment(){};
    void Usable(){};
    void Flask(){};
    void Stack();
    void HeistContract();
    void Map();
    void HeistBlueprint();
    void Quality();
    void LocalStats(){};
    void Shield(){};
    void Armour();
    void Weapon();
    void AttributeRequirements();
    void Charges();
    void Sockets();
    void SkillGem();
    void Quest() {}

public:
    QJsonObject fs_ItemTypeRegister_Mods();
};
