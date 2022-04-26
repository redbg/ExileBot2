#include "ItemObject.h"

#define HIBYTE(v) (v >> 8) & 0xff

ItemObject::ItemObject(QDataStream *dataStream, QObject *parent)
    : AbstractObject(dataStream, parent)
{
    qDebug() << "==================================================";

    int hash = readData<int>();
    readData<quint8>();

    m_BaseItemType = Helper::Data::GetBaseItemType(hash);

    QString InheritsFrom = m_BaseItemType.value("InheritsFrom").toString();
    qDebug() << m_BaseItemType.value("Name").toString() << InheritsFrom;

    this->ProcessDataStream(Helper::Data::GetItemComponentNames(InheritsFrom));
}

ItemObject::~ItemObject() {}

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

    MyMod.insert(mod.value("Id").toString(), stats);

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
    QJsonObject JsonObject = Helper::Data::GetArmourType(m_BaseItemType["_rid"].toInt());

    // 防具品质
    JsonObject.insert("Quality", readData<quint8>());

    m_Components.insert("Armour", JsonObject);
}

void ItemObject::Weapon()
{
    m_Components.insert("Weapon", Helper::Data::GetWeaponType(m_BaseItemType["_rid"].toInt()));
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

    QJsonObject Sockets;

    if ((v31 & 1) == 0 && (v31 & 2) == 0)
    {
        //  插槽数量
        quint8 socketSize     = (v31 >> 2) & 7;
        quint8 socketLinkSize = v31 >> 5;

        QJsonArray SocketsArray;
        QJsonArray socketsGroup;

        for (size_t i = 0; i < socketSize; i++)
        {
            quint8 socketInfo = readData<quint8>();
            quint8 isItem     = socketInfo & 1;  // 插槽里是否有宝石
            quint8 color      = socketInfo >> 1; // 1 = 红色, 2 = 绿色, 3 = 蓝色, 4 = 白色

            QJsonObject socket;
            socket.insert("color", color);

            if (isItem)
            {
                socket.insert("item", ItemObject(this->m_DataStream).toJsonObject());
            }

            SocketsArray.append(socket);
        }

        for (size_t i = 0; i < socketLinkSize; i++)
        {
            socketsGroup.append(readData<quint8>());
        }

        Sockets.insert("SocketsArray", SocketsArray);
        Sockets.insert("socketsGroup", socketsGroup);
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