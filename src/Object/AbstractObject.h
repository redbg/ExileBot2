#pragma once
#include "src/Helper.h"
#include <QObject>
#include <QPoint>

class AbstractObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QJsonObject Components MEMBER m_Components)

protected:
    // Debug Info
    int Index;

protected:
    QByteArray   m_Data;
    QDataStream *m_DataStream;
    QJsonObject  m_Components;

public:
    explicit AbstractObject(QByteArray &data, QObject *parent = nullptr);
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
