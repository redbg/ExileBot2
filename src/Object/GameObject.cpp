#include "GameObject.h"

GameObject::GameObject(quint32 hash, QDataStream *dataStream, QObject *parent)
    : m_Hash(hash)
    , AbstractObject(dataStream, parent)
{
    QNetworkAccessManager *mgr = new QNetworkAccessManager;
    QNetworkRequest        req(QUrl(QString("http://127.0.0.1:6112/ot?hash=%1").arg(hash)));
    mgr->get(req);

    connect(mgr, &QNetworkAccessManager::finished, [=](QNetworkReply *reply)
            {
                qDebug() << "==================================================";

                QJsonObject JsonObject = QJsonDocument::fromJson(reply->readAll()).object();

                this->m_MetadataId = JsonObject.begin().key();

                qDebug() << this->m_MetadataId;

                this->ProcessDataStream(JsonObject.begin().value().toArray());

                reply->deleteLater();
                mgr->deleteLater();
            });
}

GameObject::~GameObject() {}
