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
    Q_PROPERTY(QString EmailPassword MEMBER m_EmailPassword)
    Q_PROPERTY(QString Password MEMBER m_Password)
    Q_PROPERTY(QString AccountName MEMBER m_AccountName)
    Q_PROPERTY(QString POESESSID MEMBER m_POESESSID)
    Q_PROPERTY(QString BackendError MEMBER m_BackendError)
    Q_PROPERTY(QString ScriptPath MEMBER m_ScriptPath)

public:
    QString m_Email;         // 邮箱
    QString m_EmailPassword; // 邮箱密码
    QString m_Password;      // 密码
    QString m_AccountName;   // 帐户名称
    QString m_POESESSID;     // POESESSID
    QString m_BackendError;  // 账号错误信息
    QString m_ScriptPath;    // 脚本路径

    QJSEngine *m_JSEngine;
    QTimer    *m_Timer;

    ExileClient *m_ExileClient;
    ExileGame   *m_ExileGame;

public:
    explicit Account();
    virtual ~Account();

protected:
    void run() override;

private slots:
    QJSValue Invoke(const QString &name);
    QJSValue Tick();
    void     on_BackendError(int result);
};
