#include "terminal.h"
#include <QDebug>
#include <QDateTime>
#include <QByteArray>
#include <QTextStream>
#include <QDataStream>
#include "stdio.h"
#include <QThread>
#include <QCoreApplication>

QByteArray conn_req  = QByteArray::fromHex("0002fff001fb03");
QByteArray phase_amp = QByteArray::fromHex("0201f0863f3ac803"); // stx dst src cmd cp res crc8 etx
//QByteArray reset = QByteArray::fromHex("02fff0183803"); // stx dst src cmd crc8 etx
QByteArray retransmit = QByteArray::fromHex("02fff003a503"); // stx dst src cmd crc8 etx
QByteArray savs = QByteArray::fromHex("02fff02ac903"); // stx dst src cmd crc8 etx
QByteArray set = QByteArray::fromHex("02fff02ac903"); // stx dst src cmd crc8 etx

QByteArray getcfg = QByteArray::fromHex("00201f029316303"); // stx dst src cmd adr crc8 etx
QByteArray last = QByteArray::fromHex("0002fff0245303"); // stx dst src cmd crc8 etx
QByteArray start_tuning = QByteArray::fromHex("0002fff0876903");
QByteArray tuningresult = QByteArray::fromHex("0002fff089f303"); // stx dst src cmd crc8 etx
QByteArray get_tuning = QByteArray::fromHex("0002fff0436c03"); // stx dst src cmd crc8 etx
QByteArray amplitude  = QByteArray::fromHex("000201f083fb03");
QByteArray phase = QByteArray::fromHex("000201f0823a1303"); // stx dst src cmd res crc8 etx
QByteArray rssi = QByteArray::fromHex("0002fff085a103"); // stx dst src cmd crc8 etx
QByteArray version = QByteArray::fromHex("0002fff0094b03"); // stx dst src cmd crc8 etx
QByteArray snr  = QByteArray::fromHex("0002fff00ae703");
QByteArray hdx = QByteArray::fromHex("000201f0912403"); // stx dst src cmd crc8 etx
QByteArray single = QByteArray::fromHex("0002fff0227a03"); // stx dst src cmd crc8 etx

enum  {
    STX = '\x02',
    BROADCAST = '\xFF',
    ACK = '\x06',
    ETX = '\x03',
    MESG = '\x23'
};

//#define QTERM_DEBUG

Terminal::Terminal( QString line, int baud, int databits,
                    int stopbits ) : QObject( 0 ), collectRSSI(false)
{
#ifdef QTERM_DEBUG
  device = new QFile( "/dev/urandom" );
  device->open( QIODevice::ReadOnly );
#else
  qDebug() << "Opening:" << line << baud << databits << stopbits;

  device = new QSerialPort();

  QSerialPort* serial = ( QSerialPort* ) device;

  serial->setPortName( line );

  if ( !serial->open( QIODevice::ReadWrite ) ) {
    qFatal( "Error Opening Device: %s",
            serial->errorString().toLocal8Bit().data() );
  }

  serial->setBaudRate( ( QSerialPort::BaudRate ) baud );
  serial->setDataBits( ( QSerialPort::DataBits ) databits );
  serial->setParity( QSerialPort::NoParity );
  serial->setStopBits( ( QSerialPort::StopBits ) stopbits );
  serial->setFlowControl( QSerialPort::NoFlowControl );
#endif
  current = new QByteArray();

  // Listen on 7755 for external commands
 // socket.bind(QHostAddress::LocalHost, 7755);
  //connect(&socket, &QUdpSocket::readyRead, this, &Terminal::read_from_sock);
}

void Terminal::read_from_sock()
{
    QByteArray cmd = phase_amp;

    device->write(cmd);

    { QTextStream input(stdin); (void) input.readLine(); }
    //{ char buffer[1024]; socket.readDatagram(buffer, 1024, 0, 0);}
}

void Terminal::process_cmd(QByteArray raw){

  char cmd = raw.at(4);

  int num;
  bool ok;

  switch(cmd){
    case '\x01': qDebug() << "Connect_RQ"; break;
    case '\x0a': qDebug() << "Serial#: " << raw.left(11).right(6); break;
    case '\x09': qDebug() << "Version: " << raw.left(21).right(16); break;

    case '\x85':
      num = QString(raw.left(6).right(1).toHex()).toInt(&ok,16);
      qDebug() << "RSSI:" << num << (float) num*0.02 << "Volts" << raw.left(6).right(1).toHex();
      if(ok){
          emit newRSSI((float) num*0.02);
          qDebug() << "Emitting" << num*0.02;
      }
      break; // y(Volts) = 0.02x

    case '\x91': qDebug() << "Hdx Sample: " << raw.toHex(); break;
    case '\x82': qDebug() << "Phase: " << raw.left(6).right(1).toHex(); break;
    case '\x83': qDebug() << "Amplitude: " << raw.left(7).right(1).toHex(); break;
    case '\x43': qDebug() << "Get Tunning: " << raw.left(7).right(2).toHex(); break;
    case '\x89': qDebug() << "Get Tunning Result: " << raw.left(8).right(3).toHex(); break;
    case '\x87': qDebug() << "Tunning Finished"; break;
    case '\x29': qDebug() << "Config(mode)" << raw.left(6).right(1).toHex(); break;
    case '\x86': qDebug() << "Phase Amplitude" << raw.left(8).right(3).toHex(); break;
    default:
      qDebug() << "Unrecognized Command"; break;
  }

}

void Terminal::process_tag(QByteArray raw){

  QByteArray tag = raw.mid(5,16);

  if( current != tag ){
    qDebug() << "Tag Detected: #" << tag.data();
    emit newTag(tag);
  }

  delete current;
  current = new QByteArray(tag);
}

void Terminal::read_from_serial( void )
{

  if(collectRSSI){
    device->write(rssi);
  }

  if(collectAmplitude){
    //device->write(amplitude);
  }

  if(collectTuning){
  }

#ifdef QTERM_DEBUG
  QByteArray raw = device->read( 8 ).toHex();
#else
  if ( device->bytesAvailable() == 0 ) {
    return;
  }
  QByteArray raw = device->readAll();
#endif

  qDebug() << "Hex:" << raw.toHex().data();

  if(raw.at(3) == MESG){
    process_tag(raw);
  }else if(raw.at(3) == ACK){
    process_cmd(raw);
  }else{
    qDebug() << "Unknown Data";
  }

}

void Terminal::toggleRSSI(bool active){
  collectRSSI = active;
}

void Terminal::startTuning(){

    /*
    // Stop
    if ( collectAmplitude  && !active ) {
    }

    // Start
    if ( !collectAmplitude && active ) {
      cpattern = 0x00;
    }

    collectAmplitude = active;
    */
}

void Terminal::toggleAmplitude(bool active){
/*

    QByteArray packet = phase_amp;

    phase_amp[5]++;

    if(cpattern <= 0x7f){
       device->write(packet)
    }else{
      // Finished
      collectAmplitude = false;
    }

*/
}
