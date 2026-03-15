

// own
#include <MainWindow.hpp>
#include <AudioPlayer.hpp>
#include <PlaylistDelegate.hpp>

// qt
#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_player(new AudioPlayer(this))
{
    setWindowTitle("Audio Player");
    setMinimumSize(420, 520);
    setupUI();
    setupConnections();
}

void MainWindow::onCurrentIndexChanged(int index)
{
    m_playlistDelegate->setActiveIndex(index);
    m_playlistWidget->viewport()->update(); // fuerza repintado
    m_playlistWidget->scrollToItem(m_playlistWidget->item(index));
}

void MainWindow::onDurationChanged(qint64 duration)
{
    m_seekSlider->setRange(0, (int)duration);
}

void MainWindow::onErrorOccurred(const QString &error)
{
    statusBar()->showMessage("Error: " + error, 5000);
}

void MainWindow::onListItemClicked(QListWidgetItem *item)
{
    int index = m_playlistWidget->row(item);
    m_player->playIndex(index);
}

void MainWindow::onMediaLoaded(const QString &fileName)
{
    m_titleLabel->setText(fileName);
}

void MainWindow::onNext()
{
    m_player->next();
}

void MainWindow::onOpenFolder()
{
    QString path = QFileDialog::getExistingDirectory(this, "Seleccionar carpeta de audio", "");

    if (!path.isEmpty())
        m_player->loadDirectory(path);
}

void MainWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    m_playPauseButton->setText(
        state == QMediaPlayer::PlayingState ? "⏸" : "▶");
}

void MainWindow::onPlayPause()
{
    m_player->isPlaying() ? m_player->pause() : m_player->play();
}

void MainWindow::onPositionChanged(qint64 position)
{
    if (!m_userSeeking)
    {
        m_seekSlider->blockSignals(true);
        m_seekSlider->setValue((int)position);
        m_seekSlider->blockSignals(false);
    }

    m_timeLabel->setText(
        QString("%1 / %2")
            .arg(formatTime(position))
            .arg(formatTime(m_player->duration())));
}

void MainWindow::onPrevious()
{
    m_player->previous();
}

void MainWindow::onQueueChanged(const QList<QUrl> &queue)
{
    m_playlistWidget->clear();

    for (const QUrl &url : queue)
    {
        m_playlistWidget->addItem(QFileInfo(url.toLocalFile()).fileName());
    }

    m_playlistLabel->setText(QString("Lista de reproducción (%1 pistas)").arg(queue.size()));
}

void MainWindow::onSeek(int position)
{
    if (!m_userSeeking)
        m_player->seek((qint64)position);
}

void MainWindow::onStop()
{
    m_player->stop();
}

void MainWindow::onVolumeChanged(int value)
{
    m_player->setVolume(value / 100.0f);
}

QString MainWindow::formatTime(qint64 ms) const
{
    qint64 s = ms / 1000;
    return QString("%1:%2")
        .arg(s / 60, 2, 10, QChar('0'))
        .arg(s % 60, 2, 10, QChar('0'));
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
    connect(m_player, &AudioPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_player, &AudioPlayer::durationChanged, this, &MainWindow::onDurationChanged);
    connect(m_player, &AudioPlayer::playbackStateChanged, this, &MainWindow::onPlaybackStateChanged);
    connect(m_player, &AudioPlayer::mediaLoaded, this, &MainWindow::onMediaLoaded);
    connect(m_player, &AudioPlayer::errorOccurred, this, &MainWindow::onErrorOccurred);
    connect(m_player, &AudioPlayer::queueChanged, this, &MainWindow::onQueueChanged);
    connect(m_player, &AudioPlayer::currentIndexChanged, this, &MainWindow::onCurrentIndexChanged);
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, centralWidget);

    // ── Título ───────────────────────────────────────────────────
    m_titleLabel = new QLabel("Sin archivo cargado", centralWidget);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setObjectName("titleLabel");

    // ── Seek slider ──────────────────────────────────────────────
    m_seekSlider = new QSlider(Qt::Horizontal, centralWidget);
    m_seekSlider->setObjectName("seekSlider");
    m_seekSlider->setRange(0, 0);

    // ── Tiempo ───────────────────────────────────────────────────
    m_timeLabel = new QLabel("00:00 / 00:00", centralWidget);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setObjectName("timeLabel");

    // ── Botones ──────────────────────────────────────────────────
    QWidget *panelWidget = new QWidget(centralWidget);
    {
        QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, panelWidget);

        m_openButton = new QPushButton("📂", panelWidget);
        m_previousButton = new QPushButton("⏮", panelWidget);
        m_playPauseButton = new QPushButton("▶", panelWidget);
        m_stopButton = new QPushButton("⏹", panelWidget);
        m_nextButton = new QPushButton("⏭", panelWidget);

        for (auto *btn : {m_openButton, m_previousButton, m_playPauseButton, m_stopButton, m_nextButton})
        {
            btn->setObjectName("controlButton");
            btn->setFixedSize(48, 48);
        }

        layout->addStretch();
        layout->addWidget(m_openButton);
        layout->addWidget(m_previousButton);
        layout->addWidget(m_playPauseButton);
        layout->addWidget(m_stopButton);
        layout->addWidget(m_nextButton);
        layout->addStretch();
    }

    // ── Volumen ──────────────────────────────────────────────────
    QWidget *volumeWidget = new QWidget(centralWidget);
    {
        QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, volumeWidget);

        m_volumeIcon = new QLabel("🔊", volumeWidget);
        m_volumeSlider = new QSlider(Qt::Horizontal, volumeWidget);
        m_volumeSlider->setObjectName("volumeSlider");
        m_volumeSlider->setRange(0, 100);
        m_volumeSlider->setValue(70);
        m_volumeSlider->setFixedWidth(100);

        layout->addStretch();
        layout->addWidget(m_volumeIcon);
        layout->addWidget(m_volumeSlider);
    }

    // ── Playlist ─────────────────────────────────────────────────
    m_playlistLabel = new QLabel("Lista de reproducción", centralWidget);
    m_playlistLabel->setObjectName("playlistLabel");

    m_playlistWidget = new QListWidget(centralWidget);
    m_playlistWidget->setObjectName("playlistWidget");
    m_playlistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // ── Layouts ──────────────────────────────────────────────────
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_seekSlider);
    layout->addWidget(m_timeLabel);
    layout->addWidget(panelWidget);
    layout->addWidget(volumeWidget);
    layout->addWidget(m_playlistLabel);
    layout->addWidget(m_playlistWidget);

    m_playlistDelegate = new PlaylistDelegate(this);
    m_playlistWidget->setItemDelegate(m_playlistDelegate);

    setCentralWidget(centralWidget);
}
