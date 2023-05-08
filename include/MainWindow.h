#ifndef MAIN_WINDOW
#define MAIN_WINDOW
#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QMenu>
#include <QPointer>
#include <QTime>
#include <QTimer>
#include <QString>
#include <QSystemTrayIcon>
#include <atomic>
#include <filesystem>
#include <memory>
#include "BreakTimer.h"
#include "ConfigurationManager.h"
#include "PopupWidget.h"
#include "version.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slotAboutTriggered(bool state);
    void slotExitTriggered(bool state);
    void slotBreakTimerTimeout();
    void slotTimeLeftTimerTick();
    void slotPopupWidgetResumed();
    void slotStartPauseClicked();
    void slotStopClicked();
    void slotModeRadioButtonClicked();
    void slotShowActionTriggered(bool);
    void slotStartActionTriggered(bool);
    void slotStopActionTriggered(bool);
    void slotTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<BreakTimer> breakTimer_;
    std::unique_ptr<PopupWidget> popupWidget_;
    QPointer<QSystemTrayIcon> trayIcon_;
    QPointer<QMenu> trayIconMenu_;
    QPointer<QAction> showAction_;
    QPointer<QAction> startAction_;
    QPointer<QAction> stopAction_;
    QPointer<QAction> quitAction_;

    QTimer timeLeftTimer_;
    bool isExiting_;

    void setupStartStopUi(bool started);
    void updateModeDescription();
    void startStop(const bool isStarted);
    void showHideToTray(const bool isShowing);
};

#endif
