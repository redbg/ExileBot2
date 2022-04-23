#include "ExileGame.h"

ExileGame::ExileGame(ExileClient *client)
    : m_ExileClient(client)
{
    connect(this, &ExileSocket::connected, this, &ExileGame::on_game_connected);
    connect(this, &ExileSocket::disconnected, this, &ExileGame::on_game_disconnected);
    connect(this, &ExileSocket::errorOccurred, this, &ExileGame::on_game_errorOccurred);
    connect(this, &ExileSocket::readyRead, this, &ExileGame::on_game_readyRead);
}

ExileGame::~ExileGame()
{
}

void ExileGame::connectToHost(quint32 Address, quint16 Port, quint32 Ticket, quint32 WorldAreaHASH16, quint32 WorldInstance)
{
    m_Ticket          = Ticket;
    m_WorldAreaHASH16 = WorldAreaHASH16;
    m_WorldInstance   = WorldInstance;

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
}

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