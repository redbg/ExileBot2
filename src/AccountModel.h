#pragma once
#include "Account.h"
#include <QAbstractTableModel>
#include <QBrush>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QMetaProperty>

class AccountModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    QList<Account *> m_Data;

public:
    explicit AccountModel(QObject *parent = nullptr);
    virtual ~AccountModel();

public:
    virtual int           rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int           columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant      data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

public:
    QJsonArray toJsonArray();
    void       fromJsonArray(QJsonArray JsonArray);
};
