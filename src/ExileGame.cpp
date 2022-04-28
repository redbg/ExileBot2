#include "ExileGame.h"

ExileGame::ExileGame(ExileClient *client)
    : m_ExileClient(client)
    , m_SendSkillCount(0)
{
    connect(this, &ExileSocket::connected, this, &ExileGame::on_game_connected);
    connect(this, &ExileSocket::disconnected, this, &ExileGame::on_game_disconnected);
    connect(this, &ExileSocket::errorOccurred, this, &ExileGame::on_game_errorOccurred);
    connect(this, &ExileSocket::readyRead, this, &ExileGame::on_game_readyRead);
}

ExileGame::~ExileGame()
{
}

QImage ExileGame::Render()
{
    QImage   image(m_TerrainWidth, m_TerrainHeight, QImage::Format_RGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿

    // Render Terrain
    for (quint32 y = 0; y < m_TerrainHeight; y++)
    {
        for (quint32 x = 0; x < m_TerrainWidth; x++)
        {
            image.setPixelColor(x, y, m_TerrainData.at((y * m_TerrainWidth) + x) == '1' ? Qt::white : Qt::black);
        }
    }

    // Render Entity
    painter.setPen(Qt::black);
    for (int i = 0; i < m_EntityList.size(); i++)
    {
        auto obj = m_EntityList.at(i);

        if (obj->m_Id == m_PlayerId)
        {
            // 设置玩家颜色
            painter.setBrush(Qt::green);
        }
        else
        {
            painter.setBrush(Qt::red);
        }

        painter.drawEllipse(obj->m_Pos.x(), obj->m_Pos.y(), 10, 10);
    }

    // Render RadarInfo
    painter.setPen(Qt::green);
    painter.setBrush(Qt::green);
    for (auto i = m_RadarInfo.begin(); i != m_RadarInfo.end(); i++)
    {
        QJsonObject posObj = m_RadarInfo.begin().value().toObject();
        QPoint      pos(posObj.value("x").toInt(), posObj.value("y").toInt());

        painter.drawText(pos.x(), pos.y(), i.key());

        painter.drawEllipse(pos.x(), pos.y(), 10, 10);
    }

    // Render Path
    for (int i = 0; i < m_Path.size(); i++)
    {
        image.setPixelColor(m_Path[i], Qt::green);
    }

    return image;
}

void ExileGame::connectToHost(quint32 Address, quint16 Port, quint32 Ticket, quint32 WorldAreaHASH16, quint32 WorldInstance, QByteArray Key)
{
    m_Ticket          = Ticket;
    m_WorldAreaHASH16 = WorldAreaHASH16;
    m_WorldInstance   = WorldInstance;
    this->SetKeyWithIV(Key);

    ExileSocket::connectToHost(QHostAddress(Address), Port);

    qDebug() << QString("connectToHost(%1, %2, %3, %4, %5)")
                    .arg(QHostAddress(Address).toString())
                    .arg(Port)
                    .arg(Ticket)
                    .arg(WorldAreaHASH16)
                    .arg(WorldInstance);
}

void ExileGame::on_game_connected()
{
    qDebug() << "on_game_connected";
    SendTicket();
}

void ExileGame::on_game_disconnected()
{
    qDebug() << "on_game_disconnected";
}

void ExileGame::on_game_errorOccurred(QAbstractSocket::SocketError socketError)
{
    qWarning() << socketError << this->errorString();
}

void ExileGame::on_game_readyRead()
{
    while (this->size())
    {
        quint16 PacketId = this->readId();

        // qDebug() << QString("PacketId:[0x%1]").arg(QString::number(PacketId, 16));

        switch (PacketId)
        {
        case 0x1:
        {
            readString();
            break;
        }
        case 0x3:
        {
            quint16 size = read<quint16>();

            for (quint16 i = 0; i < size; i++)
            {
                read<quint32>();
                read<quint32>();
                readString();
                read<quint16>();

                qint8  ret1 = read<qint8>();
                quint8 ret2 = read<quint8>();

                read<quint16>();
                read<quint8>();
                read<quint8>();

                read<quint8>();

                if (ret1 < 0)
                {
                    read<quint64>();
                }
                if ((ret1 & 0x40) != 0)
                {
                    read<quint8>();
                }
                if ((ret2 & 1) != 0)
                {
                    read<quint32>();
                }
            }

            readString();
            read<quint16>();
            read<quint8>();
            break;
        }
        case 0x5:
        {
            // 开启加密
            this->EnableCrypto();
            break;
        }
        case 0xa:
        {
            // 聊天信息
            this->RecvChat();
            break;
        }
        case 0xb:
        {
            this->read<quint16>();
            this->read<quint16>();
            this->read<quint16>();
            this->read<quint32>();
            {
                quint8 size1 = this->read<quint8>();
                quint8 size2 = this->read<quint8>();

                for (size_t i = 0; i < size1; i++)
                {
                    this->read<quint8>();
                    this->read<quint32>();
                }

                for (size_t i = 0; i < size2; i++)
                {
                    this->readString();
                }
            }
            break;
        }
        case 0xf:
        {
            this->read<quint16>();
            this->read<quint32>();
            break;
        }
        case 0x10:
        {
            // 地图信息
            this->RecvInitWorld();
            break;
        }
        case 0x13:
        {
            quint16 size = this->read<quint16>();
            this->read<quint16>();
            this->read(size * 2);
            this->read(size);
            break;
        }
        case 0x14:
        {
            quint16 size = this->read<quint16>();

            for (quint16 i = 0; i < size; i++)
            {
                this->read<quint32>();
                this->read<quint32>();
                this->readString();
                this->read<quint16>();

                quint8 v5_1 = this->read<quint8>();
                quint8 v5_2 = this->read<quint8>();

                this->read<quint16>();
                this->read<quint8>();
                this->read<quint8>();
                this->read<quint8>();

                if ((v5_1 & 0x80) != 0)
                {
                    this->read<quint64>();
                }

                if ((v5_1 & 0x40) != 0)
                {
                    this->read<quint8>();
                }

                if ((v5_2 & 1) != 0)
                {
                    this->read<quint32>();
                }
            }

            this->readString();
            this->read<quint16>();
            this->read<quint8>();
            break;
        }
        case 0x15:
        {
            this->RecvPlayerId();
            break;
        }
        case 0x19:
        {
            // 收到错误信息
            this->RecvBackendError();
            break;
        }
        case 0x1a:
        {
            read<quint32>(); // Ticket1
            read<quint32>();
            read<quint16>(); // WorldAreaId
            read<quint32>(); // Ticket2
            read<quint8>();

            quint8 size = read<quint8>(); // AddressCount

            for (quint8 i = 0; i < size; i++)
            {
                read<quint16>();
                read<quint16>(); // Port
                read<quint32>(); // Address
                read(0x14);
            }

            read(0x40);
            break;
        }
        case 0x24:
        {
            this->read<quint16>();
            this->read<quint16>();
            this->read<quint16>();

            this->read<quint8>();
            this->read<quint8>();

            if ((this->read<quint8>() & 2) == 0)
            {
                this->read(this->read<quint16>() * 2);
                this->read(this->read<quint16>() * 2);
                this->read(this->read<quint16>() * 2);
                this->read(this->read<quint16>() * 2);
            }

            break;
        }
        case 0x2b:
        {
            this->read<quint16>();
            this->read<quint16>();

            if ((this->read<quint8>() & 2) == 0)
            {
                quint16 size = this->read<quint16>();
                this->read(size * 2);
            }
            break;
        }
        case 0x39:
        {
            this->read<quint32>();
            this->read(0x13);
            break;
        }
        case 0x3e:
        {
            this->read(0x1a);
            this->read<quint16>();
            this->read<quint16>();
            break;
        }
        case 0xa2:
        {
            read<quint8>();
            break;
        }
        case 0xa3:
        {
            quint8 size = read<quint8>();
            for (quint8 i = 0; i < size; i++)
            {
                read<quint16>();
                read<quint8>();
                read<quint32>();
            }
            break;
        }

        case 0xcb:
        {
            quint16 size = read<quint16>();
            for (quint16 i = 0; i < size; i++)
            {
                readString();
                readString();
                readString();
                readString();

                read<quint32>();
                read<quint16>();
                read<quint16>();
                read<quint8>();

                read<quint64>();
                read<quint64>();
                read<quint64>();
                read<quint16>();
                read<quint16>();
            }
            break;
        }
        case 0x6e:
        {
            this->readString();
            this->readString();

            quint8 v4 = this->read<quint8>();

            if (v4 != 0)
            {
                this->readString();

                this->read<quint8>();
                this->read<quint8>();
                this->read<quint16>();
                this->read<quint16>();
                this->read<quint16>();
                this->read<quint16>();
                this->read<quint8>();

                this->readString();

                this->read<quint8>();
                this->read<quint16>();
                this->read<quint8>();
                this->read<quint8>();
            }
            else
            {
                this->read<quint64>();
            }

            break;
        }
        case 0x84:
        {
            this->read<quint8>();
            quint32 inventoryId = this->read<quint32>();

            {
                this->read<quint32>();
                quint32 size = this->read<quint32>();

                for (quint32 i = 0; i < size; i++)
                {
                    this->read<quint32>();
                }

                quint32 itemSize = this->read<quint32>();

                for (quint32 i = 0; i < itemSize; i++)
                {
                    this->read<quint32>();
                    this->read<quint8>(); // x
                    this->read<quint8>(); // y

                    // item info
                    this->read(this->read<quint16>());
                }

                quint8 v35 = this->read<quint8>();
                if (v35 != 0)
                {
                    this->read<quint32>();
                    this->read<quint32>();
                    this->read<quint32>();
                    this->read<quint32>();
                }
            }
            break;
        }
        case 0x85:
        {
            // 库存信息
            this->RecvInventory();
            break;
        }
        case 0xd5:
        {
            read<quint8>();
            read(read<quint16>());
            break;
        }
        case 0xd7:
        {
            quint8 size = read<quint8>();
            for (quint8 i = 0; i < size; i++)
            {
                this->read<quint32>();
                this->read<quint32>();
                this->read<quint32>();
            }

            break;
        }
        case 0xd9:
        {
            read<quint16>();
            this->readString();
            read<quint8>();
            read<quint8>();
            read<quint8>();
            read<quint64>();
            break;
        }
        case 0xde:
        {
            // DeleteObject
            read<quint32>();
            read<quint32>();
            read<quint32>();
            break;
        }
        case 0x8e:
        {
            this->read<quint8>();
            this->readString();
            this->readString();
            break;
        }
        case 0xe2:
        {
            this->read<quint16>();
            this->read<quint8>();
            this->read<quint8>();
            break;
        }
        case 0x143:
        {
            this->read(0xe);
            this->read(0xe);
            this->read(0xe);

            this->read<quint8>();
            this->read<quint8>();
            this->read<quint8>();
            quint8 result = this->read<quint8>();

            {
                this->read(this->read<quint8>() * 2);

                if ((result & 1) != 0)
                {
                    this->read(this->read<quint8>() * 2);
                }
                if ((result & 2) != 0)
                {
                    this->read(this->read<quint8>() * 2);
                }
                if ((result & 4) != 0)
                {
                    this->read(this->read<quint8>() * 2);
                }
            }
            break;
        }
        case 0x147:
        {
            // 游戏对象消失
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();
            break;
        }
        case 0x148:
        {
            // 释放技能
            this->RecvSkill();
            break;
        }
        case 0x149:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();
            break;
        }
        case 0x150:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            quint32 size = this->ReadVarint();

            for (quint32 i = 0; i < size; i++)
            {
                this->ReadVarint();
                this->ReadVarint1();
            }

            break;
        }
        case 0x151:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint16>();

            quint32 size = this->ReadVarint();

            for (quint32 i = 0; i < size; i++)
            {
                this->ReadVarint();
                this->ReadVarint1();
            }

            break;
        }
        case 0x153:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint32>();
            this->read<quint32>();
            this->read<quint32>();
            this->read<quint32>();
            break;
        }
        case 0x15e:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint16>();

            break;
        }
        case 0x184:
        {
            this->read<quint32>();
            this->read<quint32>();
            this->read<quint32>();
            this->read<quint32>();

            this->read<quint64>();
            this->read<quint8>();
            this->read<quint32>();
            break;
        }
        case 0x1a9:
        {
            this->read<quint8>();
            this->read<quint8>();
            this->read<quint8>();
            break;
        }
        case 0x1ab:
        {
            quint8 size = read<quint8>();
            for (quint8 i = 0; i < size; i++)
            {
                read(0xc);
                quint8 size1 = read<quint8>();
                for (quint8 i = 0; i < size1; i++)
                {

                    QJsonObject stats;
                    QJsonObject mod = Helper::Data::GetMods(read<quint16>());

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
                }
            }

            size = read<quint8>();
            for (quint8 i = 0; i < size; i++)
            {
                read<quint32>();
            }
            size = read<quint8>();
            for (quint8 i = 0; i < size; i++)
            {
                read<quint32>();
            }
            break;
        }
        case 0x1de:
        {
            read<quint32>();
            read<quint32>();
            read<quint16>();
            read<quint32>();
            read<quint32>();
            read<quint8>();
            break;
        }
        case 0x1e0:
        {
            read<quint32>();
            read<quint32>();
            read<quint32>();
            read<quint32>();
            break;
        }
        case 0x1f5:
        {
            this->read<quint8>();
            quint8 size = this->read<quint8>();

            for (quint8 i = 0; i < size; i++)
            {
                this->read<quint16>();
                this->read<quint8>();
            }

            break;
        }
        case 0x17c:
        {
            quint8 size = this->read<quint8>();

            for (quint8 i = 0; i < size; i++)
            {
                this->read<quint8>();
                this->read<quint32>();
                this->read<quint64>();
            }

            this->read<quint8>();
            break;
        }
        case 0x212:
        {
            // Data/ArchnemesisMods.dat
            for (int i = 0; i < (0x610 / 0x18); i++)
            {
                quint8 v15 = this->read<quint8>();
                if (v15 != 0xff)
                {
                    this->read<quint8>();
                }
            }

            break;
        }
        case 0x21a:
        {
            // 服务器心跳
            this->ServerTick();
            break;
        }
        case 0x21e:
        {
            // 游戏实体对象
            this->RecvGameObject();
            break;
        }
        case 0x21f:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            quint16    size = this->read<quint16>();
            QByteArray data = this->read(size);
            break;
        }
        case 0x220:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();
            break;
        }
        default:
            qWarning() << QString("[!] UnknownPacket PacketId:[0x%1] Data:[%2]")
                              .arg(QString::number(PacketId, 16))
                              .arg(this->readAll().toHex(' '));
            break;
        }
    }
}

// ====================================================================================================

void ExileGame::SendTicket()
{
    qDebug() << "SendTicket";

    this->writeId(0x3);

    this->write<quint32>(this->m_WorldInstance);
    this->write<quint16>(0x2);
    this->writeString(m_ExileClient->m_CharacterList.at(m_ExileClient->m_LastSelectIndex)->m_Name);
    this->write<quint32>(this->m_Ticket);

    this->write<quint32>(0x00);
    this->write<quint8>(0x40);
    this->write<quint8>(0x00);
    this->write<quint8>(0x00);
    this->write<quint8>(0x01);
}

void ExileGame::SendTileHash(quint32 tileHash, quint32 doodadHash)
{
    qDebug() << QString("SendTileHash(%1, %2)").arg(tileHash).arg(doodadHash);
    this->writeId(0x53);
    this->write(tileHash);
    this->write(doodadHash);
}

void ExileGame::SendSkill(qint32 x, qint32 y, quint16 skill, quint16 u)
{
    this->writeId(0x18);

    this->write<qint32>(x);
    this->write<qint32>(y);

    this->write<quint16>(skill);
    this->write<quint16>(m_SendSkillCount);
    this->write<quint16>(u);
}

// ====================================================================================================

void ExileGame::RecvInitWorld()
{
    m_WorldAreaHASH16 = this->read<quint16>();
    m_League          = this->readString();
    m_Seed            = this->read<quint32>();
    this->readAll();

    QNetworkAccessManager *mgr = new QNetworkAccessManager;
    QNetworkRequest        req(QUrl(QString("http://127.0.0.1:6112/world?hash16=%1&seed=%2").arg(m_WorldAreaHASH16).arg(m_Seed)));
    mgr->get(req);

    connect(mgr, &QNetworkAccessManager::finished, [=](QNetworkReply *reply)
            {
                m_TileHash      = reply->rawHeader("TileHash").toUInt();
                m_DoodadHash    = reply->rawHeader("DoodadHash").toUInt();
                m_TerrainWidth  = reply->rawHeader("TerrainWidth").toUInt();
                m_TerrainHeight = reply->rawHeader("TerrainHeight").toUInt();
                m_WorldAreaId   = reply->rawHeader("WorldAreaId");
                m_WorldAreaName = reply->rawHeader("WorldAreaName");
                m_RadarInfo     = QJsonDocument::fromJson(reply->rawHeader("RadarInfo")).object();
                m_TerrainData   = reply->readAll();

                this->SendTileHash(m_TileHash, m_DoodadHash);
                reply->deleteLater();
                mgr->deleteLater();
            });
}

void ExileGame::RecvChat()
{
    // 聊天信息
    QString name = readString();
    QString text = readString();
    readString();

    qDebug() << name << text;

    read<quint16>();
    read<quint8>();
    read<quint8>();

    quint8 itemCount = read<quint8>();

    for (int i = 0; i < itemCount; i++)
    {
        quint32 index = read<quint32>();

        quint16    size     = read<quint16>();
        QByteArray itemData = read(size);
    }
}

void ExileGame::RecvBackendError()
{
    // 错误信息
    quint16 BackendError = this->read<quint16>(); // error Id
    this->readString();                           // ??
    emit signal_BackendError(BackendError);
}

void ExileGame::RecvInventory()
{
    // 库存信息
    this->read<quint8>();
    quint32 inventoryId = this->read<quint32>();

    {
        // sub_15F00C0
        this->read<quint8>();
        this->read<quint8>();
        this->read<quint8>(); // Width
        this->read<quint8>(); // Height
        quint8 v16 = this->read<quint8>();
        if ((v16 & 0x8) != 0)
        {
            this->read<quint64>();
        }

        if ((v16 & 0x10) != 0)
        {
            this->read<quint32>();
            this->read<quint32>();
            this->read<quint32>();
            this->read<quint32>();
        }

        {
            // 物品数量
            quint32 size = this->read<quint32>();

            for (quint32 i = 0; i < size; i++)
            {
                quint32 index = this->read<quint32>();
                quint8  y     = this->read<quint8>();
                quint8  x     = this->read<quint8>();

                // item info
                QByteArray Data = this->read(this->read<quint16>());

                // new ItemObject
                m_ItemList.append(new ItemObject(index, QPoint(x, y), Data, this));
            }
        }
    }

    this->read<quint8>();
    this->read<quint8>();
}

void ExileGame::RecvGameObject()
{
    quint32 id = this->read<quint32>(); // GameObjectId
    this->read<quint32>();
    this->read<quint16>();

    quint32    Hash = this->read<quint32>();             // GameObject Hash
    QByteArray Data = this->read(this->read<quint16>()); // Components Data

    // new GameObject
    m_EntityList.append(new GameObject(id, Hash, Data, this));
}

void ExileGame::RecvPlayerId()
{
    m_PlayerId = this->read<quint32>();
}

void ExileGame::RecvSkill()
{
    quint32 id = this->read<quint32>(); // GameObjectId
    this->read<quint32>();
    this->read<quint16>();

    quint16 v4 = this->read<quint16>();
    if ((v4 & 0x80) != 0)
    {
        this->read<quint32>();
    }

    qint32 currentX = this->ReadVarint1();
    qint32 currentY = this->ReadVarint1();
    qint32 targetX  = this->ReadVarint1();
    qint32 targetY  = this->ReadVarint1();

    this->read<quint16>();
    this->read<quint16>();

    // 更新坐标
    GameObject *obj = FindEntity(id);
    if (obj != nullptr)
    {
        obj->m_Pos.setX(currentX);
        obj->m_Pos.setY(currentY);
    }
}

// ====================================================================================================

GameObject *ExileGame::FindEntity(int id)
{
    for (int i = 0; i < m_EntityList.size(); i++)
    {
        if (m_EntityList.at(i)->m_Id == id)
        {
            return m_EntityList.at(i);
        }
    }

    return nullptr;
}

void ExileGame::ServerTick()
{
    GameObject *obj = FindEntity(m_PlayerId);

    // Pathfinding
    if (m_Path.isEmpty() && !m_RadarInfo.isEmpty())
    {
        QJsonObject pos = m_RadarInfo.begin().value().toObject();
        this->Pathfinding(pos.value("x").toInt(), pos.value("y").toInt());
    }

    // 更新路径
    if (m_Path.size() && obj != nullptr)
    {
        if (m_Path.size() > 20 && obj->size(m_Path.first()) < 10)
        {
            m_Path.remove(0, 20);
        }
    }

    // 移动
    if (!m_Path.isEmpty())
    {
        this->SendSkill(m_Path.first().x(), m_Path.first().y(), 0x2909, 0x408);
    }
}

void ExileGame::Pathfinding(int x, int y)
{
    GameObject *obj = FindEntity(m_PlayerId);

    if (obj != nullptr)
    {
        qDebug() << QString("Pathfinding(%1, %2)").arg(x).arg(y);

        QPoint start = obj->m_Pos;

        AStar::Params param;
        param.corner   = true;
        param.width    = (uint16_t)m_TerrainWidth;
        param.height   = (uint16_t)m_TerrainHeight;
        param.start    = AStar::Vec2((uint16_t)start.x(), (uint16_t)start.y());
        param.end      = AStar::Vec2((uint16_t)x, (uint16_t)y);
        param.can_pass = [this](const AStar::Vec2 &pos) -> bool
        {
            return m_TerrainData.at((pos.y * m_TerrainWidth) + pos.x) > '1';
        };

        BlockAllocator allocator;
        AStar          algorithm(&allocator);

        m_Path.clear();
        auto path = algorithm.find(param);

        for (auto i = path.begin(); i != path.end(); i++)
        {
            m_Path.append(QPoint(i->x, i->y));
        }
    }
}