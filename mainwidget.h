#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "TouchScreen.h"
#include "configwindow.h"
#include "global.h"

class Widget : public QWidget
{
private:
    QVBoxLayout *layout;
    QFormLayout *formLayout;
    QPushButton *homeButton, *powerButton, *longPowerButton, *remapConfigButton,
        *connectButton;
    QLineEdit *addrLineEdit;
    QSlider *touchOpacitySlider;
    ConfigWindow *remapConfig;

public:
    TouchScreen *touchScreen;

    Widget(QWidget *parent = nullptr) : QWidget(parent)
    {
        layout = new QVBoxLayout(this);

        addrLineEdit = new QLineEdit(this);
        addrLineEdit->setClearButtonEnabled(true);
        connectButton = new QPushButton(tr("&CONNECT"), this);

        formLayout = new QFormLayout;
        formLayout->addRow(tr("IP &address"), addrLineEdit);
        formLayout->addRow(tr(""), connectButton);

        touchOpacitySlider = new QSlider(Qt::Horizontal);
        touchOpacitySlider->setRange(1, 10);
        touchOpacitySlider->setValue(10);
        touchOpacitySlider->setTickInterval(1);
        formLayout->addRow(tr("TS &Opacity"), touchOpacitySlider);

        homeButton = new QPushButton(tr("&HOME"), this);
        powerButton = new QPushButton(tr("&POWER"), this);
        longPowerButton = new QPushButton(tr("POWER (&long)"), this);
        remapConfigButton = new QPushButton(tr("BUTTON &CONFIG"), this);

        layout->addLayout(formLayout);
        layout->addWidget(homeButton);
        layout->addWidget(powerButton);
        layout->addWidget(longPowerButton);
        layout->addWidget(remapConfigButton);

        connect(addrLineEdit, &QLineEdit::textChanged, this,
                [](const QString &text)
        {
            ipAddress = text;
            settings.setValue("ipAddress", text);
        });

        connect(connectButton, &QPushButton::released, this,
                [this](void)
        {
            if (!timerEnabled)
            {
                timerEnabled = true;
                connectButton->setText("DISCONNECT");
            } else {
                timerEnabled = false;
                connectButton->setText("CONNECT");
            }
        });

        connect(homeButton, &QPushButton::pressed, this,
                [](void)
        {
           interfaceButtons |= 1;
           sendFrame();
        });

        connect(homeButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~1;
           sendFrame();
        });

        connect(powerButton, &QPushButton::pressed, this,
                [](void)
        {
           interfaceButtons |= 2;
           sendFrame();
        });

        connect(powerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~2;
           sendFrame();
        });

        connect(longPowerButton, &QPushButton::pressed, this,
                [](void)
        {
           interfaceButtons |= 4;
           sendFrame();
        });

        connect(longPowerButton, &QPushButton::released, this,
                [](void)
        {
           interfaceButtons &= ~4;
           sendFrame();
        });

        connect(remapConfigButton, &QPushButton::released, this,
                [this](void)
        {
           remapConfig->show();
        });

        connect(touchOpacitySlider, &QSlider::valueChanged, this,
                [this](int value)
        {
            touchScreen->setWindowOpacity(value / 10.0);
            touchScreen->update();
        });


        touchScreen = new TouchScreen(nullptr);
        remapConfig = new ConfigWindow(nullptr, touchScreen);
        this->setWindowTitle(tr("InputRedirectionClient-Qt"));

        addrLineEdit->setText(settings.value("ipAddress", "").toString());
    }

    void show(void)
    {
        QWidget::show();
        touchScreen->show();
        remapConfig->hide();
    }

    void closeEvent(QCloseEvent *ev)
    {
        touchScreen->close();
        remapConfig->close();
        ev->accept();
    }

    virtual ~Widget(void)
    {
        lx = ly = rx = ry = 0.0;
        buttons = 0;
        interfaceButtons = 0;
        touchScreen->setTouchScreenPressed(false);
        sendFrame();
        delete touchScreen;
        delete remapConfig;
    }
};

#endif // MAINWIDGET_H
