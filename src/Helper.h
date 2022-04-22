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

        template <typename T>
        static QJsonArray toJsonArray(const QList<T *> &list)
        {
            QJsonArray JsonArray;

            for (int i = 0; i < list.size(); i++)
            {
                JsonArray.append(list.at(i)->toJsonObject());
            }

            return JsonArray;
        }

        QJSValue toJSValue(QJSEngine *engine, const QJsonValue &val);
    } // namespace Json

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
