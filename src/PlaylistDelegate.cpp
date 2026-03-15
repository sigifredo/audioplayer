

// own
#include <PlaylistDelegate.hpp>

PlaylistDelegate::PlaylistDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_activeIndex = -1;
}

void PlaylistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    painter->fillRect(opt.rect, (opt.state & QStyle::State_MouseOver) ? QColor("#0f3460") : QColor("#16213e"));

    // Bold para pista activa
    if (index.row() == m_activeIndex)
    {
        QFont f = opt.font;
        f.setBold(true);
        painter->setFont(f);
    }
    else
        painter->setFont(opt.font);

    painter->setPen((index.row() == m_activeIndex) ? QColor("#e94560") : QColor("#e0e0e0"));
    painter->drawText(opt.rect.adjusted(10, 0, -10, 0), Qt::AlignVCenter, opt.text);
}
