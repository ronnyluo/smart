#include "calendaritem.h"
#include "ui_calendaritem.h"
#include <QDebug>
#include "calendar.h"

CalendarItem::CalendarItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalendarItem)
{
    ui->setupUi(this);
    //ui->labelDate->setText("3");
    //ui->labelAdultPrice->setText("888");
    //ui->labelChildPrice->setText("666");
    ui->checkBox->setVisible(false);
    QPalette pa = QPalette();
    pa.setColor(QPalette::Background,QColor(235, 206, 255));
    ui->frame->setPalette(pa);
    ui->checkBox->installEventFilter(this);
}

CalendarItem::~CalendarItem()
{
    delete ui;
}

void CalendarItem::showCheckBox()
{
    ui->checkBox->setVisible(true);
}

void CalendarItem::hideCheckBox()
{
    ui->checkBox->setVisible(false);
}

void CalendarItem::setText(QString strText)
{
    ui->labelDate->setText(strText);
}

void CalendarItem::setCheck(bool bCheck)
{
    if(bCheck)
    {
        QPalette pa = QPalette();
        pa.setColor(QPalette::Background,QColor(255,255,0));//黄色
        ui->frame->setPalette(pa);
    }
    else
    {
        QPalette pa = QPalette();
        pa.setColor(QPalette::Background,QColor(235, 206, 255));
        ui->frame->setPalette(pa);
    }
    ui->checkBox->setChecked(bCheck);
    qDebug() << "setCheck" << bCheck;
    emit calendarItemCheckSignal(this, bCheck);
}

void CalendarItem::setAdultPrice(QString strAdultPrice)
{
    ui->labelAdultPrice->setText(strAdultPrice);
}
void CalendarItem::setChildPrice(QString strChildPrice)
{
    ui->labelChildPrice->setText(strChildPrice);
}

void CalendarItem::setSingleRoom(QString strSingleRoom)
{
    ui->labelSingleRoom->setText(strSingleRoom);
}

QString CalendarItem::getText()
{
    return ui->labelDate->text();
}

QString CalendarItem::getAdultPrice()
{
    return ui->labelAdultPrice->text();
}

QString CalendarItem::getChildPrice()
{
    return ui->labelChildPrice->text();
}

QString CalendarItem::getSingleRoom()
{
    return ui->labelSingleRoom->text();
}

bool CalendarItem::isChecked()
{
    return ui->checkBox->isChecked();
}

bool CalendarItem::eventFilter(QObject *target, QEvent *event)
{
    //qDebug() << "CalendarItem::eventFilter" << event->type();

    if(QEvent::MouseButtonPress == event->type())
    {
        if(ui->checkBox == dynamic_cast<QCheckBox*>(target))
        {
            if(SELECTMODE_SINGLE == ((Calendar*)parentWidget())->getSelectMode())
            {
                //ui->checkBox->setChecked(true);
                setCheck(true);
            }
            else
            {
                bool bCheck = !ui->checkBox->isChecked();
                //ui->checkBox->setChecked(bCheck);
                setCheck(bCheck);
                qDebug() << "CalendarItem::eventFilter" << bCheck;
            }
            return true;
        }
    }

    return QWidget::eventFilter(target, event);
}

