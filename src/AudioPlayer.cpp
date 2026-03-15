

// own
#include <AudioPlayer.hpp>

// qt
#include <QFileInfo>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent), m_player(new QMediaPlayer(this)), m_audioOutput(new QAudioOutput(this))
{
    // Conectar el pipeline de audio
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.7f); // volumen inicial 70%

    // Propagar señales internas hacia afuera
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &AudioPlayer::playbackStateChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &AudioPlayer::durationChanged);
    connect(m_player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error, const QString &errorString)
            { emit errorOccurred(errorString); });
}

void AudioPlayer::loadFile(const QUrl &url)
{
    m_player->setSource(url);
    QString fileName = QFileInfo(url.toLocalFile()).fileName();
    emit mediaLoaded(fileName);
}

void AudioPlayer::play()
{
    m_player->play();
}

void AudioPlayer::pause()
{
    m_player->pause();
}

void AudioPlayer::stop()
{
    m_player->stop();
}

void AudioPlayer::seek(qint64 position)
{
    m_player->setPosition(position);
}

void AudioPlayer::setVolume(float volume)
{
    m_audioOutput->setVolume(volume);
}

bool AudioPlayer::isPlaying() const
{
    return m_player->playbackState() == QMediaPlayer::PlayingState;
}

qint64 AudioPlayer::position() const
{
    return m_player->position();
}

qint64 AudioPlayer::duration() const
{
    return m_player->duration();
}

float AudioPlayer::volume() const
{
    return m_audioOutput->volume();
}