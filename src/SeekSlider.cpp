
// own
#include <SeekSlider.hpp>

// qt
#include <QMouseEvent>
#include <QStyle>

void SeekSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int value = QStyle::sliderValueFromPosition(
            minimum(), maximum(),
            event->pos().x(), width());

        setValue(value);
        emit seekRequested(value);
        event->accept();
        return;
    }

    QSlider::mouseMoveEvent(event);
}

void SeekSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        int value = QStyle::sliderValueFromPosition(minimum(), maximum(), event->pos().x(), width());

        setValue(value);
        emit seekRequested(value);
        event->accept();
        return;
    }

    QSlider::mousePressEvent(event);
}
