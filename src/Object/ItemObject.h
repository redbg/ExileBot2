#pragma once
#include "AbstractObject.h"
#include "src/Helper.h"

class ItemObject : public AbstractObject
{
    Q_OBJECT

private:
    QJsonObject m_BaseItemType;

public:
    explicit ItemObject(QDataStream *dataStream, QObject *parent = nullptr);
    virtual ~ItemObject();

    virtual QJsonArray GetComponentNames();
    virtual void       ProcessDataStream() {}
};
