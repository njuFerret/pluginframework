#ifndef PAINTTRIBUTTON_H
#define PAINTTRIBUTTON_H

#include <QPushButton>

class PaintTriButton : public QPushButton {
  Q_OBJECT
  Q_PROPERTY(Color paintColor READ paintColor WRITE setPaintColor NOTIFY paintColorChanged FINAL)
  Q_PROPERTY(Layouts lightsLayout READ lightsLayout WRITE setLightsLayout NOTIFY lightsLayoutChanged FINAL)
public:
  explicit PaintTriButton(QWidget *parent = nullptr);
  enum Color { bRed = 0, bYellow, bGreen, bGray };
  Q_ENUM(Color)
  enum Layouts { lHorizontal = 0, lVertical, lAuto };
  Q_ENUM(Layouts);

  Color paintColor() const;
  void setPaintColor(Color newPaintColor);

  Layouts lightsLayout() const;
  void setLightsLayout(const Layouts &newLightsLayout);

public slots:

  void goNext();
  void goPrevious();
  void goRed();
  void goGreen();
  void goYellow();
  void goGray();
  void turnOff();
  void turnOn();

signals:

  // QWidget interface
  void paintColorChanged();

  void lightsLayoutChanged();

protected:
  virtual void paintEvent(QPaintEvent *event) override;

private:
  /***********************************************************
   * @brief setupPainter Painter配置
   * @param painter
   **********************************************************/
  void setupPainter(QPainter &painter);
  /***********************************************************
   * @brief scaledShortSide   根据横向还是纵向布局，判断绘制缩放边长
   * @param layout 布局模式：自动布局、横向布局、纵向布局
   * @return 等效短边长度
   *  根据横向还是纵向布局，判断绘制缩放边长
   *  变换基本思想:
   *  1) 如果是等比变化，只需要判断最小边长，以最小边长进行缩放，
   *     例如： side = qMin(w,h);
   *  2) 如果是非等比变换，例如保持长边:短边=k:1时，将长边除以k之后再与
   *     短边取较小值进行等比变换，如本处长短比=3，则
   *           side = qMin(长边/3.0,短边)
   *  3) 水平布局时长边为宽度，垂直布局时长边为高度
   *  返回值为等效边长最小值
   *
   **********************************************************/
  float scaledShortSide(Layouts layout);
  /***********************************************************
   * @brief drawFrame 绘制外部圆环部分
   * @param painter
   **********************************************************/
  void drawFrame(QPainter &painter);
  /***********************************************************
   * @brief drawFrames 使用坐标平移方法绘制三个按钮
   * @param painter
   * @param distance   色块间距
   **********************************************************/
  void drawFrames(QPainter &painter, int distance);

  /***********************************************************
   * @brief drawInner 绘制内部彩色块部分
   * @param painter
   * @param radius
   * @param index 当前绘制索引(0:红色块, 1: 黄色色块, 2:绿色色块, 3: 灰色色块)
   **********************************************************/
  void drawInner(QPainter &painter, int radius, int index);
  void drawInners(QPainter &painter, int radius);
  /***********************************************************
   * @brief drawHighlight 绘制颜色高光部分
   * @param painter
   * @param radius
   **********************************************************/
  void drawHighlight(QPainter &painter, int radius);
  void drawHighlights(QPainter &painter, int radius);

  Color m_paintColor = bRed;
  Layouts m_lightsLayout = lAuto;
};

#endif        // PAINTTRIBUTTON_H
