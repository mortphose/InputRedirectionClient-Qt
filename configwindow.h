#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include "global.h"
#include "touchscreen.h"
#include <QComboBox>

class ConfigWindow : public QDialog
{
private:
    QVBoxLayout *layout;
    QFormLayout *formLayout;
    QComboBox *comboBoxA, *comboBoxB, *comboBoxX, *comboBoxY, *comboBoxL, *comboBoxR,
        *comboBoxUp, *comboBoxDown, *comboBoxLeft, *comboBoxRight, *comboBoxStart, *comboBoxSelect,
        *comboBoxZL, *comboBoxZR, *comboBoxHome, *comboBoxPower, *comboBoxPowerLong, *comboBoxTouch1, *comboBoxTouch2;
    QLineEdit *touchButton1XEdit, *touchButton1YEdit,
        *touchButton2XEdit, *touchButton2YEdit;
    QPushButton *saveButton, *closeButton;
    QCheckBox *invertYCheckbox, *invertYCppCheckbox, *swapSticksCheckbox, *mhCameraCheckbox, *rsSmashCheckbox;

    QComboBox* populateItems(QGamepadManager::GamepadButton button);

    QVariant currentData(QComboBox *comboBox);

public:
    ConfigWindow(QWidget *parent = nullptr, TouchScreen *ts = nullptr);
};

#endif // CONFIGWINDOW_H
