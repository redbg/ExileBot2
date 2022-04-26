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
    quint8 level = readData<quint8>();
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
        quint8 identified = v8 & 0b1000;

        // 装备颜色
        quint8 frameType = v8 & 0b0111;

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
                fs_ItemTypeRegister_Mods();
            }
        }

        // enchantMods 附魔的词条
        if (v8 & 0x40)
        {
            quint16 size = readData<quint8>();

            for (int i = 0; i < size; i++)
            {
                fs_ItemTypeRegister_Mods();
            }
        }

        // craftedMods 手工附魔词缀(工艺台)
        if ((v8 & 0x400) != 0)
        {
            quint16 size = readData<quint8>();

            for (int i = 0; i < size; i++)
            {
                fs_ItemTypeRegister_Mods();
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
                    fs_ItemTypeRegister_Mods();
                }
            }
        }
    }
}

void ItemObject::fs_ItemTypeRegister_Mods()
{
    quint16     hash16 = readData<quint16>();
    QJsonObject mod    = Helper::Data::GetMods(hash16);

    if (!mod["StatsKey1"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey1").toInt());
        this->ReadVarint1();
    }
    if (!mod["StatsKey2"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey2").toInt());
        this->ReadVarint1();
    }
    if (!mod["StatsKey3"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey3").toInt());
        this->ReadVarint1();
    }
    if (!mod["StatsKey4"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey4").toInt());
        this->ReadVarint1();
    }
    if (!mod["StatsKey5"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey5").toInt());
        this->ReadVarint1();
    }
    if (!mod["StatsKey6"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("StatsKey6").toInt());
        this->ReadVarint1();
    }
    if (!mod["Heist_StatsKey0"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("Heist_StatsKey0").toInt());
        this->ReadVarint1();
    }
    if (!mod["Heist_StatsKey1"].isNull())
    {
        QJsonObject stat = Helper::Data::GetStats(mod.value("Heist_StatsKey1").toInt());
        this->ReadVarint1();
    }
}

void ItemObject::Stack()
{
    quint16 size = readData<quint16>();
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
    quint8 Quality = readData<quint8>();
}

void ItemObject::Armour()
{
    quint8 Quality = readData<quint8>();
}

void ItemObject::Weapon()
{
}

void ItemObject::AttributeRequirements()
{
}

void ItemObject::Charges()
{
    this->ReadVarint1();
}

void ItemObject::Sockets()
{
    quint8 v31 = readData<quint8>();

    if ((v31 & 1) == 0 && (v31 & 2) == 0)
    {
        //  插槽数量
        quint8 socketNumber = (v31 >> 2) & 7;
        quint8 v35          = v31 >> 5;

        QJsonArray sockets;
        QJsonArray socketItems;
        QJsonArray socketLinks;
        QJsonArray socketColors;

        for (size_t i = 0; i < socketNumber; i++)
        {
            quint8 socketInfo = readData<quint8>();
            quint8 isItem     = socketInfo & 1;
            quint8 color      = socketInfo >> 1; // 1 = 红色, 2 = 绿色, 3 = 蓝色, 4 = 白色
            socketColors.append(color);

            sockets.append(socketInfo);

            if (socketInfo & 1)
            {
                ItemObject(this->m_DataStream);
            }
        }

        for (size_t i = 0; i < v35; i++)
        {
            socketLinks.append(readData<quint8>());
        }
    }
}

void ItemObject::SkillGem()
{
    qint32 Experience = this->readData<qint32>();
}