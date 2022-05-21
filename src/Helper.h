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
    QString GetRandomString(int len); // 获取随机字符串

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
        QJsonObject GetBackendErrors(int _rid);
        QJsonObject GetBaseItemTypes(int HASH);
        QJsonObject GetMods(int HASH16);
        QJsonObject GetStats(int _rid);
        QJsonObject GetArmourTypes(int BaseItemTypesKey);
        QJsonObject GetWeaponTypes(int BaseItemTypesKey);
        QJsonObject GetComponentAttributeRequirements(QString BaseItemTypesKey);
        QJsonObject GetGrantedEffectsPerLevel(int _rid);
        QJsonObject GetGrantedEffects(int _rid);
        QJsonObject GetActiveSkill(int _rid);
        QJsonObject GetBuffDefinitions(int _rid);
        QJsonObject GetInventories(int _rid);
        QJsonArray  GetItemComponentNames(QString name);
    } // namespace Data

} // namespace Helper
