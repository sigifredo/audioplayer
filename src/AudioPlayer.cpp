

// own
#include <AudioPlayer.hpp>

// qt
#include <QAudioOutput>
#include <QDirIterator>
#include <QFileInfo>

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent), m_player(new QMediaPlayer(this)), m_audioOutput(new QAudioOutput(this))
{
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(1.0f);

    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &AudioPlayer::onPlaybackStateChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &AudioPlayer::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &AudioPlayer::durationChanged);
    connect(m_player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error, const QString &errorString)
            { emit errorOccurred(errorString); });
}

// ── Helpers ──────────────────────────────────────────────────────
QStringList AudioPlayer::supportedExtensions()
{
    return {"*.mp3", "*.wav", "*.ogg", "*.flac", "*.m4a", "*.aac", "*.opus"};
}

QString AudioPlayer::nameForIndex(int index) const
{
    if (index < 0 || index >= m_queue.size())
        return {};

    // Si tenemos nombre explícito (Android content:// URIs) usarlo
    if (index < m_names.size() && !m_names.at(index).isEmpty())
        return m_names.at(index);

    // Linux: extraer nombre del path local
    QString localPath = m_queue.at(index).toLocalFile();

    if (!localPath.isEmpty())
        return QFileInfo(localPath).fileName();

    // Fallback: último segmento de la URL
    return m_queue.at(index).fileName();
}

// ── Carga ────────────────────────────────────────────────────────
void AudioPlayer::loadFile(const QUrl &url)
{
    m_queue.clear();
    m_names.clear();
    m_queue.append(url);
    m_names.append(nameForIndex(0));

    emit queueChanged(m_queue, m_names);
    loadQueue(0);
}

void AudioPlayer::loadDirectory(const QString &path)
{
    m_queue.clear();
    m_names.clear();

    QDirIterator it(path, supportedExtensions(),
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext())
        m_queue.append(QUrl::fromLocalFile(it.next()));

    // Ordenar alfabéticamente
    std::sort(m_queue.begin(), m_queue.end(), [](const QUrl &a, const QUrl &b)
              { return QFileInfo(a.toLocalFile()).fileName().toLower() < QFileInfo(b.toLocalFile()).fileName().toLower(); });

    // Poblar nombres desde paths locales
    for (const QUrl &url : m_queue)
        m_names.append(QFileInfo(url.toLocalFile()).fileName());

    emit queueChanged(m_queue, m_names);

    if (!m_queue.isEmpty())
        loadQueue(0);
}

void AudioPlayer::loadUrls(const QList<QUrl> &urls, const QStringList &names)
{
    m_queue.clear();
    m_names.clear();

    for (int i = 0; i < urls.size(); i++)
    {
        m_queue.append(urls.at(i));
        m_names.append(i < names.size() ? names.at(i) : urls.at(i).fileName());
    }

    emit queueChanged(m_queue, m_names);

    if (!m_queue.isEmpty())
        loadQueue(0);
}

void AudioPlayer::loadQueue(int index)
{
    if (index < 0 || index >= m_queue.size())
        return;

    m_currentIndex = index;
    m_player->setSource(m_queue.at(index));

    emit mediaLoaded(nameForIndex(index));
    emit currentIndexChanged(m_currentIndex);

    m_player->play();
}

// ── Control ──────────────────────────────────────────────────────
void AudioPlayer::playIndex(int index) { loadQueue(index); }
void AudioPlayer::play() { m_player->play(); }
void AudioPlayer::pause() { m_player->pause(); }
void AudioPlayer::stop() { m_player->stop(); }

void AudioPlayer::next()
{
    if (m_queue.isEmpty())
        return;
    loadQueue((m_currentIndex + 1) % m_queue.size());
}

void AudioPlayer::previous()
{
    if (m_queue.isEmpty())
        return;

    if (m_player->position() > 3000)
    {
        m_player->setPosition(0);
        return;
    }

    loadQueue((m_currentIndex - 1 + m_queue.size()) % m_queue.size());
}

void AudioPlayer::seek(qint64 position) { m_player->setPosition(position); }
void AudioPlayer::setVolume(float volume) { m_audioOutput->setVolume(volume); }

void AudioPlayer::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    emit playbackStateChanged(state);

    if (state == QMediaPlayer::StoppedState && m_player->mediaStatus() == QMediaPlayer::EndOfMedia)
        next();
}

// ── Getters ──────────────────────────────────────────────────────
bool AudioPlayer::isPlaying() const { return m_player->playbackState() == QMediaPlayer::PlayingState; }
qint64 AudioPlayer::position() const { return m_player->position(); }
qint64 AudioPlayer::duration() const { return m_player->duration(); }
float AudioPlayer::volume() const { return m_audioOutput->volume(); }
int AudioPlayer::currentIndex() const { return m_currentIndex; }
int AudioPlayer::queueSize() const { return m_queue.size(); }
