

// own
#include <MainWindow.hpp>
#include <AudioPlayer.hpp>
#include <PlaylistDelegate.hpp>

// qt
#include <QBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QStatusBar>

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QJniEnvironment>
#include <QOperatingSystemVersion>
#include <private/qandroidextras_p.h>

static void requestAndroidPermissions(MainWindow *mainWindow)
{
    QString permission;

    if (QOperatingSystemVersion::current() >= QOperatingSystemVersion::Android13)
        permission = "android.permission.READ_MEDIA_AUDIO";
    else
        permission = "android.permission.READ_EXTERNAL_STORAGE";

    QJniObject activity = QJniObject::callStaticObjectMethod(
        "org/qtproject/qt/android/QtNative",
        "activity",
        "()Landroid/app/Activity;");

    QJniObject jPermission = QJniObject::fromString(permission);

    jint result = activity.callMethod<jint>(
        "checkSelfPermission",
        "(Ljava/lang/String;)I",
        jPermission.object<jstring>());

    if (result != 0)
    {
        QJniObject::callStaticMethod<void>(
            "org/qtproject/qt/android/QtNative",
            "requestPermissions",
            "([Ljava/lang/String;I)V");
        mainWindow->statusBar()->showMessage("Solicitando permiso de audio...", 3000);
    }
}
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_player(new AudioPlayer(this))
{
    setWindowTitle("Audio Player");
    setupUI();
    setupConnections();

#ifdef Q_OS_ANDROID
    requestAndroidPermissions(this);
#endif
}

void MainWindow::onCurrentIndexChanged(int index)
{
    m_playlistDelegate->setActiveIndex(index);
    m_playlistWidget->viewport()->update();
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
#ifdef Q_OS_ANDROID
    QJniObject intent("android/content/Intent");
    QJniObject action = QJniObject::fromString("android.intent.action.OPEN_DOCUMENT_TREE");
    intent.callObjectMethod("setAction",
                            "(Ljava/lang/String;)Landroid/content/Intent;",
                            action.object<jstring>());

    QtAndroidPrivate::startActivity(intent, 43, [this](int requestCode, int resultCode, const QJniObject &data)
                                    {
        if (requestCode != 43 || resultCode != -1 || !data.isValid())
            return;

        QJniObject treeUri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
        if (!treeUri.isValid()) return;

        QJniObject context = QNativeInterface::QAndroidApplication::context();

        QJniObject docFile = QJniObject::callStaticObjectMethod(
            "androidx/documentfile/provider/DocumentFile",
            "fromTreeUri",
            "(Landroid/content/Context;Landroid/net/Uri;)Landroidx/documentfile/provider/DocumentFile;",
            context.object(),
            treeUri.object());

        if (!docFile.isValid()) return;

        QJniObject files = docFile.callObjectMethod(
            "listFiles",
            "()[Landroidx/documentfile/provider/DocumentFile;");

        if (!files.isValid()) return;

        QList<QUrl> urls;
        QStringList names;
        QJniEnvironment env;
        jobjectArray array = files.object<jobjectArray>();
        jsize count = env->GetArrayLength(array);

        for (jsize i = 0; i < count; i++) {
            QJniObject file(env->GetObjectArrayElement(array, i));
            if (!file.isValid()) continue;

            QJniObject mimeType = file.callObjectMethod("getType", "()Ljava/lang/String;");
            if (!mimeType.isValid()) continue;
            if (!mimeType.toString().startsWith("audio/")) continue;

            QJniObject uri = file.callObjectMethod("getUri", "()Landroid/net/Uri;");
            if (!uri.isValid()) continue;

            QJniObject nameObj = file.callObjectMethod("getName", "()Ljava/lang/String;");
            QString name = nameObj.isValid() ? nameObj.toString() : QString();

            QString uriStr = uri.callObjectMethod("toString", "()Ljava/lang/String;").toString();
            urls.append(QUrl(uriStr));
            names.append(name);
        }

        if (!urls.isEmpty())
            m_player->loadUrls(urls, names);
        else
            this->statusBar()->showMessage("No se encontraron archivos de audio", 3000); });
#else
    QString path = QFileDialog::getExistingDirectory(
        this, "Seleccionar carpeta de audio", "");
    if (!path.isEmpty())
        m_player->loadDirectory(path);
#endif
}

void MainWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    m_playPauseButton->setText(state == QMediaPlayer::PlayingState ? "⏸" : "▶");
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

void MainWindow::onQueueChanged(const QList<QUrl> &queue, const QStringList &names)
{
    m_playlistWidget->clear();

    for (int i = 0; i < queue.size(); i++)
    {
        QString name;
        if (i < names.size() && !names.at(i).isEmpty())
            name = names.at(i);
        else
        {
            QString localPath = queue.at(i).toLocalFile();
            name = localPath.isEmpty()
                       ? queue.at(i).fileName()
                       : QFileInfo(localPath).fileName();
        }
        m_playlistWidget->addItem(name);
    }

    m_playlistLabel->setText(
        QString("Lista de reproducción (%1 pistas)").arg(queue.size()));
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
    connect(m_openButton, &QPushButton::clicked, this, &MainWindow::onOpenFolder);
    connect(m_previousButton, &QPushButton::clicked, this, &MainWindow::onPrevious);
    connect(m_playPauseButton, &QPushButton::clicked, this, &MainWindow::onPlayPause);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStop);
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNext);
#ifndef Q_OS_ANDROID
    connect(m_volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);
#endif

    connect(m_seekSlider, &QSlider::sliderPressed, this, [this]
            { m_userSeeking = true; });
    connect(m_seekSlider, &QSlider::sliderReleased, this, [this]
            {
                m_userSeeking = false;
                onSeek(m_seekSlider->value()); });
    connect(m_seekSlider, &QSlider::valueChanged, this, &MainWindow::onSeek);

    connect(m_playlistWidget, &QListWidget::itemClicked,
            this, &MainWindow::onListItemClicked);

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

#ifndef Q_OS_ANDROID
    // ── Volumen ──────────────────────────────────────────────────
    QWidget *volumeWidget = new QWidget(centralWidget);
    {
        QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight, volumeWidget);

        QLabel *volumeIcon = new QLabel("🔊", volumeWidget);
        m_volumeSlider = new QSlider(Qt::Horizontal, volumeWidget);

        m_volumeSlider->setObjectName("volumeSlider");
        m_volumeSlider->setRange(0, 100);
        m_volumeSlider->setValue(100);
        m_volumeSlider->setFixedWidth(100);

        layout->addStretch();
        layout->addWidget(volumeIcon);
        layout->addWidget(m_volumeSlider);
    }
#endif

    // ── Playlist ─────────────────────────────────────────────────
    m_playlistLabel = new QLabel("Lista de reproducción", centralWidget);
    m_playlistLabel->setObjectName("playlistLabel");

    m_playlistWidget = new QListWidget(centralWidget);
    m_playlistWidget->setObjectName("playlistWidget");
    m_playlistWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_playlistDelegate = new PlaylistDelegate(this);
    m_playlistWidget->setItemDelegate(m_playlistDelegate);

    // ── Layout principal ─────────────────────────────────────────
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_seekSlider);
    layout->addWidget(m_timeLabel);
    layout->addWidget(panelWidget);
#ifndef Q_OS_ANDROID
    layout->addWidget(volumeWidget);
#endif
    layout->addWidget(m_playlistLabel);
    layout->addWidget(m_playlistWidget);

    setCentralWidget(centralWidget);
}
