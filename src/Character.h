#pragma once
#include "Helper.h"
#include <QJsonObject>
#include <QMetaProperty>
#include <QObject>

class Character : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_Name)
    Q_PROPERTY(QString league MEMBER m_League)
    Q_PROPERTY(int level MEMBER m_Level)
    Q_PROPERTY(quint32 experience MEMBER m_Experience)
    Q_PROPERTY(int classId MEMBER m_ClassId)

public:
    QString m_Name;       // 名字
    QString m_League;     // 赛区名
    int     m_LeagueId;   // 赛区Id
    int     m_Unknown1;   // ??
    int     m_Level;      // 等级
    quint32 m_Experience; // 经验
    int     m_ClassId;    // 职业Id
    quint16 m_Unknown2;   // ??
    int     m_Unknown3;   // ??
    int     m_Unknown4;   // ??

public:
    enum ClassType
    {
        StrDexInt, // 貴族
        Str,       // 野蠻人    特性：力量，近戰
        Dex,       // 遊俠      特性：敏捷，遠距離
        Int,       // 女巫      特性：智慧，法術
        StrDex,    // 決鬥者
        StrInt,    // 聖堂武僧
        DexInt,    // 暗影刺客
    };
    Q_ENUM(ClassType)

public:
    explicit Character(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    virtual ~Character() {}

public:
    static QString GetClassTypeById(int classId)
    {
        QMetaEnum MetaEnum = QMetaEnum::fromType<Character::ClassType>();
        return MetaEnum.valueToKey(classId);
    }
};
