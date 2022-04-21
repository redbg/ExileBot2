#pragma once
#include "ExileClient.h"
#include <QJSEngine>
#include <QThread>

class Account : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QString Email MEMBER m_Email)
    Q_PROPERTY(QString Password MEMBER m_Password)
    Q_PROPERTY(QString AccountName MEMBER m_AccountName)
    Q_PROPERTY(QString POESESSID MEMBER m_POESESSID)
    Q_PROPERTY(QString BackendError MEMBER m_BackendError)

public:
    QString m_Email;
    QString m_Password;
    QString m_AccountName;
    QString m_POESESSID;

    QString m_BackendError;

    QJSEngine   *m_JSEngine;
    ExileClient *m_ExileClient;

public:
    explicit Account(QObject *parent = nullptr);
    virtual ~Account();

protected:
    void run() override;

public:
    void on_BackendError(int result);
};
