

#ifndef AUDIOPLAYER_HPP
#define AUDIOPLAYER_HPP

// qt
#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

class AudioPlayer : public QObject
{
    Q_OBJECT

public:
    explicit AudioPlayer(QObject *parent = nullptr);

    // Estado
    bool isPlaying() const;
    qint64 position() const;
    qint64 duration() const;
    float volume() const;

public slots:
    void loadFile(const QUrl &url);
    void play();
    void pause();
    void stop();
    void seek(qint64 position);
    void setVolume(float volume);

signals:
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void mediaLoaded(const QString &fileName);
    void errorOccurred(const QString &error);

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
};

#endif
