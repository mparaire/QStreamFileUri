#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QFileDialog>
#include <QToolButton>
#include <QSlider>
#include <QStyle>
#include <QLineEdit>

class MediaPlayer;


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow( QWidget *parent = nullptr );

public Q_SLOTS:
    void open();

private Q_SLOTS:
    void onPropertiesChanged();
    void setPosition(int position);
    void downFps();
    void upFps();
    void openUri(QString fileUri);

private:
    void init( );
    QToolButton* initButton(QStyle::StandardPixmap icon, const QString & tip,
                            QObject *dstobj, const char *slot_method, QLayout *layout);

private:
    QSlider*   m_positionSlider;
    QLabel*   m_positionLabel;
    QLabel*   m_frameNLabel;
    QSlider*   m_frameRateSlider;
    QLabel*   m_frameRateLabel;
    MediaPlayer* m_player;
    bool m_updateProperties;
};


class SelectionFileWidget : public QDialog
{
    Q_OBJECT
public:
    SelectionFileWidget(QWidget* parent = nullptr);

signals:
    void uriSelected(QString uri);

private slots:
    void fileDialog();
    void urlDialog();
};

class UrlWidget : public QDialog
{
    Q_OBJECT
public:
    UrlWidget(QWidget* parent = nullptr);

signals:
    void uriSelected(QString uri);

private slots:
    void closeDialog();

private:
    QLineEdit* m_url;
};
#endif // MAINWINDOW_H
