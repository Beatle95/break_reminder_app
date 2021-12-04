#ifndef POPUP_WIDGET
#define POPUP_WIDGET
#include <ctime>
#include <QApplication>
#include <QFileInfo>
#include <QTimer>
#include <QPixmap>
#include <QKeyEvent>
#include <QString>
#include <QScreen>
#include <QSoundEffect>
#include <QRect>
#include <QWidget>
#include "Log.h"

QT_BEGIN_NAMESPACE
namespace Ui { class fPopupWgt; }
QT_END_NAMESPACE

class PopupWidget : public QWidget {
    Q_OBJECT
public:
    PopupWidget(QWidget *parent = nullptr);
    ~PopupWidget();

signals:
    void sigRestartClicked();
    void sigStopClicked();

private slots:
    void slotTimeout();

private:
    static const int kTopOffset   = 80;
    static const int kRightOffset = 30;

    Ui::fPopupWgt *ui;
    QTimer timer_;
    QSoundEffect sound_;
    std::time_t popupTime_;
    void setPosition();
};

#endif