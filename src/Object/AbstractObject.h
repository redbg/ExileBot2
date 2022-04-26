#pragma once
#include "src/Helper.h"
#include <QObject>

class AbstractObject : public QObject
{
    Q_OBJECT

protected:
    QDataStream *m_DataStream;

public:
    explicit AbstractObject(QDataStream *dataStream, QObject *parent = nullptr);
    virtual ~AbstractObject();

    // 处理数据流
    virtual void ProcessDataStream(QJsonArray componentNames)
    {
        for (int i = 0; i < componentNames.size(); i++)
        {
            QString name = componentNames.at(i).toString();
            qDebug() << name;

            this->metaObject()->invokeMethod(this, name.toLatin1().data());
        }
    }

    template <typename T>
    T readData()
    {
        QByteArray data = this->readData(sizeof(T));
        return *(T *)data.data();
    }

    QByteArray readData(int size)
    {
        QByteArray data(size, 0);
        m_DataStream->readRawData(data.data(), size);
        qDebug() << "    " << data.toHex(' ');
        return data;
    }

    quint32 ReadVarint()
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

    qint32 ReadVarint1()
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
};
