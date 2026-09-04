#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "uartcontroller.h"
#include <QThread>
#include <QMetaObject>
#include <QSerialPort>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    uartController = new UartController();
    thread = new QThread();
    connect(thread, &QThread::started, uartController, &UartController::SlotInit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    uartController->moveToThread(thread);
    uartController->port->moveToThread(thread);
    connect(uartController, &UartController::RefreshTemperature, this, [this](float temp) {
            ui->txtTemperature->setText(QString::number(temp, 'f', 1));
        });
    connect(ui->btnOn, &QPushButton::clicked, this, [this]() {
        QString value = QString::number(ui->dsbSetpoint->value(), 'f', 1);
        QMetaObject::invokeMethod(
            uartController,
            "SlotEnable",
            Qt::QueuedConnection,
            Q_ARG(QString, value)
        );
    });
    connect(ui->btnOff, &QPushButton::clicked, uartController, &UartController::SlotDisable, Qt::QueuedConnection);
    thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

