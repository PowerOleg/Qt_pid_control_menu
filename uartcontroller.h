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
    QString m_portName;
    QByteArray m_buffer;
    QTimer *m_idleTimer;

public slots:
    void SlotInit();
    void SlotClosePort();
    void SlotEnable(const QString &data);
    void SlotDisable();
    void SlotRead();
};

#endif // UART_H
