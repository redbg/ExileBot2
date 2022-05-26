#include "ExileGame.h"

ExileGame::ExileGame(ExileClient *client)
    : m_ExileClient(client)
{
    this->clear();
    connect(this, &ExileSocket::connected, this, &ExileGame::on_game_connected);
    connect(this, &ExileSocket::disconnected, this, &ExileGame::on_game_disconnected);
    connect(this, &ExileSocket::errorOccurred, this, &ExileGame::on_game_errorOccurred);
    connect(this, &ExileSocket::readyRead, this, &ExileGame::on_game_readyRead);
}

ExileGame::~ExileGame()
{
    qDebug() << "~ExileGame";
}

QImage ExileGame::Render()
{
    if (m_TerrainData.isEmpty())
    {
        return QImage();
    }

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
            painter.setPen(Qt::green);

            painter.drawText(obj->m_Pos, QString("Global:[%1]").arg(m_ChatChannel));
        }
        else
        {
            painter.setBrush(Qt::red);
            painter.setPen(Qt::red);
        }

        // painter.drawText(obj->m_Pos, obj->objectName());
        painter.drawEllipse(obj->m_Pos.x() - 3, obj->m_Pos.y() - 3, 6, 6);

        if (!obj->m_TargetPos.isNull())
        {
            painter.drawLine(obj->m_Pos.x(), obj->m_Pos.y(), obj->m_TargetPos.x(), obj->m_TargetPos.y());
        }
    }

    // Render RadarInfo
    painter.setPen(Qt::green);
    painter.setBrush(Qt::green);
    for (auto i = m_RadarInfo.begin(); i != m_RadarInfo.end(); i++)
    {
        QJsonObject posObj = m_RadarInfo.begin().value().toObject();
        QPoint      pos(posObj.value("x").toInt(), posObj.value("y").toInt());

        painter.drawText(pos.x(), pos.y(), i.key());

        painter.drawEllipse(pos.x() - 3, pos.y() - 3, 6, 6);
    }

    // Render Path
    for (int i = 0; i < m_PathList.size(); i++)
    {
        image.setPixelColor(m_PathList[i], Qt::green);
    }

    return image;
}

void ExileGame::connectToHost(quint32 Address, quint16 Port, quint32 Ticket, quint32 WorldAreaHASH16, quint32 WorldInstance, QByteArray Key)
{
    this->clear();

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

void ExileGame::clear()
{
    m_Ticket          = 0;
    m_WorldAreaHASH16 = 0;
    m_WorldInstance   = 0;
    m_League.clear();
    m_Seed = 0;

    m_TileHash      = 0;
    m_DoodadHash    = 0;
    m_TerrainWidth  = 0;
    m_TerrainHeight = 0;
    m_TerrainData.clear();

    m_WorldAreaId.clear();
    m_WorldAreaName.clear();
    m_RadarInfo = QJsonObject();

    m_PlayerId = 0;
    qDeleteAll(m_ItemList);
    m_ItemList.clear();
    qDeleteAll(m_EntityList);
    m_EntityList.clear();

    m_SendSkillCount = 0;

    m_PathList.clear();

    m_50 = 0;
    m_5b = 0;

    m_Preload.clear();
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
        case 0x5:
        {
            // 开启加密
            this->EnableCrypto();
            m_5b = m_Encryption2.ProcessByte(0);
            m_50 = 8;
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
            // 心跳
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
        case 0x16:
        {
            read<quint32>();
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
            quint32 Ticket = this->read<quint32>();
            this->read<quint32>();
            quint16 WorldAreaHASH16 = this->read<quint16>();
            quint32 WorldInstance   = this->read<quint32>();
            this->read<quint8>();

            quint8 size = this->read<quint8>();

            quint16 Port    = 0;
            quint32 Address = 0;

            for (quint8 i = 0; i < size; i++)
            {
                this->read<quint16>();
                Port    = this->read<quint16>();
                Address = this->read<quint32>();
                this->read(0x14);
            }

            QByteArray Key = this->read(0x40);

            // this->connectToHost(Address, Port, Ticket, WorldAreaHASH16, WorldInstance, Key);
            this->disconnectFromHost();
            break;
        }
        case 0x25:
        {
            // 天赋

            this->read<quint16>(); // 后悔点
            this->read<quint16>();
            this->read<quint16>();

            quint8 总天赋点 = this->read<quint8>();
            this->read<quint8>();

            if ((this->read<quint8>() & 2) == 0)
            {
                quint16 已经分配的天赋点 = this->read<quint16>();
                this->read(已经分配的天赋点 * 2);

                m_SkillPoint = 总天赋点 - 已经分配的天赋点;

                this->read(this->read<quint16>() * 2);
                this->read(this->read<quint16>() * 2);
                this->read(this->read<quint16>() * 4);
            }

            break;
        }
        case 0x2c:
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
        case 0x3a:
        {
            this->read<quint32>();
            this->read(0x13);
            break;
        }
        case 0x3f:
        {
            this->read(0x1a);
            this->read<quint16>();
            this->read<quint16>();
            break;
        }
        case 0x5b:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint8>();

            this->SendContinue();
            break;
        }
        case 0x6f:
        {
            readString();
            readString();
            quint8 v4 = read<quint8>();
            if (v4 != 0)
            {
                readString();

                read<quint8>();
                read<quint8>();
                read<quint16>();
                read<quint16>();
                read<quint16>();
                read<quint16>();
                read<quint8>();
                readString();

                read<quint8>();
                read<quint16>();
                read<quint8>();
                read<quint8>();
            }
            else
            {
                read<quint64>();
            }
            break;
        }
        case 0x85:
        {
            this->RecvUpdateInventory();
            break;
        }
        case 0x86:
        {
            // 库存信息
            this->RecvInventory();
            break;
        }
        case 0x89:
        {
            this->read<quint32>();
            break;
        }
        case 0x8a:
        {
            quint8 size = read<quint8>();

            for (quint8 i = 0; i < size; i++)
            {
                read<quint8>();
                read<quint16>();
            }

            break;
        }
        case 0x8c:
        {
            read<quint32>();
            read<quint32>();
            break;
        }
        case 0x8e:
        {
            read<quint16>();
            read<quint32>();
            read<quint8>();
            break;
        }
        case 0x8f:
        {
            read<quint8>();
            readString();
            readString();
            break;
        }
        case 0xa3:
        {
            read<quint8>();
            break;
        }
        case 0xa4:
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
        case 0xda:
        {
            quint8 size = read<quint8>();

            for (int i = 0; i < size; i++)
            {
                read<quint32>();
                read<quint32>();
                read<quint32>();
            }

            break;
        }
        case 0xe5:
        {
            quint16 v1 = read<quint16>();
            quint8  v2 = read<quint8>();
            quint8  v3 = read<quint8>();

            if (v2 == 0)
            {
                // Global
                m_ChatChannel = v1;
            }

            break;
        }
        case 0x146:
        {
            this->read<quint8>();
            this->read<quint8>();
            this->read<quint8>();

            quint8 result = this->read<quint8>();

            this->read(this->read<quint8>() * 2);

            if ((result & 1) != 0)
            {
                this->read(0x10);
            }

            if ((result & 2) != 0)
            {
                this->read(0x10);
            }

            if ((result & 4) != 0)
            {
                this->read(0x10);
            }

            if ((result & 8) != 0)
            {
                this->read(this->read<quint8>() * 2);
            }

            if ((result & 0x10) != 0)
            {
                this->read(0xe);
                this->read(0xe);
                this->read(0xe);
            }

            break;
        }
        case 0x147:
        {
            this->read<quint16>();
            this->read<quint8>();
            this->read<quint8>();
            this->read<quint8>();
            break;
        }
        case 0x14a:
        {
            // 游戏对象消失
            this->RecvRemoveGameObject();
            break;
        }
        case 0x14b:
        {
            // 释放技能
            this->RecvSkill();
            break;
        }
        case 0x14c:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();
            break;
        }
        case 0x14f:
        {
            read<quint32>();
            read<quint32>();
            read<quint16>();

            {
                read<quint16>();
                read<quint8>();
                read<quint8>();
                read<quint32>();
            }
            quint16 v6 = read<quint16>();
            quint8  v4 = read<quint8>();

            if (((v4 & 2) != 0))
            {
                if ((v6 & 0x400) != 0)
                {
                    read<quint16>();
                }
                if ((v6 & 0x4000) != 0)
                {
                    read<quint8>();
                }
                read<quint8>();
                if ((v4 & 4) != 0)
                {
                    read<quint16>();
                }
                if ((v4 & 8) != 0)
                {
                    read<quint16>();
                }
            }
            break;
        }
        case 0x150:
        {
            {
                read<quint32>();
                read<quint32>();
                read<quint16>();
            }
            {
                read<quint16>();
                read<quint8>();
                read<quint8>();
                read<quint32>();
            }
            read<quint8>();
            break;
        }
        case 0x151:
        {
            this->RecvUpdateLife();
            break;
        }
        case 0x153:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            quint32 size = this->ReadVaruint();

            for (quint32 i = 0; i < size; i++)
            {
                this->ReadVaruint();
                this->ReadVarint();
            }

            break;
        }
        case 0x154:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint16>();

            quint32 size = this->ReadVaruint();

            for (quint32 i = 0; i < size; i++)
            {
                this->ReadVaruint();
                this->ReadVarint();
            }

            break;
        }
        case 0x155:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint8>();
            break;
        }
        case 0x156:
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
        case 0x15c:
        {
            this->RecvUpdateChest();
            break;
        }
        case 0x15d:
        {
            {
                this->read<quint32>();
                this->read<quint32>();
                this->read<quint16>();
            }
            read<quint8>();
            break;
        }
        case 0x160:
        {
            {
                read<quint32>();
                read<quint32>();
                read<quint16>();
            }
            read<quint16>();
            quint16 bufferId = read<quint16>();
            read<quint32>();
            read<quint16>();
            quint8 v43 = read<quint8>();

            if ((v43 & 1) != 0)
            {
                read<quint8>();
                read<quint32>();
                read<quint8>();
            }
            if ((v43 & 2) != 0)
            {
                read<quint32>();
            }

            if ((v43 & 0x80) != 0)
            {
                read<quint32>();
            }

            if ((v43 & 4) != 0)
            {
                read<quint16>();
            }

            if ((v43 & 8) != 0)
            {
                read<quint16>();
                read<quint16>();
            }

            if ((v43 & 0x10) != 0)
            {
                read<quint16>();
            }

            //-- LABEL_48:
            if ((v43 & 0x20) != 0)
            {
                read<quint8>();
            }

            QJsonObject type = Helper::Data::GetBuffDefinitions(bufferId);

            if ((type.value("Unknown43").toInt() & 16777216) == 16777216)
            {
                read<quint16>();
            }

            if (type.value("IsRecovery").toBool())
            {
                for (auto v : type.value("Unknown41").toArray())
                {
                    read<quint32>();
                }
            }
            quint32 size = read<quint32>();
            for (quint32 i = 0; i < size; i++)
            {
                read<quint32>();
            }
            break;
        }
        case 0x161:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint16>();

            break;
        }
        case 0x162:
        {
            {
                read<quint32>();
                read<quint32>();
                read<quint16>();
            }
            read<quint32>();
            read<quint32>();
            read<quint32>();
            read<quint32>();
            read<quint8>();
            read<quint8>();
            break;
        }
        case 0x165:
        {
            {
                read<quint32>();
                read<quint32>();
                read<quint16>();
            }

            if (read<quint8>())
            {
                read<quint8>();
            }

            break;
        }
        case 0x172:
        {
            read<quint8>();
            break;
        }
        case 0x17f:
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
        case 0x187:
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
        case 0x1ac:
        {
            this->read<quint8>();
            this->read<quint8>();
            this->read<quint8>();
            break;
        }
        case 0x1e1:
        {
            read<quint32>();
            read<quint32>();
            read<quint16>();

            read<quint32>();
            read<quint32>();
            read<quint8>();
            break;
        }
        case 0x1e3:
        {
            read<quint32>();
            read<quint32>();
            read<quint32>();
            read<quint32>();
            break;
        }
        case 0x1f8:
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
        case 0x205:
        {
            this->read<quint8>();

            for (int i = 0; i < 9; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    this->read<quint32>();
                }
            }

            this->read<quint8>();
            this->read<quint8>();
            this->read<quint8>();

            break;
        }
        case 0x215:
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
            this->read(0x10);
            break;
        }
        case 0x21c:
        {
            this->read<quint32>();
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint8>();
            this->read<quint8>();
            break;
        }
        case 0x21e:
        {
            this->read<quint16>();
            break;
        }
        case 0x221:
        {
            // 服务器心跳
            break;
        }
        case 0x225:
        {
            // 游戏实体对象
            this->RecvGameObject();
            break;
        }
        case 0x226:
        {
            // 更新游戏对象组件
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            quint16    size = this->read<quint16>();
            QByteArray data = this->read(size);
            break;
        }
        case 0x227:
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
    this->writeId(0x54);
    this->write(tileHash);
    this->write(doodadHash);
}

void ExileGame::SendSkill(qint32 x, qint32 y, quint16 skill, quint16 u)
{
    m_SendSkillCount++;

    this->writeId(0x18);

    this->write<qint32>(x);
    this->write<qint32>(y);

    this->write<quint16>(skill);
    this->write<quint16>(m_SendSkillCount);
    this->write<quint16>(u);
}

void ExileGame::SendSkillById(int id, quint16 skill, quint16 u)
{
    m_SendSkillCount++;

    this->writeId(0x17);

    this->write<int>(id);

    this->write<quint16>(skill);
    this->write<quint16>(m_SendSkillCount);
    this->write<quint16>(u);
}

void ExileGame::SendUseGem(int inventoryId, int id, int index)
{
    qDebug() << "SendUseGem" << inventoryId << id << index;

    this->writeId(0x23);

    this->write<qint32>(inventoryId);
    this->write<qint32>(id);
    this->write<qint32>(index);
}

void ExileGame::SendSkipAllTutorials()
{
    this->writeId(0x18b);

    this->write<quint8>(0x11);
}

void ExileGame::SendResurrect(quint8 arg1)
{
    this->writeId(0x40);

    this->write<quint8>(arg1);
}

void ExileGame::SendContinue()
{
    this->writeId(0x5c);
}

void ExileGame::SendUpItem(int inventoryId, int id)
{
    this->writeId(0x1b);

    int local_2  = 2;
    int local_15 = 0x15;

    int v7  = inventoryId;
    int v10 = 0;

    int local_5b = 0;
    int local_50 = m_50;

    while (1)
    {
        local_5b = m_5b;

        v10 = 0x1e - local_15;

        if (local_50 > (0x1e - local_15))
        {
            break;
        }

        int v11 = local_15;
        local_15 += local_50;
        v7 |= (local_5b << v11);

        m_5b = m_Encryption2.ProcessByte(0);

        m_50     = 8;
        local_50 = 8;

        if (local_15 == 0x1e)
        {
            goto LABEL_9;
        }
    }

    m_50 = local_50 - v10;
    v7 |= (local_5b & ((1 << v10) - 1)) << local_15;
    m_5b = (unsigned char)local_5b >> v10;
LABEL_9:

    this->write<quint32>(v7);
    this->write<quint32>(id);

    char v16 = 0;
    local_50 = m_50;

    while (1)
    {
        local_5b = m_5b;

        v10 = 8 - local_2;

        if (local_50 > (8 - local_2))
        {
            break;
        }

        int v20 = local_2;
        local_2 += local_50;
        v16 |= (local_5b << v20);

        m_5b = m_Encryption2.ProcessByte(0);

        local_50 = 8;
        m_50     = 8;

        if (local_2 == 8)
        {
            goto LABEL_17;
        }
    }
    m_50 = local_50 - v10;
    v16 |= (local_5b & (unsigned char)((1 << v10) - 1)) << local_2;
    m_5b = (unsigned char)local_5b >> v10;

LABEL_17:
    this->write<quint8>(v16);
}

void ExileGame::SendChat(QString msg)
{
    this->writeId(0x8);
    this->writeString(msg);
    this->write<quint8>(0);
}

// ====================================================================================================

void ExileGame::RecvInitWorld()
{
    int size = 0;

    m_WorldAreaHASH16 = this->read<quint16>();
    m_League          = this->readString();
    m_Seed            = this->read<quint32>();

    this->read<quint8>();
    this->read<quint8>();
    this->read<quint8>();
    this->read<quint8>();

    quint8 a1_1d = this->read<quint8>();
    quint8 a_1e  = this->read<quint8>();
    this->read<quint8>();

    this->read<quint8>();
    this->read<quint8>();
    this->read<quint8>();

    if ((a1_1d & 8) != 0)
    {
        this->read<quint16>();
    }

    size = this->read<quint16>();

    for (int i = 0; i < size; i++)
    {
        quint32 hash = this->read<quint32>();
        m_Preload.append(hash);
    }

    size = this->read<quint16>();

    for (int i = 0; i < size; i++)
    {
        this->read<quint8>();
    }

    size = this->read<quint8>();

    for (int i = 0; i < size; i++)
    {
        this->read<quint8>();
    }

    size = this->read<quint16>();

    for (int i = 0; i < size; i++)
    {
        this->read<quint16>();
    }

    if ((a_1e & 1) != 0)
    {
        size = this->read<quint8>();

        for (int i = 0; i < size; i++)
        {
            this->read<quint8>();
        }
    }

    if ((a_1e & 2) != 0)
    {
        size = this->read<quint8>();

        for (int i = 0; i < size; i++)
        {
            this->read<quint8>();
        }
    }

    if ((a_1e & 4) != 0)
    {
        size = this->read<quint8>();

        for (int i = 0; i < size; i++)
        {
            this->read<quint16>();
            this->read<quint8>();
            this->read<quint32>();
        }
    }

    if ((a1_1d & 0x80) != 0)
    {
        size = this->ReadVaruint();

        for (int i = 0; i < size; i++)
        {
            this->ReadVaruint();
            this->ReadVarint();
        }

        size = this->ReadVaruint();

        for (int i = 0; i < size; i++)
        {
            this->ReadVaruint();
            this->ReadVarint();
        }

        size = this->read<quint8>();

        for (int i = 0; i < size; i++)
        {
            int size1 = this->ReadVaruint();

            for (int i = 0; i < size1; i++)
            {
                this->ReadVaruint();
                this->ReadVarint();
            }
        }
    }

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
                quint32 id = this->read<quint32>();
                quint8  y  = this->read<quint8>();
                quint8  x  = this->read<quint8>();

                // item info
                QByteArray Data = this->read(this->read<quint16>());

                // new ItemObject
                m_ItemList.append(new ItemObject(inventoryId, id, QPoint(x, y), Data, this));
            }
        }
    }

    this->read<quint8>();
    this->read<quint8>();
}

void ExileGame::RecvUpdateInventory()
{
    this->read<quint8>();
    quint32 inventoryId = this->read<quint32>();

    {
        this->read<quint32>();
        quint32 size = this->read<quint32>();

        // 删除物品
        for (quint32 i = 0; i < size; i++)
        {
            quint32 id = this->read<quint32>();

            for (int i = 0; i < m_ItemList.size(); i++)
            {
                if (m_ItemList.at(i)->m_InventoryId == inventoryId)
                {
                    if (m_ItemList.at(i)->m_Id == id)
                    {
                        m_ItemList.removeAt(i);
                    }
                }
            }
        }

        // 物品数量
        quint32 itemSize = this->read<quint32>();

        for (quint32 i = 0; i < itemSize; i++)
        {
            quint32 id = this->read<quint32>();
            quint8  y  = this->read<quint8>();
            quint8  x  = this->read<quint8>();

            // item info
            QByteArray Data = this->read(this->read<quint16>());

            // new ItemObject
            m_ItemList.append(new ItemObject(inventoryId, id, QPoint(x, y), Data, this));
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

void ExileGame::RecvRemoveGameObject()
{
    quint32 id = this->read<quint32>(); // GameObjectId
    this->read<quint32>();
    this->read<quint16>();

    GameObject *obj = FindEntity(id);

    if (obj != nullptr)
    {
        m_EntityList.removeOne(obj);
    }
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

    qint32 currentX = this->ReadVarint();
    qint32 currentY = this->ReadVarint();
    qint32 targetX  = this->ReadVarint();
    qint32 targetY  = this->ReadVarint();

    this->read<quint16>();
    this->read<quint16>();

    // 更新坐标
    GameObject *obj = FindEntity(id);
    if (obj != nullptr)
    {
        obj->m_Pos.setX(currentX);
        obj->m_Pos.setY(currentY);

        obj->m_TargetPos.setX(targetX);
        obj->m_TargetPos.setY(targetY);
    }
}

void ExileGame::RecvUpdateLife()
{
    qDebug() << "RecvUpdateLife";

    quint32 id = this->read<quint32>(); // GameObjectId
    this->read<quint32>();
    this->read<quint16>();

    qint32 val = this->read<qint32>();
    this->read<quint32>();
    quint8 valType = this->read<quint8>();
    this->read<quint16>();
    this->read<quint8>();

    // 更新血量
    GameObject *obj = FindEntity(id);
    if (obj != nullptr)
    {
        QJsonObject life = obj->m_Components.value("Life").toObject();

        switch (valType)
        {
        case 0:
            life.insert("Life", val);
            break;
        case 1:
            life.insert("Mana", val);
            break;
        case 2:
            life.insert("Shield", val);
            break;
        }

        obj->m_Components.insert("Life", life);
    }
}

void ExileGame::RecvUpdateChest()
{
    quint32 id = this->read<quint32>();
    this->read<quint32>();
    this->read<quint16>();

    quint8 v1 = read<quint8>();
    quint8 v2 = read<quint8>();
    quint8 v3 = read<quint8>();
    qint32 v4 = read<qint32>();

    GameObject *obj = FindEntity(id);

    if (obj != nullptr)
    {
        QJsonObject Chest;

        Chest.insert("v1", v1);
        Chest.insert("v2", v2);
        Chest.insert("v3", v3);
        Chest.insert("v4", v4);

        obj->m_Components.insert("Chest", Chest);
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

GameObject *ExileGame::FindEntityByObjectName(QString objectName)
{
    for (int i = 0; i < m_EntityList.size(); i++)
    {
        if (m_EntityList.at(i)->objectName() == objectName)
        {
            return m_EntityList.at(i);
        }
    }

    return nullptr;
}

void ExileGame::Tick()
{
    GameObject *obj = FindEntity(m_PlayerId);

    // 更新路径
    if (m_PathList.size() && obj != nullptr)
    {
        if (obj->size(m_PathList.first()) > 100)
        {
            m_PathList.clear();
        }

        if (obj->size(m_PathList.first()) < 30)
        {
            m_PathList.remove(0, m_PathList.size() >= 5 ? 5 : m_PathList.size());
        }
    }

    // 移动
    if (!m_PathList.isEmpty())
    {
        int Random = QRandomGenerator::global()->bounded(-3, 3);
        this->SendSkill(m_PathList.first().x() + Random, m_PathList.first().y() + Random, 0x2909, 0x408);
    }
}

void ExileGame::MoveTo(int x, int y)
{
    if (!m_PathList.isEmpty() && m_PathList.last() == QPoint(x, y))
    {
        return;
    }

    GameObject *obj = FindEntity(m_PlayerId);

    if (obj != nullptr)
    {
        qDebug() << QString("MoveTo(%1, %2)").arg(x).arg(y);

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

        m_PathList.clear();
        auto path = algorithm.find(param);

        for (auto i = path.begin(); i != path.end(); i++)
        {
            m_PathList.append(QPoint(i->x, i->y));
        }
    }
}

void ExileGame::Click(int id)
{
    this->SendSkillById(id, 0x266, 0x408);
    m_PathList.clear();
}

void ExileGame::ClickByObjectName(QString objectName)
{
    GameObject *obj = FindEntityByObjectName(objectName);
    if (obj != nullptr)
    {
        this->Click(obj->m_Id);
    }
}

void ExileGame::Attack(int id, quint16 skillId)
{
    this->SendSkillById(id, skillId, 0x408);
    m_PathList.clear();
}

void ExileGame::Resurrect()
{
    this->disconnectFromHost();
}