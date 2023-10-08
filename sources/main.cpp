#include <QApplication>
#include <QCheckBox>
#include <QDesktopServices>
#include <QDialog>
#include <QFormLayout>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QTranslator>
#include <QUrl>
#include <windows.h>
#include "updates.h"
#include "version.h"

unsigned int batteryPercentage(){
    SYSTEM_POWER_STATUS spsPwr;
    GetSystemPowerStatus(&spsPwr);
    return spsPwr.BatteryLifePercent;
}

bool isOnAc(){
    SYSTEM_POWER_STATUS spsPwr;
    GetSystemPowerStatus(&spsPwr);
    return spsPwr.ACLineStatus != 0;
}

//Local variables of the main function never go out of scope, so this warning is useless in this particular file (although it's useful in other files)
//clazy:excludeall=lambda-in-connect

int main(int argc, char **argv){
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    (void) translator.load(":/battery_" + QLocale::system().name().section('_', 0, 0));
    app.installTranslator(&translator);
    QTranslator baseTranslator;
    (void) baseTranslator.load(":/qtbase_" + QLocale::system().name().section('_', 0, 0));
    app.installTranslator(&baseTranslator);
    
    checkForUpdates(
        QUrl("https://github.com/GustavLindberg99/Battery"),
        QUrl("versionHeader"),
        QUrl("zip"),
        QUrl("installer")
    );

    QSettings settings("Gustav Lindberg", "Battery");

    QDialog settingsWindow;
    settingsWindow.setWindowTitle(QObject::tr("Settings"));
    settingsWindow.setWindowIcon(QIcon(":/icon.ico"));
    QFormLayout settingsLayout;

    QCheckBox showMaxMessage(QObject::tr("Warn me when the battery is higher than"));
    showMaxMessage.setChecked(settings.value("max", 80).toInt() != -1);
    showMaxMessage.setWhatsThis(QObject::tr("Check this checkbox to get a warning when the battery is charging and its percentage is more than the indicated percentage."));
    QSpinBox maxValue;
    maxValue.setRange(1, 100);
    maxValue.setSuffix("%");
    maxValue.setEnabled(showMaxMessage.isChecked());
    if(settings.value("max", 80).toInt() < 1 || settings.value("max", 80).toInt() > 100){
        maxValue.setValue(80);
    }
    else{
        maxValue.setValue(settings.value("max", 80).toInt());
    }
    QObject::connect(&showMaxMessage, &QCheckBox::toggled, &maxValue, &QSpinBox::setEnabled);
    settingsLayout.addRow(&showMaxMessage, &maxValue);

    QCheckBox showMinMessage(QObject::tr("Warn me when the battery is lower than"));
    showMinMessage.setChecked(settings.value("min", -1).toInt() != -1);
    showMinMessage.setWhatsThis(QObject::tr("Check this checkbox to get a warning when the battery is not charging and its percentage is less than the indicated percentage."));
    QSpinBox minValue;
    minValue.setRange(0, 99);
    minValue.setSuffix("%");
    minValue.setEnabled(showMinMessage.isChecked());
    if(settings.value("min", -1).toInt() < 0 || settings.value("min", -1).toInt() > 99){
        minValue.setValue(20);
    }
    else{
        minValue.setValue(settings.value("min", -1).toInt());
    }
    QObject::connect(&showMinMessage, &QCheckBox::toggled, &minValue, &QSpinBox::setEnabled);
    settingsLayout.addRow(&showMinMessage, &minValue);

    QHBoxLayout buttonsLayout;
    QPushButton okButton(QObject::tr("OK"));
    okButton.setWhatsThis(QObject::tr("Closes this window and saves the settings."));
    QObject::connect(&okButton, &QPushButton::clicked, &settingsWindow, [&](){
        settings.setValue("max", showMaxMessage.isChecked() ? maxValue.value() : -1);
        settings.setValue("min", showMinMessage.isChecked() ? minValue.value() : -1);
        settingsWindow.hide();
    });
    buttonsLayout.addWidget(&okButton);
    QPushButton cancelButton(QObject::tr("Cancel"));
    QObject::connect(&cancelButton, &QPushButton::clicked, &settingsWindow, &QDialog::hide);
    buttonsLayout.addWidget(&cancelButton);
    cancelButton.setWhatsThis(QObject::tr("Closes this window without saving the settings."));
    settingsLayout.addRow(&buttonsLayout);

    settingsWindow.setLayout(&settingsLayout);

    QSystemTrayIcon trayIcon;
    bool maxMessageShown = false, minMessageShown = false;

    QTimer timer;
    timer.setInterval(3000);
    QObject::connect(&timer, &QTimer::timeout, &trayIcon, [&](){
        trayIcon.setToolTip(QObject::tr("%1% charged").arg(batteryPercentage()));
        const unsigned int percentage = batteryPercentage();
        const bool onAc = isOnAc();
        QString message;
        if(!maxMessageShown && onAc && settings.value("max", 80).toInt() >= 0 && percentage >= settings.value("max", 80).toUInt()){
            maxMessageShown = true;
            message = QObject::tr("The battery is almost fully charged (%1%).").arg(percentage);
        }
        else if(!minMessageShown && !onAc && settings.value("min", -1).toInt() >= 0 && percentage <= settings.value("min", -1).toUInt()){
            minMessageShown = true;
            message = QObject::tr("The battery is running low (%1%).").arg(percentage);
        }
        if(!message.isEmpty()){
            QMessageBox msg;
            msg.setIcon(QMessageBox::Warning);
            msg.setWindowIcon(QIcon(":/icon.ico"));
            msg.setWindowTitle(QObject::tr("Battery"));
            msg.setText(message);
            msg.exec();
        }
        if(onAc){
            minMessageShown = false;
        }
        else{
            maxMessageShown = false;
        }
    });
    timer.start();

    QMenu contextMenu;
    QAction *settingsAction = contextMenu.addAction(QObject::tr("&Settings"));
    QObject::connect(settingsAction, &QAction::triggered, &settingsWindow, [&](){
        showMaxMessage.setChecked(settings.value("max", 80).toInt() != -1);
        maxValue.setEnabled(showMaxMessage.isChecked());
        if(settings.value("max", 80).toInt() < 1 || settings.value("max", 80).toInt() > 100){
            maxValue.setValue(80);
        }
        else{
            maxValue.setValue(settings.value("max", 80).toInt());
        }
        showMinMessage.setChecked(settings.value("min", -1).toInt() != -1);
        minValue.setEnabled(showMinMessage.isChecked());
        if(settings.value("min", -1).toInt() < 0 || settings.value("min", -1).toInt() > 99){
            minValue.setValue(20);
        }
        else{
            minValue.setValue(settings.value("min", -1).toInt());
        }
        settingsWindow.show();
    });
    QAction *aboutAction = contextMenu.addAction(QObject::tr("&About"));
    QObject::connect(aboutAction, &QAction::triggered, aboutAction, [](){
        QMessageBox msg;
        msg.setIconPixmap(QPixmap(":/icon.ico").scaled(64, 64));
        msg.setWindowIcon(QIcon(":/icon.ico"));
        msg.setWindowTitle(QObject::tr("About"));
        msg.setText(QObject::tr("Battery warning program by Gustav Lindberg.") + "<br><br>" + QObject::tr("Version %1").arg(PROGRAMVERSION) + "<br><br>" + QObject::tr("This program is licensed under the GNU GPL 3.0.") + "<br><br>" + QObject::tr("Source code:") + " <a href=\"https://github.com/GustavLindberg99/Battery\">https://github.com/GustavLindberg99/Battery</a><br/><br/>" + QObject::tr("Icons made by %3 from %1 are licensed by %2.").arg("<a href=\"https://www.flaticon.com/\">www.flaticon.com</a>", "<a href=\"http://creativecommons.org/licenses/by/3.0/\">CC 3.0 BY</a>", "<a href=\"https://www.flaticon.com/authors/smashicons\">Smashicons</a>"));
        msg.exec();
    });
    QAction *aboutQtAction = contextMenu.addAction(QObject::tr("About &Qt"));
    QObject::connect(aboutQtAction, &QAction::triggered, [](){
        QMessageBox::aboutQt(nullptr);
    });
    QAction *helpAction = contextMenu.addAction(QObject::tr("&Help"));
    QObject::connect(helpAction, &QAction::triggered, [](){
        QDesktopServices::openUrl(QUrl("https://github.com/GustavLindberg99/Battery/blob/main/README.md"));
    });
    QAction *exitAction = contextMenu.addAction(QObject::tr("E&xit"));
    QObject::connect(exitAction, &QAction::triggered, &app, &QApplication::quit);
    trayIcon.setContextMenu(&contextMenu);

    trayIcon.setIcon(QIcon(":/icon.ico"));
    trayIcon.show();

    return app.exec();
}
