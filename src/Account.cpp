#include "Account.h"

Account::Account(QObject *parent)
    : QThread(parent)
{
}

Account::~Account()
{
    this->quit();
    this->wait();
}

void Account::run()
{
    // ExileClient
    m_ExileClient = new ExileClient;
    connect(this, &Account::finished, m_ExileClient, &ExileClient::deleteLater);
    connect(m_ExileClient, &ExileClient::signal_BackendError, this, &Account::on_BackendError);

    m_ExileClient->connectToHost(m_Email, m_Password);
    this->exec();
}

void Account::on_BackendError(int result)
{
    quint16     BackendErrorIndex  = result - 1;
    QJsonObject BackendErrorObject = Helper::Data::GetBackendError(BackendErrorIndex);
    this->m_BackendError           = BackendErrorObject.value("Id").toString();

    qWarning() << "BackendError:" << m_BackendError;

    this->quit();
}
