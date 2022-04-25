#pragma once
#include "ExileClient.h"
#include "ExileSocket.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class ExileGame : public ExileSocket
{
    Q_OBJECT
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
    QByteArray m_Terrain;

    QString     m_WorldAreaId;
    QString     m_WorldAreaName;
    QJsonObject m_RadarInfo;

public:
    explicit ExileGame(ExileClient *client);
    virtual ~ExileGame();

public slots:
    void connectToHost(quint32 Address, quint16 Port, quint32 Ticket, quint32 WorldAreaHASH16, quint32 WorldInstance, QByteArray Key);

    void on_game_connected();
    void on_game_disconnected();
    void on_game_errorOccurred(QAbstractSocket::SocketError socketError);
    void on_game_readyRead();

    void SendTicket();
    void SendTileHash(quint32 tileHash, quint32 doodadHash);

    void RecvInitWorld();
    void RecvChat();
    void RecvBackendError();

signals:
    void signal_BackendError(int result);
};
