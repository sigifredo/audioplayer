

#ifndef PLAYLISTDELEGATE_HPP
#define PLAYLISTDELEGATE_HPP

// own
#include <QPainter>
#include <QStyledItemDelegate>

class PlaylistDelegate : public QStyledItemDelegate
{
public:
    explicit PlaylistDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setActiveIndex(const int &activeIndex);

private:
    int m_activeIndex;
};

inline void PlaylistDelegate::setActiveIndex(const int &activeIndex) { m_activeIndex = activeIndex; }

#endif
