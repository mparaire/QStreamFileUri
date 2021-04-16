#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QString>
#include <QTime>
#include <QTimer>
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>

class MediaPlayer : public QGst::Ui::VideoWidget
{
    Q_OBJECT
public:
    MediaPlayer(QWidget *parent = 0);
    ~MediaPlayer();

    void setUri(const QString & uri);
    QTime position() const;
    double frameRate() const;
    void setPosition(const QTime & pos);
    void setFrameRate(double value);

    QTime length() const;
    QGst::State state() const;

public Q_SLOTS:
    void play();
    void pause();
    void stop();

Q_SIGNALS:
    void positionChanged();
    void stateChanged();

private:
    void onBusMessage(const QGst::MessagePtr & message);
    void handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm);
    void seekEvent(const QTime & pos, double value);
    QGst::PipelinePtr m_pipeline;
    QTimer m_positionTimer;
};

#endif // MEDIAPLAYER_H
