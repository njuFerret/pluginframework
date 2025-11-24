#include "helloworld.h"
//#include <QWidget>
#include <painttributton.h>
#include <QTimer>

const int redDuration = 2;
const int yellowDuration = 3;
const int greenDuration = 4;




HelloWorldPlugin::HelloWorldPlugin() {}

HelloWorldPlugin::~HelloWorldPlugin() {
  // delete w;
}

bool HelloWorldPlugin::initialize(const QStringList &arguments, QString *errorString) {
  w.reset(new PaintTriButton);
  w->setWindowTitle("尼玛 from HelloWordPlugin");
  w->resize(800,600);
  auto timer = new QTimer(w.data());
  timer->setInterval(500);
  int timing = 0;

  connect(timer, &QTimer::timeout, this, [=,&timing](){
    PaintTriButton::Color c = w->paintColor();
    timing++;

    if (((c == PaintTriButton::bRed) && (timing == redDuration)) |
        ((c == PaintTriButton::bGreen) && (timing == greenDuration)) |
        ((c == PaintTriButton::bYellow) && (timing == yellowDuration))) {

      w->goNext();
      timing = 0;
      }
  });

  timer->start();
  w->show();
  return true;
}

void HelloWorldPlugin::extensionsInitialized() {}

void HelloWorldPlugin::shutdown() {}
