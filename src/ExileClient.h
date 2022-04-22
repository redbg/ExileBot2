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
    explicit ExileClient(QObject *parent = nullptr);
    virtual ~ExileClient();

public:
    void connectToHost(const QString &email, const QString &password)
    {
        this->m_Email    = email;
        this->m_Password = password;
        ExileSocket::connectToHost("sjc01.login.pathofexile.com", 20481);
    }

    QJsonObject toJsonObject()
    {
        QJsonObject JsonObject = Helper::Json::toJsonObject(this);
        JsonObject.insert("CharacterList", Helper::Json::toJsonArray<Character>(m_CharacterList));
        return JsonObject;
    }

public slots:
    void on_client_connected();
    void on_client_disconnected();
    void on_client_errorOccurred(QAbstractSocket::SocketError socketError);
    void on_client_readyRead();

public slots:

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
};
