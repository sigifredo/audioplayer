

#ifndef AUDIOPLAYER_HPP
#define AUDIOPLAYER_HPP

// qt
#include <QMediaPlayer>
#include <QUrl>
#include <QList>
#include <QStringList>

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
    void loadDirectory(const QString &path);
    void loadFile(const QUrl &url);
    void loadUrls(const QList<QUrl> &urls, const QStringList &names = {});
    void next();
    void pause();
    void play();
    void playIndex(int index);
    void previous();
    void seek(qint64 position);
    void setVolume(float volume);
    void stop();

signals:
    void playbackStateChanged(QMediaPlayer::PlaybackState state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void mediaLoaded(const QString &fileName);
    void errorOccurred(const QString &error);
    void queueChanged(const QList<QUrl> &queue, const QStringList &names);
    void currentIndexChanged(int index);
    void playbackFinished();

private slots:
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);

private:
    void loadQueue(int index);
    QString nameForIndex(int index) const;

    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    QList<QUrl> m_queue;
    QStringList m_names;
    int m_currentIndex = -1;
};

#endif
