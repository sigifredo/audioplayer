

// own
#include <MainWindow.hpp>
#include <PlaylistDelegate.hpp>

// qt
#include <QStatusBar>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_player(new AudioPlayer(this))
{
    setWindowTitle("Audio Player");
    setMinimumSize(420, 520);
    setupUI();
    setupConnections();
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // ── Título ───────────────────────────────────────────────────
    m_titleLabel = new QLabel("Sin archivo cargado", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setObjectName("titleLabel");

    // ── Seek slider ──────────────────────────────────────────────
    m_seekSlider = new QSlider(Qt::Horizontal, this);
    m_seekSlider->setObjectName("seekSlider");
    m_seekSlider->setRange(0, 0);

    // ── Tiempo ───────────────────────────────────────────────────
    m_timeLabel = new QLabel("00:00 / 00:00", this);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setObjectName("timeLabel");

    // ── Botones ──────────────────────────────────────────────────
    m_openButton = new QPushButton("📂", this);
    m_previousButton = new QPushButton("⏮", this);
    m_playPauseButton = new QPushButton("▶", this);
    m_stopButton = new QPushButton("⏹", this);
    m_nextButton = new QPushButton("⏭", this);

    for (auto *btn : {m_openButton, m_previousButton,
                      m_playPauseButton, m_stopButton, m_nextButton})
    {
        btn->setObjectName("controlButton");
        btn->setFixedSize(48, 48);
    }

    // ── Volumen ──────────────────────────────────────────────────
    m_volumeIcon = new QLabel("🔊", this);
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setObjectName("volumeSlider");
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(70);
    m_volumeSlider->setFixedWidth(100);

    // ── Playlist ─────────────────────────────────────────────────
    m_playlistLabel = new QLabel("Lista de reproducción", this);
    m_playlistLabel->setObjectName("playlistLabel");

    m_playlistWidget = new QListWidget(this);
    m_playlistWidget->setObjectName("playlistWidget");
    m_playlistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // ── Layouts ──────────────────────────────────────────────────
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->addStretch();
    controlsLayout->addWidget(m_openButton);
    controlsLayout->addWidget(m_previousButton);
    controlsLayout->addWidget(m_playPauseButton);
    controlsLayout->addWidget(m_stopButton);
    controlsLayout->addWidget(m_nextButton);
    controlsLayout->addStretch();

    QHBoxLayout *volumeLayout = new QHBoxLayout();
    volumeLayout->addStretch();
    volumeLayout->addWidget(m_volumeIcon);
    volumeLayout->addWidget(m_volumeSlider);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(12);
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addWidget(m_seekSlider);
    mainLayout->addWidget(m_timeLabel);
    mainLayout->addLayout(controlsLayout);
    mainLayout->addLayout(volumeLayout);
    mainLayout->addWidget(m_playlistLabel);
    mainLayout->addWidget(m_playlistWidget);

    m_playlistDelegate = new PlaylistDelegate(this);
    m_playlistWidget->setItemDelegate(m_playlistDelegate);
}

void MainWindow::setupConnections()
{
    // UI → Player
    connect(m_openButton, &QPushButton::clicked, this, &MainWindow::onOpenFolder);
    connect(m_previousButton, &QPushButton::clicked, this, &MainWindow::onPrevious);
    connect(m_playPauseButton, &QPushButton::clicked, this, &MainWindow::onPlayPause);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStop);
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNext);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);

    // Seek
    connect(m_seekSlider, &QSlider::sliderPressed, this, [this]
            { m_userSeeking = true; });
    connect(m_seekSlider, &QSlider::sliderReleased, this, [this]
            {
        m_userSeeking = false;
        onSeek(m_seekSlider->value()); });
    connect(m_seekSlider, &QSlider::valueChanged, this, &MainWindow::onSeek);

    // Playlist — doble clic para reproducir
    connect(m_playlistWidget, &QListWidget::itemClicked,
            this, &MainWindow::onListItemClicked);

    // Player → UI
    connect(m_player, &AudioPlayer::positionChanged,
            this, &MainWindow::onPositionChanged);
    connect(m_player, &AudioPlayer::durationChanged,
            this, &MainWindow::onDurationChanged);
    connect(m_player, &AudioPlayer::playbackStateChanged,
            this, &MainWindow::onPlaybackStateChanged);
    connect(m_player, &AudioPlayer::mediaLoaded,
            this, &MainWindow::onMediaLoaded);
    connect(m_player, &AudioPlayer::errorOccurred,
            this, &MainWindow::onErrorOccurred);
    connect(m_player, &AudioPlayer::queueChanged,
            this, &MainWindow::onQueueChanged);
    connect(m_player, &AudioPlayer::currentIndexChanged,
            this, &MainWindow::onCurrentIndexChanged);
}

// ── Slots ────────────────────────────────────────────────────────

void MainWindow::onOpenFolder()
{
    QString path = QFileDialog::getExistingDirectory(
        this, "Seleccionar carpeta de audio", "");
    if (!path.isEmpty())
        m_player->loadDirectory(path);
}

void MainWindow::onPlayPause()
{
    m_player->isPlaying() ? m_player->pause() : m_player->play();
}

void MainWindow::onStop()
{
    m_player->stop();
}

void MainWindow::onNext()
{
    m_player->next();
}

void MainWindow::onPrevious()
{
    m_player->previous();
}

void MainWindow::onSeek(int position)
{
    if (!m_userSeeking)
        m_player->seek((qint64)position);
}

void MainWindow::onVolumeChanged(int value)
{
    m_player->setVolume(value / 100.0f);
}

void MainWindow::onPositionChanged(qint64 position)
{
    if (!m_userSeeking)
        m_seekSlider->setValue((int)position);

    m_timeLabel->setText(
        QString("%1 / %2")
            .arg(formatTime(position))
            .arg(formatTime(m_player->duration())));
}

void MainWindow::onDurationChanged(qint64 duration)
{
    m_seekSlider->setRange(0, (int)duration);
}

void MainWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    m_playPauseButton->setText(
        state == QMediaPlayer::PlayingState ? "⏸" : "▶");
}

void MainWindow::onMediaLoaded(const QString &fileName)
{
    m_titleLabel->setText(fileName);
}

void MainWindow::onErrorOccurred(const QString &error)
{
    statusBar()->showMessage("Error: " + error, 5000);
}

void MainWindow::onQueueChanged(const QList<QUrl> &queue)
{
    m_playlistWidget->clear();
    for (const QUrl &url : queue)
    {
        QString name = QFileInfo(url.toLocalFile()).fileName();
        m_playlistWidget->addItem(name);
    }
    m_playlistLabel->setText(
        QString("Lista de reproducción (%1 pistas)").arg(queue.size()));
}

void MainWindow::onCurrentIndexChanged(int index)
{
    m_playlistDelegate->activeIndex = index;
    m_playlistWidget->viewport()->update(); // fuerza repintado
    m_playlistWidget->scrollToItem(m_playlistWidget->item(index));
}

void MainWindow::onListItemClicked(QListWidgetItem *item)
{
    int index = m_playlistWidget->row(item);
    m_player->playIndex(index);
}

QString MainWindow::formatTime(qint64 ms) const
{
    qint64 s = ms / 1000;
    return QString("%1:%2")
        .arg(s / 60, 2, 10, QChar('0'))
        .arg(s % 60, 2, 10, QChar('0'));
}
