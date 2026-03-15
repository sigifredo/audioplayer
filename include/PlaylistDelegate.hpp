#include <QStyledItemDelegate>
#include <QPainter>

class PlaylistDelegate : public QStyledItemDelegate
{
public:
    explicit PlaylistDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    int activeIndex = -1;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // Fondo
        painter->fillRect(opt.rect,
                          (opt.state & QStyle::State_MouseOver)
                              ? QColor("#0f3460")
                              : QColor("#16213e"));

        // Color del texto
        QColor textColor = (index.row() == activeIndex)
                               ? QColor("#e94560")
                               : QColor("#e0e0e0");

        // Bold para pista activa
        if (index.row() == activeIndex)
        {
            QFont f = opt.font;
            f.setBold(true);
            painter->setFont(f);
        }
        else
        {
            painter->setFont(opt.font);
        }

        painter->setPen(textColor);
        painter->drawText(
            opt.rect.adjusted(10, 0, -10, 0),
            Qt::AlignVCenter,
            opt.text);
    }
};