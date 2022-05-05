#include "Account.h"

Account::Account(QObject *parent)
    : QThread(parent)
    , m_ExileClient(nullptr)
    , m_Tick(nullptr)
{
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
    m_Tick        = new QTimer;
    m_ExileClient = new ExileClient(m_Email, m_Password);
    m_ExileGame   = new ExileGame(m_ExileClient);

    connect(this, &Account::finished, m_JSEngine, &QJSEngine::deleteLater);
    connect(this, &Account::finished, m_Tick, &QTimer::deleteLater);
    connect(this, &Account::finished, m_ExileClient, &ExileClient::deleteLater);
    connect(this, &Account::finished, m_ExileGame, &ExileGame::deleteLater);

    // Init QJSEngine
    m_JSEngine->installExtensions(QJSEngine::AllExtensions);
    m_JSEngine->evaluate(Helper::File::ReadAll(m_JSPath), m_JSPath);
    m_JSEngine->globalObject().setProperty("Client", m_JSEngine->newQObject(m_ExileClient));
    m_JSEngine->globalObject().setProperty("Game", m_JSEngine->newQObject(m_ExileGame));

    // Init m_Tick
    connect(m_Tick, &QTimer::timeout, this, &Account::Tick, Qt::DirectConnection);

    // Init m_ExileClient
    connect(m_ExileClient, &ExileClient::signal_BackendError, this, &Account::on_BackendError, Qt::DirectConnection);
    connect(m_ExileClient, &ExileClient::signal_LoginSuccess, [this]()
            {
                this->m_AccountName = m_ExileClient->m_AccountName;
                this->CallFunction("OnClientLoginSuccess");
            });
    connect(m_ExileClient, &ExileClient::signal_CharacterList, [this]()
            {
                this->CallFunction("OnClientCharacterList");
            });

    // EnterGame
    connect(m_ExileClient, &ExileClient::signal_EnterGame, m_ExileGame, &ExileGame::connectToHost, Qt::DirectConnection);

    // Init m_ExileGame
    connect(m_ExileGame, &ExileGame::signal_BackendError, this, &Account::on_BackendError, Qt::DirectConnection);

    // Start
    m_Tick->start(100);
    this->exec();
}

QJSValue Account::CallFunction(const QString &name)
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

    return CallFunction("Tick");
}

void Account::on_BackendError(int result)
{
    quint16     BackendErrorIndex  = result - 1;
    QJsonObject BackendErrorObject = Helper::Data::GetBackendError(BackendErrorIndex);
    this->m_BackendError           = BackendErrorObject.value("Id").toString();

    qWarning() << "BackendError:" << m_BackendError;

    this->quit();
}
