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

        QJsonObject GetDataObject(QString fileName)
        {
            QJsonDocument JsonDocument = QJsonDocument::fromJson(Helper::File::ReadAll(fileName));
            Q_ASSERT_X(JsonDocument.isObject(), "Helper::Data::GetDataObject", fileName.toLatin1().data());
            return JsonDocument.object();
        }

        QJsonObject GetBackendError(int index)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/BackendErrors.json");
            return JsonArray.at(index).toObject();
        }

        QJsonObject GetBaseItemType(int hash)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/BaseItemTypes.json");

            for (int i = 0; i < JsonArray.size(); i++)
            {
                if (JsonArray.at(i).toObject().value("HASH").toInt() == hash)
                {
                    return JsonArray.at(i).toObject();
                }
            }

            return QJsonObject();
        }

        QJsonObject GetMods(int hash16)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/Mods.json");

            for (int i = 0; i < JsonArray.size(); i++)
            {
                if (JsonArray.at(i).toObject().value("HASH16").toInt() == hash16)
                {
                    return JsonArray.at(i).toObject();
                }
            }

            return QJsonObject();
        }

        QJsonObject GetStats(int _rid)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/Stats.json");

            return JsonArray.at(_rid).toObject();
        }

        QJsonArray GetItemComponentNames(QString name)
        {
            static QJsonObject JsonArray = GetDataObject(":/Data/ItemCompenentNames.json");

            return JsonArray.value(name).toArray();
        }

        QJsonObject GetArmourType(quint64 BaseItemTypesKey)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/ArmourTypes.json");
            for (int i = 0; i < JsonArray.size(); i++)
            {
                if (JsonArray.at(i).toObject().value("BaseItemTypesKey").toInt() == BaseItemTypesKey)
                {
                    return JsonArray.at(i).toObject();
                }
            }
            return QJsonObject();
        }

        QJsonObject GetWeaponType(quint64 BaseItemTypesKey)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/WeaponTypes.json");
            for (int i = 0; i < JsonArray.size(); i++)
            {
                if (JsonArray.at(i).toObject().value("BaseItemTypesKey").toInt() == BaseItemTypesKey)
                {
                    return JsonArray.at(i).toObject();
                }
            }
            return QJsonObject();
        }

        QJsonObject GetBuffDefinitions(quint16 id)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/BuffDefinitions.json");
            return JsonArray.at(id).toObject();
        }

        QJsonObject GetComponentAttributeRequirements(QString BaseItemTypesKey)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/ComponentAttributeRequirements.json");
            for (int i = 0; i < JsonArray.size(); i++)
            {
                if (JsonArray.at(i).toObject().value("BaseItemTypesKey").toString() == BaseItemTypesKey)
                {
                    return JsonArray.at(i).toObject();
                }
            }
            return QJsonObject();
        }

    } // namespace Data

} // namespace Helper
