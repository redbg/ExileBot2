#pragma once
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaProperty>
#include <QObject>
#include <QRandomGenerator>

namespace Helper
{
    QJsonObject toJson(QObject *obj);          // QObject to QJsonObject
    QString     GetRandomString(int len = 10); // 获取随机字符串

    namespace File
    {
        QByteArray ReadAll(QString fileName);

    } // namespace File

    namespace Data
    {
        QJsonArray  GetDataArray(QString fileName);
        QJsonObject GetBackendError(quint16 index);

    } // namespace Data

} // namespace Helper
