#include "AbstractObject.h"

AbstractObject::AbstractObject(QDataStream *dataStream, QObject *parent)
    : QObject(parent)
    , m_DataStream(dataStream)
{
}

AbstractObject::~AbstractObject() {}
