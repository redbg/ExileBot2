#pragma once
#include "src/Helper.h"
#include <QObject>

class AbstractObject : public QObject
{
    Q_OBJECT

protected:
    QDataStream *m_DataStream;
    QJsonObject  m_Components;

public:
    explicit AbstractObject(QDataStream *dataStream, QObject *parent = nullptr);
    virtual ~AbstractObject();

    // 处理数据流
    virtual void ProcessDataStream(QJsonArray componentNames);

    template <typename T>
    T readData()
    {
        QByteArray data = this->readData(sizeof(T));
        return *(T *)data.data();
    }

    QByteArray readData(int size);
    quint32    ReadVarint();
    qint32     ReadVarint1();
};
