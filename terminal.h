#ifndef TERMINAL_H
#define TERMINAL_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtGui/QResizeEvent>
#include <QtCore/QSocketNotifier>
#include <QtNetwork/qudpsocket.h>
#include <QByteArray>

class Terminal : public QObject
{
  Q_OBJECT

public:
  Terminal( QString line = "/dev/ttyUSB0", int baud = QSerialPort::Baud9600,
            int databits = QSerialPort::Data8, int stopbits = QSerialPort::OneStop );

public slots:

  void read_from_serial( void );
  void read_from_sock( void );

  void toggleRSSI( bool active ); // toggle_rssi
  void toggleAmplitude( bool active );
  void startTuning();

signals:
  void newTag( QString id );
  void newRSSI( float value );
  void newAmplitude( float value );
  void newTuning( float value );
  void newVersion( QString version );

  void quit();

private:
  void process_cmd(QByteArray raw);
  void process_tag(QByteArray raw);

  QIODevice* device;
  QByteArray* current;
  QUdpSocket socket;

  bool collectRSSI;
  bool collectAmplitude;
  bool collectTuning;

};

#endif // TERMINAL_H
