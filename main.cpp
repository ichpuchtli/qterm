#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <terminal.h>

int main( int argc, char *argv[] )
{
  QCoreApplication app( argc, argv );

  QTimer t;

  Terminal terminal;

  QObject::connect( &t, &QTimer::timeout, &terminal, &Terminal::poll );

  t.start( 500 );

  return app.exec();
}
