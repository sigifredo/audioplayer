

#ifndef SEEKSLIDER_HPP
#define SEEKSLIDER_HPP

// qt
#include <QSlider>

class QMouseEvent;

class SeekSlider : public QSlider
{
    Q_OBJECT

public:
    explicit SeekSlider(QWidget *parent = nullptr) : QSlider(Qt::Horizontal, parent) {}

signals:
    void seekRequested(int value);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif
