#ifndef TAGDBADAPTER_H
#define TAGDBADAPTER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDir>

class TagDBAdapter : public QObject
{
  Q_OBJECT

public:
  TagDBAdapter( QDir db );

signals:
  void sqlStatement( QString statement );

  void rssiActive(bool isActive);
  void amplitudeActive(bool isActive);
  void startTuning();
  void checkTuningStatus();
  void startHDX();
  void startFDX();

public slots:
  void insertTag( QString tagid );
  void insertRSSI( float value );
  void insertAmplitude( float value );
  void insertTuning( float value );
  void insertTuningStatus( int value );

  void check_for_changes(void);

private:
  QSqlDatabase db;
  bool valid;
};

#endif // TAGDBADAPTER_H
