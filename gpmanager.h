#ifndef GPMANAGER_H
#define GPMANAGER_H

#include <QGamepadManager>
#include <QGamepad>

#include "global.h"

class GamepadMonitor : public QObject
{
public:
    GamepadMonitor(QObject *parent);

private:
    QGamepadManager::GamepadAxis axLeftX, axLeftY, axRightX, axRightY;

    void leftX(QGamepadManager::GamepadAxis axis, double value);
    void leftY(QGamepadManager::GamepadAxis axis, double value);
    void rightX(QGamepadManager::GamepadAxis axis, double value);
    void rightY(QGamepadManager::GamepadAxis axis, double value);

    void rsSmashX(double value);
    void rsSmashY(void);
    void samusReturnsAimingX(double value);
    void samusReturnsAimingY(void);
    void rsFaceButtonsX(double value);
    void rsFaceButtonsY(void);
    void rsDpadX(double value);
    void rsDPadY(void);
};

#endif // GPMANAGER_H
