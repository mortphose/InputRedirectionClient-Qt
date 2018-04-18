#include "configwindow.h"
#include "gpmanager.h"

ConfigWindow::ConfigWindow(QWidget *parent, TouchScreen *ts) : QDialog(parent)
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->setWindowTitle(tr("InputRedirectionClient-Qt - Button Config"));

    touchScreen = ts;

    comboBoxProfiles = new QComboBox(nullptr);
    comboBoxProfiles->addItems(profileSettings.childGroups());
    if (comboBoxProfiles->currentText() == "")
        comboBoxProfiles->addItem("Default");
    comboBoxProfiles->setCurrentIndex(comboBoxProfiles->findText(buttonProfile));

    profileSettings.beginGroup(buttonProfile);

    comboBoxA = populateItems(variantToButton(
                profileSettings.value("3DS/A", QGamepadManager::ButtonA)));
    comboBoxB = populateItems(variantToButton(
                profileSettings.value("3DS/B", QGamepadManager::ButtonB)));
    comboBoxX = populateItems(variantToButton(
                profileSettings.value("3DS/X", QGamepadManager::ButtonX)));
    comboBoxY = populateItems(variantToButton(
                profileSettings.value("3DS/Y", QGamepadManager::ButtonY)));
    comboBoxUp = populateItems(variantToButton(
                profileSettings.value("3DS/Up", QGamepadManager::ButtonUp)));
    comboBoxDown = populateItems(variantToButton(
                profileSettings.value("3DS/Down", QGamepadManager::ButtonDown)));
    comboBoxLeft = populateItems(variantToButton(
                profileSettings.value("3DS/Left", QGamepadManager::ButtonLeft)));
    comboBoxRight = populateItems(variantToButton(
                profileSettings.value("3DS/Right", QGamepadManager::ButtonRight)));
    comboBoxL = populateItems(variantToButton(
                profileSettings.value("3DS/L", QGamepadManager::ButtonL1)));
    comboBoxR = populateItems(variantToButton(
                profileSettings.value("3DS/R", QGamepadManager::ButtonR1)));
    comboBoxSelect = populateItems(variantToButton(
                profileSettings.value("3DS/Select", QGamepadManager::ButtonSelect)));
    comboBoxStart = populateItems(variantToButton(
                profileSettings.value("3DS/Start", QGamepadManager::ButtonStart)));
    comboBoxZL = populateItems(variantToButton(
                profileSettings.value("3DS/ZL", QGamepadManager::ButtonL2)));
    comboBoxZR = populateItems(variantToButton(
                profileSettings.value("3DS/ZR", QGamepadManager::ButtonR2)));
    comboBoxHome = populateItems(variantToButton(
                profileSettings.value("3DS/Home", QGamepadManager::ButtonInvalid)));
    comboBoxPower = populateItems(variantToButton(
                profileSettings.value("3DS/Power", QGamepadManager::ButtonInvalid)));
    comboBoxPowerLong = populateItems(variantToButton(
                profileSettings.value("3DS/PowerLong", QGamepadManager::ButtonInvalid)));

    txtCppVal = new QLineEdit();
    txtStickVal = new QLineEdit();
    validator = new QIntValidator();

    txtCppVal->setValidator(validator);
    txtStickVal->setValidator(validator);

    txtCppVal->setText(tr("%1").arg(CPP_BOUND));
    txtStickVal->setText(tr("%1").arg(CPAD_BOUND));

    txtCppVal->setClearButtonEnabled(true);
    txtCppVal->setInputMethodHints(Qt::ImhPreferNumbers);
    txtStickVal->setClearButtonEnabled(true);
    txtStickVal->setInputMethodHints(Qt::ImhPreferNumbers);

    invertYCheckbox = new QCheckBox(this);
    invertYCppCheckbox = new QCheckBox(this);
    swapSticksCheckbox = new QCheckBox(this);
    disableCStickCheckbox = new QCheckBox();
    mhCameraCheckbox = new QCheckBox(this);
    rsSmashCheckbox = new QCheckBox(this);
    rsFaceButtonsCheckbox = new QCheckBox();
    rsSamusReturnsCheckbox = new QCheckBox();

    invertYCheckbox->setChecked(profileSettings.value("Misc/invertY", false).toBool());
    invertYCppCheckbox->setChecked(profileSettings.value("Misc/invertCPPY", false).toBool());
    swapSticksCheckbox->setChecked(profileSettings.value("Misc/swapSticks", false).toBool());
    disableCStickCheckbox->setChecked(profileSettings.value("Misc/DisableC", false).toBool());
    mhCameraCheckbox->setChecked(profileSettings.value("RightStick/DPad", false).toBool());
    rsSmashCheckbox->setChecked(profileSettings.value("RightStick/Smash", false).toBool());
    rsFaceButtonsCheckbox->setChecked(profileSettings.value("RightStick/ABXY", false).toBool());
    rsSamusReturnsCheckbox->setChecked(profileSettings.value("RightStick/SamusReturns", false).toBool());

    configNameEdit = new QLineEdit(this);
    configNameEdit->setClearButtonEnabled(true);
    configNameEdit->setText(profileSettings.value("ButtonConfig/Name", "Default").toString());

    if (QSysInfo::productType() == "windows")
    {
        windowsTouchScreenScale = new QDoubleSpinBox();
        windowsTouchScreenScale->setSingleStep(0.1);
        windowsTouchScreenScale->setMinimum(1.0);
        windowsTouchScreenScale->setValue(profileSettings.value("/TouchScreen/Scale", 1.0).toDouble());
    }

    profileSettings.endGroup();

    applyButton = new QPushButton(tr("💾 &Save"), this);
    saveAsButton = new QPushButton(tr("💾 Save &As"), this);
    saveAsButton->setEnabled(false);
    loadButton = new QPushButton(tr("💿 &Load"), this);
    loadButton->setEnabled(false);
    deleteButton = new QPushButton(tr("🗑️ &Delete"), this);

    if (buttonProfile == "Default"){
        deleteButton->setEnabled(false);
    }

    comboBoxPowerLong->setFocusPolicy(Qt::NoFocus);
    comboBoxPower->setFocusPolicy(Qt::NoFocus);
    comboBoxHome->setFocusPolicy(Qt::NoFocus);
    comboBoxZR->setFocusPolicy(Qt::NoFocus);
    comboBoxZL->setFocusPolicy(Qt::NoFocus);
    comboBoxStart->setFocusPolicy(Qt::NoFocus);
    comboBoxSelect->setFocusPolicy(Qt::NoFocus);
    comboBoxR->setFocusPolicy(Qt::NoFocus);
    comboBoxL->setFocusPolicy(Qt::NoFocus);
    comboBoxRight->setFocusPolicy(Qt::NoFocus);
    comboBoxLeft->setFocusPolicy(Qt::NoFocus);
    comboBoxDown->setFocusPolicy(Qt::NoFocus);
    comboBoxUp->setFocusPolicy(Qt::NoFocus);
    comboBoxY->setFocusPolicy(Qt::NoFocus);
    comboBoxX->setFocusPolicy(Qt::NoFocus);
    comboBoxB->setFocusPolicy(Qt::NoFocus);
    comboBoxA->setFocusPolicy(Qt::NoFocus);

    disableCStickCheckbox->setFocusPolicy(Qt::NoFocus);
    rsFaceButtonsCheckbox->setFocusPolicy(Qt::NoFocus);
    rsSmashCheckbox->setFocusPolicy(Qt::NoFocus);
    mhCameraCheckbox->setFocusPolicy(Qt::NoFocus);
    rsSamusReturnsCheckbox->setFocusPolicy(Qt::NoFocus);
    swapSticksCheckbox->setFocusPolicy(Qt::NoFocus);
    invertYCppCheckbox->setFocusPolicy(Qt::NoFocus);
    invertYCheckbox->setFocusPolicy(Qt::NoFocus);

    applyButton->setFocusPolicy(Qt::NoFocus);
    saveAsButton->setFocusPolicy(Qt::NoFocus);
    loadButton->setFocusPolicy(Qt::NoFocus);
    deleteButton->setFocusPolicy(Qt::NoFocus);

    layout = new QGridLayout(this);

    layout->addWidget(new QLabel("Y Button"), 0, 0);
    layout->addWidget(comboBoxY, 0, 1);
    layout->addWidget(new QLabel("X Button"), 0, 2);
    layout->addWidget(comboBoxX, 0, 3);
    layout->addWidget(new QLabel("B Button"), 1, 0);
    layout->addWidget(comboBoxB, 1, 1);
    layout->addWidget(new QLabel("A Button"), 1, 2);
    layout->addWidget(comboBoxA, 1, 3);

    layout->addWidget(new QLabel("DPad-Down"), 2, 0);
    layout->addWidget(comboBoxDown, 2, 1);
    layout->addWidget(new QLabel("DPad-Up"), 2, 2);
    layout->addWidget(comboBoxUp, 2, 3);
    layout->addWidget(new QLabel("DPad-Left"), 3, 0);
    layout->addWidget(comboBoxLeft, 3, 1);
    layout->addWidget(new QLabel("DPad-Right"), 3, 2);
    layout->addWidget(comboBoxRight, 3, 3);

    layout->addWidget(new QLabel("L Button"), 4, 0);
    layout->addWidget(comboBoxL, 4, 1);
    layout->addWidget(new QLabel("R Button"), 4, 2);
    layout->addWidget(comboBoxR, 4, 3);
    layout->addWidget(new QLabel("ZL Button"), 5, 0);
    layout->addWidget(comboBoxZL, 5, 1);
    layout->addWidget(new QLabel("ZR Button"), 5, 2);
    layout->addWidget(comboBoxZR, 5, 3);

    layout->addWidget(new QLabel("Select"), 6, 0);
    layout->addWidget(comboBoxSelect, 6, 1);
    layout->addWidget(new QLabel("Start"), 6, 2);
    layout->addWidget(comboBoxStart, 6, 3);

    layout->addWidget(new QLabel("Power Button"), 7, 0);
    layout->addWidget(comboBoxPower, 7, 1);
    layout->addWidget(new QLabel("Power-Long"), 7, 2);
    layout->addWidget(comboBoxPowerLong, 7, 3);
    layout->addWidget(new QLabel("Home Button"), 8, 0);
    layout->addWidget(comboBoxHome, 8, 1, 1, 3);
    layout->addWidget(new QLabel("Stick Range:"), 9, 0);
    layout->addWidget(txtStickVal, 9, 1, 1, 3);
    layout->addWidget(new QLabel("CPP Range:"), 10, 0);
    layout->addWidget(txtCppVal, 10, 1, 1, 3);

    layout->addWidget(new QLabel("Invert Y axis"), 11, 0);
    layout->addWidget(invertYCheckbox, 11, 1);
    layout->addWidget(new QLabel("Invert CPP Y"), 11, 2);
    layout->addWidget(invertYCppCheckbox, 11, 3);
    layout->addWidget(new QLabel("Swap CPads"), 12, 2);
    layout->addWidget(swapSticksCheckbox, 12, 3);
    layout->addWidget(new QLabel("Disable C"), 12, 0);
    layout->addWidget(disableCStickCheckbox, 12, 1);

    layout->addWidget(new QLabel("RS as DPad"), 13, 0);
    layout->addWidget(mhCameraCheckbox, 13, 1);
    layout->addWidget(new QLabel("RS as Smash"), 14, 0);
    layout->addWidget(rsSmashCheckbox, 14, 1);
    layout->addWidget(new QLabel("RS as ABXY"), 13, 2);
    layout->addWidget(rsFaceButtonsCheckbox, 13, 3);
    layout->addWidget(new QLabel("RS as L+Pad"), 14, 2);
    layout->addWidget(rsSamusReturnsCheckbox, 14, 3);

    layout->addWidget(applyButton, 15, 0, 1, 4);

    layout->addWidget(saveAsButton, 16, 2, 1, 2);
    layout->addWidget(configNameEdit, 16, 0, 1, 2);

    layout->addWidget(comboBoxProfiles, 17, 0, 1, 2);
    layout->addWidget(loadButton, 17, 2, 1, 1);
    layout->addWidget(deleteButton, 17, 3, 1, 1);

    if (QSysInfo::productType() == "windows")
    {
        QFrame *lineA = new QFrame;
        lineA->setFrameShape(QFrame::HLine);
        lineA->setFrameShadow(QFrame::Sunken);
        QFrame *lineB = new QFrame;
        lineB->setFrameShape(QFrame::HLine);
        lineB->setFrameShadow(QFrame::Sunken);
        layout->addWidget(lineA, 18, 0, 1, 4);
        layout->addWidget(lineB, 19, 0, 1, 4);
        layout->addWidget(new QLabel("Touch Screen Scale"), 20, 0, 1, 2);
        layout->addWidget(windowsTouchScreenScale, 20, 2, 1, 2);
    }

    connect(applyButton, &QPushButton::pressed, this,
            [this, ts](void)
    {
        applySettings();
    });

    connect(configNameEdit, &QLineEdit::textChanged, this,
            [this](const QString &text)
    {
        QString toCheck = text.simplified();
        saveAsButton->setEnabled(toCheck != buttonProfile && toCheck != "Default" && toCheck != "");
    });

    if (QSysInfo::productType() == "windows")
    {
        connect(windowsTouchScreenScale, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [=](double d)
        {
            touchScreen->resize(TOUCH_SCREEN_WIDTH*d, TOUCH_SCREEN_HEIGHT*d);
        });
    }

    connect(saveAsButton, &QPushButton::pressed, this,
            [this](void)
    {
        QString newName = configNameEdit->text();
        buttonProfile = newName.simplified();
        profileSettings.setValue(buttonProfile+"/ButtonConfig/Name", newName);
        applySettings();
        settings.setValue("InputRedirection/ButtonConfig", newName);

        touchScreen->tsShortcutGui.loadNewShortcuts();

        comboBoxProfiles->addItem(buttonProfile);
        comboBoxProfiles->setCurrentIndex(comboBoxProfiles->findText(buttonProfile));
        applyButton->setEnabled(true);
        saveAsButton->setEnabled(false);
    });

    connect(comboBoxProfiles, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [this](void)
    {
        QString currentSelection = comboBoxProfiles->currentText();
        loadButton->setEnabled(buttonProfile != currentSelection);
        deleteButton->setEnabled(currentSelection != "Default");
    });

    connect(loadButton, &QPushButton::pressed, this,
            [this](void)
    {
        loadSettings();
    });

    connect(deleteButton, &QPushButton::pressed, this,
            [this](void)
    {
        deleteProfile();
    });

    applySettings();
}

QComboBox* ConfigWindow::populateItems(QGamepadManager::GamepadButton button)
{
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("A", QGamepadManager::ButtonA);
    comboBox->addItem("B", QGamepadManager::ButtonB);
    comboBox->addItem("X", QGamepadManager::ButtonX);
    comboBox->addItem("Y", QGamepadManager::ButtonY);
    comboBox->addItem("Up", QGamepadManager::ButtonUp);
    comboBox->addItem("Down", QGamepadManager::ButtonDown);
    comboBox->addItem("Right", QGamepadManager::ButtonRight);
    comboBox->addItem("Left", QGamepadManager::ButtonLeft);
    comboBox->addItem("LB", QGamepadManager::ButtonL1);
    comboBox->addItem("RB", QGamepadManager::ButtonR1);
    comboBox->addItem("LT", QGamepadManager::ButtonL2);
    comboBox->addItem("RT", QGamepadManager::ButtonR2);
    comboBox->addItem("Start", QGamepadManager::ButtonStart);
    comboBox->addItem("Back", QGamepadManager::ButtonSelect);
    comboBox->addItem("L3", QGamepadManager::ButtonL3);
    comboBox->addItem("R3", QGamepadManager::ButtonR3);
    comboBox->addItem("Guide", QGamepadManager::ButtonGuide);
    comboBox->addItem("None", QGamepadManager::ButtonInvalid);

    int index = comboBox->findData(button);
    comboBox->setCurrentIndex(index);

    return comboBox;
}

void ConfigWindow::setIndexFromValue(QComboBox *comboBox, QVariant value)
{
    int index = comboBox->findData(value);
    comboBox->setCurrentIndex(index);
}

QVariant ConfigWindow::currentData(QComboBox *comboBox)
{
    QVariant variant;

    variant = comboBox->itemData(comboBox->currentIndex());

    return variant;
}

void ConfigWindow::applySettings(void)
{
    profileSettings.beginGroup(buttonProfile);

    QGamepadManager::GamepadButton a = variantToButton(currentData(comboBoxA));
    hidButtonsAB[0] = a;
    profileSettings.setValue("3DS/A", a);
    QGamepadManager::GamepadButton b = variantToButton(currentData(comboBoxB));
    hidButtonsAB[1] = b;
    profileSettings.setValue("3DS/B", b);
    QGamepadManager::GamepadButton x = variantToButton(currentData(comboBoxX));
    hidButtonsXY[0] = x;
    profileSettings.setValue("3DS/X", x);
    QGamepadManager::GamepadButton y = variantToButton(currentData(comboBoxY));
    hidButtonsXY[1] = y;
    profileSettings.setValue("3DS/Y", y);

    QGamepadManager::GamepadButton right = variantToButton(currentData(comboBoxRight));
    hidButtonsMiddle[2] = right;
    profileSettings.setValue("3DS/Right", right);
    QGamepadManager::GamepadButton left = variantToButton(currentData(comboBoxLeft));
    hidButtonsMiddle[3] = left;
    profileSettings.setValue("3DS/Left", left);
    QGamepadManager::GamepadButton up = variantToButton(currentData(comboBoxUp));
    hidButtonsMiddle[4] = up;
    profileSettings.setValue("3DS/Up", up);
    QGamepadManager::GamepadButton down = variantToButton(currentData(comboBoxDown));
    hidButtonsMiddle[5] = down;
    profileSettings.setValue("3DS/Down", down);

    QGamepadManager::GamepadButton r = variantToButton(currentData(comboBoxR));
    hidButtonsMiddle[6] = r;
    profileSettings.setValue("3DS/R", r);
    QGamepadManager::GamepadButton l = variantToButton(currentData(comboBoxL));
    hidButtonsMiddle[7] = l;
    profileSettings.setValue("3DS/L", l);

    QGamepadManager::GamepadButton select = variantToButton(currentData(comboBoxSelect));
    hidButtonsMiddle[0] = select;
    profileSettings.setValue("3DS/Select", select);
    QGamepadManager::GamepadButton start = variantToButton(currentData(comboBoxStart));
    hidButtonsMiddle[1] = start;
    profileSettings.setValue("3DS/Start", start);

    QGamepadManager::GamepadButton zr = variantToButton(currentData(comboBoxZR));
    irButtons[0] = zr;
    profileSettings.setValue("3DS/ZR", zr);
    QGamepadManager::GamepadButton zl = variantToButton(currentData(comboBoxZL));
    irButtons[1] = zl;
    profileSettings.setValue("3DS/ZL", zl);

    QGamepadManager::GamepadButton power = variantToButton(currentData(comboBoxPower));
    powerButton = power;
    profileSettings.setValue("3DS/Power", power);

    QGamepadManager::GamepadButton powerLong = variantToButton(currentData(comboBoxPowerLong));
    powerLongButton = powerLong;
    profileSettings.setValue("3DS/PowerLong", powerLong);

    QGamepadManager::GamepadButton home = variantToButton(currentData(comboBoxHome));
    homeButton = home;
    profileSettings.setValue("3DS/Home", home);

    bool invy = invertYCheckbox->isChecked();
    yAxisMultiplier = (invy) ? -1 : 1;
    profileSettings.setValue("Misc/InvertY", invy);

    bool invcy = invertYCppCheckbox->isChecked();
    yAxisMultiplierCpp = (invcy) ? -1 : 1;
    profileSettings.setValue("Misc/InvertCPPY", invcy);

    bool swap = swapSticksCheckbox->isChecked();
    btnSettings.setShouldSwapStick(swap);
    profileSettings.setValue("Misc/SwapSticks", swap);

    bool rssmash = rsSmashCheckbox->isChecked();
    btnSettings.setRightStickSmash(rssmash);
    profileSettings.setValue("RightStick/Smash", rssmash);

    bool rsabxy = rsFaceButtonsCheckbox->isChecked();
    btnSettings.setRightStickFaceButtons(rsabxy);
    profileSettings.setValue("RightStick/ABXY", rsabxy);

    bool rsdpad = mhCameraCheckbox->isChecked();
    btnSettings.setMonsterHunterCamera(rsdpad);
    profileSettings.setValue("RightStick/DPad", rsdpad);

    bool disablec = disableCStickCheckbox->isChecked();
    btnSettings.setCStickDisabled(disablec);
    profileSettings.setValue("Misc/DisableC", disablec);

    bool rssamus = rsSamusReturnsCheckbox->isChecked();
    btnSettings.setSamusReturnsAiming(rssamus);
    profileSettings.setValue("RightStick/SamusReturns", rssamus);

    CPP_BOUND = txtCppVal->text().toInt();
    CPAD_BOUND = txtStickVal->text().toInt();
    profileSettings.setValue("3DS/StickBound", CPAD_BOUND);
    profileSettings.setValue("3DS/CppBound", CPP_BOUND);

    profileSettings.endGroup();

    touchScreen->updatePixmap();
}

void ConfigWindow::loadSettings(void)
{
    buttonProfile = comboBoxProfiles->currentText();

    profileSettings.beginGroup(buttonProfile);

    setIndexFromValue(comboBoxA, variantToButton(profileSettings.value("3DS/A", QGamepadManager::ButtonA)));
    setIndexFromValue(comboBoxB, variantToButton(profileSettings.value("3DS/B", QGamepadManager::ButtonB)));
    setIndexFromValue(comboBoxX, variantToButton(profileSettings.value("3DS/X", QGamepadManager::ButtonX)));
    setIndexFromValue(comboBoxY, variantToButton(profileSettings.value("3DS/Y", QGamepadManager::ButtonY)));
    setIndexFromValue(comboBoxUp, variantToButton(profileSettings.value("3DS/Up", QGamepadManager::ButtonUp)));
    setIndexFromValue(comboBoxDown, variantToButton(profileSettings.value("3DS/Down", QGamepadManager::ButtonDown)));
    setIndexFromValue(comboBoxLeft, variantToButton(profileSettings.value("3DS/Left", QGamepadManager::ButtonLeft)));
    setIndexFromValue(comboBoxRight, variantToButton(profileSettings.value("3DS/Right", QGamepadManager::ButtonRight)));
    setIndexFromValue(comboBoxL, variantToButton(profileSettings.value("3DS/L", QGamepadManager::ButtonL1)));
    setIndexFromValue(comboBoxR, variantToButton(profileSettings.value("3DS/R", QGamepadManager::ButtonR1)));
    setIndexFromValue(comboBoxSelect, variantToButton(profileSettings.value("3DS/Select", QGamepadManager::ButtonSelect)));
    setIndexFromValue(comboBoxStart, variantToButton(profileSettings.value("3DS/Start", QGamepadManager::ButtonStart)));
    setIndexFromValue(comboBoxZL, variantToButton(profileSettings.value("3DS/ZL", QGamepadManager::ButtonL2)));
    setIndexFromValue(comboBoxZR, variantToButton(profileSettings.value("3DS/ZR", QGamepadManager::ButtonR2)));
    setIndexFromValue(comboBoxHome, variantToButton(profileSettings.value("3DS/Home", QGamepadManager::ButtonInvalid)));
    setIndexFromValue(comboBoxPower, variantToButton(profileSettings.value("3DS/Power", QGamepadManager::ButtonInvalid)));
    setIndexFromValue(comboBoxPowerLong, variantToButton(profileSettings.value("3DS/PowerLong", QGamepadManager::ButtonInvalid)));

    configNameEdit->setText(buttonProfile);

    invertYCheckbox->setChecked(profileSettings.value("Misc/InvertY", false).toBool());
    invertYCppCheckbox->setChecked(profileSettings.value("Misc/InvertCPPY", false).toBool());
    swapSticksCheckbox->setChecked(profileSettings.value("Misc/SwapSticks", false).toBool());
    mhCameraCheckbox->setChecked(profileSettings.value("RightStick/DPad", false).toBool());
    rsSmashCheckbox->setChecked(profileSettings.value("RightStick/Smash", false).toBool());
    disableCStickCheckbox->setChecked(profileSettings.value("Misc/DisableC", false).toBool());
    rsFaceButtonsCheckbox->setChecked(profileSettings.value("RightStick/ABXY", false).toBool());
    rsSamusReturnsCheckbox->setChecked(profileSettings.value("RightStick/SamusReturns", false).toBool());

    txtCppVal->setText(profileSettings.value("3DS/CppBound", 127).toString());
    txtStickVal->setText(profileSettings.value("3DS/StickBound", 1488).toString());

    if (QSysInfo::productType() == "windows")
    {
        windowsTouchScreenScale->setValue(profileSettings.value("/TouchScreen/Scale", 1.0).toDouble());
    }

    profileSettings.endGroup();

    applySettings();
    settings.setValue("InputRedirection/ButtonConfig", buttonProfile);
    touchScreen->tsShortcutGui.loadNewShortcuts();
    double newTSScale = profileSettings.value(buttonProfile+"/TouchScreen/Scale", 1).toDouble();
    touchScreen->resize(TOUCH_SCREEN_WIDTH*newTSScale, TOUCH_SCREEN_HEIGHT*newTSScale);

    loadButton->setEnabled(false);
}

void ConfigWindow::deleteProfile(void)
{
    QString toDelete = comboBoxProfiles->currentText();

    comboBoxProfiles->setCurrentIndex(comboBoxProfiles->findText("Default"));
    comboBoxProfiles->removeItem(comboBoxProfiles->findText(toDelete));

    profileSettings.remove(toDelete);

    if (buttonProfile == toDelete)
    {
        buttonProfile = "Default";
        settings.setValue("InputRedirection/ButtonConfig", buttonProfile);
        loadSettings();
    }
}
