#include "Account.h"

Account::Account()
    : m_ExileClient(nullptr)
    , m_ExileGame(nullptr)
    , m_JSEngine(nullptr)
    , m_Timer(nullptr)
{
    this->moveToThread(this);
}

Account::~Account()
{
    this->quit();
    this->wait();
}

void Account::run()
{
    m_BackendError.clear();

    m_JSEngine    = new QJSEngine;
    m_Timer       = new QTimer;
    m_ExileClient = new ExileClient(m_Email, m_Password);
    m_ExileGame   = new ExileGame(m_ExileClient);

    // Init m_JSEngine
    m_JSEngine->installExtensions(QJSEngine::AllExtensions);                                 // 安装所有扩展
    m_JSEngine->evaluate(Helper::File::ReadAll(m_ScriptPath), m_ScriptPath);                 // 执行脚本文件
    m_JSEngine->globalObject().setProperty("Client", m_JSEngine->newQObject(m_ExileClient)); // 注册 Client 类对象
    m_JSEngine->globalObject().setProperty("Game", m_JSEngine->newQObject(m_ExileGame));     // 注册  Game  类对象

    // Init m_Timer
    connect(m_Timer, &QTimer::timeout, this, &Account::Tick, Qt::DirectConnection);

    // Init m_ExileClient
    connect(m_ExileClient, &ExileClient::signal_BackendError, this, &Account::on_BackendError, Qt::DirectConnection);
    connect(m_ExileClient, &ExileClient::signal_LoginSuccess, [this]()
            {
                this->m_AccountName = m_ExileClient->m_AccountName;
                this->Invoke("OnClientLoginSuccess");
            });
    connect(m_ExileClient, &ExileClient::signal_CharacterList, [this]()
            {
                this->Invoke("OnClientCharacterList");
            });

    // EnterGame
    connect(m_ExileClient, &ExileClient::signal_EnterGame, m_ExileGame, &ExileGame::connectToHost, Qt::DirectConnection);

    // Init m_ExileGame
    connect(m_ExileGame, &ExileGame::signal_BackendError, this, &Account::on_BackendError, Qt::DirectConnection);

    // Start
    m_Timer->start(100);

    this->exec();

    m_Timer->stop();
    m_ExileClient->disconnectFromHost();
    m_ExileGame->disconnectFromHost();

    // delete 会有Bug,待修复
    // m_Tick->deleteLater();
    // m_ExileClient->deleteLater();
    // m_ExileGame->deleteLater();
    // m_JSEngine->deleteLater();
}

QJSValue Account::Invoke(const QString &name)
{
    QJSValue result = m_JSEngine->globalObject().property(name).call();

    if (result.isError())
    {
        qDebug() << result.toString();
    }

    return result;
}

QJSValue Account::Tick()
{
    if (m_ExileGame->state() == QTcpSocket::ConnectedState)
    {
        m_ExileGame->Tick();
    }

    return Invoke("Tick");
}

void Account::on_BackendError(int result)
{
    quint16     BackendErrorIndex  = result - 1;
    QJsonObject BackendErrorObject = Helper::Data::GetBackendErrors(BackendErrorIndex);
    this->m_BackendError           = BackendErrorObject.value("Id").toString();

    qWarning() << "BackendError:" << m_BackendError;

    this->quit();
}
