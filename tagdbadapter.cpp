#include "tagdbadapter.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>

TagDBAdapter::TagDBAdapter( QDir dbFile ) : QObject( 0 ), valid( true )
{
  db = QSqlDatabase::addDatabase( "QSQLITE" );
  db.setDatabaseName( dbFile.absolutePath() );

  if ( !dbFile.isReadable() || !db.open() || !db.isValid() ) {
    valid = false;
  }

  qDebug() << " Tag db:" << dbFile.absolutePath() << " Readable:" <<
           dbFile.isReadable() << "Valid:" << valid;
}

void TagDBAdapter::insertTag( QString tag )
{
  if ( !valid ) return;

  QString statement( "insert into tags (tag) values (\"" + tag + "\")" );

  emit sqlStatement( statement );

  QSqlQuery query( db );

  query.exec( statement );
}

void TagDBAdapter::insertRSSI(float value){
  QString statement( "insert into rssi (value) values (" + QString::number(value) + ")" );
  QSqlQuery query(statement, db );
}

void TagDBAdapter::check_for_changes(){

  {
    QString statement( "select value from settings where key = \"rssi_active\"");
    QSqlQuery query( statement, db );
    query.first();
    bool active = query.value(0).toBool();
    emit rssiActive(active);
  }

  {
    QString statement( "select value from settings where key = \"amplitude_active\"");
    QSqlQuery query( statement, db );
    query.first();
    bool active = query.value(0).toBool();
    emit amplitudeActive(active);
  }

  {
    QString statement1( "select value from settings where key = \"start_tuning_sample\"");
    QString statement2( "update settings set value=\"false\" where key = \"start_tuning_sample\"");
    QSqlQuery query( statement1, db );
    query.first();
    bool active = query.value(0).toBool();
    if(active){
      emit startTuning();
      QSqlQuery query( statement2, db );
    }
  }

}

void TagDBAdapter::insertAmplitude(float value){
  QString statement( "insert into amplitude (value) values (" + QString::number(value) + ")" );
  QSqlQuery query(statement, db);
}

void TagDBAdapter::insertTuning(float value){
  QString statement( "insert into tuning (value) values (" + QString::number(value) + ")" );
  QSqlQuery query(statement, db);
}

void TagDBAdapter::insertTuningStatus(int value){
    // consider REPLACE INTO table (keys) values (values)
  QString statement( "update settings set value = " + QString::number(value) + " where key = " );
  QSqlQuery query(statement, db);
}
