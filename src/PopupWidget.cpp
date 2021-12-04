#include "PopupWidget.h"
#include "ui_PopupWidget.h"

PopupWidget::PopupWidget(QWidget *parent) 
    : ui(new Ui::fPopupWgt), QWidget(parent)
{
    ui->setupUi(this);
    popupTime_ = std::time(0);

    QPixmap pixmap("./resources/alarm.png");
    ui->lPopupWgtIcon->setPixmap(
        pixmap.scaled(
            ui->lPopupWgtIcon->width(), 
            ui->lPopupWgtIcon->height(), 
            Qt::KeepAspectRatio));
    Qt::WindowFlags flags = this->windowFlags(); 
    setWindowFlags(flags | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(width(), height());
    setPosition();

    ui->pbRestart->setObjectName("popupButton");
    ui->pbStop->setObjectName("popupButton");

    connect(ui->pbRestart, SIGNAL(clicked()), this, SIGNAL(sigRestartClicked()));
    connect(ui->pbStop, SIGNAL(clicked()), this, SIGNAL(sigStopClicked()));
    connect(&timer_, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    timer_.start(1000);

    sound_.setSource(QUrl::fromLocalFile(QFileInfo("./resources/notification.wav").absoluteFilePath()));
    sound_.setVolume(0.8f);
    sound_.play();
}

PopupWidget::~PopupWidget() 
{
    delete ui;
}

void PopupWidget::slotTimeout() 
{
    auto time = std::time(0) - popupTime_;
    auto tm = std::gmtime(&time);
    int min = tm->tm_min + tm->tm_hour * 60;
    ui->lPopupWgtTime->setText(QString("%1:%2")
        .arg(min, 2, 10, QLatin1Char('0'))
        .arg(tm->tm_sec, 2, 10, QLatin1Char('0')));
}

void PopupWidget::setPosition() 
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    move(screenGeometry.width() - width() - kRightOffset, kTopOffset);
}
