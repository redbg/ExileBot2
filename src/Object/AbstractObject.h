#pragma once
#include "src/Helper.h"
#include <QObject>

class AbstractObject : public QObject
{
    Q_OBJECT

private:
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

            this->metaObject()->invokeMethod(this, name.toLatin1().data());
        }
    }

    template <typename T>
    T readData()
    {
        QByteArray data(sizeof(T), 0);
        m_DataStream->readRawData(data.data(), sizeof(T));
        return *(T *)data.data();
    }

    QByteArray readData(int size)
    {
        QByteArray data(size, 0);
        m_DataStream->readRawData(data.data(), size);
        return data;
    }
};
