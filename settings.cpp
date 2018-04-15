#include "settings.h"

bool Settings::isShouldSwapStick()
{
    return shouldSwapStick;
}

bool Settings::isMonsterHunterCamera()
{
    return monsterHunterCamera;
}

bool Settings::isRightStickSmash()
{
    return rightStickSmash;
}

bool Settings::isSmashingV()
{
    return smashingV;
}

bool Settings::isSmashingH()
{
    return smashingH;
}

bool Settings::isRightStickFaceButtons()
{
    return rightStickFaceButtons;
}

bool Settings::isCStickDisabled()
{
    return cStickDisabled;
}

bool Settings::isSamusReturnsAiming()
{
    return samusReturnsAiming;
}

bool Settings::isSamusAimingV()
{
    return samusAimingV;
}

bool Settings::isSamusAimingH()
{
    return samusAimingH;
}

/***SETTERS***/
void Settings::setShouldSwapStick(bool b)
{
    shouldSwapStick = b;
}

void Settings::setMonsterHunterCamera(bool b)
{
    monsterHunterCamera = b;
}

void Settings::setRightStickSmash(bool b)
{
    rightStickSmash = b;
}

void Settings::setSmashingV(bool b)
{
    smashingV = b;
}

void Settings::setSmashingH(bool b)
{
    smashingH = b;
}

void Settings::setRightStickFaceButtons(bool b)
{
    rightStickFaceButtons = b;
}

void Settings::setCStickDisabled(bool b)
{
    cStickDisabled = b;
}

void Settings::setSamusReturnsAiming(bool b)
{
    samusReturnsAiming = b;
}

void Settings::setSamusAimingV(bool b)
{
    samusAimingV = b;
}

void Settings::setSamusAimingH(bool b)
{
    samusAimingH = b;
}
