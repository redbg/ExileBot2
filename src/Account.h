#pragma once
#include "ExileClient.h"
#include "ExileGame.h"
#include <QJSEngine>
#include <QThread>
#include <QTimer>

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

    ExileClient *m_ExileClient;
    ExileGame   *m_ExileGame;

    QJSEngine *m_JSEngine;
    QTimer    *m_Tick;

public:
    explicit Account(QObject *parent = nullptr);
    virtual ~Account();

protected:
    void run() override;

private slots:
    QJSValue CallFunction(const QString &name);
    QJSValue Tick();
    void     on_BackendError(int result);
};
