#ifndef UPLINK_H
#define UPLINK_H

#include <Enginio/enginioreply.h>
#include <Enginio/Enginio>

#include <QObject>

class UpLink : public QObject
{
  Q_OBJECT
public:
  explicit UpLink( QObject *parent = 0 );
  void reply( EnginioReply* reply );

signals:

public slots:
  void send( QString data );

private:
  EnginioClient* client;

};

#endif // UPLINK_H
