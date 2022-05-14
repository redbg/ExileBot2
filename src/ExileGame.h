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
#include <QRandomGenerator>
#include <cmath>

class ExileGame : public ExileSocket
{
    Q_OBJECT
    Q_PROPERTY(QList<ItemObject *> ItemList MEMBER m_ItemList)
    Q_PROPERTY(QList<GameObject *> EntityList MEMBER m_EntityList)
    Q_PROPERTY(quint32 PlayerId MEMBER m_PlayerId)
    Q_PROPERTY(QString WorldAreaId MEMBER m_WorldAreaId)
    Q_PROPERTY(QJsonObject RadarInfo MEMBER m_RadarInfo)
    Q_PROPERTY(QList<QPoint> PathList MEMBER m_PathList)

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

    quint32             m_PlayerId;
    QList<ItemObject *> m_ItemList;
    QList<GameObject *> m_EntityList;

    quint16 m_SendSkillCount;
    quint32 m_SendUseGemCount;

    QList<QPoint> m_PathList;

public:
    explicit ExileGame(ExileClient *client);
    virtual ~ExileGame();

public:
    QImage Render();

public slots:
    void connectToHost(quint32 Address, quint16 Port, quint32 Ticket, quint32 WorldAreaHASH16, quint32 WorldInstance, QByteArray Key);
    void clear();

    void on_game_connected();
    void on_game_disconnected();
    void on_game_errorOccurred(QAbstractSocket::SocketError socketError);
    void on_game_readyRead();

public slots:
    void SendTicket();
    void SendTileHash(quint32 tileHash, quint32 doodadHash);
    void SendSkill(qint32 x, qint32 y, quint16 skill, quint16 u);
    void SendSkillById(int id, quint16 skill, quint16 u);
    void SendUseGem(int inventoryId, int index); // 暂未测试
    void SendSkipAllTutorials();                 // 不建议使用
    void SendResurrect(quint8 arg1);
    void SendContinue();

public slots:
    void RecvInitWorld();
    void RecvChat();
    void RecvBackendError();
    void RecvInventory();
    void RecvInventory1();
    void RecvGameObject();
    void RecvRemoveGameObject();
    void RecvPlayerId();
    void RecvSkill();
    void RecvUpdateLife();

public slots:
    GameObject *FindEntity(int id);
    GameObject *FindEntityByObjectName(QString objectName);
    void        Tick();
    void        MoveTo(int x, int y);
    void        Click(int id);
    void        ClickByObjectName(QString objectName);
    void        Attack(int id, quint16 skillId);
    void        Resurrect();

signals:
    void signal_BackendError(int result);
};
