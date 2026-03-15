

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

// own
#include <AudioPlayer.hpp>

// qt
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onOpenFile();
    void onPlayPause();
    void onStop();
    void onSeek(int position);
    void onVolumeChanged(int value);
    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onMediaLoaded(const QString &fileName);
    void onErrorOccurred(const QString &error);

private:
    void setupUI();
    void setupConnections();
    QString formatTime(qint64 ms) const;

    // Audio
    AudioPlayer *m_player;

    // Widgets principales
    QWidget *m_centralWidget;
    QLabel *m_titleLabel;
    QLabel *m_timeLabel;
    QSlider *m_seekSlider;
    QPushButton *m_openButton;
    QPushButton *m_playPauseButton;
    QPushButton *m_stopButton;
    QSlider *m_volumeSlider;
    QLabel *m_volumeIcon;

    // Estado
    bool m_userSeeking = false;
};

#endif