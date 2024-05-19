#include "tsshortcut.h"

TsShortcut::TsShortcut(QWidget *parent) : QWidget(parent)
{
    instantiateWidgets();
    addWidgetsToLayout();

    this->setMinimumSize(TOUCH_SCREEN_WIDTH, 200);
    this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    this->setVisible(false);

    connectEvents();
}

void TsShortcut::instantiateWidgets()
{
    layout = new QVBoxLayout(this);
    formLayout= new QFormLayout(this);

    //lblDirections = new QLabel("Select a shortcut, then press a button to map it.", this);
    lstWidget = new QListWidget(this);
    btnColorDialog = new QPushButton("Ch&oose", this);
    btnCreateShort = new QPushButton("💾 &Create", this);
    btnPressNow = new QPushButton("🖊️ &Press Selected Shortcut", this);
    btnDelShort = new QPushButton("🗑️ &Delete Selected Shortcut", this);
    btnHelp = new QPushButton("🤔 &Help", this);
    txtShortName = new QLineEdit(this);
    cboxBtns = populateItems();

    btnCreateShort->setEnabled(txtShortName->text() != "" && cboxBtns->currentIndex() != 0);
    bool anyShortcutSelected = !lstWidget->selectedItems().isEmpty();
    btnPressNow->setEnabled(anyShortcutSelected);
    btnDelShort->setEnabled(anyShortcutSelected);
}

void TsShortcut::addWidgetsToLayout()
{
    //layout->addWidget(lblDirections);
    formLayout->addRow("Name", txtShortName);
    formLayout->addRow("Button 🎮", cboxBtns);
    formLayout->addRow("Color   🎨", btnColorDialog);
    formLayout->addRow(btnCreateShort);
    layout->addLayout(formLayout);
    layout->addSpacing(5);
    QFrame *lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);
    layout->addWidget(lineA);
    layout->addSpacing(5);
    layout->addWidget(lstWidget);
    layout->addSpacing(5);
    layout->addWidget(btnPressNow);
    layout->addWidget(btnDelShort);
    layout->addWidget(btnHelp);
}

void TsShortcut::connectEvents()
{
    connectColorButtonEvent();
    connectCreateButtonEvent();
    connectDeleteButtonEvent();
    connectPressButtonEvent();
    connectHelpButtonEvent();
    connectNameChangeEvent();
    connectSelectedChangeEvent();
    connectButtonChangeEvent();
}

void TsShortcut::connectButtonChangeEvent()
{
    connect(cboxBtns, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index)
    {
        btnCreateShort->setEnabled(txtShortName->text() != "" && index != 0);
    });
}

void TsShortcut::connectSelectedChangeEvent()
{
    connect(lstWidget, &QListWidget::itemSelectionChanged, this,
            [this](void)
    {
        bool anyShortcutSelected = !lstWidget->selectedItems().isEmpty();
        btnPressNow->setEnabled(anyShortcutSelected);
        btnDelShort->setEnabled(anyShortcutSelected);

        if (anyShortcutSelected)
        {
            QString shortcutName = lstWidget->selectedItems()[0]->text();
            btnPressNow->setText("🖊️ &Press " + shortcutName);
            btnDelShort->setText("🗑️ &Delete " + shortcutName);
        } else {
            btnPressNow->setText("🖊️ &Press Selected Shortcut");
            btnDelShort->setText("🗑️ &Delete Selected Shortcut");
        }
    });
}

void TsShortcut::connectNameChangeEvent()
{
    connect(txtShortName, &QLineEdit::textChanged, this,
            [this](const QString &text)
    {
        btnCreateShort->setEnabled(text != "" && cboxBtns->currentIndex() != 0);
    });
}

void TsShortcut::connectColorButtonEvent()
{
    connect(btnColorDialog, &QPushButton::released, this,
            [this](void)
    {
        QColor color = QColorDialog::getColor(Qt::yellow, this );
        if( color.isValid() )
        {
            curColor = color;
            QString qss = QString("background-color: %1").arg(color.name());
            btnColorDialog->setStyleSheet(qss);
        }
    });
}

void TsShortcut::connectCreateButtonEvent()
{
    connect(btnCreateShort, &QPushButton::released, this,
            [this](void)
    {
        ShortCut newShortCut;
        QString newShortName = txtShortName->text();

        if(newShortName != "")
        {
            newShortCut.name = newShortName;
            newShortCut.button = variantToButton(cboxBtns->currentData());
            newShortCut.pos = curPos;
            newShortCut.color = ( curColor.isValid() )? curColor : Qt::blue;

            QListWidgetItem *listItemToAdd = createShortcutListItem(newShortCut);

            lstWidget->addItem(listItemToAdd);
            listShortcuts.push_back(newShortCut);
            txtShortName->clear();
            cboxBtns->setCurrentIndex(0);
            btnColorDialog->setStyleSheet("");
            curColor = nullptr;

            for (uint i = 0; i < listShortcuts.size(); i++)
            {
                saveShortcut(i);
            }
            profileSettings.setValue(buttonProfile+"/TouchScreen/Shortcut/Count", QVariant::fromValue(listShortcuts.size()));
        }
        else
        {
            QMessageBox *msgBox = new QMessageBox(0);
            msgBox->setInformativeText(tr("Cannot create new shortcut without a name."));
            msgBox->show();
        }

        lstWidget->clearSelection();
    });
}

QListWidgetItem * TsShortcut::createShortcutListItem(ShortCut newShortCut)
{
    QListWidgetItem *listItemToAdd = new QListWidgetItem(newShortCut.name);
    listItemToAdd->setBackground(newShortCut.color);
    listItemToAdd->setForeground((newShortCut.color.green() > 127)? Qt::black : Qt::white);

    return listItemToAdd;
}

void TsShortcut::saveShortcut(uint i)
{
    QString shortcutSavePath =
            buttonProfile + "/TouchScreen/Shortcut/" + QString::number(i+1) + "/";
    profileSettings.setValue(shortcutSavePath+"Name", listShortcuts[i].name);
    profileSettings.setValue(shortcutSavePath+"Button", listShortcuts[i].button);
    profileSettings.setValue(shortcutSavePath+"X", listShortcuts[i].pos.x());
    profileSettings.setValue(shortcutSavePath+"Y", listShortcuts[i].pos.y());
    profileSettings.setValue(shortcutSavePath+"R", listShortcuts[i].color.red());
    profileSettings.setValue(shortcutSavePath+"G", listShortcuts[i].color.green());
    profileSettings.setValue(shortcutSavePath+"B", listShortcuts[i].color.blue());
}

void TsShortcut::connectDeleteButtonEvent()
{
    connect(btnDelShort, &QPushButton::released, this,
            [this](void)
    {
        if(lstWidget->selectedItems().size() != 0)
        {
            QString toRemove =
                    buttonProfile + "/TouchScreen/Shortcut/" + QString::number(lstWidget->currentRow()+1) + "/";
            profileSettings.remove(toRemove+"Name");
            profileSettings.remove(toRemove+"Button");
            profileSettings.remove(toRemove+"X");
            profileSettings.remove(toRemove+"Y");
            profileSettings.remove(toRemove+"R");
            profileSettings.remove(toRemove+"G");
            profileSettings.remove(toRemove+"B");

            listShortcuts.erase(listShortcuts.begin()+(lstWidget->currentRow()));
            profileSettings.setValue(buttonProfile+"/TouchScreen/Shortcut/Count", QVariant::fromValue(listShortcuts.size()));
            qDeleteAll(lstWidget->selectedItems());
        }

        lstWidget->clearSelection();
    });
}

void TsShortcut::connectPressButtonEvent()
{
    connect(btnPressNow, &QPushButton::pressed, this,
            [this](void)
    {
        if(lstWidget->selectedItems().size() != 0)
        {
            touchScreenPressed = true;
            touchScreenPosition = listShortcuts[lstWidget->currentRow()].pos*tsRatio;
        }

    });

    connect(btnPressNow, &QPushButton::released, this,
            [this](void)
    {
        touchScreenPressed = false;
    });
}

void TsShortcut::connectHelpButtonEvent()
{
    connect(btnHelp, &QPushButton::released, this,
            [this](void)
    {
        QMessageBox *msgBox = new QMessageBox(0);

        msgBox->setText("Map Touchpad to Button");
        msgBox->setInformativeText(
                    QString("1. Right-click touchpad in the position you want then open this menu.\n")+
                    QString("2. Type a name for your shortcut in the textbox.\n")+
                    QString("3. Choose a button on the gamepad to map this point to.\n")+
                    QString("4. Choose a color for your shortcut,\n    (this will be the circle's color ")+
                    QString(   "on the touchscreen window)\n")+
                    QString("5. Press create, then close this window."));
        msgBox->show();
    });
}

void TsShortcut::setCurrentPos(QPoint pos)
{
    curPos = pos;
}

void TsShortcut::updateTitleText()
{
    wTitle = QString("Current X: %1 Y: %2").arg(QString::number(curPos.x())).arg(QString::number(curPos.y()));
    this->setWindowTitle(wTitle);

    btnCreateShort->setText(QString("💾 &Create new shortcut at (%1, %2)")
                            .arg(QString::number(curPos.x())).arg(QString::number(curPos.y())));

    if (txtShortName->text() == "")
    {
        txtShortName->setText(QString("(%1, %2)")
                              .arg(QString::number(curPos.x())).arg(QString::number(curPos.y())));
    }
}

void TsShortcut::showEvent(QShowEvent * event)
{
    lstWidget->clear();
    for (unsigned int i=0; i<listShortcuts.size(); i++)
    {
        lstWidget->addItem(createShortcutListItem(listShortcuts[i]));
    }

    updateTitleText();
    event->accept();
}

void TsShortcut::loadNewShortcuts(void)
{
    lstWidget->clear();
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

    for (unsigned int i=0; i<listShortcuts.size(); i++)
    {
        lstWidget->addItem(createShortcutListItem(listShortcuts[i]));
    }
}

QComboBox* TsShortcut::populateItems()
{
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("None", QGamepadManager::ButtonInvalid);
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

    return comboBox;
}
