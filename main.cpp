#include <QtCore/qcoreapplication.h>
#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>
#include <QtCore/qsettings.h>
#include <QtCore/QSocketNotifier>

#include <terminal.h>
#include <tagdbadapter.h>

int main( int argc, char **argv )
{
  QCoreApplication app( argc, argv );

  QSettings settings( QCoreApplication::applicationDirPath() + "/qterm.ini", QSettings::IniFormat );

  QString line = settings.value("serial/line").toString();
  int baud = settings.value("serial/baud").toInt();
  int databits = settings.value("serial/databits").toInt();
  int stopbits = settings.value("serial/stopbits").toInt();

  Terminal terminal(line,baud,databits,stopbits);

  TagDBAdapter db(QDir("/srv/http/dash/htdocs/tag.db"));
  QObject::connect( &terminal, &Terminal::newTag, &db, &TagDBAdapter::insertTag);
  QObject::connect( &terminal, &Terminal::newRSSI, &db, &TagDBAdapter::insertRSSI);
  QObject::connect( &terminal, &Terminal::newAmplitude, &db, &TagDBAdapter::insertAmplitude);
  QObject::connect( &terminal, &Terminal::newTuning, &db, &TagDBAdapter::insertTuning);

  QObject::connect( &db, &TagDBAdapter::rssiActive, &terminal, &Terminal::toggleRSSI);

  QObject::connect( &db, &TagDBAdapter::amplitudeActive, &terminal, &Terminal::toggleAmplitude);

  QObject::connect( &db, &TagDBAdapter::startTuning, &terminal, &Terminal::startTuning);

  //QObject::connect( &db, &TagDBAdapter::checkTuningStatus, &terminal, &Terminal::checkTuningStatus);

  QSocketNotifier async_stdin(fileno(stdin), QSocketNotifier::Read);

  QObject::connect(&async_stdin, SIGNAL(activated(int)), &terminal, SLOT(read_from_sock()));
  async_stdin.setEnabled(true);

  QTimer timer;
  QObject::connect( &timer, &QTimer::timeout, &terminal, &Terminal::read_from_serial );
  QObject::connect( &timer, &QTimer::timeout, &db, &TagDBAdapter::check_for_changes );
  timer.start( 250 );

  return app.exec();
}
