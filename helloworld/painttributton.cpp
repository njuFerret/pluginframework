#include "painttributton.h"
#include <QMetaEnum>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QTimer>

const int VALID_COLOR_COUNT = 3;

PaintTriButton::PaintTriButton(QWidget *parent) : QPushButton{parent} {
  // 颜色改变时或布局改变时，重新绘制
  connect(this, &PaintTriButton::paintColorChanged, this, [this]() { update(); });
  connect(this, &PaintTriButton::lightsLayoutChanged, this, [this]() { update(); });
}

void PaintTriButton::paintEvent(QPaintEvent *event) {
  //  QPushButton::paintEvent(event);
  Q_UNUSED(event)

  QPainter painter(this);

  setupPainter(painter);
  painter.setPen(Qt::transparent);
  int radius = 100;
  int frameOffset = 13;
  //  画3个边框
  drawFrames(painter, 210);
  radius = radius - frameOffset;
  // 画3个内部色块
  drawInners(painter, radius);
  // 画3个高亮
  drawHighlights(painter, radius);
}

// *  根据横向还是纵向布局，判断绘制缩放边长
// *  变换基本思想:
// *  1) 如果是等比变化，只需要判断最小边长，以最小边长进行缩放，
// *     例如： side = qMin(w,h);
// *  2) 如果是非等比变换，例如保持长边:短边=k:1时，将长边除以k之后再与
// *     短边取较小值进行等比变换，如本处长短比=3，则
// *           side = qMin(长边/3.0,短边)
// *  3) 水平布局时长边为宽度，垂直布局时长边为高度
// *  返回值为等效边长最小值
float PaintTriButton::scaledShortSide(Layouts layout) {

  int w = width();
  int h = height();
  // 水平布局时长边为宽度(宽度除以变换比)，垂直布局时长边为高度(高度除以变换比)
  int side = (layout == lHorizontal) ? int(qMin(float(w) / VALID_COLOR_COUNT, h / 1.0))
                                     : int(qMin(w / 1.0, float(h) / VALID_COLOR_COUNT));

  side &= ~0x01;        // 确保边长为偶数
  return side;
}

void PaintTriButton::setupPainter(QPainter &painter) {

  painter.setRenderHint(QPainter::Antialiasing);
  int w = width();
  int h = height();

  painter.translate(w / 2, h / 2);
  int side = 0;
  if (m_lightsLayout == lAuto) {        // 自动布局，根据长宽大小，判断横向布局还是纵向布局
    side = scaledShortSide(w > h ? lHorizontal : lVertical);
  } else {        // 非自动布局，按照用户指定布局形式进行
    side = scaledShortSide(m_lightsLayout);
  }

  painter.scale(side / 256.0, side / 256.0);
}

void PaintTriButton::drawFrame(QPainter &painter) {
  int radius = 100;
  int offset = 13;
  // 1. 外边框
  QLinearGradient lg1(0, -radius, 0, radius);
  lg1.setColorAt(0.0, QColor(255, 255, 255));
  lg1.setColorAt(1.0, QColor(166, 166, 166));
  painter.setBrush(lg1);
  painter.drawEllipse(-radius, -radius, radius << 1, radius << 1);

  // 2. 内边框
  //  radius -= 13;
  radius -= offset;
  QLinearGradient lg2(0, -radius, 0, radius);
  lg2.setColorAt(0.0, QColor(155, 155, 155));
  lg2.setColorAt(1.0, QColor(255, 255, 255));
  painter.setBrush(lg2);
  painter.drawEllipse(-radius, -radius, radius << 1, radius << 1);
}

void PaintTriButton::drawFrames(QPainter &painter, int distance) {
  Layouts currentLayout = lightsLayout();
  if (currentLayout == lAuto) {
    currentLayout = width() > height() ? lHorizontal : lVertical;
  }
  // 使用平移的方法绘制多个相同的图形
  for (int idx = 0; idx < VALID_COLOR_COUNT; ++idx) {
    painter.save();
    (currentLayout == lHorizontal) ? painter.translate(distance * (idx - 1), 0)
                                   : painter.translate(0, distance * (idx - 1));
    drawFrame(painter);
    painter.restore();
  }
}

void PaintTriButton::drawInner(QPainter &painter, int radius, int index) {
  // 3. 内部的圆
  QRadialGradient rg(0, 0, radius);
  auto c = paintColor();
  if (!isEnabled() or index != c) {
    c = bGray;
  }

  switch (c) {
  case bRed:
    rg.setColorAt(0.0, QColor(245, 0, 0));
    rg.setColorAt(0.6, QColor(210, 0, 0));
    rg.setColorAt(1.0, QColor(140, 0, 0));
    break;
  case bGreen:
    rg.setColorAt(0.0, QColor(0, 245, 0));
    rg.setColorAt(0.6, QColor(0, 210, 0));
    rg.setColorAt(1.0, QColor(0, 140, 0));
    break;

  case bYellow:
    rg.setColorAt(0.0, QColor(245, 245, 0));
    rg.setColorAt(0.6, QColor(210, 210, 0));
    rg.setColorAt(1.0, QColor(140, 140, 0));
    break;
  case bGray:
    rg.setColorAt(0.0, QColor(245, 245, 245));
    rg.setColorAt(0.6, QColor(210, 210, 210));
    rg.setColorAt(1.0, QColor(140, 140, 140));
    break;

  default: break;
  }

  painter.setBrush(rg);
  painter.drawEllipse(-radius, -radius, radius << 1, radius << 1);
}

void PaintTriButton::drawInners(QPainter &painter, int radius) {
  Layouts currentLayout = lightsLayout();
  if (currentLayout == lAuto) {
    currentLayout = width() > height() ? lHorizontal : lVertical;
  }
  int distance = 210;
  for (int idx = 0; idx < VALID_COLOR_COUNT; ++idx) {
    painter.save();
    (currentLayout == lHorizontal) ? painter.translate(distance * (idx - 1), 0)
                                   : painter.translate(0, distance * (idx - 1));
    drawInner(painter, radius, idx);
    painter.restore();
  }
}

void PaintTriButton::drawHighlight(QPainter &painter, int radius) {

  // 4. 高光
  QPainterPath smallCircle;
  QPainterPath bigCircle;
  radius -= 3;
  smallCircle.addEllipse(-radius, -radius - 2, radius << 1, radius << 1);
  radius *= 2;
  bigCircle.addEllipse(-radius, -radius + 140, radius << 1, radius << 1);

  // 高光的形状为小圆扣掉大圆的部分
  QPainterPath highlight = smallCircle - bigCircle;

  QLinearGradient lg3(0, -radius / 2, 0, 0);
  lg3.setColorAt(0.0, QColor(255, 255, 255, 220));
  lg3.setColorAt(1.0, QColor(255, 255, 255, 30));
  painter.setBrush(lg3);
  painter.rotate(-20);
  painter.drawPath(highlight);
}

void PaintTriButton::drawHighlights(QPainter &painter, int radius) {

  int distance = 210;
  Layouts currentLayout = lightsLayout();
  if (currentLayout == lAuto) {
    currentLayout = width() > height() ? lHorizontal : lVertical;
  }

  for (int idx = 0; idx < VALID_COLOR_COUNT; ++idx) {
    painter.save();
    //    painter.translate(distance * (idx - 1), 0);
    (currentLayout == lHorizontal) ? painter.translate(distance * (idx - 1), 0)
                                   : painter.translate(0, distance * (idx - 1));
    drawHighlight(painter, radius);
    painter.restore();
  }
}

PaintTriButton::Color PaintTriButton::paintColor() const { return m_paintColor; }

void PaintTriButton::setPaintColor(Color newPaintColor) {
  if (m_paintColor == newPaintColor)
    return;
  m_paintColor = newPaintColor;
  emit paintColorChanged();
}

PaintTriButton::Layouts PaintTriButton::lightsLayout() const { return m_lightsLayout; }

void PaintTriButton::setLightsLayout(const Layouts &newLightsLayout) {
  if (m_lightsLayout == newLightsLayout)
    return;
  m_lightsLayout = newLightsLayout;
  emit lightsLayoutChanged();
}

void PaintTriButton::goNext() {
  int c = paintColor();
  setPaintColor(Color((++c) % VALID_COLOR_COUNT));
}

void PaintTriButton::goPrevious() {
  int c = paintColor();
  if (--c < 0) {
    c = VALID_COLOR_COUNT - 1;
  }

  setPaintColor(Color(c));
}

void PaintTriButton::goRed() { setPaintColor(bRed); }

void PaintTriButton::goGreen() { setPaintColor(bGreen); }

void PaintTriButton::goYellow() { setPaintColor(bYellow); }

void PaintTriButton::goGray() { setPaintColor(bGray); }

void PaintTriButton::turnOff() { goGray(); }

void PaintTriButton::turnOn() { goRed(); }
