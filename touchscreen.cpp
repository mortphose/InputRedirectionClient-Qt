#include "touchscreen.h"
#include "global.h"
#include <QMenu>

void TouchScreen::setTouchScreenPressed(bool b)
{
    touchScreenPressed = b;
}

bool TouchScreen::isTouchScreenPressed()
{
    return touchScreenPressed;
}

QSize TouchScreen::getTouchScreenSize()
{
    return touchScreenSize;
}

QPoint TouchScreen::getTouchScreenPosition()
{
    return touchScreenPosition;
}

TouchScreen::TouchScreen(QWidget *parent) : QWidget(parent)
{
    Qt::WindowFlags windowFlags = Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint;
    if (QSysInfo::productType() == "windows")
        windowFlags |=  Qt::MSWindowsFixedSizeDialogHint;
    this->setWindowFlags(windowFlags);
    this->setWindowTitle(tr("InputRedirectionClient-Qt - Touch screen"));
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint&)));

    bgLabel = new QLabel(this);

    updatePixmap();
    setMinimumWidth(TOUCH_SCREEN_WIDTH);
    setMinimumHeight(TOUCH_SCREEN_HEIGHT);

    bgLabel->setScaledContents(true);
    bgLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    ellipNeedDraw = true;

    float loadedScale = profileSettings.value(buttonProfile+"/TouchScreen/Scale", 1).toDouble();
    this->resize(TOUCH_SCREEN_WIDTH*loadedScale, TOUCH_SCREEN_HEIGHT*loadedScale);
}

void TouchScreen::ShowContextMenu(const QPoint& pos)
{
    QMenu* myMenu = new QMenu();
    QString strOpenOverlay = "Set Overlay Image...";
    QString clearOverlayBtn = "Clear Overlay";
    QString strSaveSize = "Save Touch Screen Scale";
    QString strPtToBtn = "Set point to button...";
    QPoint globalPos = this->mapToGlobal(pos);

    myMenu->addAction(strPtToBtn);
    myMenu->addSeparator();
    myMenu->addAction(strSaveSize);
    myMenu->addSeparator();
    myMenu->addAction(strOpenOverlay);
    myMenu->addAction(clearOverlayBtn);

    myMenu->popup(globalPos);
    QAction* selectedItem = myMenu->exec(globalPos);

    QPoint newPos;
    newPos.setX(pos.x());
    newPos.setY(pos.y());
    //If custom size, scale the point to orig 3ds touchscreen size
    if(this->width() != TOUCH_SCREEN_WIDTH ||
            this->height() != TOUCH_SCREEN_HEIGHT)
    {
        newPos.setX((TOUCH_SCREEN_WIDTH*pos.x())/this->width());
        newPos.setY((TOUCH_SCREEN_HEIGHT*pos.y())/this->height());
    }

    //Update the shortcut gui position title text
    tsShortcutGui.setCurrentPos(newPos);

    if(!selectedItem)
    {
        myMenu->close();
        return;
    }

    if(selectedItem->text() == strOpenOverlay)
    {
        QString strPic = QFileDialog::getOpenFileName(this,
                tr("Open Touchscreen Image (320x240)"), "MyDocuments",
                tr("Image Files (*.jpg *.jpeg *.png *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm)"));

        if(!strPic.isNull())
        {
            profileSettings.setValue(buttonProfile+"/TouchScreen/Image", strPic);
            updatePixmap();
        }
    }

    if(selectedItem->text() == strPtToBtn )
    {
        if(tsShortcutGui.isVisible())
        {
            tsShortcutGui.updateTitleText();
        }

        tsShortcutGui.setVisible(true);

    }

    if(selectedItem->text() == clearOverlayBtn)
    {
        clearImage();
    }

    if(selectedItem->text() == strSaveSize)
    {
        double scaleToSave = ((double)this->width()/TOUCH_SCREEN_WIDTH);
        profileSettings.setValue(buttonProfile+"/TouchScreen/Scale", scaleToSave);
    }

    myMenu->deleteLater();
}

void TouchScreen::resizeEvent(QResizeEvent* e)
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

    tsRatio = (double)this->width() / (double)TOUCH_SCREEN_WIDTH;
}

void TouchScreen::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        touchScreenPressed = true;
        touchScreenPosition = ev->pos();
    }
}

void TouchScreen::mouseMoveEvent(QMouseEvent *ev)
{
    if(touchScreenPressed && (ev->buttons() & Qt::LeftButton))
    {
        touchScreenPosition = ev->pos();
    }
}

void TouchScreen::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        touchScreenPressed = false;
    }
}

void TouchScreen::closeEvent(QCloseEvent *ev)
{
    touchScreenPressed = false;
    ev->accept();
}

void TouchScreen::updatePixmap(void)
{
    QString strPic = profileSettings.value(buttonProfile+"/TouchScreen/Image", "").toString();
    QPixmap newPic(strPic);

    if (newPic.isNull())
    {
        newPic = QPixmap(TOUCH_SCREEN_WIDTH, TOUCH_SCREEN_HEIGHT);
        newPic.fill(Qt::transparent);
    }
}


void TouchScreen::paintEvent(QPaintEvent* e)
{
    QString strPic = profileSettings.value(buttonProfile+"/TouchScreen/Image", "").toString();
    QPixmap newPic(strPic);

    if (newPic.isNull())
    {
        newPic = QPixmap(TOUCH_SCREEN_WIDTH, TOUCH_SCREEN_HEIGHT);
        newPic.fill(Qt::transparent);
    }

    QPainter painter;
    painter.begin(&newPic);
    QPen pen;
    pen.setWidth(2);
    //painter.setBrush(QBrush(Qt::black));
    pen.setColor(Qt::black);
    //painter.setRenderHint(QPainter::Antialiasing, true);

    for (unsigned i = 0; i < listShortcuts.size(); ++i)
    {
        ShortCut curShort = listShortcuts[i];
        //pen.setColor(curShort.color);
        painter.setBrush(QBrush(curShort.color));
        painter.setPen(pen);
        painter.drawEllipse(
                    TOUCH_SCREEN_WIDTH*((height()*curShort.pos.x())/TOUCH_SCREEN_HEIGHT)/width(),
                    TOUCH_SCREEN_HEIGHT*((width()*curShort.pos.y())/TOUCH_SCREEN_WIDTH)/height(),
                    7, 7);
    }

    bgLabel->setPixmap(newPic);
    bgLabel->show();
    e->accept();
}

void TouchScreen::clearImage(void)
{
    profileSettings.setValue(buttonProfile+"/TouchScreen/Image", "");
    updatePixmap();
}

