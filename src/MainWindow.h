#pragma once
#include "Account.h"
#include "AccountModel.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QSettings>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    AccountModel   *m_AccountModel;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

public:
    void InitAccountView();

    void saveSettings();
    void readSettings();

private slots:
    void on_actionStart_triggered();
    void on_actionQuit_triggered();
    void on_actionInsert_triggered();
    void on_actionRemove_triggered();
    void on_actionRender_triggered();
    void on_actionImport_triggered();
};
