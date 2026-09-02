#include <QDebug>
#include <QtSerialPort>
#include "uartcontroller.h"

UartController::UartController(QObject *parent) : QObject{parent}, m_portName("COM4")
{
    port = new QSerialPort();
    m_idleTimer = new QTimer(this);
    m_idleTimer->setSingleShot(true);
    m_idleTimer->setInterval(1000);

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
        m_buffer.append(port->readAll());
        m_idleTimer->start();
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

void UartController::SlotEnable()
{
    QByteArray byteArray;//=QByteArray::fromHex("01");
    byteArray.resize(0);
    byteArray.append(static_cast<char>(0x01));
    SendData(byteArray, byteArray.size());
}

void UartController::SendData(QByteArray &byteArray, int length)
{
    if (!port->isOpen())
    {
        qDebug() << "Порт не открыт!";
        return;
    }
    if (byteArray.length() == length)
    {
        for (int i = 0; i<length; i++)
        {
            QByteArray temp;
            temp.resize(0);
            temp.append(byteArray[i]);
            port->write(temp);
            port->flush();
            //port->waitForBytesWritten();//waitForBytesWritten блокирует поток и readyRead всё крашит
            temp.clear();
        }
        byteArray.clear();
    }
}

void UartController::SlotDisable()
{
    QByteArray byteArray;
    byteArray.resize(0);
    byteArray.append(static_cast<unsigned char>(0x02));
    SendData(byteArray, byteArray.size());
}
