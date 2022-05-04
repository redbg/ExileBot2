#include "AbstractObject.h"

AbstractObject::AbstractObject(QByteArray &data, QObject *parent)
    : QObject(parent)
    , m_Data(data)
    , m_DataStream(new QDataStream(data))
    , Index(0)
{
    Q_ASSERT_X(!data.isEmpty(), "AbstractObject()", "data.isEmpty()");
}

AbstractObject::AbstractObject(QDataStream *dataStream, QObject *parent)
    : QObject(parent)
    , m_DataStream(dataStream)
    , Index(0)
{
}

AbstractObject::~AbstractObject()
{
    if (!m_Data.isEmpty())
    {
        delete m_DataStream;
    }
}

// 处理数据流
void AbstractObject::ProcessDataStream(QJsonArray componentNames)
{
    for (int i = 0; i < componentNames.size(); i++)
    {
        QString name = componentNames.at(i).toString();
        qDebug() << "----" << name << "[" << Index << "]";

        if (this->metaObject()->invokeMethod(this, name.toLatin1().data()) == false)
        {
            break;
        }
    }

    qDebug() << m_Components;

    if (!m_Data.isEmpty())
    {
        if (Index != m_Data.size())
        {
            qWarning() << "数据未解析完,剩余字节数:" << m_Data.size() - Index << this->objectName();
        }
    }
}

QByteArray AbstractObject::readData(int size)
{
    QByteArray data(size, 0);
    int        readSize = m_DataStream->readRawData(data.data(), size);

    if (readSize != size)
    {
        qWarning() << "数据不够" << this->objectName();
        qWarning() << m_Components;
    }

    // Q_ASSERT_X(readSize == size, "AbstractObject::readData(int size)",
    //            QString("数据不够 %1").arg(this->objectName()).toLatin1().data());

    Index += readSize;

    qDebug() << "--------" << data.toHex(' ');
    return data;
}

quint32 AbstractObject::ReadVarint()
{
    quint8 v = readData<quint8>();

    if ((v & 0x80) == 0)
    {
        return v;
    }

    if ((v & 0xC0) == 0x80)
    {
        return readData<quint8>() | (v & 0x3f) << 8;
    }
    else if ((v & 0xE0) == 0xC0)
    {
        quint8 v1 = readData<quint8>();
        quint8 v2 = readData<quint8>();
        return v2 | v1 << 8 | (v & 0x1f) << (8 * 2);
    }
    else if ((v & 0xF0) == 0xE0)
    {
        quint8 v1 = readData<quint8>();
        quint8 v2 = readData<quint8>();
        quint8 v3 = readData<quint8>();

        return v3 | v2 << 8 | v1 << (8 * 2) | (v & 0xf) << (8 * 3);
    }
    else
    {
        quint8 v1 = readData<quint8>();
        quint8 v2 = readData<quint8>();
        quint8 v3 = readData<quint8>();
        quint8 v4 = readData<quint8>();
        return v4 | v3 << 8 | v2 << (8 * 2) | v1 << (8 * 3);
    }
}

qint32 AbstractObject::ReadVarint1()
{
    quint8 v = readData<quint8>();

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
        return readData<quint8>() | (v & 0x3f) << 8;
    }
    else if ((v & 0xE0) == 0xC0)
    {
        quint8 v1 = readData<quint8>();
        quint8 v2 = readData<quint8>();
        return v2 | v1 << 8 | (v & 0x1f) << (8 * 2);
    }
    else if ((v & 0xF0) == 0xE0)
    {
        quint8 v1 = readData<quint8>();
        quint8 v2 = readData<quint8>();
        quint8 v3 = readData<quint8>();

        qint64 result = v3 | v2 << 8 | v1 << (8 * 2) | (v & 0xf) << (8 * 3);
        result        = v & 8 ? result : result | 0xF0000000;
        return result;
    }
    else
    {
        quint8 v1 = readData<quint8>();
        quint8 v2 = readData<quint8>();
        quint8 v3 = readData<quint8>();
        quint8 v4 = readData<quint8>();
        return v4 | v3 << 8 | v2 << (8 * 2) | v1 << (8 * 3);
    }
}