#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Account *a    = new Account(this);
    a->m_Email    = "sj2uaw@gmailfsjs.com";
    a->m_Password = "As686366Dg.";
    a->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
