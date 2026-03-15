

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

// qt
#include <QMainWindow>
#include <QMediaPlayer>

class AudioPlayer;
class PlaylistDelegate;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSlider;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onCurrentIndexChanged(int index);
    void onDurationChanged(qint64 duration);
    void onErrorOccurred(const QString &error);
    void onListItemClicked(QListWidgetItem *item);
    void onMediaLoaded(const QString &fileName);
    void onNext();
    void onOpenFolder();
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onPlayPause();
    void onPositionChanged(qint64 position);
    void onPrevious();
    void onQueueChanged(const QList<QUrl> &queue);
    void onSeek(int position);
    void onStop();
    void onVolumeChanged(int value);

private:
    QString formatTime(qint64 ms) const;
    void setupConnections();
    void setupUI();

    // Audio
    AudioPlayer *m_player;

    // Widgets — controles
    QLabel *m_titleLabel;
    QLabel *m_timeLabel;
    QSlider *m_seekSlider;
    QPushButton *m_openButton;
    QPushButton *m_previousButton;
    QPushButton *m_playPauseButton;
    QPushButton *m_stopButton;
    QPushButton *m_nextButton;
    QSlider *m_volumeSlider;
    QLabel *m_volumeIcon;

    // Widgets — lista
    QListWidget *m_playlistWidget;
    QLabel *m_playlistLabel;

    // Estado
    bool m_userSeeking = false;
    PlaylistDelegate *m_playlistDelegate;
};

#endif
