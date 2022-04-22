#pragma once
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

    bool isCrypto;

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
    T read()
    {
        T data = *(T *)this->readData(sizeof(T)).data();
        return qbswap(data);
    }

    QByteArray read(qint64 maxlen);
    QByteArray readAll();
    QString    readString();
    quint16    readId();

    // write
    template <typename T>
    qint64 write(T data)
    {
        data = qbswap(data);
        return this->writeData(QByteArray((char *)&data, sizeof(data)));
    }
    qint64 write(QByteArray data);
    qint64 write(const char *data, qint64 len);
    qint64 writeString(QString data);
    qint64 writeId(quint16 id);
};
