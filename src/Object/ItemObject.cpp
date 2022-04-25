#include "ItemObject.h"

ItemObject::ItemObject(QDataStream *dataStream, QObject *parent)
    : AbstractObject(dataStream, parent)
{
    int hash = readData<int>();
    readData<quint8>();

    m_BaseItemType = Helper::Data::GetBaseItemType(hash);

    this->ProcessDataStream(Helper::Data::GetItemComponentNames(m_BaseItemType.value("InheritsFrom").toString()));
}

ItemObject::~ItemObject() {}
