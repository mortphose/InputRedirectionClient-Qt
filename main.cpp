#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include <QWidget>
#include <QApplication>
#include <QGamepadManager>
#include <QGamepad>
#include <QtEndian>
#include <QUdpSocket>
#include <QTimer>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDialog>
#include <QSettings>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QSettings>
#include <QComboBox>
#include <QPainter>
#include <QSlider>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <algorithm>
#include <cmath>

#define CPAD_BOUND          0x5d0
#define CPP_BOUND           0x7f

#define TOUCH_SCREEN_WIDTH  320
#define TOUCH_SCREEN_HEIGHT 240

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

double lx = 0.0, ly = 0.0;
double rx = 0.0, ry = 0.0;
double previousLX=lx, previousLY=ly;
QGamepadManager::GamepadButtons buttons = 0;
u32 interfaceButtons = 0;
QString ipAddress;
bool monsterHunterCamera = false;
bool rightStickSmash = false;
bool isSmashingV = false;
bool isSmashingH = false;
int yAxisMultiplier = 1, yAxisMultiplierCpp = 1;

bool touchScreenPressed;
QSize touchScreenSize;
QPoint touchScreenPosition;
int touchScreenScale = 1;

bool shouldSwapStick = false;

bool timerConnected = false;

QSettings settings("TuxSH", "InputRedirectionClient-Qt");

QGamepadManager::GamepadButton variantToButton(QVariant variant)
{
    QGamepadManager::GamepadButton button;

    button = static_cast<QGamepadManager::GamepadButton>(variant.toInt());

    return button;
}

QGamepadManager::GamepadButton homeButton = variantToButton(settings.value("ButtonHome", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton powerButton = variantToButton(settings.value("ButtonPower", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton powerLongButton = variantToButton(settings.value("ButtonPowerLong", QGamepadManager::ButtonInvalid));

QGamepadManager::GamepadButton touchButton1 = variantToButton(settings.value("ButtonT1", QGamepadManager::ButtonInvalid));
QGamepadManager::GamepadButton touchButton2 = variantToButton(settings.value("ButtonT2", QGamepadManager::ButtonInvalid));
int touchButton1X = settings.value("touchButton1X", 0).toInt(),
    touchButton1Y = settings.value("touchButton1Y", 0).toInt(),
    touchButton2X = settings.value("touchButton2X", 0).toInt(),
    touchButton2Y = settings.value("touchButton2Y", 0).toInt();

QGamepadManager::GamepadButton hidButtonsAB[] = {
    variantToButton(settings.value("ButtonA", QGamepadManager::ButtonA)),
    variantToButton(settings.value("ButtonB", QGamepadManager::ButtonB)),
};

QGamepadManager::GamepadButton hidButtonsMiddle[] = {
    variantToButton(settings.value("ButtonSelect", QGamepadManager::ButtonSelect)),
    variantToButton(settings.value("ButtonStart", QGamepadManager::ButtonStart)),
    variantToButton(settings.value("ButtonRight", QGamepadManager::ButtonRight)),
    variantToButton(settings.value("ButtonLeft", QGamepadManager::ButtonLeft)),
    variantToButton(settings.value("ButtonUp", QGamepadManager::ButtonUp)),
    variantToButton(settings.value("ButtonDown", QGamepadManager::ButtonDown)),
    variantToButton(settings.value("ButtonR", QGamepadManager::ButtonR1)),
    variantToButton(settings.value("ButtonL", QGamepadManager::ButtonL1)),
};

QGamepadManager::GamepadButton hidButtonsXY[] = {
    variantToButton(settings.value("ButtonX", QGamepadManager::ButtonX)),
    variantToButton(settings.value("ButtonY", QGamepadManager::ButtonY)),
};

QGamepadManager::GamepadButton irButtons[] = {
    variantToButton(settings.value("ButtonZR", QGamepadManager::ButtonR2)),
    variantToButton(settings.value("ButtonZL", QGamepadManager::ButtonL2)),
};

void sendFrame(void)
{
    u32 hidPad = 0xfff;
    for(u32 i = 0; i < 2; i++)
    {
         if(buttons & (1 << hidButtonsAB[i]))
                hidPad &= ~(1 << i);
    }

    for(u32 i = 2; i < 10; i++)
    {
        if(buttons & (1 << hidButtonsMiddle[i-2]))
            hidPad &= ~(1 << i);
    }

    for(u32 i = 10; i < 12; i++)
   {
            if(buttons & (1 << hidButtonsXY[i-10]))
                hidPad &= ~(1 << i);
   }

    u32 irButtonsState = 0;
    for(u32 i = 0; i < 2; i++)
    {
            if(buttons & (1 << irButtons[i]))
                irButtonsState |= 1 << (i + 1);
    }

    u32 touchScreenState = 0x2000000;
    u32 circlePadState = 0x7ff7ff;
    u32 cppState = 0x80800081;

    if(touchScreenPressed)
    {
        u32 x = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.x()/touchScreenScale),
                                       TOUCH_SCREEN_WIDTH)) / TOUCH_SCREEN_WIDTH;
        u32 y = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.y()/touchScreenScale),
                                       TOUCH_SCREEN_HEIGHT)) / TOUCH_SCREEN_HEIGHT;
        touchScreenState = (1 << 24) | (y << 12) | x;
    }

    if(lx != 0.0 || ly != 0.0)
      {
          u32 x = (u32)(lx * CPAD_BOUND + 0x800);
          u32 y = (u32)(ly * CPAD_BOUND + 0x800);
          x = x >= 0xfff ? (lx < 0.0 ? 0x000 : 0xfff) : x;
          y = y >= 0xfff ? (ly < 0.0 ? 0x000 : 0xfff) : y;

          circlePadState = (y << 12) | x;
      }

      if(rx != 0.0 || ry != 0.0 || irButtonsState != 0)
      {
          // We have to rotate the c-stick position 45°. Thanks, Nintendo.
          u32 x = (u32)(M_SQRT1_2 * (rx + ry) * CPP_BOUND + 0x80);
          u32 y = (u32)(M_SQRT1_2 * (ry - rx) * CPP_BOUND + 0x80);
          x = x >= 0xff ? (rx < 0.0 ? 0x00 : 0xff) : x;
          y = y >= 0xff ? (ry < 0.0 ? 0x00 : 0xff) : y;

          cppState = (y << 24) | (x << 16) | (irButtonsState << 8) | 0x81;
      }

      QByteArray ba(20, 0);
      qToLittleEndian(hidPad, (uchar *)ba.data());
      qToLittleEndian(touchScreenState, (uchar *)ba.data() + 4);
      qToLittleEndian(circlePadState, (uchar *)ba.data() + 8);
      qToLittleEndian(cppState, (uchar *)ba.data() + 12);
      qToLittleEndian(interfaceButtons, (uchar *)ba.data() + 16);
      QUdpSocket().writeDatagram(ba, QHostAddress(ipAddress), 4950);

//      if(touchScreenPressed)
//      {
//          u32 x = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.x()),
//                                         touchScreenSize.width())) / touchScreenSize.width();
//          u32 y = (u32)(0xfff * std::min(std::max(0, touchScreenPosition.y()),
//                                         touchScreenSize.height())) / touchScreenSize.height();
//
//          touchScreenState = (1 << 24) | (y << 12) | x;
//      }

}

struct GamepadMonitor : public QObject {

    GamepadMonitor(QObject *parent = nullptr) : QObject(parent)
    {
        connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonPressEvent, this,
            [](int deviceId, QGamepadManager::GamepadButton button, double value)
        {
            (void)deviceId;
            (void)value;
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

            if (button == touchButton1)
            {
                touchScreenPressed = true;
                touchScreenPosition = QPoint(touchButton1X*touchScreenScale,
                                             touchButton1Y*touchScreenScale);
            }
            if (button == touchButton2)
            {
                touchScreenPressed = true;
                touchScreenPosition = QPoint(touchButton2X*touchScreenScale,
                                             touchButton2Y*touchScreenScale);
            }


            sendFrame();
        });

        connect(QGamepadManager::instance(), &QGamepadManager::gamepadButtonReleaseEvent, this,
            [](int deviceId, QGamepadManager::GamepadButton button)
        {
            (void)deviceId;
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

            if ((button == touchButton1) || (button == touchButton2))
            {
                touchScreenPressed = false;
            }

            sendFrame();
        });

        connect(QGamepadManager::instance(), &QGamepadManager::gamepadAxisEvent, this,
            [](int deviceId, QGamepadManager::GamepadAxis axis, double value)
        {
            (void)deviceId;
            (void)value;
            QGamepadManager::GamepadAxis axLeftX = QGamepadManager::AxisLeftX,
                                         axLeftY= QGamepadManager::AxisLeftY,
                                         axRightX= QGamepadManager::AxisRightX,
                                         axRightY= QGamepadManager::AxisRightY;

            if(shouldSwapStick)
            {
                axLeftX = QGamepadManager::AxisRightX;
                axLeftY = QGamepadManager::AxisRightY;

                axRightX = QGamepadManager::AxisLeftX;
                axRightY = QGamepadManager::AxisLeftY;
            }

            if(axis==axLeftX)
            {
                lx = value;
                previousLX = lx;
            }
            else
            if(axis==axLeftY)
            {
                ly = yAxisMultiplier * -value; // for some reason qt inverts this
                previousLY = ly;
            }
            else
            if(axis==axRightX)
            {
                rx = value;
                if (monsterHunterCamera)
                {
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
                } else if (rightStickSmash)
                {
                    if (value > -1.2 && value < -0.5) // RS tilted left
                    {
                        isSmashingH = true;
                        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                        lx = -1.2;
                    } else if (value > 0.5 && value < 1.2) // RS tilted right
                    {
                        isSmashingH = true;
                        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                        lx = 1.2;
                    } else { // RS neutral, release buttons
                        if (isSmashingH)
                        {
                            if (!isSmashingV)
                                buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[0])); // Release A
                            lx = previousLX;
                            isSmashingH = false;
                        }
                    }
                }
            }
            else
            if(axis==axRightY)
            {
                ry = yAxisMultiplierCpp * -value;
                if (monsterHunterCamera)
                {
                    if (ry > -1.2 && ry < -0.5) // RS tilted down
                    {
                        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[5]); // press Down
                    } else if (ry > 0.5 && ry < 1.2) // RS tilted up
                    {
                        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsMiddle[4]); // press Up
                    } else { // RS neutral, release buttons
                        buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[5])); // release Down
                        buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsMiddle[4])); // Release Up
                    }
                } else if (rightStickSmash)
                {
                    if (ry > -1.2 && ry < -0.5) // RS tilted down
                    {
                        isSmashingV = true;
                        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                        ly = -1.2;
                    } else if (ry > 0.5 && ry < 1.2) // RS tilted up
                    {
                        isSmashingV = true;
                        buttons |= QGamepadManager::GamepadButtons(1 << hidButtonsAB[0]); // press A
                        ly = 1.2;
                    } else { // RS neutral, release button A
                        if (isSmashingV)
                        {
                            if (!isSmashingH)
                                buttons &= QGamepadManager::GamepadButtons(~(1 << hidButtonsAB[0])); // Release A
                            ly = previousLY;
                            isSmashingV = false;
                        }
                    }
                }
            }

            sendFrame();
        });
    }
};

struct TouchScreen : public QDialog {
private:
    QLabel *bgLabel;
public:
    TouchScreen(QWidget *parent = nullptr) : QDialog(parent)
    {
        this->setFixedSize(TOUCH_SCREEN_WIDTH, TOUCH_SCREEN_HEIGHT);
        this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint);
        this->setWindowTitle(tr("InputRedirectionClient-Qt - Touch screen"));

        QString curPath = qApp->QCoreApplication::applicationDirPath()+"/Touchscreen.jpg";
        QPixmap background = QPixmap(curPath);

        if (background.isNull())
        {
            background = QPixmap(TOUCH_SCREEN_WIDTH, TOUCH_SCREEN_HEIGHT);
            background.fill(Qt::transparent);
        }

        QPainter painter(&background);
        QPen pen;
        pen.setWidth(2*touchScreenScale);

        if (touchButton1 != QGamepadManager::ButtonInvalid)
        {
            pen.setColor(Qt::red);
            painter.setPen(pen);

            painter.drawEllipse(QPoint(touchButton1X*touchScreenScale,
                                       touchButton1Y*touchScreenScale),
                                        3*touchScreenScale, 3*touchScreenScale);
        }
        if (touchButton2 != QGamepadManager::ButtonInvalid)
        {
            pen.setColor(Qt::blue);
            painter.setPen(pen);

            painter.drawEllipse(QPoint(touchButton2X*touchScreenScale,
                                       touchButton2Y*touchScreenScale),
                                        3*touchScreenScale, 3*touchScreenScale);
        }

        bgLabel = new QLabel(this);
        bgLabel->setFixedHeight(TOUCH_SCREEN_HEIGHT);
        bgLabel->setFixedWidth(TOUCH_SCREEN_WIDTH);
        bgLabel->setPixmap(background);
        bgLabel->setScaledContents(true);
        bgLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    }

    void resizeEvent(QResizeEvent* e)
    {

        QSize newWinSize = e->size();
        QSize curWinSize = e->oldSize();
        QSize propWinSize = e->size();

        if(curWinSize.height() != newWinSize.height())
        {
           propWinSize.setWidth((TOUCH_SCREEN_WIDTH*newWinSize.height())/TOUCH_SCREEN_HEIGHT);
           propWinSize.setHeight(newWinSize.height());
        }

        if(curWinSize.width() != newWinSize.width())
        {
           propWinSize.setWidth(newWinSize.width());
           propWinSize.setHeight((TOUCH_SCREEN_HEIGHT*newWinSize.width())/TOUCH_SCREEN_WIDTH);
        }

        touchScreenSize = propWinSize;
        this->resize(propWinSize);
        bgLabel->setFixedHeight(this->height());
        bgLabel->setFixedWidth(this->width());
    }

    void mousePressEvent(QMouseEvent *ev)
    {
        if(ev->button() == Qt::LeftButton)
        {
            touchScreenPressed = true;
            touchScreenPosition = ev->pos();
            sendFrame();
        }
        if(ev->button() == Qt::RightButton)
        {

           QString strPic = QFileDialog::getOpenFileName(this,
                          tr("Open Touchscreen Image (320x240)"), "MyDocuments",
                          tr("Image Files (*.jpg *.jpeg *.png *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm)"));

            if(!strPic.isNull())
            {
               QPixmap newPic(strPic);

               QPainter painter(&newPic);
               QPen pen;
               pen.setWidth(2*touchScreenScale);

               if (touchButton1 != QGamepadManager::ButtonInvalid)
               {
                   pen.setColor(Qt::red);
                   painter.setPen(pen);

                   painter.drawEllipse(QPoint(touchButton1X*touchScreenScale,
                                              touchButton1Y*touchScreenScale),
                                               3*touchScreenScale, 3*touchScreenScale);
               }
               if (touchButton2 != QGamepadManager::ButtonInvalid)
               {
                   pen.setColor(Qt::blue);
                   painter.setPen(pen);

                   painter.drawEllipse(QPoint(touchButton2X*touchScreenScale,
                                              touchButton2Y*touchScreenScale),
                                               3*touchScreenScale, 3*touchScreenScale);
               }

               bgLabel->setPixmap(newPic);
            }
        }
    }

    void mouseMoveEvent(QMouseEvent *ev)
    {
        if(touchScreenPressed && (ev->buttons() & Qt::LeftButton))
        {
            touchScreenPosition = ev->pos();
            sendFrame();
        }
    }

    void mouseReleaseEvent(QMouseEvent *ev)
    {
        if(ev->button() == Qt::LeftButton)
        {
            touchScreenPressed = false;
            sendFrame();
        }
    }

    void closeEvent(QCloseEvent *ev)
    {
        touchScreenPressed = false;
        sendFrame();
        ev->accept();
    }
};

struct FrameTimer : public QTimer {
    FrameTimer(QObject *parent = nullptr) : QTimer(parent)
    {
        connect(this, &QTimer::timeout, this,
                [](void)
        {
            if (timerConnected)
            sendFrame();
        });
    }
};

struct RemapConfig : public QDialog {
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

    QComboBox* populateItems(QGamepadManager::GamepadButton button)
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

    QVariant currentData(QComboBox *comboBox)
    {
        QVariant variant;

        variant = comboBox->itemData(comboBox->currentIndex());

        return variant;
    }

public:
    RemapConfig(QWidget *parent = nullptr, QDialog *ts = nullptr) : QDialog(parent)
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
            ts->update();
            settings.setValue("touchButton1X", text);
        });
        connect(touchButton1YEdit, &QLineEdit::textChanged, this,
                [ts](const QString &text)
        {
            touchButton1Y = text.toUInt();
            ts->update();
            settings.setValue("touchButton1Y", text);
        });
        connect(touchButton2XEdit, &QLineEdit::textChanged, this,
                [ts](const QString &text)
        {
            touchButton2X = text.toUInt();
            ts->update();
            settings.setValue("touchButton2X", text);
        });
        connect(touchButton2YEdit, &QLineEdit::textChanged, this,
                [ts](const QString &text)
        {
            touchButton2Y = text.toUInt();
            ts->update();
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
            ts->update();

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
};

class Widget : public QWidget
{
private:
    QVBoxLayout *layout;
    QFormLayout *formLayout;
    QPushButton *homeButton, *powerButton, *longPowerButton, *remapConfigButton,
        *connectButton;
    QLineEdit *addrLineEdit, *touchScreenScaleEdit;
    QSlider *touchOpacitySlider;
    TouchScreen *touchScreen;
    RemapConfig *remapConfig;
public:
    Widget(QWidget *parent = nullptr) : QWidget(parent)
    {
        layout = new QVBoxLayout(this);

        addrLineEdit = new QLineEdit(this);
        addrLineEdit->setClearButtonEnabled(true);
        connectButton = new QPushButton(tr("&CONNECT"), this);

        touchScreenScaleEdit = new QLineEdit(this);
        touchScreenScaleEdit->setText("1");

        formLayout = new QFormLayout;
        formLayout->addRow(tr("IP &address"), addrLineEdit);
        formLayout->addRow(tr(""), connectButton);
        formLayout->addRow(tr("Touch Screen &Scale"), touchScreenScaleEdit);

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
            if (!timerConnected)
            {
                timerConnected = true;
                connectButton->setText("DISCONNECT");
            } else {
                timerConnected = false;
                connectButton->setText("CONNECT");
            }
        });

        connect(touchScreenScaleEdit, &QLineEdit::textChanged, this,
                [this](const QString &text)
        {
            touchScreenScale = text.toInt();
            touchScreenScale = touchScreenScale > 1 ? touchScreenScale : 1;
            touchScreen->setFixedSize(TOUCH_SCREEN_WIDTH*touchScreenScale,
                                      TOUCH_SCREEN_HEIGHT*touchScreenScale);
            touchScreen->update();
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
        remapConfig = new RemapConfig(nullptr, touchScreen);

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
        touchScreenPressed = false;
        sendFrame();
        delete touchScreen;
        delete remapConfig;
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    GamepadMonitor m(&w);
    FrameTimer t(&w);
    t.start(50);
    w.show();

    return a.exec();
}
