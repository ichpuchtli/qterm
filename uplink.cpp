#include "uplink.h"

UpLink::UpLink( QObject *parent ) : QObject( parent )
{
  QByteArray
  id;// = settings.value( "conn/id", QByteArray( "default" ) ).toByteArray();
  QByteArray
  secret;// = settings.value( "conn/secret", QByteArray( "default" ) ).toByteArray();

  qDebug() << "id:" << id;
  qDebug() << "secret:" << secret;

  if ( id == "default" || secret == "default" ) {
    qFatal( "Error enginio keys are not set\n" );
  }

  client = new EnginioClient();
  client->setBackendId( id );
  client->setBackendSecret( secret );

  connect( client, &EnginioClient::finished, this, &UpLink::reply );

}

void UpLink::reply( EnginioReply* reply )
{

  if ( reply->isError() ) {
    qDebug() << "Error:" << reply->networkError() << reply->errorType() <<
             reply->errorString();
  } else {
    qDebug() << "Success:" << QJsonDocument( reply->data() ).toJson(
               QJsonDocument::Indented );
  }


}

void UpLink::send( QString data )
{

  QJsonObject* tag = new QJsonObject();

  QDateTime local( QDateTime::currentDateTimeUtc() );

  tag->insert( "objectType", QString( "objects.tag" ) );
  tag->insert( "Date", local.toString() );
  tag->insert( "Tag", data );

  EnginioReply* reply = client->create( *tag );
  QObject::connect( reply, &EnginioReply::finished, reply,
                    &EnginioReply::deleteLater );
}
