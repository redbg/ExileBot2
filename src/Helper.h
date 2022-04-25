#pragma once
#include <QFile>
#include <QJSEngine>
#include <QJSValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaProperty>
#include <QObject>
#include <QRandomGenerator>

namespace Helper
{
    QString GetRandomString(int len = 10); // 获取随机字符串

    namespace Json
    {
        QJsonObject toJsonObject(QObject *obj);
        void        fromJsonObject(QObject *obj, const QJsonObject &jsonObject);
    } // namespace Json

    namespace File
    {
        QByteArray ReadAll(QString fileName);
        qint64     Write(QString fileName, const QByteArray &data);
    } // namespace File

    namespace Data
    {
        QJsonArray  GetDataArray(QString fileName);
        QJsonObject GetDataObject(QString fileName);
        QJsonObject GetBackendError(int index);
        QJsonObject GetBaseItemType(int hash);

        QJsonArray GetItemComponentNames(QString name);

    } // namespace Data

} // namespace Helper
