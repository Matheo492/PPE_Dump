#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include<QDebug>
#include<iostream>
#include "settingsdialog.h"
#include "console.h"
#include "selectexport.h"


namespace Ui {
class MainWindow;
}
class SettingsDialog;
class Console;
class selectexport;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_actionQuitter_triggered();
    void clearData();
    void plot();

    void on_Start_clicked();


    void on_Clear_clicked();
    void serialEvent();
    void writeData(const QByteArray &data);

    void on_actionPorts_triggered();

    void on_pushButton_clicked();


    void on_pushButton_2_clicked();

    void on_actionExportation_2_triggered();

private:
    Ui::MainWindow *ui;
    QVector<double> qv_x, qv_y, bv_y;
    SettingsDialog *m_settings = nullptr;
    QSerialPort arduino;
    selectexport *m_export = nullptr;


};

#endif // MAINWINDOW_H
