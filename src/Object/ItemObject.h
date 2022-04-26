#pragma once
#include "AbstractObject.h"
#include "src/Helper.h"

class ItemObject : public AbstractObject
{
    Q_OBJECT

private:
    QJsonObject m_BaseItemType;

public:
    explicit ItemObject(QDataStream *dataStream, QObject *parent = nullptr);
    virtual ~ItemObject();

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
    void fs_ItemTypeRegister_Mods();
};
