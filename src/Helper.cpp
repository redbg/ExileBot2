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
                const char *name = obj->metaObject()->property(i).name();
                JsonObject.insert(name, QJsonValue::fromVariant(obj->property(name)));
            }

            return JsonObject;
        }

        QJSValue toJSValue(QJSEngine *engine, const QJsonValue &val)
        {
            switch (val.type())
            {
            case QJsonValue::Null:
                return QJSValue(QJSValue::NullValue);
            case QJsonValue::Bool:
                return QJSValue(val.toBool());
            case QJsonValue::Double:
                return QJSValue(val.toDouble());
            case QJsonValue::String:
                return QJSValue(val.toString());
            case QJsonValue::Array:
            {
                QJsonArray arr    = val.toArray();
                QJSValue   newArr = engine->newArray(arr.size());
                for (int i = 0; i < arr.size(); i++)
                {
                    newArr.setProperty(i, toJSValue(engine, arr[i]));
                }
                return newArr;
            }
            case QJsonValue::Object:
            {
                QJsonObject obj    = val.toObject();
                QJSValue    newObj = engine->newObject();
                for (auto itor = obj.begin(); itor != obj.end(); ++itor)
                {
                    newObj.setProperty(itor.key(), toJSValue(engine, itor.value()));
                }
                return newObj;
            }
            case QJsonValue::Undefined:
                return QJSValue(QJSValue::UndefinedValue);
            default:
                return QJSValue(QJSValue::UndefinedValue);
            }
        }
    } // namespace Json

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
