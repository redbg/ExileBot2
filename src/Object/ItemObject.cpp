#include "ItemObject.h"

ItemObject::ItemObject(QDataStream *dataStream, QObject *parent)
    : AbstractObject(dataStream, parent)
{
    int hash = readData<int>();
    readData<quint8>();

    m_BaseItemType = Helper::Data::GetBaseItemType(hash);

    qDebug() << this->GetComponentNames();
}

ItemObject::~ItemObject() {}

QJsonArray ItemObject::GetComponentNames()
{
    return Helper::Data::GetItemComponentNames(m_BaseItemType.value("InheritsFrom").toString());
}