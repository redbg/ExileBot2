#pragma once
#include "Global.h"
#include <QTcpSocket>
#include <QtEndian>
#include <cryptopp/salsa.h>

class ExileSocket : public QTcpSocket
{
    Q_OBJECT
    Q_PROPERTY(SocketState SocketState READ state)

private:
    CryptoPP::Salsa20::Encryption m_Encryption;
    CryptoPP::Salsa20::Decryption m_Decryption;

public:
    bool isCrypto;

    // 心跳
    CryptoPP::Salsa20::Encryption m_Encryption2;
    CryptoPP::Salsa20::Decryption m_Decryption2;

public:
    explicit ExileSocket(QObject *parent = nullptr);
    virtual ~ExileSocket();

public:
    void SetKeyWithIV(QByteArray hash);
    void EnableCrypto() { isCrypto = true; }
    void DisableCrypto() { isCrypto = false; }

protected:
    QByteArray readData(qint64 maxlen);
    qint64     writeData(QByteArray data);

public:
    // read
    template <typename T>
    T read(bool swap = true)
    {
        T data = *(T *)this->readData(sizeof(T)).data();
        return swap ? qbswap(data) : data;
    }

    quint16    readId() { return this->read<quint16>(); };
    QByteArray read(qint64 maxlen);
    QByteArray readAll();
    QString    readString();
    quint32    ReadVaruint();
    qint32     ReadVarint();

    // write
    template <typename T>
    qint64 write(T data)
    {
        data = qbswap(data);
        return this->writeData(QByteArray((char *)&data, sizeof(data)));
    }

    qint64 writeId(quint16 id) { return this->write<quint16>(id); };
    qint64 write(QByteArray data);
    qint64 write(const char *data, qint64 len);
    qint64 writeString(QString data);
};
