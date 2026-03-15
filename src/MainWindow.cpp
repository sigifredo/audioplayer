

// own
#include <MainWindow.hpp>

// qt
#include <QStatusBar>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_player(new AudioPlayer(this))
{
    setWindowTitle("Audio Player");
    setMinimumSize(400, 220);
    setupUI();
    setupConnections();
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // --- Título ---
    m_titleLabel = new QLabel("Sin archivo cargado", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setObjectName("titleLabel");

    // --- Slider de posición ---
    m_seekSlider = new QSlider(Qt::Horizontal, this);
    m_seekSlider->setObjectName("seekSlider");
    m_seekSlider->setRange(0, 0);

    // --- Tiempo ---
    m_timeLabel = new QLabel("00:00 / 00:00", this);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setObjectName("timeLabel");

    // --- Botones de control ---
    m_openButton = new QPushButton("📂", this);
    m_playPauseButton = new QPushButton("▶", this);
    m_stopButton = new QPushButton("⏹", this);

    m_openButton->setObjectName("controlButton");
    m_playPauseButton->setObjectName("controlButton");
    m_stopButton->setObjectName("controlButton");

    m_openButton->setFixedSize(48, 48);
    m_playPauseButton->setFixedSize(48, 48);
    m_stopButton->setFixedSize(48, 48);

    // --- Volumen ---
    m_volumeIcon = new QLabel("🔊", this);
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setObjectName("volumeSlider");
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(70);
    m_volumeSlider->setFixedWidth(100);

    // --- Layouts ---
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->addStretch();
    controlsLayout->addWidget(m_openButton);
    controlsLayout->addWidget(m_playPauseButton);
    controlsLayout->addWidget(m_stopButton);
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
}

void MainWindow::setupConnections()
{
    // UI → Player
    connect(m_openButton, &QPushButton::clicked, this, &MainWindow::onOpenFile);
    connect(m_playPauseButton, &QPushButton::clicked, this, &MainWindow::onPlayPause);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStop);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);

    // Seek: distinguir arrastre manual vs. actualización del player
    connect(m_seekSlider, &QSlider::sliderPressed, this, [this]
            { m_userSeeking = true; });
    connect(m_seekSlider, &QSlider::sliderReleased, this, [this]
            {
        m_userSeeking = false;
        m_player->seek((qint64)m_seekSlider->value()); });

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
}

void MainWindow::onOpenFile()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Abrir archivo de audio", "",
        "Audio (*.mp3 *.wav *.ogg *.flac *.m4a)");
    if (!path.isEmpty())
        m_player->loadFile(QUrl::fromLocalFile(path));
}

void MainWindow::onPlayPause()
{
    m_player->isPlaying() ? m_player->pause() : m_player->play();
}

void MainWindow::onStop()
{
    m_player->stop();
}

void MainWindow::onVolumeChanged(int value)
{
    m_player->setVolume(value / 100.0f);
}

void MainWindow::onPositionChanged(qint64 position)
{
    if (!m_userSeeking)
        m_seekSlider->setValue((int)position);

    qint64 duration = m_player->duration();
    m_timeLabel->setText(
        QString("%1 / %2")
            .arg(formatTime(position))
            .arg(formatTime(duration)));
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
    m_player->play();
}

void MainWindow::onErrorOccurred(const QString &error)
{
    statusBar()->showMessage("Error: " + error, 5000);
}

QString MainWindow::formatTime(qint64 ms) const
{
    qint64 s = ms / 1000;
    return QString("%1:%2")
        .arg(s / 60, 2, 10, QChar('0'))
        .arg(s % 60, 2, 10, QChar('0'));
}
