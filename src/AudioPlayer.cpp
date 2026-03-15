

// own
#include <AudioPlayer.hpp>

// qt
#include <QAudioOutput>
#include <QDirIterator>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent), m_player(new QMediaPlayer(this)), m_audioOutput(new QAudioOutput(this))
{
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.7f);

    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &AudioPlayer::onPlaybackStateChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &AudioPlayer::durationChanged);
    connect(m_player, &QMediaPlayer::errorOccurred,
            this, [this](QMediaPlayer::Error, const QString &errorString)
            { emit errorOccurred(errorString); });
}

// ── Formatos soportados ──────────────────────────────────────────
QStringList AudioPlayer::supportedExtensions()
{
    return {"*.mp3", "*.wav", "*.ogg", "*.flac", "*.m4a", "*.aac", "*.opus"};
}

// ── Carga de archivos ────────────────────────────────────────────
void AudioPlayer::loadFile(const QUrl &url)
{
    m_queue.clear();
    m_queue.append(url);
    loadQueue(0);
}

void AudioPlayer::loadDirectory(const QString &path)
{
    m_queue.clear();

    QDirIterator it(
        path,
        supportedExtensions(),
        QDir::Files,
        QDirIterator::Subdirectories);

    while (it.hasNext())
        m_queue.append(QUrl::fromLocalFile(it.next()));

    // Ordenar alfabéticamente por nombre de archivo
    std::sort(m_queue.begin(), m_queue.end(), [](const QUrl &a, const QUrl &b)
              { return QFileInfo(a.toLocalFile()).fileName().toLower() < QFileInfo(b.toLocalFile()).fileName().toLower(); });

    emit queueChanged(m_queue);

    if (!m_queue.isEmpty())
        loadQueue(0);
}

void AudioPlayer::loadQueue(int index)
{
    if (index < 0 || index >= m_queue.size())
        return;

    m_currentIndex = index;
    m_player->setSource(m_queue.at(index));

    QString fileName = QFileInfo(m_queue.at(index).toLocalFile()).fileName();
    emit mediaLoaded(fileName);
    emit currentIndexChanged(m_currentIndex);

    m_player->play();
}

// ── Control de reproducción ──────────────────────────────────────
void AudioPlayer::playIndex(int index)
{
    loadQueue(index);
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

void AudioPlayer::next()
{
    if (m_queue.isEmpty())
        return;

    // Loop: si estamos al final, vuelve al inicio
    int nextIndex = (m_currentIndex + 1) % m_queue.size();
    loadQueue(nextIndex);
}

void AudioPlayer::previous()
{
    if (m_queue.isEmpty())
        return;

    // Si llevamos más de 3s reproducidos, reinicia la pista actual
    if (m_player->position() > 3000)
    {
        m_player->setPosition(0);
        return;
    }

    // Loop inverso
    int prevIndex = (m_currentIndex - 1 + m_queue.size()) % m_queue.size();
    loadQueue(prevIndex);
}

void AudioPlayer::seek(qint64 position)
{
    m_player->setPosition(position);
}

void AudioPlayer::setVolume(float volume)
{
    m_audioOutput->setVolume(volume);
}

// ── Auto-avance al terminar una pista ───────────────────────────
void AudioPlayer::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    emit playbackStateChanged(state);

    if (state == QMediaPlayer::StoppedState && m_player->mediaStatus() == QMediaPlayer::EndOfMedia)
    {
        next(); // loop automático
    }
}

// ── Getters ──────────────────────────────────────────────────────
bool AudioPlayer::isPlaying() const
{
    return m_player->playbackState() == QMediaPlayer::PlayingState;
}

qint64 AudioPlayer::position() const { return m_player->position(); }
qint64 AudioPlayer::duration() const { return m_player->duration(); }
float AudioPlayer::volume() const { return m_audioOutput->volume(); }
int AudioPlayer::currentIndex() const { return m_currentIndex; }
int AudioPlayer::queueSize() const { return m_queue.size(); }
