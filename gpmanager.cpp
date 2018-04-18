#include "gpmanager.h"

GamepadMonitor::GamepadMonitor(QObject *parent) : QObject(parent)
{
    axLeftX  = QGamepadManager::AxisLeftX;
    axLeftY  = QGamepadManager::AxisLeftY;
    axRightX = QGamepadManager::AxisRightX;
    axRightY = QGamepadManager::AxisRightY;

    connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonPressEvent, this,
            [this](int deviceId, QGamepadManager::GamepadButton button, double value)
    {
        (void)value;

        if (deviceId != selectedControllerId) return;

        buttons |= QGamepadManager::GamepadButtons(1 << button);

        if (button == homeButton)
        {
            interfaceButtons |= 1;
        }
        if (button == powerButton)
        {
            interfaceButtons |= 2;
        }
        if (button == powerLongButton)
        {
            interfaceButtons |= 4;
        }

        for (auto it = listShortcuts.begin(); it != listShortcuts.end(); ++it) {
            int index = std::distance(listShortcuts.begin(), it);
            ShortCut curShort = listShortcuts.at(index);

            if(curShort.button == button)
            {
                touchScreenPressed = true;
                touchScreenPosition = curShort.pos*tsRatio;
            }
        }
    });

    connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,
            [this](int deviceId, QGamepadManager::GamepadButton button)
    {
        if (deviceId != selectedControllerId) return;

        buttons &= QGamepadManager::GamepadButtons(~(1 << button));

        if (button == homeButton)
        {
            interfaceButtons &= ~1;
        }
        if (button == powerButton)
        {
            interfaceButtons &= ~2;
        }
        if (button == powerLongButton)
        {
            interfaceButtons &= ~4;
        }

        for (auto it = listShortcuts.begin(); it != listShortcuts.end(); ++it) {
            int index = std::distance(listShortcuts.begin(), it);
            ShortCut curShort = listShortcuts.at(index);

            if(curShort.button == button)
            {
                touchScreenPressed = false;
                return;
            }
        }
    });

    connect(QGamepadManager::instance(), &QGamepadManager::gamepadAxisEvent, this,
            [this](int deviceId, QGamepadManager::GamepadAxis axis, double value)
    {
        if (deviceId != selectedControllerId) return;

        if(btnSettings.isShouldSwapStick())
        {
            axLeftX = QGamepadManager::AxisRightX;
            axLeftY = QGamepadManager::AxisRightY;

            axRightX = QGamepadManager::AxisLeftX;
            axRightY = QGamepadManager::AxisLeftY;
        }

        leftX(axis, value);
        leftY(axis, value);
        rightX(axis, value);
        rightY(axis, value);

    });
}

void GamepadMonitor::leftX(QGamepadManager::GamepadAxis axis, double value)
{
    if(axis != axLeftX) return;

    worker->setLeftAxis(value, worker->getLeftAxis().y);
    if (!btnSettings.isSamusAimingH() && !btnSettings.isSamusAimingV())
        worker->setPreviousLAxis(worker->getLeftAxis().x, worker->getPreviousLAxis().y);
}
void GamepadMonitor::leftY(QGamepadManager::GamepadAxis axis, double value)
{
    if(axis != axLeftY) return;

    worker->setLeftAxis(worker->getLeftAxis().x, yAxisMultiplier * -value); // for some reason qt inverts this
    if (!btnSettings.isSamusAimingH() && !btnSettings.isSamusAimingV())
        worker->setPreviousLAxis(worker->getPreviousLAxis().x, worker->getLeftAxis().y);
}
void GamepadMonitor::rightX(QGamepadManager::GamepadAxis axis, double value)
{
    if(axis != axRightX) return;

    if (!btnSettings.isCStickDisabled())
        worker->setRightAxis(value, worker->getRightAxis().y);

    rsSmashX(value);
    samusReturnsAimingX(value);
    rsDpadX(value);
    rsFaceButtonsX(value);
}
void GamepadMonitor::rightY(QGamepadManager::GamepadAxis axis, double value)
{
    if(axis != axRightY) return;

    worker->setRightAxis(worker->getRightAxis().x, yAxisMultiplierCpp * -value);

    rsSmashY();
    samusReturnsAimingY();
    rsDPadY();
    rsFaceButtonsY();

    if (btnSettings.isCStickDisabled())
    {
        worker->setRightAxis(0.0, 0.0);
    }
}

void GamepadMonitor::rsSmashX(double value)
{
    if (!btnSettings.isRightStickSmash()) return;

    if (value > -1.2 && value < -0.5) // RS tilted left
    {
        btnSettings.setSmashingH(true);
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
        worker->setLeftAxis(-1.2, worker->getLeftAxis().y);
    } else if (value > 0.5 && value < 1.2) // RS tilted right
    {
        btnSettings.setSmashingH(true);
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
        worker->setLeftAxis(1.2, worker->getLeftAxis().y);
    } else { // RS neutral, release buttons
        if (!btnSettings.isSmashingH()) return;

        if (!btnSettings.isSmashingV())
            buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[0])); // Release A
        worker->setLeftAxis(worker->getPreviousLAxis().x, worker->getRightAxis().y);
        btnSettings.setSmashingH(false);
    }
}
void GamepadMonitor::rsSmashY()
{
    if (!btnSettings.isRightStickSmash()) return;

    if (worker->getRightAxis().y > -1.2 && worker->getRightAxis().y < -0.5) // RS tilted down
    {
        btnSettings.setSmashingV(true);
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
        worker->setLeftAxis(worker->getLeftAxis().x, -1.2);
    } else if (worker->getRightAxis().y  > 0.5 && worker->getRightAxis().y  < 1.2) // RS tilted up
    {
        btnSettings.setSmashingV(true);
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
        worker->setLeftAxis(worker->getLeftAxis().x, 1.2);
    } else { // RS neutral, release button A
        if (!btnSettings.isSmashingV()) return;

        if (!btnSettings.isSmashingH())
            buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[0])); // Release A
        worker->setLeftAxis(worker->getLeftAxis().x, worker->getPreviousLAxis().y);
        btnSettings.setSmashingV(false);
    }
}

void GamepadMonitor::samusReturnsAimingX(double value)
{
    if (!btnSettings.isSamusReturnsAiming()) return;

    if ((value > -1.2 && value < -0.05) || (value > 0.05 && value < 1.2)) // RS tilted left
    {
        btnSettings.setSamusAimingH(true);
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[7]); // press L
        worker->setLeftAxis(value, worker->getLeftAxis().y);
    } else
    {
        if (!btnSettings.isSamusAimingH()) return;

        if (!btnSettings.isSamusAimingV())
        {
            buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[7])); // Release L
            worker->setLeftAxis(worker->getPreviousLAxis().x-0.01, worker->getPreviousLAxis().y-0.01);
        }
        btnSettings.setSamusAimingH(false);
    }
}
void GamepadMonitor::samusReturnsAimingY(void)
{
    if (!btnSettings.isSamusReturnsAiming()) return;

    if ((worker->getRightAxis().y > -1.2 && worker->getRightAxis().y < -0.05) ||
            (worker->getRightAxis().y > 0.05 && worker->getRightAxis().y < 1.2)) // RS tilted down
    {
        btnSettings.setSamusAimingV(true);
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[7]); // press L
        worker->setLeftAxis(worker->getLeftAxis().x, worker->getRightAxis().y);
    } else
    {
        if (!btnSettings.isSamusAimingV()) return;

        if (!btnSettings.isSamusAimingH())
        {
            buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[7])); // Release L
            worker->setLeftAxis(worker->getLeftAxis().x-0.01, worker->getPreviousLAxis().y-0.01);
        }
        btnSettings.setSamusAimingV(false);
    }
}

void GamepadMonitor::rsDpadX(double value)
{
    if (!btnSettings.isMonsterHunterCamera()) return;

    if (value > -1.2 && value < -0.5) // RS tilted left
    {
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[3]); // press Left
    } else if (value > 0.5 && value < 1.2) // RS tilted right
    {
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[2]); // press Right
    } else { // RS neutral, release buttons
        buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[3])); // Release Left
        buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[2])); // release Right
    }
}
void GamepadMonitor::rsDPadY(void)
{
    if (!btnSettings.isMonsterHunterCamera()) return;

    if (worker->getRightAxis().y > -1.2 && worker->getRightAxis().y  < -0.5) // RS tilted down
    {
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[5]); // press Down
    } else if (worker->getRightAxis().y  > 0.5 && worker->getRightAxis().y  < 1.2) // RS tilted up
    {
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[4]); // press Up
    } else { // RS neutral, release buttons
        buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[5])); // release Down
        buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[4])); // Release Up
    }
}

void GamepadMonitor::rsFaceButtonsX(double value)
{
    if (!btnSettings.isRightStickFaceButtons()) return;

    if (value > -1.2 && value < -0.5) // RS tilted left
    {
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsXY[1]); // press Y
    } else if (value > 0.5 && value < 1.2) // RS tilted right
    {
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
    } else { // RS neutral, release buttons
        buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsXY[1])); // Release Y
        buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[0])); // release A
    }
}
void GamepadMonitor::rsFaceButtonsY()
{
    if (!btnSettings.isRightStickFaceButtons()) return;

    if (worker->getRightAxis().y > -1.2 && worker->getRightAxis().y < -0.5) // RS tilted down
    {
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[1]); // press B
    } else if (worker->getRightAxis().y  > 0.5 && worker->getRightAxis().y < 1.2) // RS tilted up
    {
        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsXY[0]); // press X
    } else { // RS neutral, release buttons
        buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[1])); // release B
        buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsXY[0])); // Release X
    }
}
