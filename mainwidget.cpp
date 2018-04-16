#include "mainwidget.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    layout = new QVBoxLayout(this);

    formLayout = new QFormLayout();

    QGroupBox *controllerGroupBox = new QGroupBox("Controller Select 🎮");
    controllerRadio1 = new QRadioButton("1");
    controllerRadio2 = new QRadioButton("2");
    controllerRadio3 = new QRadioButton("3");
    controllerRadio4 = new QRadioButton("4");
    controllerRadio1->setChecked(true);
    QHBoxLayout *controllerRadioLayout = new QHBoxLayout;
    controllerRadioLayout->addWidget(controllerRadio1);
    controllerRadioLayout->addWidget(controllerRadio2);
    controllerRadioLayout->addWidget(controllerRadio3);
    controllerRadioLayout->addWidget(controllerRadio4);
    controllerGroupBox->setLayout(controllerRadioLayout);

    addrLineEdit = new QLineEdit(this);
    addrLineEdit->setClearButtonEnabled(true);
    addrLineEdit->setInputMethodHints(Qt::ImhPreferNumbers);
    formLayout->addRow(tr("IP address 📡"), addrLineEdit);

    touchOpacitySlider = new QSlider(Qt::Horizontal);
    touchOpacitySlider->setRange(0, 10);
    touchOpacitySlider->setValue(10);
    touchOpacitySlider->setTickInterval(1);

    if (QSysInfo::productType() != "android")
        formLayout->addRow(tr("TS Opacity 🔅"), touchOpacitySlider);

    homeButton = new QPushButton(tr("Home 🏠"), this);
    homeButton->setFocusPolicy(Qt::NoFocus);
    powerButton = new QPushButton(tr("Power ⥁"), this);
    powerButton->setFocusPolicy(Qt::NoFocus);
    longPowerButton = new QPushButton(tr("Power-Long ⥁⥁⥁"), this);
    longPowerButton->setFocusPolicy(Qt::NoFocus);
    settingsConfigButton = new QPushButton(tr("Settings ⚙️"), this);
    settingsConfigButton->setFocusPolicy(Qt::NoFocus);
    configGamepadButton = new QPushButton(tr("Configure Custom Gamepad 🎮"));
    configGamepadButton->setFocusPolicy(Qt::NoFocus);

    QScreen *screen = QApplication::screens().at(0);
    titleLabel = new  QLabel;
    titleLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
    QPixmap title;
    if (QSysInfo::productType() == "android")
    {
        title.load(":/myressources/logo/InputRedirectionClient.png");
        titleLabel->setPixmap(title.scaledToWidth(screen->availableGeometry().width()-20));
    } else {
        title.load(":/myressources/logo/512x512_v2.png");
        titleLabel->setPixmap(title.scaledToWidth(width()/2));
    }
    titleLabel->setAlignment(Qt::AlignCenter);

    lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);

    lineB = new QFrame;
    lineB->setFrameShape(QFrame::HLine);
    lineB->setFrameShadow(QFrame::Sunken);

    lineC = new QFrame;
    lineC->setFrameShape(QFrame::HLine);
    lineC->setFrameShadow(QFrame::Sunken);

    instructionsLabel = new QLabel;
    instructionsLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    instructionsLabel->setAlignment(Qt::AlignLeft);
    instructionsLabel->setText("<html><b>Instructions:</b>"
                               "<br>1. Start InputRedirection on your 3DS via Rosalina menu (Press L+⬇️+Select)."
                               "<br>2. Connect your Gamepad to your Android device.</br>"
                               "<br>3. Enter the IP address displayed on your 3DS in the IRC application.</br>"
                               "<br>ℹ️ Work via Wi-Fi or via Mobile Data if both devices share the same network.</br></html>");
    instructionsLabel->setTextFormat(Qt::RichText);
    instructionsLabel->setWordWrap(true);

    creatorLabel = new QLabel;
    creatorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    creatorLabel->setAlignment(Qt::AlignCenter);
    creatorLabel->setText("<html>Created by <a href=\"https://github.com/TuxSH/\">TuxSH</a>"
                          "<br>Based on <a href=\"https://github.com/mastermune/\">mastermune</a> & <a href=\"https://github.com/gbrown5/\">gbrown5</a>'s forks"
                          "<br>Version "+version+" for Android by JambonBeurreMan</html>");
    creatorLabel->setTextFormat(Qt::RichText);
    creatorLabel->setOpenExternalLinks(true);

    if(screen->availableGeometry().height()<=800)
    {
        instructionsLabel->setStyleSheet("font: 10pt;");
        creatorLabel->setStyleSheet("font: 10pt");
    }

    setContextMenuPolicy(Qt::CustomContextMenu);

    // Disable/hide the configurator button if running windows since it's not supported
     if (QSysInfo::productType() == "windows" ||
         QSysInfo::productType() == "osx")
     {
         configGamepadButton->setEnabled(false);
         configGamepadButton->setVisible(false);
     }

    layout->setContentsMargins(10,10,10,0);
    layout->addWidget(titleLabel);
    layout->addSpacing(10);
    layout->addWidget(controllerGroupBox);
    layout->addLayout(formLayout);
    layout->addWidget(lineA);
    layout->addWidget(homeButton);
    if(screen->availableGeometry().height()>=1280)
    {
        layout->addSpacing(50);
    }
    layout->addWidget(powerButton);
    layout->addWidget(longPowerButton);
    layout->addWidget(configGamepadButton);
    layout->addWidget(settingsConfigButton);
    layout->addWidget(lineB);
    layout->addWidget(instructionsLabel);
    layout->addStretch(1);
    layout->addWidget(lineC);
    layout->addWidget(creatorLabel);

    gpConfigurator = new GamepadConfigurator();

    connect(addrLineEdit, &QLineEdit::textChanged, this,
            [](const QString &text)
    {
        ipAddress = text;
        settings.setValue("InputRedirection/ipAddress", text);
    });

    connect(configGamepadButton, &QPushButton::released, this,
            [](void)
    {
       gpConfigurator->showGui();
     });

    connect(homeButton, &QPushButton::pressed, this,
            [](void)
    {
       interfaceButtons |= 1;
    });

    connect(homeButton, &QPushButton::released, this,
            [](void)
    {
       interfaceButtons &= ~1;
    });

    connect(powerButton, &QPushButton::pressed, this,
            [](void)
    {
       interfaceButtons |= 2;
    });

    connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,
        [](int deviceId, QGamepadManager::GamepadButton button)
    {
        (void)deviceId;

        gpConfigurator->setCurDeviceId(deviceId);

        if(gpConfigurator->isVisible())
        {
           gpConfigurator->getInput(deviceId, button);
            return;
        }

            buttons &= QGamepadManager::GamepadButtons(~(1 << button));
    });

    connect(gpConfigurator->skipButton, &QPushButton::released, this,
            [](void)
    {
           gpConfigurator->next();
    });


    connect(gpConfigurator->resetConfigButton, &QPushButton::released, this,
            [](void)
    {
           QMessageBox *msgBox = new QMessageBox(0);
           QGamepadManager::instance()->resetConfiguration(gpConfigurator->getCurDeviceId());

           msgBox->setText("Reset");
           msgBox->setInformativeText("Please restart the program for changes to take affect.");
           msgBox->show();

    });

    connect(powerButton, &QPushButton::released, this,
            [](void)
    {
       interfaceButtons &= ~2;
    });

    connect(longPowerButton, &QPushButton::pressed, this,
            [](void)
    {
       interfaceButtons |= 4;
    });

    connect(longPowerButton, &QPushButton::released, this,
            [](void)
    {
       interfaceButtons &= ~4;
    });

    connect(settingsConfigButton, &QPushButton::released, this,
            [this](void)
    {
        if (!settingsConfig->isVisible())
        {
            settingsConfig->move(this->x() - settingsConfig->width() - 5,this->y());
            settingsConfig->show();
        } else settingsConfig->hide();
    });

    connect(touchOpacitySlider, &QSlider::valueChanged, this,
            [this](int value)
    {
        touchScreen->setWindowOpacity(value / 10.0);
        touchScreen->update();
    });

    connect(controllerRadio1, &QRadioButton::clicked, this,
            [](void)
    {
       selectedControllerId = 0;
    });

    connect(controllerRadio2, &QRadioButton::clicked, this,
            [](void)
    {
       selectedControllerId = 1;
    });

    connect(controllerRadio3, &QRadioButton::clicked, this,
            [](void)
    {
       selectedControllerId = 2;
    });

    connect(controllerRadio4, &QRadioButton::clicked, this,
            [](void)
    {
       selectedControllerId = 3;
    });

    if (QSysInfo::productType() != "android")
        touchScreen = new TouchScreen(nullptr);
    settingsConfig = new ConfigWindow(nullptr, touchScreen);
    this->setWindowTitle(tr("InputRedirectionClient-Qt"));

    addrLineEdit->setText(settings.value("InputRedirection/ipAddress", "").toString());
}

void Widget::show(void)
{
    QWidget::show();
    if (QSysInfo::productType() != "android")
    {
        touchScreen->move(this->x() + this->width() + 5,this->y());
        touchScreen->show();
    }
}

void Widget::closeEvent(QCloseEvent *ev)
{
    settingsConfig->close();

    if (QSysInfo::productType() != "android")
    {
        touchScreen->close();
        touchScreen->setTouchScreenPressed(false);
        delete touchScreen;
    }

    ev->accept();
}

//Move touchscreen window with main window if moved
void Widget::moveEvent(QMoveEvent *event)
{
    if (QSysInfo::productType() != "android")
        touchScreen->move(touchScreen->pos() + (event->pos() - event->oldPos()));
}

//When main window is opened, load shortcut settings
void Widget::showEvent(QShowEvent *event)
{
    (void)event;

    listShortcuts.clear();
    uint shortCutCount =
        profileSettings.value(buttonProfile+"/TouchScreen/Shortcut/Count").toInt();

    QString valPath = buttonProfile + "/TouchScreen/Shortcut/1";
    for(int i = 1; listShortcuts.size() < shortCutCount || i > 17; i++)
    {
        valPath = buttonProfile + tr("/TouchScreen/Shortcut/%1").arg(i);

        ShortCut shortCutToLoad;
        shortCutToLoad.name = profileSettings.value(valPath+"/Name").toString();
        shortCutToLoad.button = variantToButton(profileSettings.value(valPath+"/Button"));
        shortCutToLoad.pos = QPoint(
                    profileSettings.value(valPath+"/X").toInt(),
                    profileSettings.value(valPath+"/Y").toInt());
        shortCutToLoad.color = QColor(
                    profileSettings.value(valPath+"/R").toInt(),
                    profileSettings.value(valPath+"/G").toInt(),
                    profileSettings.value(valPath+"/B").toInt());

        if (shortCutToLoad.name != "")
        {
            listShortcuts.push_back(shortCutToLoad);
        }
    }
}

Widget::~Widget(void)
{
    worker->setLeftAxis(0.0, 0.0);
    worker->setRightAxis(0.0, 0.0);

    buttons = 0;
    interfaceButtons = 0;

    delete settingsConfig;
}
