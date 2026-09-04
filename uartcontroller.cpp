#include <QDebug>
#include <QtSerialPort>
#include "uartcontroller.h"

UartController::UartController(QObject *parent) : QObject{parent}, m_portName("COM4")
{
    port = new QSerialPort();

    connect(port, &QSerialPort::readyRead, this, &UartController::SlotRead);
    connect(port, &QSerialPort::bytesWritten, [](qint64 bytes)
    {
        qDebug() << "Отправлено байт:" << bytes;
    });
    connect(port, &QSerialPort::errorOccurred, [this](QSerialPort::SerialPortError error)
    {
        if (error == QSerialPort::ResourceError)
        {
            qDebug() << "Ошибка порта:" << port->errorString();
            port->close();
        }
    });
}

UartController::~UartController()
{
    port->close();
    delete port;
}

void UartController::SlotRead()
{
    bool ok = false;
    float temperature = port->readAll().toFloat(&ok);
    if (ok)
    {
        emit RefreshTemperature(temperature);
    }
}

void UartController::SlotInit()
{
    if (PortInit())//"/dev/ttyUSB0" в Linux
    {
        qDebug() << "Порт инициализирован";
    }
    else
    {
        qDebug() << "Ошибка инициализации порта";
    }
}

void UartController::SlotClosePort()
{
    port->close();
}

bool UartController::PortInit()
{
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setDataBits(QSerialPort::Data8);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setBaudRate(QSerialPort::Baud9600);
    port->setPortName(m_portName);//"/dev/ttyUSB0" в Linux //"/dev/ttyACM0"
    port->close();//Закрываем порт
    return port->open(QSerialPort::ReadWrite);
}

void UartController::SetPortName(const QString &name)
{
    m_portName = name;
}

void UartController::SlotEnable(const QString &data)
{
    if (!port->isOpen() || !port)
        return;

    QByteArray dataArray = data.toUtf8();
    qint64 written = port->write(dataArray);
    if (written == -1)
    {
        qWarning() << "Ошибка записи в порт";
    }
    else
    {
        qDebug() << "Отправлено:" << written << "байт, данные:" << data;
    }
}

void UartController::SlotDisable()
{
    if (!port->isOpen() || !port)
        return;

    QByteArray dataArray = QStringLiteral("-1").toUtf8();//QByteArray dataArray;dataArray.append(static_cast<char>(-1));
    qint64 written = port->write(dataArray);
    if (written == -1)
    {
        qWarning() << "Ошибка записи в порт";
    }
    else
    {
        qDebug() << "Отправлено:" << written << "байт" << "| ascii:" << dataArray;
    }
}
