#pragma once
#include <QObject>

class AbstractObject : public QObject
{
    Q_OBJECT

private:
    QDataStream *m_DataStream;

public:
    explicit AbstractObject(QDataStream *dataStream, QObject *parent = nullptr);
    virtual ~AbstractObject();

    virtual QJsonArray GetComponentNames() = 0; // 获取组件名称顺序数组
    virtual void       ProcessDataStream() = 0; // 处理数据流

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
