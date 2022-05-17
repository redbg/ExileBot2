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
        QJsonObject GetMods(int hash16);
        QJsonObject GetStats(int _rid);
        QJsonObject GetArmourType(quint64 BaseItemTypesKey);
        QJsonObject GetWeaponType(quint64 BaseItemTypesKey);
        QJsonObject GetComponentAttributeRequirements(QString BaseItemTypesKey);
        QJsonObject GetGrantedEffectsPerLevel(quint16 _rid);
        QJsonObject GetGrantedEffects(quint16 _rid);
        QJsonObject GetActiveSkill(quint16 _rid);
        QJsonArray  GetItemComponentNames(QString name);
        QJsonObject GetBuffDefinitions(quint16 _rid);
        QJsonObject GetInventories(quint16 _rid);

    } // namespace Data

} // namespace Helper
