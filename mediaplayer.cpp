#include "mediaplayer.h"

#include <QDir>
#include <QUrl>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/ClockTime>
#include <QGst/Event>

MediaPlayer::MediaPlayer(QWidget *parent)
    : QGst::Ui::VideoWidget(parent)
{
    connect(&m_positionTimer, SIGNAL(timeout()), this, SIGNAL(positionChanged()));
}
MediaPlayer::~MediaPlayer()
{
    if (m_pipeline) {
        m_pipeline->setState(QGst::StateNull);
        stopPipelineWatch();
    }
}

void MediaPlayer::setUri(const QString & uri)
{
    QString realUri = uri;
    //if uri is not a real uri, assume it is a file path
    if (realUri.indexOf("://") < 0) {
        realUri = QUrl::fromLocalFile(realUri).toEncoded();
    }
    if (!m_pipeline) {
        m_pipeline = QGst::ElementFactory::make("playbin").dynamicCast<QGst::Pipeline>();
        if (m_pipeline) {
            //let the video widget watch the pipeline for new video sinks
            watchPipeline(m_pipeline);
            //watch the bus for messages
            QGst::BusPtr bus = m_pipeline->bus();
            bus->addSignalWatch();
            QGlib::connect(bus, "message", this, &MediaPlayer::onBusMessage);
        } else {
            qCritical() << "Failed to create the pipeline";
        }
    }
    if (m_pipeline) {
        m_pipeline->setProperty("uri", realUri);
    }
}
QTime MediaPlayer::position() const
{
    if (m_pipeline) {
        //here we query the pipeline about its position
        //and we request that the result is returned in time format
        QGst::PositionQueryPtr query = QGst::PositionQuery::create(QGst::FormatTime);
        m_pipeline->query(query);
        return QGst::ClockTime(query->position()).toTime();
    } else {
        return QTime(0,0);
    }
}
double MediaPlayer::frameRate() const
{
    if (m_pipeline) {
        QGst::SegmentQueryPtr query = QGst::SegmentQuery::create(QGst::FormatDefault);
        m_pipeline->query(query);
        return query->rate();
    } else {
        return 1.0;
    }
}
void MediaPlayer::setPosition(const QTime & pos)
{
    if (m_pipeline)
        seekEvent(pos, frameRate());
}

void MediaPlayer::setFrameRate(double value)
{
    if (m_pipeline)
        seekEvent(position(), value);
}

void MediaPlayer::seekEvent(const QTime & pos, double value)
{
    QGst::SeekEventPtr evt = QGst::SeekEvent::create(
        value, QGst::FormatTime, QGst::SeekFlagFlush,
        QGst::SeekTypeSet, QGst::ClockTime::fromTime(pos),
        QGst::SeekTypeNone, QGst::ClockTime::None
    );
    m_pipeline->sendEvent(evt);
}

QTime MediaPlayer::length() const
{
    if (m_pipeline) {
        //here we query the pipeline about the content's duration
        //and we request that the result is returned in time format
        QGst::DurationQueryPtr query = QGst::DurationQuery::create(QGst::FormatTime);
        m_pipeline->query(query);
        return QGst::ClockTime(query->duration()).toTime();
    } else {
        return QTime(0,0);
    }
}
QGst::State MediaPlayer::state() const
{
    return m_pipeline ? m_pipeline->currentState() : QGst::StateNull;
}
void MediaPlayer::play()
{
    if (m_pipeline) {
        m_pipeline->setState(QGst::StatePlaying);
    }
}
void MediaPlayer::pause()
{
    if (m_pipeline) {
        m_pipeline->setState(QGst::StatePaused);
    }
}
void MediaPlayer::stop()
{
    if (m_pipeline) {
        m_pipeline->setState(QGst::StateNull);
        //once the pipeline stops, the bus is flushed so we will
        //not receive any StateChangedMessage about this.
        //so, to inform the ui, we have to emit this signal manually.
        Q_EMIT stateChanged();
    }
}
void MediaPlayer::onBusMessage(const QGst::MessagePtr & message)
{
    switch (message->type()) {
    case QGst::MessageEos: //End of stream. We reached the end of the file.
        stop();
        break;
    case QGst::MessageError: //Some error occurred.
        qCritical() << message.staticCast<QGst::ErrorMessage>()->error();
        stop();
        break;
    case QGst::MessageStateChanged: //The element in message->source() has changed state
        if (message->source() == m_pipeline) {
            handlePipelineStateChange(message.staticCast<QGst::StateChangedMessage>());
        }
        break;
    default:
        break;
    }
}
void MediaPlayer::handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm)
{
    switch (scm->newState()) {
    case QGst::StatePlaying:
        //start the timer when the pipeline starts playing
        m_positionTimer.start(100);
        break;
    case QGst::StatePaused:
        //stop the timer when the pipeline pauses
        if(scm->oldState() == QGst::StatePlaying) {
            m_positionTimer.stop();
        }
        break;
    default:
        break;
    }
    Q_EMIT stateChanged();
}
