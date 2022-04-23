#pragma once
#include "ExileClient.h"
#include "ExileSocket.h"

class ExileGame : public ExileSocket
{
    Q_OBJECT
private:
    ExileClient *m_ExileClient;

    quint32 m_Ticket;
    quint32 m_WorldAreaHASH16;
    quint32 m_WorldInstance;

public:
    explicit ExileGame(ExileClient *client);
    virtual ~ExileGame();

public slots:
    void connectToHost(quint32 Address, quint16 Port, quint32 Ticket, quint32 WorldAreaHASH16, quint32 WorldInstance);

    void on_game_connected();
    void on_game_disconnected();
    void on_game_errorOccurred(QAbstractSocket::SocketError socketError);
    void on_game_readyRead();

    void SendTicket();
};
