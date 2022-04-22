#pragma once
#include "Character.h"
#include "ExileSocket.h"
#include "Global.h"
#include <QCryptographicHash>
#include <QMetaProperty>
#include <cryptopp/dh.h>
#include <cryptopp/osrng.h>

class ExileClient : public ExileSocket
{
    Q_OBJECT
    Q_PROPERTY(QString Email MEMBER m_Email)
    Q_PROPERTY(QString Password MEMBER m_Password)
    Q_PROPERTY(QString AccountName MEMBER m_AccountName)
    Q_PROPERTY(QList<Character *> CharacterList MEMBER m_CharacterList)

private:
    QString m_Email;
    QString m_Password;
    QString m_AccountName;

    QList<Character *> m_CharacterList;

    CryptoPP::byte m_PrivateKey[0x80];
    CryptoPP::byte m_PublicKey[0x80];

public:
    enum SEND
    {
        ClientPublicKey = 0x2,
        Login           = 0x3,
        SelectCharacter = 0xd,
        CreateCharacter = 0x11,
        GetLeagueList   = 0x16,
    };
    Q_ENUM(SEND)

    enum RECV
    {
        ServerPublicKey       = 0x2,
        LoginResult           = 0x4,
        CreateCharacterResult = 0x12,
        SelectCharacterResult = 0x13,
        CharacterList         = 0x14,
        CloseSocket           = 0x15,
        LeagueList            = 0x19,
    };
    Q_ENUM(RECV)

public:
    explicit ExileClient(const QString &email, const QString &password);
    virtual ~ExileClient();

public slots:
    void connectToHost(const QString &hostName, quint16 port)
    {
        // sjc01.login.pathofexile.com:20481
        qDebug() << QString("connectToHost(%1, %2)").arg(hostName).arg(port);
        ExileSocket::connectToHost(hostName, port);
    }

    void on_client_connected();
    void on_client_disconnected();
    void on_client_errorOccurred(QAbstractSocket::SocketError socketError);
    void on_client_readyRead();

    // Send
    void SendClientPublicKey();
    void SendLogin(const QString &email, const QString &password);
    void SendSelectCharacter(quint32 index = 0);
    void SendCreateCharacter(QString name, QString league, Character::ClassType classType);
    void SendGetLeagueList();

    // Recv
    void RecvServerPublicKey();
    bool RecvLoginResult();
    void RecvCreateCharacterResult();
    void RecvSelectCharacterResult();
    void RecvCharacterList();
    void RecvCloseSocket();
    void RecvLeagueList();

signals:
    void signal_BackendError(int result);
    void signal_LoginSuccess();
    void signal_CharacterList();
};
