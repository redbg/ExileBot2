#include "ExileClient.h"

ExileClient::ExileClient(const QString &email, const QString &password)
    : m_Email(email)
    , m_Password(password)
{
    connect(this, &ExileSocket::connected, this, &ExileClient::on_client_connected);
    connect(this, &ExileSocket::disconnected, this, &ExileClient::on_client_disconnected);
    connect(this, &ExileSocket::errorOccurred, this, &ExileClient::on_client_errorOccurred);
    connect(this, &ExileSocket::readyRead, this, &ExileClient::on_client_readyRead);
}

ExileClient::~ExileClient() {}

void ExileClient::on_client_connected()
{
    qDebug() << "on_client_connected";
    this->SendClientPublicKey();
}

void ExileClient::on_client_disconnected()
{
    qDebug() << "on_client_disconnected";
}

void ExileClient::on_client_errorOccurred(QAbstractSocket::SocketError socketError)
{
    qWarning() << socketError;
}

void ExileClient::on_client_readyRead()
{
    while (this->size())
    {
        quint16 PacketId = this->readId();

        switch ((RECV)PacketId)
        {
        case RECV::ServerPublicKey:
            this->RecvServerPublicKey();
            this->SendLogin(m_Email, m_Password);
            break;
        case RECV::LoginResult:
            if (this->RecvLoginResult())
            {
                emit this->signal_LoginSuccess();
            }
            break;
        case RECV::CreateCharacterResult:
            this->RecvCreateCharacterResult();
            break;
        case RECV::SelectCharacterResult:
            this->RecvSelectCharacterResult();
            break;
        case RECV::CharacterList:
            this->RecvCharacterList();
            emit this->signal_CharacterList();
            break;
        case RECV::CloseSocket:
            this->RecvCloseSocket();
            break;
        case RECV::LeagueList:
            this->RecvLeagueList();
            break;
        default:
            qWarning() << QString("[!] UnknownPacket PacketId:[0x%1] Data:[%2]")
                              .arg(QString::number(PacketId, 16))
                              .arg(this->readAll().toHex(' '));
            break;
        }
    }
}

// ====================================================================================================

/**
 * @brief 发送公钥
 *
 */
void ExileClient::SendClientPublicKey()
{
    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::DH dh(Global::p, Global::q, Global::g);
    dh.GenerateKeyPair(rng, this->m_PrivateKey, this->m_PublicKey);

    this->writeId((quint16)SEND::ClientPublicKey); // PacketId
    this->write<quint16>(0x80);                    // PublicKeySize
    this->write((char *)this->m_PublicKey, 0x80);  // PublicKey
    this->write<quint16>(0);                       // DsaKeySize
}

/**
 * @brief 登录
 *
 * @param email 邮箱
 * @param password 密码
 */
void ExileClient::SendLogin(const QString &email, const QString &password)
{
    qDebug() << QString("SendLogin(%1, %2)").arg(email).arg(password);

    // PasswordHash
    QByteArray PasswordHash = QCryptographicHash::hash(
        QByteArray((char *)password.data(), password.size() * 2),
        QCryptographicHash::Sha256);

    // MacAddressHash
    QByteArray MacAddressHash = QCryptographicHash::hash(
        QByteArray((char *)email.data(), email.size() * 2),
        QCryptographicHash::Sha256);

    this->writeId((quint16)SEND::Login);                                 // PacketId
    this->write<quint32>(0x2);                                           // ??
    this->writeString(email);                                            // Email
    this->write((char *)Global::ClientHash, sizeof(Global::ClientHash)); // ClientHash
    this->write(PasswordHash);                                           // PasswordHash
    this->write(MacAddressHash);                                         // MacAddressHash
    this->write<quint8>(1);                                              // ??
    this->write<quint8>(0);                                              // 是否使用记住的密码
    this->write<quint8>(0x40);                                           // ??
}

/**
 * @brief 选择角色
 *
 * @param index 角色索引
 */
void ExileClient::SendSelectCharacter(quint32 index)
{
    qDebug() << QString("选择角色,进入游戏 [%1]").arg(index);

    this->writeId((quint16)SEND::SelectCharacter); // PacketId
    this->write<quint8>(0);                        // 语言
    this->write<quint32>(index);                   // 角色下标
}

/**
 * @brief 创建角色
 *
 * @param name 角色名
 * @param league 赛区
 * @param classType 职业
 */
void ExileClient::SendCreateCharacter(QString name, QString league, Character::ClassType classType)
{
    QString ClassTypeString = Character::GetClassTypeById(classType);

    qDebug() << QString("创建角色 name:%1 league:%2 classType:%3")
                    .arg(name)
                    .arg(league)
                    .arg(ClassTypeString);

    this->writeId((quint16)SEND::CreateCharacter);  // PacketId
    this->writeString(name);                        // 角色名
    this->writeString(league);                      // 赛区名
    this->write<quint32>(0);                        // ??
    this->write<quint32>(0);                        // ??
    this->writeString(ClassTypeString + "Default"); // 职业
    this->write(QByteArray(0x20, 0));               // ??
}

/**
 * @brief 获取赛区列表
 *
 */
void ExileClient::SendGetLeagueList()
{
    this->writeId((quint16)SEND::GetLeagueList);
}

void ExileClient::RecvServerPublicKey()
{
    CryptoPP::DH dh(Global::p, Global::q, Global::g);

    QByteArray serverPublicKey = this->read(this->read<quint16>()); // serverPublicKey
    QByteArray serverDsaKey    = this->read(this->read<quint16>()); // serverDsaKey

    // 生成密钥
    CryptoPP::byte agreedValue[0x80] = {};
    dh.Agree(agreedValue, this->m_PrivateKey, (CryptoPP::byte *)serverPublicKey.data());

    QByteArray key((char *)Global::KeyHeader, sizeof(Global::KeyHeader));
    key.append((char *)agreedValue, sizeof(agreedValue));

    QByteArray keyHash = QCryptographicHash::hash(key, QCryptographicHash::Sha512);

    // 开启加密
    this->SetKeyWithIV(keyHash);
    this->EnableCrypto();
}

// ====================================================================================================

bool ExileClient::RecvLoginResult()
{
    quint16 Result = this->read<quint16>(); // LoginResult

    if (Result != 0)
    {
        this->readAll();
        emit signal_BackendError(Result);
        return false;
    }

    this->readString();                 // ??
    this->read(0x20);                   // 保存的密码
    this->read<quint8>();               // ??
    m_AccountName = this->readString(); // AccountName

    qDebug() << QString("登录成功! AccountName:%1").arg(m_AccountName);

    return true;
}

void ExileClient::RecvCreateCharacterResult()
{
}

void ExileClient::RecvSelectCharacterResult()
{
    qDebug() << "收到选择角色结果,正在进入游戏...";

    quint32 Ticket          = this->read<quint32>();
    quint32 WorldAreaHASH16 = this->read<quint32>();
    quint32 WorldInstance   = this->read<quint32>();

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

    // 连接游戏服务器
    qDebug() << QString("收到游戏服务器连接地址:[%1:%2] Ticket:[%3] WorldAreaHASH16:[%4] WorldInstance:[%5]")
                    .arg(QHostAddress(Address).toString())
                    .arg(Port)
                    .arg(Ticket)
                    .arg(WorldAreaHASH16)
                    .arg(WorldInstance);
}

void ExileClient::RecvCharacterList()
{
    qDebug() << "角色列表";

    qDeleteAll(m_CharacterList);
    m_CharacterList.clear();

    quint32 size = this->read<quint32>(); // Size

    for (quint32 i = 0; i < size; i++)
    {
        Character *character = new Character(this);

        character->m_Name       = this->readString();    // Name
        character->m_League     = this->readString();    // League
        character->m_LeagueId   = this->read<quint8>();  // LeagueId
        character->m_Unknown1   = this->read<quint8>();  // ??
        character->m_Level      = this->read<quint8>();  // Level
        character->m_Experience = this->read<quint32>(); // Experience
        character->m_ClassId    = this->read<quint8>();  // ClassId
        character->m_Unknown2   = this->read<quint16>(); // ??
        character->m_Unknown3   = this->read<quint8>();  // ??
        character->m_Unknown4   = this->read<quint8>();  // ??

        m_CharacterList.append(character);
    }

    this->read<quint32>(); // LastSelectIndex
    this->read<quint8>();  // ??
}

void ExileClient::RecvCloseSocket()
{
    qDebug() << "进入游戏成功,ClientSocket关闭";
    this->read<quint16>();
    this->disconnectFromHost();
}

void ExileClient::RecvLeagueList()
{
    qDebug() << "收到赛区列表";

    this->read(8);

    quint32 size = this->read<quint32>();

    for (size_t i = 0; i < size; i++)
    {
        qDebug() << "[" << i << "]" << this->readString();
        this->readString();

        this->readString();
        this->readString();

        this->read(8);
        this->read(8);
        this->read(8);
        this->read(2);
        this->read(1);
        this->read(1);
    }
}
