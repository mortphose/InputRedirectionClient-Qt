#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include "global.h"
#include "touchscreen.h"
#include <QComboBox>

class ConfigWindow : public QDialog
{
private:
    QGridLayout *layout;
    QComboBox   *comboBoxA, *comboBoxB, *comboBoxX,
                *comboBoxY, *comboBoxL, *comboBoxR,
                *comboBoxUp, *comboBoxDown, *comboBoxLeft,
                *comboBoxRight, *comboBoxStart, *comboBoxSelect,
                *comboBoxZL, *comboBoxZR, *comboBoxHome,
                *comboBoxPower, *comboBoxPowerLong, *comboBoxProfiles;

    QPushButton *applyButton, *saveAsButton, *loadButton, *deleteButton, *saveButton;

    QCheckBox   *invertYCheckbox, *invertYCppCheckbox, *swapSticksCheckbox,
                *mhCameraCheckbox, *rsSmashCheckbox,
                *disableCStickCheckbox, *rsFaceButtonsCheckbox;

    QLineEdit *txtStickVal, *txtCppVal, *configNameEdit;

    QValidator *validator;

    TouchScreen * touchScreen;

    QComboBox* populateItems(QGamepadManager::GamepadButton button);    

    QVariant currentData(QComboBox *comboBox);

    void setIndexFromValue(QComboBox *comboBox, QVariant value);

    void applySettings(void);

    void loadSettings(void);

    void deleteProfile(void);

public:
    ConfigWindow(QWidget *parent = nullptr, TouchScreen *ts = nullptr);
};

#endif // CONFIGWINDOW_H
