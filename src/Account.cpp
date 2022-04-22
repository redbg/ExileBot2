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
    // Init m_ExileClient
    m_ExileClient = new ExileClient(m_Email, m_Password);
    connect(this, &Account::finished, m_ExileClient, &ExileClient::deleteLater);
    connect(m_ExileClient, &ExileClient::signal_BackendError, this, &Account::on_BackendError, Qt::DirectConnection);

    // Init QJSEngine
    m_JSEngine = new QJSEngine;
    m_JSEngine->installExtensions(QJSEngine::AllExtensions);
    m_JSEngine->globalObject().setProperty("ExileClient", m_JSEngine->newQMetaObject(&ExileClient::staticMetaObject));
    m_JSEngine->globalObject().setProperty("Client", m_JSEngine->newQObject(m_ExileClient));
    m_JSEngine->evaluate(Helper::File::ReadAll("scripts/script.js"), "scripts/script.js");
    connect(this, &Account::finished, m_JSEngine, &QJSEngine::deleteLater);
    connect(m_ExileClient, &ExileClient::signal_LoginSuccess, this, [this]()
            { this->m_JSEngine->globalObject().property("OnClientLoginSuccess").call(); });
    connect(m_ExileClient, &ExileClient::signal_CharacterList, this, [this]()
            { this->m_JSEngine->globalObject().property("OnClientCharacterList").call(); });

    // Init m_Tick
    m_Tick = new QTimer;
    m_Tick->setInterval(100);
    connect(this, &Account::finished, m_Tick, &QTimer::deleteLater);
    connect(m_Tick, &QTimer::timeout, this, &Account::Tick, Qt::DirectConnection);
    m_Tick->start();

    this->exec();
}

void Account::Tick()
{
    m_JSEngine->globalObject().property("Tick").call();
}

void Account::on_BackendError(int result)
{
    quint16     BackendErrorIndex  = result - 1;
    QJsonObject BackendErrorObject = Helper::Data::GetBackendError(BackendErrorIndex);
    this->m_BackendError           = BackendErrorObject.value("Id").toString();

    qWarning() << "BackendError:" << m_BackendError;

    this->quit();
}
