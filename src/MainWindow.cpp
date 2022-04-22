#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_AccountModel(new AccountModel(this))
{
    ui->setupUi(this);

    // Init Account
    ui->AccountView->setModel(m_AccountModel);
    ui->AccountView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->AccountView->addActions(ui->menuAccount->actions());
    ui->AccountView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    this->readSettings();
}

MainWindow::~MainWindow()
{
    this->saveSettings();
    delete ui;
}

void MainWindow::saveSettings()
{
    // save
    QSettings settings;
    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("windowState", this->saveState());

    // AccountList.json
    QByteArray AccountList = QJsonDocument(m_AccountModel->toJsonArray()).toJson();
    Helper::File::Write("AccountList.json", AccountList);
}

void MainWindow::readSettings()
{
    // read
    QSettings settings;
    this->restoreGeometry(settings.value("geometry").toByteArray());
    this->restoreState(settings.value("windowState").toByteArray());

    // AccountList.json
    QByteArray AccountList = Helper::File::ReadAll("AccountList.json");
    m_AccountModel->fromJsonArray(QJsonDocument::fromJson(AccountList).array());
}

void MainWindow::on_actionStart_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        m_AccountModel->m_Data.at(currentIndex.row())->start();
    }
}

void MainWindow::on_actionQuit_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        m_AccountModel->m_Data.at(currentIndex.row())->quit();
    }
}

void MainWindow::on_actionInsert_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        m_AccountModel->insertRow(currentIndex.row());
    }
    else
    {
        m_AccountModel->insertRow(0);
    }
}

void MainWindow::on_actionRemove_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        m_AccountModel->removeRow(currentIndex.row());
    }
}
