#include "Helper.h"

namespace Helper
{
    /**
     * @brief QObject to QJsonObject
     *
     * @param obj
     * @return QJsonObject
     */
    QJsonObject toJson(QObject *obj)
    {
        QJsonObject JsonObject;

        for (int i = 1; i < obj->metaObject()->propertyCount(); i++)
        {
            const char *name = obj->metaObject()->property(i).name();
            JsonObject.insert(name, QJsonValue::fromVariant(obj->property(name)));
        }

        return JsonObject;
    }

    /**
     * @brief Get the Random String object
     *
     * @param len
     * @return QString
     */
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
    namespace File
    {
        QByteArray ReadAll(QString fileName)
        {
            QFile file(fileName);
            bool  isOpen = file.open(QFile::ReadOnly);
            Q_ASSERT_X(isOpen, "Helper::File::ReadAll", fileName.toLatin1().data());
            return file.readAll();
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
