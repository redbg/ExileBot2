#pragma once
#include "AStar/astar.h"
#include "AStar/blockallocator.h"
#include "ExileClient.h"
#include "ExileSocket.h"
#include "Object/GameObject.h"
#include "Object/ItemObject.h"
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <cmath>

class ExileGame : public ExileSocket
{
    Q_OBJECT
    Q_PROPERTY(QList<ItemObject *> ItemList MEMBER m_ItemList)
    Q_PROPERTY(QList<GameObject *> EntityList MEMBER m_EntityList)
    Q_PROPERTY(quint32 PlayerId MEMBER m_PlayerId)

private:
    ExileClient *m_ExileClient;

    quint32 m_Ticket;
    quint32 m_WorldAreaHASH16;
    quint32 m_WorldInstance;
    QString m_League;
    quint32 m_Seed;

    quint32    m_TileHash;
    quint32    m_DoodadHash;
    quint32    m_TerrainWidth;
    quint32    m_TerrainHeight;
    QByteArray m_TerrainData;

    QString     m_WorldAreaId;
    QString     m_WorldAreaName;
    QJsonObject m_RadarInfo;

    QList<ItemObject *> m_ItemList;
    QList<GameObject *> m_EntityList;
    quint32             m_PlayerId;

    quint16 m_SendSkillCount;

    QList<QPoint> m_Path;

public:
    explicit ExileGame(ExileClient *client);
    virtual ~ExileGame();

public:
    QImage Render();

public slots:
    void connectToHost(quint32 Address, quint16 Port, quint32 Ticket, quint32 WorldAreaHASH16, quint32 WorldInstance, QByteArray Key);

    void on_game_connected();
    void on_game_disconnected();
    void on_game_errorOccurred(QAbstractSocket::SocketError socketError);
    void on_game_readyRead();

public slots:
    void SendTicket();
    void SendTileHash(quint32 tileHash, quint32 doodadHash);
    void SendSkill(qint32 x, qint32 y, quint16 skill, quint16 u);

public slots:
    void RecvInitWorld();
    void RecvChat();
    void RecvBackendError();
    void RecvInventory();
    void RecvGameObject();
    void RecvPlayerId();
    void RecvSkill();

public slots:
    GameObject *FindEntity(int id);
    void        Tick();
    void        Pathfinding(int x, int y);

signals:
    void signal_BackendError(int result);
};
