#include "Helper.h"

namespace Helper
{
    QString GetRandomString(int len)
    {
        const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        QString randomString;

        for (int i = 0; i < len; ++i)
        {
            qint64 randomInt = QRandomGenerator::global()->bounded(possibleCharacters.size());
            randomString.append(possibleCharacters.at(randomInt));
        }

        return randomString;
    }

    namespace Json
    {
        QJsonObject toJsonObject(QObject *obj)
        {
            QJsonObject JsonObject;

            for (int i = 1; i < obj->metaObject()->propertyCount(); i++)
            {
                const char *key = obj->metaObject()->property(i).name();
                JsonObject.insert(key, QJsonValue::fromVariant(obj->property(key)));
            }

            return JsonObject;
        }

        void fromJsonObject(QObject *obj, const QJsonObject &jsonObject)
        {
            for (int i = 0; i < obj->metaObject()->propertyCount(); i++)
            {
                const char *key = obj->metaObject()->property(i).name();
                obj->setProperty(key, jsonObject.value(key));
            }
        }

    } // namespace Json

    namespace File
    {
        QByteArray ReadAll(QString fileName)
        {
            QFile file(fileName);
            bool  isOpen = file.open(QFile::ReadOnly);
            // Q_ASSERT_X(isOpen, "Helper::File::ReadAll", fileName.toLatin1().data());
            return file.readAll();
        }

        qint64 Write(QString fileName, const QByteArray &data)
        {
            QFile file(fileName);
            bool  isOpen = file.open(QFile::WriteOnly);
            // Q_ASSERT_X(isOpen, "Helper::File::Write", fileName.toLatin1().data());
            return file.write(data);
        }

    } // namespace File

    namespace Data
    {
        QJsonArray GetDataArray(QString fileName)
        {
            QJsonDocument JsonDocument = QJsonDocument::fromJson(Helper::File::ReadAll(fileName));
            Q_ASSERT_X(JsonDocument.isArray(), "Helper::Data::GetDataArray", fileName.toLatin1().data());
            return JsonDocument.array();
        }

        QJsonObject GetBackendError(quint16 index)
        {
            static QJsonArray JsonArray = GetDataArray(":Data/BackendErrors.json");
            return JsonArray.at(index).toObject();
        }
    } // namespace Data

} // namespace Helper
