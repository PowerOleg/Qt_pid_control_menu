#ifndef UART_H
#define UART_H

#include <QObject>
#include <QByteArray>
#include <QTimer>

class QSerialPort;

class UartController : public QObject
{
    Q_OBJECT
public:
    explicit UartController(QObject *parent = nullptr);
    ~UartController();
    void SetPortName(const QString &name);

    QSerialPort* port;
private:
    bool PortInit();
    void SendData(QByteArray &byteArray, int length);

    QString m_portName;
    QByteArray m_buffer;
    QTimer *m_idleTimer;
//signals:
//   void SendDataUart();
public slots:
    void SlotRead();
    void SlotInit();
    void SlotEnable();
    void SlotDisable();
    void SlotClosePort();

};

#endif // UART_H
