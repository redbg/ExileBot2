#include "ItemObject.h"

#define HIBYTE(v) (v >> 8) & 0xff

ItemObject::ItemObject(quint32 inventoryId, quint32 index, QPoint pos, QByteArray &data, QObject *parent)
    : m_InventoryId(inventoryId)
    , m_Id(index)
    , m_Pos(pos)
    , AbstractObject(data, parent)
{
    m_InventoryName = Helper::Data::GetInventories(inventoryId - 1).value("Id").toString();
    Init();
}

ItemObject::ItemObject(QDataStream *dataStream, QObject *parent)
    : AbstractObject(dataStream, parent)
{
    Init();
}

ItemObject::~ItemObject() {}

void ItemObject::Init()
{
    qDebug() << "==================================================";

    int hash = readData<int>();
    readData<quint8>();

    m_BaseItemType = Helper::Data::GetBaseItemTypes(hash);

    QString InheritsFrom = m_BaseItemType.value("InheritsFrom").toString();

    this->setObjectName(m_BaseItemType.value("Id").toString());

    qDebug() << m_BaseItemType.value("Name").toString() << "<-" << this->objectName() << "<-" << InheritsFrom;

    this->ProcessDataStream(Helper::Data::GetItemComponents(InheritsFrom));
}

QJsonObject ItemObject::toJsonObject()
{
    QJsonObject JsonObject;
    JsonObject.insert("BaseItemType", m_BaseItemType);
    JsonObject.insert("Components", m_Components);
    return JsonObject;
}

void ItemObject::Base()
{
    readData<quint16>();
    readData<quint8>();
    readData<quint8>();
    quint16 v22 = readData<quint16>();
    readData<quint8>();
    if ((v22 & 1) != 0)
    {
        readData<quint8>();
    }
    quint16 v23 = HIBYTE(v22);
    if ((v23 & 0x10) != 0)
    {
        readData<quint32>();
        readData<quint32>();
    }

    if ((v22 & 4) != 0)
    {
        quint32 size = readData<quint32>();
        if (size > 0)
        {
            readData(2 * size);
        }
    }
    else if ((v22 & 2) != 0)
    {
        readData<quint16>();
    }

    if ((v22 & 8) != 0)
    {
        readData<quint32>();
    }

    if ((v22 & 0x10) != 0)
    {
        readData(readData<quint32>() * 2);
    }
    if ((v22 & 0x20) != 0)
    {
        quint8 v57 = readData<quint8>();

        for (int i = 0; i < v57; i++)
        {
            readData<quint32>();
            readData<quint32>();
        }
    }

    if ((v22 & 0x40) != 0)
    {
        ItemObject(this->m_DataStream);
    }
    if ((v22 & 0x80) != 0)
    {
        quint8 size = readData<quint8>();
        for (int i = 0; i < size; i++)
        {
            readData<quint32>();
            ReadVarint();
        }
    }
}

void ItemObject::Mods()
{
    QJsonObject JsonObject;

    QJsonArray implicitMods;
    QJsonArray explicitMods;
    QJsonArray enchantMods;
    QJsonArray craftedMods;

    JsonObject.insert("level", readData<quint8>());

    readData<quint8>();
    quint8 v15 = readData<quint8>();
    if ((v15 & 0x20) != 0)
    {
        readData<quint16>();
        readData<quint16>();
        readData<quint8>();
    }

    // AlternateQualityTypes
    {
        quint16 v8 = readData<quint16>(); // 是否鉴定,颜色
        readData<quint8>();

        // 是否已鉴定
        bool identified = v8 & 0b1000;
        JsonObject.insert("identified", identified);

        // 装备颜色
        quint8 frameType = v8 & 0b0111;
        JsonObject.insert("frameType", frameType);

        if (v8 & 0x200)
        {
            readData<quint8>();
            readData<quint8>();
        }

        // implicitMods 固定的词条,未鉴定的也会有
        if (v8 & 0x10)
        {
            quint16 size = readData<quint8>();

            for (int i = 0; i < size; i++)
            {
                implicitMods.append(fs_ItemTypeRegister_Mods());
            }
        }

        // enchantMods 附魔的词条
        if (v8 & 0x40)
        {
            quint16 size = readData<quint8>();

            for (int i = 0; i < size; i++)
            {
                enchantMods.append(fs_ItemTypeRegister_Mods());
            }
        }

        // craftedMods 手工附魔词缀(工艺台)
        if ((v8 & 0x400) != 0)
        {
            quint16 size = readData<quint8>();

            for (int i = 0; i < size; i++)
            {
                craftedMods.append(fs_ItemTypeRegister_Mods());
            }
        }

        // 判断是否已鉴定
        if (identified)
        {
            if ((v8 & 0x80) == 0)
            {
                if ((v8 & 0x100) != 0)
                {
                    quint16 size = readData<quint8>();

                    for (int i = 0; i < size; i++)
                    {
                        readData<quint32>();
                    }
                }
            }
            else
            {
                readData(readData<quint32>() * 2);
            }

            // explicitMods 鉴定后会出现的词条
            if (v8 & 0x20)
            {
                quint16 size = readData<quint8>();

                for (int i = 0; i < size; i++)
                {
                    explicitMods.append(fs_ItemTypeRegister_Mods());
                }
            }
        }
    }

    JsonObject.insert("implicitMods", implicitMods);
    JsonObject.insert("explicitMods", explicitMods);
    JsonObject.insert("enchantMods", enchantMods);
    JsonObject.insert("craftedMods", craftedMods);
    m_Components.insert("Mods", JsonObject);
}

QJsonObject ItemObject::fs_ItemTypeRegister_Mods()
{
    quint16     hash16 = readData<quint16>();
    QJsonObject mod    = Helper::Data::GetMods(hash16);

    QJsonObject MyMod;
    QJsonObject stats;

    if (!mod["StatsKey1"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey1").toInt());
        stats.insert(stat.value("Text").toString(), this->ReadVarint1());
    }
    if (!mod["StatsKey2"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey2").toInt());
        stats.insert(stat.value("Text").toString(), this->ReadVarint1());
    }
    if (!mod["StatsKey3"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey3").toInt());
        stats.insert(stat.value("Text").toString(), this->ReadVarint1());
    }
    if (!mod["StatsKey4"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey4").toInt());
        stats.insert(stat.value("Text").toString(), this->ReadVarint1());
    }
    if (!mod["StatsKey5"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey5").toInt());
        stats.insert(stat.value("Text").toString(), this->ReadVarint1());
    }
    if (!mod["StatsKey6"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey6").toInt());
        stats.insert(stat.value("Text").toString(), this->ReadVarint1());
    }
    if (!mod["Heist_StatsKey0"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("Heist_StatsKey0").toInt());
        stats.insert(stat.value("Text").toString(), this->ReadVarint1());
    }
    if (!mod["Heist_StatsKey1"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("Heist_StatsKey1").toInt());
        stats.insert(stat.value("Text").toString(), this->ReadVarint1());
    }

    MyMod.insert(mod.value("CorrectGroup").toString(), stats);

    return MyMod;
}

void ItemObject::Stack()
{
    // 物品数量
    m_Components.insert("Stack", readData<quint16>());
}

void ItemObject::HeistContract()
{
    readData<quint32>();
    readData<quint8>();
    readData<quint8>();
    quint8 v14 = readData<quint8>();
    quint8 v68 = readData<quint8>();

    for (size_t i = 0; i < v14; i++)
    {
        readData<quint8>();
        readData<quint8>();
    }

    for (size_t i = 0; i < v68; i++)
    {
        readData<quint8>();
    }
}

void ItemObject::Map()
{
    readData<quint8>();
    readData<quint8>();
}

void ItemObject::HeistBlueprint()
{
    readData<quint32>();
    readData<quint8>();
    readData<quint8>();

    quint8 size = readData<quint8>();

    for (size_t i = 0; i < size; i++)
    {
        quint8 size = readData<quint8>();
        for (size_t i = 0; i < size >> 1; i++)
        {
            readData<quint8>();
            readData<quint8>();
        }

        size = readData<quint8>();
        for (size_t i = 0; i < size; i++)
        {
            readData<quint8>();
        }

        size = readData<quint8>();
        for (size_t i = 0; i < size; i++)
        {
            readData<quint8>();
        }
    }
}

void ItemObject::Quality()
{
    m_Components.insert("Quality", readData<quint8>());
}

void ItemObject::Armour()
{
    QJsonObject JsonObject = Helper::Data::GetArmourTypes(m_BaseItemType["_rid"].toInt());

    // 防具品质
    JsonObject.insert("Quality", readData<quint8>());

    m_Components.insert("Armour", JsonObject);
}

void ItemObject::Weapon()
{
    m_Components.insert("Weapon", Helper::Data::GetWeaponTypes(m_BaseItemType["_rid"].toInt()));
}

void ItemObject::AttributeRequirements()
{
    m_Components.insert("AttributeRequirements", Helper::Data::GetComponentAttributeRequirements(m_BaseItemType["Id"].toString()));
}

void ItemObject::Charges()
{
    this->ReadVarint1();
}

void ItemObject::Sockets()
{
    quint8 v31 = readData<quint8>();

    QJsonArray Sockets;

    if ((v31 & 1) == 0 && (v31 & 2) == 0)
    {
        QJsonObject socket;

        //  插槽数量
        quint8 socketSize     = (v31 >> 2) & 7;
        quint8 socketLinkSize = v31 >> 5;

        for (size_t i = 0; i < socketSize; i++)
        {
            quint8 socketInfo = readData<quint8>();
            quint8 color      = socketInfo >> 1; // 1 = 红色, 2 = 绿色, 3 = 蓝色, 4 = 白色
            bool   isItem     = socketInfo & 1;  // 插槽里是否有宝石

            socket.insert("color", color);
            socket.insert("isItem", isItem);

            if (isItem)
            {
                ItemObject item = ItemObject(this->m_DataStream);
                socket.insert("item", item.toJsonObject());
                Index += item.Index; // 调试信息,不要动
            }

            Sockets.append(socket);
        }

        for (size_t i = 0; i < socketLinkSize; i++)
        {
            quint8 group = readData<quint8>();
        }

        m_Components.insert("Sockets", Sockets);
    }
}

void ItemObject::SkillGem()
{
    QJsonObject SkillGem;
    SkillGem.insert("Experience", this->readData<qint32>());
    SkillGem.insert("u1", this->readData<qint32>());
    SkillGem.insert("u2", this->readData<quint8>());

    m_Components.insert("SkillGem", SkillGem);
}