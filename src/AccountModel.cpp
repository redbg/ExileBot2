#include "AccountModel.h"

AccountModel::AccountModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

AccountModel::~AccountModel() {}

int AccountModel::rowCount(const QModelIndex &parent) const
{
    return m_Data.size();
}

int AccountModel::columnCount(const QModelIndex &parent) const
{
    return Account::staticMetaObject.propertyCount();
}

QVariant AccountModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    {
        const char *name = Account::staticMetaObject.property(index.column()).name();
        return m_Data.at(index.row())->property(name);
    }
    case Qt::ForegroundRole:
    {
        return m_Data.at(index.row())->isRunning() ? QBrush(Qt::darkGreen) : QVariant();
    }
    default:
        return QVariant();
    }
}

QVariant AccountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return Account::staticMetaObject.property(section).name();
    }
    return QVariant();
}

Qt::ItemFlags AccountModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool AccountModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case Qt::EditRole:
    {
        const char *name = Account::staticMetaObject.property(index.column()).name();
        m_Data.at(index.row())->setProperty(name, value);
        return true;
    }
    default:
        return false;
    }
}

bool AccountModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);

    for (int i = 0; i < count; i++)
    {
        m_Data.insert(row, new Account(this));
    }

    endInsertRows();
    return true;
}

bool AccountModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);

    qDeleteAll(m_Data.begin() + row, m_Data.begin() + row + count); // delete
    m_Data.remove(row, count);                                      // remove

    endRemoveRows();
    return true;
}

QJsonArray AccountModel::toJsonArray()
{
    QJsonArray JsonArray;

    for (int i = 0; i < m_Data.size(); i++)
    {
        JsonArray.append(Helper::Json::toJsonObject(m_Data.at(i)));
    }

    return JsonArray;
}

void AccountModel::fromJsonArray(QJsonArray JsonArray)
{
    for (int i = 0; i < JsonArray.size(); i++)
    {
        Account *newAccount = new Account(this);
        Helper::Json::fromJsonObject(newAccount, JsonArray.at(i).toObject());
        m_Data.append(newAccount);
    }
}