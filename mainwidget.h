#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "global.h"

#include <QMessageBox>
#include <QVariant>
#include <QScreen>
#include <QGroupBox>
#include <QRadioButton>

#include "touchscreen.h"
#include "configwindow.h"
#include "gpconfigurator.h"

class Widget : public QWidget
{
public:
    TouchScreen *touchScreen;
    QString version = QCoreApplication::applicationVersion();

    Widget(QWidget *parent = nullptr);

    void show(void);

    void closeEvent(QCloseEvent *ev);

    //Move touchscreen window with main window if moved
    void moveEvent(QMoveEvent *event);

    //When main window is opened, load shortcut settings
    void showEvent(QShowEvent *event);

    virtual ~Widget(void);

private slots:
    void ShowContextMenu(const QPoint &pos);
private:
    QVBoxLayout  *layout;
    QFormLayout  *formLayout;
    QPushButton  *homeButton, *powerButton, *longPowerButton, *settingsConfigButton,
    *configGamepadButton;
    QLineEdit    *addrLineEdit;
    QRadioButton *controllerRadio1, *controllerRadio2, *controllerRadio3, *controllerRadio4;
    QSlider      *touchOpacitySlider;
    QLabel       *creatorLabel, *titleLabel, *instructionsLabel;
    ConfigWindow *settingsConfig;
    QPixmap      *logo, *title;
    QScreen      *screen;
    QFrame       *lineA, *lineB, *lineC;
};

#endif // MAINWIDGET_H
