

#ifndef AUDIOPLAYER_HPP
#define AUDIOPLAYER_HPP

// qt
#include <QMediaPlayer>

class QAudioOutput;

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
    int currentIndex() const;
    int queueSize() const;

    // Formatos soportados
    static QStringList supportedExtensions();

public slots:
    void loadFile(const QUrl &url);
    void loadDirectory(const QString &path);
    void playIndex(int index);
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void seek(qint64 position);
    void setVolume(float volume);

signals:
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void mediaLoaded(const QString &fileName);
    void errorOccurred(const QString &error);
    void queueChanged(const QList<QUrl> &queue);
    void currentIndexChanged(int index);
    void playbackFinished();

private slots:
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);

private:
    void loadQueue(int index);

    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    QList<QUrl> m_queue;
    int m_currentIndex = -1;
};

#endif
