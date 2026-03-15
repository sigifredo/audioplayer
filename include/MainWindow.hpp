

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

// own
#include <AudioPlayer.hpp>

// qt
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>

class PlaylistDelegate;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onOpenFolder();
    void onPlayPause();
    void onStop();
    void onNext();
    void onPrevious();
    void onSeek(int position);
    void onVolumeChanged(int value);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onMediaLoaded(const QString &fileName);
    void onErrorOccurred(const QString &error);
    void onQueueChanged(const QList<QUrl> &queue);
    void onCurrentIndexChanged(int index);
    void onListItemClicked(QListWidgetItem *item);

private:
    void setupUI();
    void setupConnections();
    QString formatTime(qint64 ms) const;

    // Audio
    AudioPlayer *m_player;

    // Widgets — controles
    QWidget *m_centralWidget;
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
