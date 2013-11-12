#ifndef TERMINAL_H
#define TERMINAL_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtSerialPort/QSerialPort>
#include <QtGui/QResizeEvent>
#include <Enginio/enginioreply.h>
#include <Enginio/Enginio>

class Terminal : public QObject
{
  Q_OBJECT

public:
  Terminal();
  ~Terminal();

public slots:
  void poll( void );

private slots:
  void reply( EnginioReply* reply );

signals:
  void data( QString line );

private:

  int sent;
  int errors;

  QIODevice* device;

  QByteArray* current;

  EnginioClient* client;

};

#endif // TERMINAL_H
