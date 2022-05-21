#include "ExileSocket.h"

ExileSocket::ExileSocket(QObject *parent)
    : QTcpSocket(parent)
    , isCrypto(false)
{
    connect(this, &QTcpSocket::disconnected, this, [this]()
            {
                this->DisableCrypto();
            });
}

ExileSocket::~ExileSocket() {}

void ExileSocket::SetKeyWithIV(QByteArray hash)
{
    DisableCrypto();

    Q_ASSERT(hash.size() == 0x40);

    this->m_Encryption.SetKeyWithIV((CryptoPP::byte *)hash.data(), 0x20,
                                    (CryptoPP::byte *)&hash.data()[0x20], 8);

    this->m_Decryption.SetKeyWithIV((CryptoPP::byte *)hash.data(), 0x20,
                                    (CryptoPP::byte *)&hash.data()[0x30], 8);

    // ==================================================

    QByteArray v4(hash);
    char       v6;
    char       v7;
    char       v8;
    char       v9;
    char       v10;
    char       v11;

    for (int i = 0; i < 0x40; i++)
    {
        v4[i] ^= Global::HeartbeatKey[i];
    }

    v6       = v4[0];
    v4[0]    = v4[0x15];
    v4[0x15] = v6;
    v7       = v4[4];
    v4[4]    = v4[2];
    v4[2]    = v7;
    v8       = v4[0xD];
    v4[0xD]  = v4[0x2A];
    v4[0x2A] = v8;
    v9       = v4[0x30];
    v4[0x30] = v4[1];
    v4[1]    = v9;
    v10      = v4[0x1D];
    v4[0x1D] = v4[0x1E];
    v4[0x1E] = v10;
    v11      = v4[0x32];
    v4[0x32] = v4[0x13];
    v4[0x13] = v11;

    this->m_Encryption2.SetKeyWithIV((CryptoPP::byte *)v4.data(), 0x20,
                                     (CryptoPP::byte *)&v4.data()[0x20], 8);

    this->m_Decryption2.SetKeyWithIV((CryptoPP::byte *)v4.data(), 0x20,
                                     (CryptoPP::byte *)&v4.data()[0x30], 8);
}

QByteArray ExileSocket::readData(qint64 maxlen)
{
    QByteArray data;

    // 检查大小
    if (maxlen == 0)
    {
        return data;
    }

    Q_ASSERT_X(!(maxlen > 0x10000), "ExileSocket::readData", "maxlen > 0x10000");

    // 读取数据
    do
    {
        data.append(QTcpSocket::read(maxlen - data.size()));
    } while (data.size() != maxlen && this->waitForReadyRead());

    if (this->isCrypto == true)
    {
        // 解密数据
        this->m_Decryption.ProcessData((CryptoPP::byte *)data.data(),
                                       (CryptoPP::byte *)data.data(),
                                       data.size());
    }

    return data;
}

qint64 ExileSocket::writeData(QByteArray data)
{
    // 检查大小
    if (data.size() == 0)
    {
        return 0;
    }

    if (this->isCrypto == true)
    {
        // 加密数据
        this->m_Encryption.ProcessData((CryptoPP::byte *)data.data(),
                                       (CryptoPP::byte *)data.data(),
                                       data.size());
    }

    // 写入数据
    return QTcpSocket::write(data);
}

// read
QByteArray ExileSocket::read(qint64 maxlen)
{
    return this->readData(maxlen);
}

QByteArray ExileSocket::readAll()
{
    return this->readData(this->size());
}

QString ExileSocket::readString()
{
    quint16 size = this->read<quint16>();
    QString data;
    data.reserve(size); // 提前申请内存,优化性能

    for (int i = 0; i < size; i++)
    {
        unsigned char c = this->read<unsigned char>();

        if (c >= 0x80)
        {
            if (c == 0x80)
            {
                data.append(QChar(this->read<char16_t>(false)));
            }
            else
            {
                Q_ASSERT_X(false, "ExileSocket::readString()", "读字符串意外情况");
            }
        }
        else
        {
            data.append(QChar(c));
        }
    }

    return data;
}

// 读取无符号
quint32 ExileSocket::ReadVaruint()
{
    quint8 v = this->read<quint8>();

    if ((v & 0x80) == 0)
    {
        return v;
    }

    if ((v & 0xC0) == 0x80)
    {
        return this->read<quint8>() | (v & 0x3f) << 8;
    }
    else if ((v & 0xE0) == 0xC0)
    {
        quint8 v1 = this->read<quint8>();
        quint8 v2 = this->read<quint8>();
        return v2 | v1 << 8 | (v & 0x1f) << (8 * 2);
    }
    else if ((v & 0xF0) == 0xE0)
    {
        quint8 v1 = this->read<quint8>();
        quint8 v2 = this->read<quint8>();
        quint8 v3 = this->read<quint8>();

        return v3 | v2 << 8 | v1 << (8 * 2) | (v & 0xf) << (8 * 3);
    }
    else
    {
        quint8 v1 = this->read<quint8>();
        quint8 v2 = this->read<quint8>();
        quint8 v3 = this->read<quint8>();
        quint8 v4 = this->read<quint8>();
        return v4 | v3 << 8 | v2 << (8 * 2) | v1 << (8 * 3);
    }
}

// 读取有符号
qint32 ExileSocket::ReadVarint()
{
    quint8 v = this->read<quint8>();

    if ((v & 0x80) == 0)
    {
        if ((v & 0x40) == 0)
        {
            return v;
        }
        else
        {
            return v | 0xffffff80;
        }
    }

    if ((v & 0xC0) == 0x80)
    {
        return this->read<quint8>() | (v & 0x3f) << 8;
    }
    else if ((v & 0xE0) == 0xC0)
    {
        quint8 v1 = this->read<quint8>();
        quint8 v2 = this->read<quint8>();
        return v2 | v1 << 8 | (v & 0x1f) << (8 * 2);
    }
    else if ((v & 0xF0) == 0xE0)
    {
        quint8 v1 = this->read<quint8>();
        quint8 v2 = this->read<quint8>();
        quint8 v3 = this->read<quint8>();

        qint64 result = v3 | v2 << 8 | v1 << (8 * 2) | (v & 0xf) << (8 * 3);
        result        = v & 8 ? result : result | 0xF0000000;
        return result;
    }
    else
    {
        quint8 v1 = this->read<quint8>();
        quint8 v2 = this->read<quint8>();
        quint8 v3 = this->read<quint8>();
        quint8 v4 = this->read<quint8>();
        return v4 | v3 << 8 | v2 << (8 * 2) | v1 << (8 * 3);
    }
}

// write
qint64 ExileSocket::write(QByteArray data)
{
    return this->writeData(data);
}

qint64 ExileSocket::write(const char *data, qint64 len)
{
    return this->writeData(QByteArray(data, len));
}

qint64 ExileSocket::writeString(QString data)
{
    qint64 size = this->write<quint16>(data.size());
    size += this->writeData(data.toLatin1());

    return size;
}
