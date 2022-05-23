#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_AccountModel(new AccountModel(this))
{
    ui->setupUi(this);
    this->InitAccountView();
    this->readSettings();
}

MainWindow::~MainWindow()
{
    this->saveSettings();
    delete ui;
}

void MainWindow::InitAccountView()
{
    ui->AccountView->setModel(m_AccountModel);
    ui->AccountView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->AccountView->addActions(ui->menuAccount->actions());
    ui->AccountView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
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

void MainWindow::on_actionRender_triggered()
{
    QModelIndex currentIndex = ui->AccountView->currentIndex();

    if (currentIndex.isValid())
    {
        Account *account = m_AccountModel->m_Data.at(currentIndex.row());

        if (account->isRunning())
        {
            static QLabel *w = new QLabel();

            QImage image = account->m_ExileGame->Render();
            if (!image.isNull())
            {
                w->resize(image.size());
                w->setPixmap(QPixmap::fromImage(image));
                w->show();
            }
        }
    }
}

void MainWindow::on_actionImport_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    QFile   file(fileName);

    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        while (!file.atEnd())
        {
            QString     line = file.readLine();
            QStringList list = line.trimmed().split("----");

            Account *account = new Account;

            for (int i = 0; i < list.size(); i++)
            {
                switch (i)
                {
                case 0:
                    account->m_Email = list.at(i);
                    break;
                case 1:
                    account->m_EmailPassword = list.at(i);
                    break;
                case 2:
                    account->m_AccountName = list.at(i);
                    break;
                case 3:
                    account->m_Password = list.at(i);
                    break;
                }
            }

            m_AccountModel->m_Data.append(account);
        }
    }
}