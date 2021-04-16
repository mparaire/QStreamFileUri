#include "mainwindow.h"

#include "mediaplayer.h"

#include <QLayout>
#include <QTime>
#include <QPushButton>

#define DEFAULT_FRAMERATE 25.0

SelectionFileWidget::SelectionFileWidget(QWidget* parent) : QDialog(parent) {
    QHBoxLayout* appLayout = new QHBoxLayout;
    QPushButton* butFileDialog = new QPushButton("File");
    QPushButton* butUrlDialog = new QPushButton("Url");
    connect(butFileDialog, SIGNAL(clicked()), this, SLOT(fileDialog()));
    connect(butUrlDialog, SIGNAL(clicked()), this, SLOT(urlDialog()));
    appLayout->addWidget(butFileDialog);
    appLayout->addWidget(butUrlDialog);
    setLayout(appLayout);
}

void SelectionFileWidget::fileDialog() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a file"), ".");
    if (!fileName.isEmpty()) {
        emit uriSelected(fileName);
    }
    close();
}
void SelectionFileWidget::urlDialog(){
    UrlWidget dialog;
    connect(&dialog, SIGNAL(uriSelected(QString)), this, SIGNAL(uriSelected(QString)));
    dialog.setModal(false);
    dialog.exec();
    close();
}


UrlWidget::UrlWidget(QWidget* parent) : QDialog(parent), m_url(nullptr) {
    QVBoxLayout* appLayout = new QVBoxLayout;
    m_url = new QLineEdit;
    QPushButton* butOkDialog = new QPushButton("Ok");
    connect(butOkDialog, SIGNAL(clicked()), this, SLOT(closeDialog()));
    appLayout->addWidget(m_url);
    appLayout->addWidget(butOkDialog);
    setLayout(appLayout);
    setWindowTitle(QStringLiteral("Url"));
}

void UrlWidget::closeDialog() {
     emit uriSelected(m_url->text());
     close();
}



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_positionSlider(nullptr)
    , m_positionLabel(nullptr)
    , m_frameNLabel(nullptr)
    , m_frameRateSlider(nullptr)
    , m_frameRateLabel(nullptr)
    , m_player(nullptr)
    , m_updateProperties(true)
{
    init();
    setWindowTitle(QStringLiteral("Stream File"));
}


void MainWindow::init( ) {
    QVBoxLayout* appLayout = new QVBoxLayout;

    m_player = new MediaPlayer;
    connect(m_player, SIGNAL(positionChanged()), this, SLOT(onPropertiesChanged()));
    connect(m_player, SIGNAL(stateChanged()), this, SLOT(onStateChanged()));

    appLayout->addWidget(m_player);

    m_positionLabel = new QLabel(QStringLiteral("00:00:00/00:00:00"));
    m_frameNLabel = new QLabel(QStringLiteral("Frame n°"));
    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setTickPosition(QSlider::TicksBelow);
    m_positionSlider->setTickInterval(10);
    m_positionSlider->setMaximum(1000);
    connect(m_positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPosition(int)));

    QGridLayout *posLayout = new QGridLayout;
    posLayout->addWidget(m_positionLabel, 1, 0);
    posLayout->addWidget(m_positionSlider, 1, 1, 1, 2);
    posLayout->addWidget(m_frameNLabel, 1, 4, Qt::AlignRight);

    QHBoxLayout *fpsLayout = new QHBoxLayout;
    m_frameRateLabel = new QLabel("Fps:25");
    fpsLayout->addStretch();
    initButton(QStyle::SP_ArrowLeft, tr("Down fps"), this, SLOT(downFps()), fpsLayout);
    fpsLayout->addWidget(m_frameRateLabel);
    initButton(QStyle::SP_ArrowRight, tr("Up fps"), this, SLOT(upFps()), fpsLayout);
    appLayout->addLayout(fpsLayout);
    fpsLayout->addStretch();

    appLayout->addLayout(posLayout);
    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    btnLayout->addStretch();

    initButton(QStyle::SP_DialogOpenButton, tr("Open File"),
                              this, SLOT(open()), btnLayout);
    initButton(QStyle::SP_MediaPlay, tr("Play"),
                              m_player, SLOT(play()), btnLayout);
    initButton(QStyle::SP_MediaPause, tr("Pause"),
                               m_player, SLOT(pause()), btnLayout);
    initButton(QStyle::SP_MediaStop, tr("Stop"),
                              m_player, SLOT(stop()), btnLayout);

    btnLayout->addStretch();
    appLayout->addLayout(btnLayout);

    QWidget* w = new QWidget;
    w->setLayout(appLayout);
    setCentralWidget(w);

    resize(640, 400);
}

QToolButton* MainWindow::initButton(QStyle::StandardPixmap icon, const QString & tip,
                                  QObject *dstobj, const char *slot_method, QLayout *layout)
{
    QToolButton *button = new QToolButton;
    button->setIcon(style()->standardIcon(icon));
    button->setIconSize(QSize(36, 36));
    button->setToolTip(tip);
    connect(button, SIGNAL(clicked()), dstobj, slot_method);
    layout->addWidget(button);
    return button;
}


void MainWindow::setPosition(int value)
{
    uint length = -m_player->length().msecsTo(QTime(0,0));
    if (length != 0 && value > 0) {
        QTime pos(0,0);
        pos = pos.addMSecs(length * (value / 1000.0));
        m_player->setPosition(pos);
    }
}

void MainWindow::downFps()
{
    double frameRate = m_player->frameRate()*DEFAULT_FRAMERATE;
    --frameRate;
    if (frameRate == 0)
        frameRate = -1;
    m_player->setFrameRate(frameRate/DEFAULT_FRAMERATE);
}
void MainWindow::upFps()
{
    double frameRate = m_player->frameRate()*DEFAULT_FRAMERATE;
    ++frameRate;
    if (frameRate == 0)
        frameRate = 1;
    m_player->setFrameRate(frameRate/DEFAULT_FRAMERATE);
}

void MainWindow::onPropertiesChanged()
{
    QTime length(0,0);
    QTime curpos(0,0);
    if (m_player->state() != QGst::StateReady &&
        m_player->state() != QGst::StateNull)
    {
        length = m_player->length();
        curpos = m_player->position();
    }
    m_positionLabel->setText(curpos.toString("hh:mm:ss.zzz")
                                        + "/" +
                             length.toString("hh:mm:ss.zzz"));
    if (length != QTime(0,0)) {
        m_positionSlider->setValue(curpos.msecsTo(QTime(0,0)) * 1000 / length.msecsTo(QTime(0,0)));
    } else {
        m_positionSlider->setValue(0);
    }
    if (curpos != QTime(0,0)) {
        m_positionLabel->setEnabled(true);
        m_positionSlider->setEnabled(true);
    }

    m_frameNLabel->setText(QStringLiteral("Frame N°:")+QString::number(m_positionSlider->value()));
    m_frameRateLabel->setText(QStringLiteral("Fps:")+QString::number(m_player->frameRate()*DEFAULT_FRAMERATE));
}

void MainWindow::openUri(QString fileUri)
{
    m_player->stop();
    m_player->setUri(fileUri);
    m_player->play();
}
void MainWindow::open()
{
    SelectionFileWidget dialog;
    connect(&dialog, SIGNAL(uriSelected(QString)), this, SLOT(openUri(QString)));
    dialog.setModal(false);
    dialog.exec();
}

