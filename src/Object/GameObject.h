#pragma once
#include "AbstractObject.h"
#include "src/Helper.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class GameObject : public AbstractObject
{
    Q_OBJECT

private:
    quint32 m_Hash;
    QString m_MetadataId;

public:
    explicit GameObject(quint32 hash, QDataStream *dataStream, QObject *parent = nullptr);
    virtual ~GameObject();
};
