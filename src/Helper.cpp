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

            for (int i = 0; i < obj->metaObject()->propertyCount(); i++)
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
            if (file.open(QFile::ReadOnly))
            {
                return file.readAll();
            }
            else
            {
                qWarning() << "Helper::File::ReadAll" << fileName;
                return QByteArray();
            }
        }

        qint64 Write(QString fileName, const QByteArray &data)
        {
            QFile file(fileName);
            if (file.open(QFile::WriteOnly))
            {
                return file.write(data);
            }
            else
            {
                qWarning() << "Helper::File::Write" << fileName;
                return 0;
            }
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

        QJsonObject GetBackendErrors(int _rid)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/BackendErrors.json");
            return JsonArray.at(_rid).toObject();
        }

        QJsonObject GetBaseItemTypes(int HASH)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/BaseItemTypes.json");

            for (int i = 0; i < JsonArray.size(); i++)
            {
                if (JsonArray.at(i).toObject().value("HASH").toInt() == HASH)
                {
                    return JsonArray.at(i).toObject();
                }
            }

            return QJsonObject();
        }

        QJsonObject GetMods(int HASH16)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/Mods.json");

            for (int i = 0; i < JsonArray.size(); i++)
            {
                if (JsonArray.at(i).toObject().value("HASH16").toInt() == HASH16)
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

        QJsonObject GetArmourTypes(int BaseItemTypesKey)
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

        QJsonObject GetWeaponTypes(int BaseItemTypesKey)
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

        QJsonObject GetGrantedEffectsPerLevel(int _rid)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/GrantedEffectsPerLevel.json");
            return JsonArray.at(_rid).toObject();
        }

        QJsonObject GetGrantedEffects(int _rid)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/GrantedEffects.json");
            return JsonArray.at(_rid).toObject();
        }

        QJsonObject GetActiveSkill(int _rid)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/ActiveSkills.json");
            return JsonArray.at(_rid).toObject();
        }

        QJsonObject GetBuffDefinitions(int _rid)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/BuffDefinitions.json");
            return JsonArray.at(_rid).toObject();
        }

        QJsonObject GetInventories(int _rid)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/Inventories.json");
            return JsonArray.at(_rid).toObject();
        }

        QJsonArray GetItemComponentNames(QString name)
        {
            static QJsonObject JsonArray = GetDataObject(":/Data/ItemCompenentNames.json");
            return JsonArray.value(name).toArray();
        }

    } // namespace Data

} // namespace Helper
