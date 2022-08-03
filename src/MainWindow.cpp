#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Platform.h"

static const int kTimeLeftCheckInterval = 250;
static const char *kStart = "Start";
static const char *kPause = "Pause";

static const auto kConfigFileName = std::wstring(L"config.cfg");
static const QString kAboutText = QString(
        "This software was developed by Tishchenko Vladislav\n"
        "Email: tishenko.vlad.apc@gmail.com\n"
        "Version: %1.%2.%3")
            .arg(PROJECT_VER_MAJOR)
            .arg(PROJECT_VER_MINOR)
            .arg(PTOJECT_VER_PATCH);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), 
    ui(new Ui::MainWindow),
    isExiting_(false)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("./resources/app_icon.png"));
    setWindowTitle(tr("Break reminder"));
    setFixedSize(width(), height());
    
    trayIconMenu_ = new QMenu(this);
    trayIconMenu_->setFixedWidth(80);
    showAction_ = trayIconMenu_->addAction("Show");
    startAction_ = trayIconMenu_->addAction("Start");
    stopAction_ = trayIconMenu_->addAction("Stop");
    quitAction_ = trayIconMenu_->addAction("Quit");
    stopAction_->setEnabled(false);

    trayIcon_ = new QSystemTrayIcon(QIcon("./resources/app_icon.png"), this);
    trayIcon_->setContextMenu(trayIconMenu_.get());

    try {
        auto cm = ConfigurationManager::readFromFile(getExecutableDirPath() / kConfigFileName);
        // if we managed to load configuration, then set it
        switch (cm.getMode()) {
        case Mode::Interactive:
            ui->rbInteractiveMode->setChecked(true);
            break;
        case Mode::TimeBased:
            ui->rbTimeBasedMode->setChecked(true);
            break;
        default:
            // error occured we don't trust this config file anymore
            std::filesystem::remove(getExecutableDirPath() / kConfigFileName);
            logError("Wrong mode in config file detected");
            throw std::runtime_error("");
            break;
        }

        ui->timeEdit->setTime(QTime(cm.getHour(), cm.getMinute(), cm.getSecond()));
    } catch (const std::runtime_error& err) {
        logError(err.what());
    }
    updateModeDescription();

    connect(showAction_.get(), SIGNAL(triggered(bool)), this, SLOT(slotShowActionTriggered(bool)));
    connect(startAction_.get(), SIGNAL(triggered(bool)), this, SLOT(slotStartActionTriggered(bool)));
    connect(stopAction_.get(), SIGNAL(triggered(bool)), this, SLOT(slotStopActionTriggered(bool)));
    connect(quitAction_.get(), SIGNAL(triggered(bool)), this, SLOT(slotExitTriggered(bool)));
    connect(trayIcon_.get(), SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slotTrayIconActivated(QSystemTrayIcon::ActivationReason)));

    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(slotExitTriggered(bool)));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(slotAboutTriggered(bool)));
    connect(ui->pbStart, SIGNAL(clicked()), this, SLOT(slotStartPauseClicked()));
    connect(ui->pbStop, SIGNAL(clicked()), this, SLOT(slotStopClicked()));
    connect(ui->rbInteractiveMode, SIGNAL(clicked()), this, SLOT(slotModeRadioButtonClicked()));
    connect(ui->rbTimeBasedMode, SIGNAL(clicked()), this, SLOT(slotModeRadioButtonClicked()));
    connect(&timeLeftTimer_, SIGNAL(timeout()), this, SLOT(slotTimeLeftTimerTick()));
    timeLeftTimer_.start(kTimeLeftCheckInterval);
}

MainWindow::~MainWindow()
{
    ConfigurationManager cm;
    auto time = ui->timeEdit->time();
    cm.setHour(time.hour());
    cm.setMinute(time.minute());
    cm.setSecond(time.second());

    if (ui->rbInteractiveMode->isChecked())
        cm.setMode(Mode::Interactive);
    else
        cm.setMode(Mode::TimeBased);

    try {
        cm.writeToFile(getExecutableDirPath() / kConfigFileName);
    } catch (std::runtime_error& err) {
        QMessageBox::critical(this, "Config write error", QString::fromStdString(err.what()));
    }

    delete ui;
}

void MainWindow::slotExitTriggered(bool) 
{
    isExiting_ = true;
    QApplication::quit();
}

void MainWindow::slotAboutTriggered(bool) 
{
    QMessageBox::information(this, 
        "About",
        kAboutText);
}

void MainWindow::slotTimeLeftTimerTick() 
{
    if (!breakTimer_) 
        return;
    const auto ttime = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::time_point(breakTimer_->getTimeLeft()));
    const auto t = std::gmtime(&ttime);
    ui->lTimeLeft->setText(QString("%1:%2:%3")
        .arg(t->tm_hour, 2, 10, QChar('0'))
        .arg(t->tm_min, 2, 10, QChar('0'))
        .arg(t->tm_sec, 2, 10, QChar('0')));
}

void MainWindow::slotStartPauseClicked() 
{
    if (!breakTimer_) {
        startStop(true);
        return;
    }
    switch (breakTimer_->getState()) {
    case BreakTimer::State::Running:
        breakTimer_->pause();
        ui->pbStart->setText(kStart);
        break;
    case BreakTimer::State::Paused:
        breakTimer_->resume();
        ui->pbStart->setText(kPause);
        break;
    default:
        logError("MainWindow::slotStartPauseClicked not proper switch branch");
        isExiting_ = true;
        QApplication::quit();
    }
}

void MainWindow::slotStopClicked() 
{
    startStop(false);
}

void MainWindow::slotStartActionTriggered(bool) 
{
    startStop(true);
}

void MainWindow::slotStopActionTriggered(bool) 
{
    startStop(false);
}

void MainWindow::slotShowActionTriggered(bool) 
{
    showHideToTray(true);
}

void MainWindow::slotTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) 
        showHideToTray(true);
}

void MainWindow::startStop(const bool isStarted) 
{
    if (isStarted) {
        startAction_->setEnabled(false);
        stopAction_->setEnabled(true);  
        auto time = ui->timeEdit->time();
        if (time.second() == 0 && time.minute() == 0 && time.hour() == 0) {
            QMessageBox::warning(this, "Error", "Interval duration has to be greater then 0");
            return;
        }
        breakTimer_.reset(new BreakTimer(
            ui->rbTimeBasedMode->isChecked() ? BreakTimer::Type::TimeBased : BreakTimer::Type::Interactive,
            time.hour(), 
            time.minute(),
            time.second()));
        connect(breakTimer_.get(), SIGNAL(sigTimeout()), this, SLOT(slotBreakTimerTimeout()));
        setupStartStopUi(true);
    } else {
        startAction_->setEnabled(true);
        stopAction_->setEnabled(false);  
        if (popupWidget_) {
            popupWidget_->hide();
            popupWidget_.reset(nullptr);
        }
        breakTimer_.reset(nullptr);
        setupStartStopUi(false);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) 
{
    if (!isExiting_) {
        event->ignore();
        showHideToTray(false);
    }
}

void MainWindow::showHideToTray(const bool isShowing) 
{
    if (isShowing) {
        show();
        trayIcon_->hide();
    } else {
        trayIcon_->show();
        hide();
    }        
}

void MainWindow::setupStartStopUi(bool started) 
{
    ui->timeEdit->setEnabled(!started);
    ui->rbInteractiveMode->setEnabled(!started);
    ui->rbTimeBasedMode->setEnabled(!started);
    ui->pbStop->setEnabled(started);
    ui->pbStart->setText(started ? kPause : kStart);
    ui->lTimeLeft->setEnabled(started);
}


void MainWindow::updateModeDescription() 
{
    if (ui->rbInteractiveMode->isChecked()) {
        ui->lModeDescription->setText(
            "In Interactive mode, time counter stops when user stops interacting with computer (e.g. goes away from computer)");
    } else {
        ui->lModeDescription->setText(
            "In Time based mode, time counter continues time measurement even if user goes away from computer");
    }
}

void MainWindow::slotModeRadioButtonClicked() 
{
    updateModeDescription();
}

void MainWindow::slotBreakTimerTimeout() 
{    
    try {
        popupWidget_ = std::make_unique<PopupWidget>();
        connect(popupWidget_.get(), SIGNAL(sigRestartClicked()), this, SLOT(slotPopupWidgetResumed()));
        connect(popupWidget_.get(), SIGNAL(sigStopClicked()), this, SLOT(slotStopClicked()));  // behavior same as if main stop button were clicked
        popupWidget_->show();
    } catch (...) {
        // system error may occur if PopupWidget could not start it's background thread
        logError("Unable to start timer");
        QMessageBox::critical(this, "Error", "Unable to start timer");
    }
}

void MainWindow::slotPopupWidgetResumed() 
{
    popupWidget_->hide();
    popupWidget_.reset(nullptr);
    if (!breakTimer_) 
        return;
    breakTimer_->restart();
}
