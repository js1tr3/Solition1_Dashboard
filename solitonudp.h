#ifndef SOLITONUDP_H
#define SOLITONUDP_H
#include <QUdpSocket>
#include <QDebug>
#include <string>
#include <QTextStream>
#include <QFile>

//#include <QDeclarativeItem>

class UDPSocket: public QObject
{
Q_OBJECT

  //  Q_PROPERTY(QString temperature
  //             READ temperature WRITE setTemperature NOTIFY dataChanged)
/*
 *    1: Running time (0-59999 ms)
   2: Running time (0-65535 s)
   3: 12 Volt supply (see comment below)
   4: Pack voltage (V)
   5: Motor current (A)
   6: Tempreature (C * 10, signed)
   7: Input 3 (see comment below)
   8: Input 2 (-"-)
   9: Input 1 (-"-)
  10: Throttle (A)
  11: CPU-load (divide by 128 for percent)
  12: Pulse width (divide by 10 for percent)
  13: RPM (not that it works very well in 1.0, but it's reported anyway)
  */
    Q_PROPERTY(QString disp_RunTime READ disp_RunTime NOTIFY dataChanged)
    Q_PROPERTY(qreal disp_aux_voltage READ disp_aux_voltage NOTIFY dataChanged)
    Q_PROPERTY(QString disp_pack_voltage READ disp_pack_voltage NOTIFY dataChanged)
    Q_PROPERTY(qreal disp_motor_current READ disp_motor_current NOTIFY dataChanged)
    Q_PROPERTY(qreal disp_Temperature READ disp_Temperature NOTIFY dataChanged)
    Q_PROPERTY(qreal disp_Throttle READ disp_Throttle NOTIFY dataChanged)
    Q_PROPERTY(qreal disp_CPU READ disp_CPU NOTIFY dataChanged)
    Q_PROPERTY(qreal disp_PWM READ disp_PWM NOTIFY dataChanged)
    Q_PROPERTY(qreal disp_RPM READ disp_RPM NOTIFY dataChanged)

    Q_PROPERTY(QString disperr READ disperr NOTIFY msgChanged)

public:
   // SolitonUDP();
    explicit UDPSocket(QObject *parent = 0);

    //explicit WeatherData(QObject *parent = 0);
    //WeatherData(const WeatherData &other);

  //  QString disptemperature() const;
    //void setTemperature(const QString &value);

    QString disp_RunTime() const;
    qreal disp_aux_voltage() const;
    QString disp_pack_voltage() const;
    qreal disp_motor_current() const;
    qreal disp_Temperature() const;
    qreal disp_Throttle() const;
    qreal disp_CPU() const;
    qreal disp_PWM() const;
    qreal disp_RPM() const;



    QString disperr() const;
 //   void dispvoltage(const QString &value);

Q_SIGNALS:
    // The change notification signal of the status property
    void dataChanged();
    void msgChanged();

//    void Hello();
    signals:

    public slots:
    void readyRead();

    private:
    QUdpSocket *socket;
    QString m_errmsg;
    QString m_RunTime;
    qreal m_aux_voltage;
    QString m_pack_voltage;
    qreal m_motor_current;
    qreal m_Temperature;
    qreal m_Throttle;
    qreal m_CPU;
    qreal m_PWM;
    qreal m_RPM;
    qreal m_INPUT1;
    qreal m_INPUT2;
    qreal m_INPUT3;

    QFile logdata;
    QTextStream out_logfile;
};

#endif // SOLITONUDP_H



