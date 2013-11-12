#include "terminal.h"

#define QTERM_DEBUG

Terminal::Terminal() : QObject( 0 ), sent( 0 ), errors( 0 ),
  device( new QSerialPort() )
{
  QSettings settings( QCoreApplication::applicationDirPath() + "/qterm.ini",
                      QSettings::IniFormat );

  QByteArray id = settings.value( "conn/id",
                                  QByteArray( "default" ) ).toByteArray();
  QByteArray secret = settings.value( "conn/secret",
                                      QByteArray( "default" ) ).toByteArray();

#ifdef QTERM_DEBUG

  qDebug() << "id:" << id;
  qDebug() << "secret:" << secret;

#endif

  //TODO toggle enginio backend if keys arn't present
  if ( id == "default" || secret == "default" ) {
    qFatal( "Error enginio keys are not set\n" );
  }

  client = new EnginioClient();
  client->setBackendId( id );
  client->setBackendSecret( secret );

  connect( client, &EnginioClient::finished, this, &Terminal::reply );

#ifdef QTERM_DEBUG

  device = new QFile( "/dev/random" );
  device->open( QIODevice::ReadOnly );

#else

  QString line = settings.value( "serial/line", "/dev/ttyUSB0" ).toString();
  int baud = settings.value( "serial/baud", QSerialPort::Baud9600 ).toInt();
  int databits = settings.value( "serial/databits", QSerialPort::Data8 ).toInt();
  int parity = settings.value( "serial/parity", QSerialPort::NoParity ).toInt();
  int stopbits = settings.value( "serial/stopbits",
                                 QSerialPort::OneStop ).toInt();
  int flowctrl = settings.value( "serial/flowcontrol",
                                 QSerialPort::NoFlowControl ).toInt();

  settings.setValue( "serial/line", line );
  settings.setValue( "serial/baud", baud );
  settings.setValue( "serial/databits", databits );
  settings.setValue( "serial/parity", parity );
  settings.setValue( "serial/stopbits", stopbits );
  settings.setValue( "serial/flowcontrol", flowctrl );
  settings.sync();

  qDebug() << "Opening:" << line << baud << databits << parity << stopbits <<
           flowctrl;

  QSerialPort* serial = ( QSerialPort* ) device;

  serial->setPortName( line );

  if ( !serial->open( QIODevice::ReadWrite ) ) {
    qFatal( "Error Opening Device: %s",
            serial->errorString().toLocal8Bit().data() );
  }

  serial->setBaudRate( ( QSerialPort::BaudRate ) baud );
  serial->setDataBits( ( QSerialPort::DataBits ) databits );
  serial->setParity( ( QSerialPort::Parity ) parity );
  serial->setStopBits( ( QSerialPort::StopBits ) stopbits );
  serial->setFlowControl( ( QSerialPort::FlowControl ) flowctrl );

#endif

  current = new QByteArray();
}

Terminal::~Terminal() { }

void Terminal::reply( EnginioReply* reply )
{

  if ( reply->isError() ) {
    errors++;
#ifdef QTERM_DEBUG
    qDebug() << "Error:" << reply->networkError() << reply->errorType() <<
             reply->errorString();
#endif
  } else {
    sent++;
#ifdef QTERM_DEBUG
    qDebug() << "Success:" << QJsonDocument( reply->data() ).toJson(
               QJsonDocument::Indented );
#endif
  }


}

void Terminal::poll( void )
{

#ifdef QTERM_DEBUG
  QByteArray* data = new QByteArray( device->read( 8 ).toHex() );
#else
  if ( device->bytesAvailable() == 0 ) {
    return;
  }
  QByteArray* data = new QByteArray( device->readAll() );
#endif

  if ( current != data ) {

    QDateTime local( QDateTime::currentDateTimeUtc() );

    qDebug() << local.toString().toLatin1() << data->data();

    emit this->data( QString( data->data() ) );

    QJsonObject* tag = new QJsonObject();

    tag->insert( "objectType", QString( "objects.tag" ) );
    tag->insert( "Date", local.toString() );
    tag->insert( "Tag", QString( data->data() ) );

    EnginioReply* reply = client->create( *tag );
    QObject::connect( reply, &EnginioReply::finished, reply,
                      &EnginioReply::deleteLater );

  }

  delete current;
  current = data;
}
