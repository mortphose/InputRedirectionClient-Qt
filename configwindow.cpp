#include "configwindow.h"
#include "gpmanager.h"

ConfigWindow::ConfigWindow(QWidget *parent, TouchScreen *ts) : QDialog(parent)
{
    this->setFixedSize(TOUCH_SCREEN_WIDTH, 700);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    this->setWindowTitle(tr("InputRedirectionClient-Qt - Button Config"));

    layout = new QVBoxLayout(this);

    comboBoxA = populateItems(variantToButton(settings.value("ButtonA", QGamepadManager::ButtonA)));
    comboBoxB = populateItems(variantToButton(settings.value("ButtonB", QGamepadManager::ButtonB)));
    comboBoxX = populateItems(variantToButton(settings.value("ButtonX", QGamepadManager::ButtonX)));
    comboBoxY = populateItems(variantToButton(settings.value("ButtonY", QGamepadManager::ButtonY)));
    comboBoxUp = populateItems(variantToButton(settings.value("ButtonUp", QGamepadManager::ButtonUp)));
    comboBoxDown = populateItems(variantToButton(settings.value("ButtonDown", QGamepadManager::ButtonDown)));
    comboBoxLeft = populateItems(variantToButton(settings.value("ButtonLeft", QGamepadManager::ButtonLeft)));
    comboBoxRight = populateItems(variantToButton(settings.value("ButtonRight", QGamepadManager::ButtonRight)));
    comboBoxL = populateItems(variantToButton(settings.value("ButtonL", QGamepadManager::ButtonL1)));
    comboBoxR = populateItems(variantToButton(settings.value("ButtonR", QGamepadManager::ButtonR1)));
    comboBoxSelect = populateItems(variantToButton(settings.value("ButtonSelect", QGamepadManager::ButtonSelect)));
    comboBoxStart = populateItems(variantToButton(settings.value("ButtonStart", QGamepadManager::ButtonStart)));
    comboBoxZL = populateItems(variantToButton(settings.value("ButtonZL", QGamepadManager::ButtonL2)));
    comboBoxZR = populateItems(variantToButton(settings.value("ButtonZR", QGamepadManager::ButtonR2)));
    comboBoxHome = populateItems(variantToButton(settings.value("ButtonHome", QGamepadManager::ButtonInvalid)));
    comboBoxPower = populateItems(variantToButton(settings.value("ButtonPower", QGamepadManager::ButtonInvalid)));
    comboBoxPowerLong = populateItems(variantToButton(settings.value("ButtonPowerLong", QGamepadManager::ButtonInvalid)));
    comboBoxTouch1 = populateItems(variantToButton(settings.value("ButtonT1", QGamepadManager::ButtonInvalid)));
    comboBoxTouch2 = populateItems(variantToButton(settings.value("ButtonT2", QGamepadManager::ButtonInvalid)));

    touchButton1XEdit = new QLineEdit(this);
    touchButton1XEdit->setClearButtonEnabled(true);
    touchButton1XEdit->setText(settings.value("touchButton1X", "0").toString());
    touchButton1YEdit = new QLineEdit(this);
    touchButton1YEdit->setClearButtonEnabled(true);
    touchButton1YEdit->setText(settings.value("touchButton1Y", "0").toString());

    touchButton2XEdit = new QLineEdit(this);
    touchButton2XEdit->setClearButtonEnabled(true);
    touchButton2XEdit->setText(settings.value("touchButton2X", "0").toString());
    touchButton2YEdit = new QLineEdit(this);
    touchButton2YEdit->setClearButtonEnabled(true);
    touchButton2YEdit->setText(settings.value("touchButton2Y", "0").toString());

    invertYCheckbox = new QCheckBox(this);
    invertYCppCheckbox = new QCheckBox(this);
    swapSticksCheckbox = new QCheckBox(this);
    mhCameraCheckbox = new QCheckBox(this);
    rsSmashCheckbox = new QCheckBox(this);

    formLayout = new QFormLayout;

    formLayout->addRow(tr("A Button"), comboBoxA);
    formLayout->addRow(tr("B Button"), comboBoxB);
    formLayout->addRow(tr("X Button"), comboBoxX);
    formLayout->addRow(tr("Y Button"), comboBoxY);
    formLayout->addRow(tr("DPad-Up"), comboBoxUp);
    formLayout->addRow(tr("DPad-Down"), comboBoxDown);
    formLayout->addRow(tr("DPad-Left"), comboBoxLeft);
    formLayout->addRow(tr("DPad-Right"), comboBoxRight);
    formLayout->addRow(tr("L Button"), comboBoxL);
    formLayout->addRow(tr("R Button"), comboBoxR);
    formLayout->addRow(tr("Select"), comboBoxSelect);
    formLayout->addRow(tr("Start"), comboBoxStart);
    formLayout->addRow(tr("Home"), comboBoxHome);
    formLayout->addRow(tr("Power"), comboBoxPower);
    formLayout->addRow(tr("Power-Long"), comboBoxPowerLong);
    formLayout->addRow(tr("ZL Button"), comboBoxZL);
    formLayout->addRow(tr("ZR Button"), comboBoxZR);

    formLayout->addRow(tr("Touch Button 1"), comboBoxTouch1);
    formLayout->addRow(tr("Touch Button X"), touchButton1XEdit);
    formLayout->addRow(tr("Touch Button Y"), touchButton1YEdit);
    formLayout->addRow(tr("Touch Button 2"), comboBoxTouch2);
    formLayout->addRow(tr("Touch Button X"), touchButton2XEdit);
    formLayout->addRow(tr("Touch Button Y"), touchButton2YEdit);

    formLayout->addRow(tr("&Invert Y axis"), invertYCheckbox);
    formLayout->addRow(tr("&Invert Cpp Y "), invertYCppCheckbox);
    formLayout->addRow(tr("&Swap Sticks"), swapSticksCheckbox);
    formLayout->addRow(tr("RightStick &DPad"), mhCameraCheckbox);
    formLayout->addRow(tr("RightStick &Smash"), rsSmashCheckbox);

    saveButton = new QPushButton(tr("&SAVE"), this);
    closeButton = new QPushButton(tr("&CANCEL"), this);

    layout->addLayout(formLayout);
    layout->addWidget(saveButton);
    layout->addWidget(closeButton);

    connect(touchButton1XEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton1X = text.toUInt();
        settings.setValue("touchButton1X", text);
    });
    connect(touchButton1YEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton1Y = text.toUInt();
        settings.setValue("touchButton1Y", text);
    });
    connect(touchButton2XEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton2X = text.toUInt();
        settings.setValue("touchButton2X", text);
    });
    connect(touchButton2YEdit, &QLineEdit::textChanged, this,
            [ts](const QString &text)
    {
        touchButton2Y = text.toUInt();
        settings.setValue("touchButton2Y", text);
    });

    connect(invertYCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
        switch(state)
        {
            case Qt::Unchecked:
                yAxisMultiplier = 1;
                settings.setValue("invertY", false);
                break;
            case Qt::Checked:
                yAxisMultiplier = -1;
                settings.setValue("invertY", true);
                break;
            default: break;
        }
    });

    connect(invertYCppCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
        switch(state)
        {
            case Qt::Unchecked:
                yAxisMultiplierCpp = 1;
                settings.setValue("invertCPPY", false);
                break;
            case Qt::Checked:
                yAxisMultiplierCpp = -1;
                settings.setValue("invertCPPY", true);
                break;
            default: break;
        }
    });

    connect(swapSticksCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
        switch(state)
        {
            case Qt::Unchecked:
                shouldSwapStick = false;
                settings.setValue("swapSticks", false);
                break;
            case Qt::Checked:
                shouldSwapStick = true;
                settings.setValue("swapSticks", true);
                break;
            default: break;
        }

    });

    connect(rsSmashCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
         switch(state)
         {
             case Qt::Unchecked:
                 rightStickSmash = false;
                 settings.setValue("rightStickSmash", false);
                 break;
             case Qt::Checked:
                 rightStickSmash = true;
                 settings.setValue("rightStickSmash", true);
                 break;
             default: break;
         }

     });

    connect(mhCameraCheckbox, &QCheckBox::stateChanged, this,
            [](int state)
    {
        switch(state)
        {
            case Qt::Unchecked:
                monsterHunterCamera = false;
                settings.setValue("monsterHunterCamera", false);
                break;
            case Qt::Checked:
                monsterHunterCamera = true;
                settings.setValue("monsterHunterCamera", true);
                break;
            default: break;
        }
    });

    connect(saveButton, &QPushButton::pressed, this,
            [this, ts](void)
    {
        QGamepadManager::GamepadButton a = variantToButton(currentData(comboBoxA));
        hidButtonsAB[0] = a;
        settings.setValue("ButtonA", a);
        QGamepadManager::GamepadButton b = variantToButton(currentData(comboBoxB));
        hidButtonsAB[1] = b;
        settings.setValue("ButtonB", b);

        QGamepadManager::GamepadButton select = variantToButton(currentData(comboBoxSelect));
        hidButtonsMiddle[0] = select;
        settings.setValue("ButtonSelect", select);
        QGamepadManager::GamepadButton start = variantToButton(currentData(comboBoxStart));
        hidButtonsMiddle[1] = start;
        settings.setValue("ButtonStart", start);
        QGamepadManager::GamepadButton right = variantToButton(currentData(comboBoxRight));
        hidButtonsMiddle[2] = right;
        settings.setValue("ButtonRight", right);
        QGamepadManager::GamepadButton left = variantToButton(currentData(comboBoxLeft));
        hidButtonsMiddle[3] = left;
        settings.setValue("ButtonLeft", left);
        QGamepadManager::GamepadButton up = variantToButton(currentData(comboBoxUp));
        hidButtonsMiddle[4] = up;
        settings.setValue("ButtonUp", up);
        QGamepadManager::GamepadButton down = variantToButton(currentData(comboBoxDown));
        hidButtonsMiddle[5] = down;
        settings.setValue("ButtonDown", down);
        QGamepadManager::GamepadButton r = variantToButton(currentData(comboBoxR));
        hidButtonsMiddle[6] = r;
        settings.setValue("ButtonR", r);
        QGamepadManager::GamepadButton l = variantToButton(currentData(comboBoxL));
        hidButtonsMiddle[7] = l;
        settings.setValue("ButtonL", l);

        QGamepadManager::GamepadButton x = variantToButton(currentData(comboBoxX));
        hidButtonsXY[0] = x;
        settings.setValue("ButtonX", x);
        QGamepadManager::GamepadButton y = variantToButton(currentData(comboBoxY));
        hidButtonsXY[1] = y;
        settings.setValue("ButtonY", y);

        QGamepadManager::GamepadButton zr = variantToButton(currentData(comboBoxZR));
        irButtons[0] = zr;
        settings.setValue("ButtonZR", zr);
        QGamepadManager::GamepadButton zl = variantToButton(currentData(comboBoxZL));
        irButtons[1] = zl;
        settings.setValue("ButtonZL", zl);

        QGamepadManager::GamepadButton power = variantToButton(currentData(comboBoxPower));
        powerButton = power;
        settings.setValue("ButtonPower", power);
        QGamepadManager::GamepadButton powerLong = variantToButton(currentData(comboBoxPowerLong));
        powerLongButton = powerLong;
        settings.setValue("ButtonPowerLong", powerLong);
        QGamepadManager::GamepadButton home = variantToButton(currentData(comboBoxHome));
        homeButton = home;
        settings.setValue("ButtonHome", home);

        QGamepadManager::GamepadButton t1 = variantToButton(currentData(comboBoxTouch1));
        touchButton1 = t1;
        settings.setValue("ButtonT1", t1);
        QGamepadManager::GamepadButton t2 = variantToButton(currentData(comboBoxTouch2));
        touchButton2 = t2;
        settings.setValue("ButtonT2", t2);
        ts->updatePixmap();

    });
    connect(closeButton, &QPushButton::pressed, this,
            [this](void)
    {
       this->hide();
    });

    invertYCheckbox->setChecked(settings.value("invertY", false).toBool());
    invertYCppCheckbox->setChecked(settings.value("invertCPPY", false).toBool());
    swapSticksCheckbox->setChecked(settings.value("swapSticks", false).toBool());
    mhCameraCheckbox->setChecked(settings.value("monsterHunterCamera", false).toBool());
    rsSmashCheckbox->setChecked(settings.value("rightStickSmash", false).toBool());
}

QComboBox* ConfigWindow::populateItems(QGamepadManager::GamepadButton button)
{
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("A (bottom)", QGamepadManager::ButtonA);
    comboBox->addItem("B (right)", QGamepadManager::ButtonB);
    comboBox->addItem("X (left)", QGamepadManager::ButtonX);
    comboBox->addItem("Y (top)", QGamepadManager::ButtonY);
    comboBox->addItem("Right", QGamepadManager::ButtonRight);
    comboBox->addItem("Left", QGamepadManager::ButtonLeft);
    comboBox->addItem("Up", QGamepadManager::ButtonUp);
    comboBox->addItem("Down", QGamepadManager::ButtonDown);
    comboBox->addItem("RB", QGamepadManager::ButtonR1);
    comboBox->addItem("LB", QGamepadManager::ButtonL1);
    comboBox->addItem("Select", QGamepadManager::ButtonSelect);
    comboBox->addItem("Start", QGamepadManager::ButtonStart);
    comboBox->addItem("RT", QGamepadManager::ButtonR2);
    comboBox->addItem("LT", QGamepadManager::ButtonL2);
    comboBox->addItem("L3", QGamepadManager::ButtonL3);
    comboBox->addItem("R3", QGamepadManager::ButtonR3);
    comboBox->addItem("Guide", QGamepadManager::ButtonGuide);
    comboBox->addItem("None", QGamepadManager::ButtonInvalid);

    int index = comboBox->findData(button);
    comboBox->setCurrentIndex(index);

    return comboBox;
}

QVariant ConfigWindow::currentData(QComboBox *comboBox)
{
    QVariant variant;

    variant = comboBox->itemData(comboBox->currentIndex());

    return variant;
}
