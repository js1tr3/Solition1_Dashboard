#include "solitonudp.h"
//#include <cstring>

#define MYPORT 48879
#define MAXBUFLEN 100

enum
  {
    LOG_MSTICS = 1,
    LOG_MINTICS,
    LOG_AUXV,
    LOG_PACKV,
    LOG_CURRENT,
    LOG_TEMP,
    LOG_INPUT3,
    LOG_INPUT2,
    LOG_INPUT1,
    LOG_THROTTLE,
    LOG_CPULOAD,
    LOG_PWM,
    LOG_RPM,
    LOG_RPMERROR,
    LOG_MODE,
    LOG_NUMFIELDS
  };

#define LIMIT_HIMOTORV	0x8000
#define LIMIT_HIMOTORP	0x4000
#define LIMIT_THROTTLE	0x2000
#define LIMIT_OVERREV	0x1000

#define LIMIT_LOWPACKV	0x0800
#define LIMIT_HIPACKC	0x0400
#define LIMIT_HVC	0x0200
#define LIMIT_CTEMP	0x0100

#define LIMIT_SLEWRATE	0x0080
#define LIMIT_BLOCKED	0x0040
#define LIMIT_BRAKES   	0x0020

std::string mode[32] =
  {
    "Starting up",
    "Precharge phase",
    "Engaging contactors",
    "Waiting for startsignal",
    "Throttle not in zero pos",
    "Running",
    "Error 6","Error 7","Error 8","Error 9","Error 10","Error 11","Error 12",
    "Error 13", "Error 14","Error 15","Error 16","Error 17",
    "Error: Motor overcurrent",
    "Error: ADC out of range",
    "Error: Current sensor failure",
    "Error: Zero voltage after precharge",
    "Error: Pack voltage too low after precharge",
    "Error: Faulty throttle signal",
    "Error: 12 Volt too high",
    "Error: 12 Volt too low",
    "Error: Pack voltage too high",
    "Error: Pack voltage too low",
    "Error: IGBT desaturation",
    "Error: Out of memory",
    "Error: Software error",
    "Controller shut down by user"
  };

uint16_t oldmstic = 0xffff;

UDPSocket::UDPSocket(QObject *parent):
    QObject(parent)
{
    m_pack_voltage="--";
    m_RunTime="0";
    // create a QUDP socket
    socket = new QUdpSocket(this);
    // The most common way to use QUdpSocket class is to bind to an address and port using bind()
   // socket->bind(QHostAddress::LocalHost, MYPORT);

    socket->bind(QHostAddress::Broadcast,48879);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

//    m_voltage=QString("%1").arg(24, 3, 10, QChar('0')); // number , width, base 10, leading zeros

//    emit dataChanged();
}



void UDPSocket::readyRead()
{
// when data comes in
QByteArray datagram;
qint64 pending_size;
//buffer.resize(socket->pendingDatagramSize());

//QHostAddress sender;
//quint16 senderPort;

// Receives a datagram no larger than maxSize bytes and stores it in data.
// The sender’s host address and port is stored in *address and *port

while(socket->hasPendingDatagrams()){
    pending_size=socket->pendingDatagramSize();
//qDebug() << "Pending Size" << pending_size;
    datagram.resize(pending_size);
socket->readDatagram(datagram.data(), datagram.size());
}
//qDebug() << "Datagram: " << datagram.toHex();
//&sender, &senderPort);
//qDebug() << “Message from: ” << sender.toString();
//qDebug() << “Message port: ” << senderPort;
//qDebug() << "Message: " << QString(datagram.toHex())    ;

//int rv;
//int numbytes;
//char buf[MAXBUFLEN];

//uint16_t *debug = (uint16_t *)buf;

//rv=datagram[LOG_MSTICS];
m_Throttle=datagram[LOG_THROTTLE];//QString("%1").arg(datagram[LOG_THROTTLE], 2, 10, QChar('0'));
m_RunTime= QString("M:S.s %1 : %2 . %3").arg(datagram[LOG_MINTICS] / 60).arg(datagram[LOG_MINTICS] % 60).arg((qreal)datagram[LOG_MSTICS] / 1000.0);
qDebug() << "Throttle" << m_Throttle;
qDebug() << "RunTime: " << QString("M:S.s %1: %2. %3").arg(datagram[LOG_MINTICS] / 60).arg(datagram[LOG_MINTICS] % 60).arg((qreal)datagram[LOG_MSTICS] / 1000.0);

m_pack_voltage=QString("%1").arg(datagram.data()[LOG_PACKV], 3, 10, QChar('0'));

m_aux_voltage=datagram[LOG_AUXV] * 0.00025939941 + 1.0;//QString("%1").arg(datagram[LOG_AUXV] * 0.00025939941 + 1.0, 3, 10, QChar('0'));
m_RPM=datagram[LOG_RPM];
qDebug() << "RPM" << m_RPM;
m_Temperature=datagram[LOG_TEMP];
qDebug() << "Temperature" << m_Temperature;
m_CPU=datagram[LOG_CPULOAD] / 128.0;

//        debug[LOG_INPUT1] * 0.000080645,
//        debug[LOG_INPUT2] * 0.000080645,
//        debug[LOG_INPUT3] * 0.000080645,




emit dataChanged();

m_errmsg=QString::fromStdString( mode[ (std::uint8_t)(datagram[LOG_MODE]) & 0x1f ] );
emit msgChanged();

//text_Volt

/*FILE *log = fopen ("logger.txt", "w");
if (!log)
  {
    perror ("logger.txt");
    return EXIT_FAILURE;
  }

fprintf (log, "Time\tCPU\tThrtl\tMotorC\tPWM\tPackV\tTemp\tRPM\t"
     "In1\tIn2\tIn3\t12V\tMode\r\n");

if (numbytes && !buf[0])
{
if (oldmstic != debug[LOG_MSTICS] / 200)
  {
    oldmstic = debug[LOG_MSTICS] / 200;
    printf ("\r%2.1d:%2.2d:%2.2d - CPU:%3.1d%% Thr:%3.1dA "
        "I:%3.1dA D:%3.1d.%1.1d Pack:%3.1dV "
        "T:%3.1d.%1.1d RPM:%4.1d ",
        debug[LOG_MINTICS] / 60,
        debug[LOG_MINTICS] % 60,
        debug[LOG_MSTICS] / 1000,
        debug[LOG_CPULOAD] >> 7,
        debug[LOG_THROTTLE],
        debug[LOG_CURRENT],
        debug[LOG_PWM] / 10,
        debug[LOG_PWM] % 10,
        debug[LOG_PACKV],
        ((int16_t) debug[LOG_TEMP])/10,
        abs((int16_t) debug[LOG_TEMP])%10,
        debug[LOG_RPM]);

    fflush (stdout);
  }

fprintf (log, "%d\t%.2f\t%d\t%d\t%.2f\t%d\t%.2f\t%d\t"
     "%.2f\t%.2f\t%.2f\t%.2f\t%d\t"
     "%s%s%s%s%s%s%s%s%s%s%s%s\r\n",
     debug[LOG_MSTICS] +
     debug[LOG_MINTICS] * 60000,
     debug[LOG_CPULOAD] / 128.0,
     debug[LOG_THROTTLE],

     debug[LOG_CURRENT],

     debug[LOG_PWM] / 10.0,

     debug[LOG_PACKV],
     ((int16_t) debug[LOG_TEMP])/10.0,
     debug[LOG_RPM],
     debug[LOG_INPUT1] * 0.000080645,
     debug[LOG_INPUT2] * 0.000080645,
     debug[LOG_INPUT3] * 0.000080645,
     debug[LOG_AUXV] * 0.00025939941 + 1.0,
     debug[LOG_MODE], mode[debug[LOG_MODE] & 0x1f],

     debug[LOG_MODE] & LIMIT_HIMOTORV ? ", High motor volt" : "",
     debug[LOG_MODE] & LIMIT_HIMOTORP ? ", High motor power" : "",
     debug[LOG_MODE] & LIMIT_THROTTLE ? ", Throttle limit" : "",
     debug[LOG_MODE] & LIMIT_OVERREV ? ", RPM high" : "",

     debug[LOG_MODE] & LIMIT_LOWPACKV ? ", Low pack volt" : "",
     debug[LOG_MODE] & LIMIT_HIPACKC ? ", High pack current" : "",
     debug[LOG_MODE] & LIMIT_HVC ?
     ", High pack volt throttle limit" : "",
     debug[LOG_MODE] & LIMIT_CTEMP ?
     ", Controller temp high" : "",

     debug[LOG_MODE] & LIMIT_SLEWRATE ? ", Slewrate active" : "",
     debug[LOG_MODE] & LIMIT_BLOCKED ? ", Throttle blocked" : "",
     debug[LOG_MODE] & LIMIT_BRAKES ? ", Brakes active" : "");
}
else
{
printf ("\r                                                                             \r");
if (numbytes)
  {
    buf[numbytes] = '\0';
    while (strlen (buf) && buf[strlen (buf) - 1] < ' ')
  buf[strlen (buf) - 1] = '\0';
    printf ("%s\n", buf);
  }
else
  {
    printf ("Empty message!\n");
  }
}
*/
}


QString UDPSocket::disperr() const
{
    return m_errmsg;
}

QString UDPSocket::disp_RunTime() const
{
    return m_RunTime;
}

qreal UDPSocket::disp_aux_voltage() const
{
    return m_aux_voltage;
}

QString UDPSocket::disp_pack_voltage() const
{
    return m_pack_voltage;
}

QString UDPSocket::disp_motor_current() const{
    return m_motor_current;
}

qreal UDPSocket::disp_Temperature() const{
    return m_Temperature;
}

qreal UDPSocket::disp_Throttle() const{
    return m_Throttle;
}

QString UDPSocket::disp_CPU() const{
    return m_CPU;
}

QString UDPSocket::disp_PWM() const{
    return m_PWM;
}

qreal UDPSocket::disp_RPM() const{
    return m_RPM;
}

