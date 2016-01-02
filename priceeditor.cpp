#include "priceeditor.h"
#include "ui_priceeditor.h"
#include <QDebug>
#include "QDate"

PriceEditor::PriceEditor(QWidget *parent, PriceEditorMode mode) :
    QWidget(parent),
    ui(new Ui::PriceEditor),
    m_mode(mode)
{
    ui->setupUi(this);
    resize(parentWidget()->size());
    qDebug() << "PriceEditor=======";
    if(PRICE_EDITOR_TICKET_MODE == m_mode)
    {
        ui->label_SingleRoom->hide();
        ui->label_AdultTotal->hide();
        ui->label_ChildTotal->hide();
        ui->lineEdit_SingleRoom->hide();
        ui->lineEdit_AdultTotal->hide();
        ui->lineEdit_ChildTotal->hide();

        ui->label_RetailPrice->hide();
        ui->lineEdit_RetailPrice->hide();
        ui->label_Stock->hide();
        ui->lineEdit_Stock->hide();
        ui->label_MinPerOrder->hide();
        ui->lineEdit_MinPerOrder->hide();
        ui->label_MaxPerOrder->hide();
        ui->lineEdit_MaxPerOrder->hide();
    }
    ui->lineEdit_AdultTotal->setReadOnly(true);
    ui->lineEdit_ChildTotal->setReadOnly(true);
    ui->lineEdit_AdultPrice->setValidator(new QIntValidator(1, 999999, ui->lineEdit_AdultPrice));
    ui->lineEdit_ChildPrice->setValidator(new QIntValidator(1, 999999, ui->lineEdit_ChildPrice));
    ui->lineEdit_SingleRoom->setValidator(new QIntValidator(1, 999999, ui->lineEdit_SingleRoom));
    ui->lineEdit_RetailPrice->setValidator(new QIntValidator(1, 999999, ui->lineEdit_RetailPrice));
    ui->lineEdit_Stock->setValidator(new QIntValidator(1, 999999, ui->lineEdit_Stock));
    ui->lineEdit_MaxPerOrder->setValidator(new QIntValidator(1, 999999, ui->lineEdit_MaxPerOrder));
    ui->lineEdit_MinPerOrder->setValidator(new QIntValidator(1, 999999, ui->lineEdit_MinPerOrder));

    for(int i=0; i<7; i++)
    {
        for(int j=0; j<7; j++)
        {
            ui->widgetCalendar->m_pCalendarItemArray[i][j]->installEventFilter(this);
            connect(ui->widgetCalendar->m_pCalendarItemArray[i][j], SIGNAL(calendarItemCheckSignal(CalendarItem*, bool)), this, SLOT(calendarItemCheckSlot(CalendarItem*, bool)));
        }
    }

    connect(ui->lineEdit_AdultPrice, SIGNAL(textChanged(QString)), this, SLOT(adultPriceLienEditChanged(QString)));
    connect(ui->lineEdit_ChildPrice, SIGNAL(textChanged(QString)), this, SLOT(childPriceLienEditChanged(QString)));
    connect(ui->lineEdit_SingleRoom, SIGNAL(textChanged(QString)), this, SLOT(singleRoomLienEditChanged(QString)));
    connect(ui->lineEdit_RetailPrice, SIGNAL(textChanged(QString)), this, SLOT(retailPriceLineEditChanged(QString)));
    connect(ui->lineEdit_Stock, SIGNAL(textChanged(QString)), this, SLOT(stockLineEditChanged(QString)));
    connect(ui->lineEdit_MinPerOrder, SIGNAL(textChanged(QString)), this, SLOT(minPerOrderLineEditChanged(QString)));
    connect(ui->lineEdit_MaxPerOrder, SIGNAL(textChanged(QString)), this, SLOT(maxPerOrderLineEditChanged(QString)));
}

PriceEditor::~PriceEditor()
{
    delete ui;
}

bool PriceEditor::eventFilter(QObject *target, QEvent *event)
{
    //qDebug() << "PriceEditor::eventFilter" << event->type();

    if(QEvent::MouseButtonPress == event->type())
    {
        for(int i=0; i<7; i++)
        {
            int j=0;
            for(; j<7; j++)
            {
                if(ui->widgetCalendar->m_pCalendarItemArray[i][j] == dynamic_cast<CalendarItem*>(target)
                        && ""!= ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText())
                {
                    //setCalendarItemCheck(dynamic_cast<CalendarItem*>(target), true);
                    if(SELECTMODE_SINGLE == ui->widgetCalendar->getSelectMode())
                    {
                        if(0 == i)
                        {
                            break;
                        }
                        dynamic_cast<CalendarItem*>(target)->setCheck(true);
                    }
                    else
                    {
                        bool bCheck = !dynamic_cast<CalendarItem*>(target)->isChecked();
                        dynamic_cast<CalendarItem*>(target)->setCheck(bCheck);
                    }

                    break;
                }
            }
            if(7 != j)
            {
                break;
            }
        }
    }

    return QWidget::eventFilter(target, event);
}

void PriceEditor::calendarItemCheckSlot(CalendarItem* item, bool bCheck)
{
    if(true == bCheck)
    {
        //item->setCheck(bCheck);
        //单选模式
        if(SELECTMODE_SINGLE == ui->widgetCalendar->getSelectMode())
        {
            for(int i=1; i<7; i++)
            {
                for(int j=0; j<7; j++)
                {
                    if(ui->widgetCalendar->m_pCalendarItemArray[i][j] != item)
                    {
                        ui->widgetCalendar->m_pCalendarItemArray[i][j]->setCheck(false);
                    }
                }
            }
        }

        showPriceOnRightUI(item);
    }

    if(SELECTMODE_MULTI == ui->widgetCalendar->getSelectMode())
    {
        for(int j=0; j<7; j++)
        {
            if(ui->widgetCalendar->m_pCalendarItemArray[0][j] == item)
            {
                for(int i=1; i<7; i++)
                {
                    ui->widgetCalendar->m_pCalendarItemArray[i][j]->setCheck(bCheck);
                    showPriceOnRightUI(ui->widgetCalendar->m_pCalendarItemArray[i][j]);
                }
            }
        }
    }
}

void PriceEditor::showPriceOnRightUI(CalendarItem* item)
{
    ui->lineEdit_AdultPrice->setText(item->getAdultPrice());
    ui->lineEdit_ChildPrice->setText(item->getChildPrice());
    ui->lineEdit_SingleRoom->setText(item->getSingleRoom());
    int nAdultPrice = item->getAdultPrice().toInt() + ui->widgetCalendar->getHelpPriceInfo(item->getText()).nTicketAdultPrice;
    if(nAdultPrice > 0)
    {
        ui->lineEdit_AdultTotal->setText(QString::number(nAdultPrice));
    }
    else
    {
        ui->lineEdit_AdultTotal->setText("");
    }

    int nChildPrice = item->getChildPrice().toInt() + ui->widgetCalendar->getHelpPriceInfo(item->getText()).nTicketChildPrice;
    if(nChildPrice > 0)
    {
        ui->lineEdit_ChildTotal->setText(QString::number(nChildPrice));
    }
    else
    {
        ui->lineEdit_ChildTotal->setText("");
    }

    int nRetailPrice = ui->widgetCalendar->getPrice(item->getText()).nTicketRetailPrice;
    if(nRetailPrice > 0)
    {
        ui->lineEdit_RetailPrice->setText(QString::number(nRetailPrice));
    }
    else
    {
        ui->lineEdit_RetailPrice->setText("");
    }

    int nStock = ui->widgetCalendar->getPrice(item->getText()).nTicketStock;
    if(nStock > 0)
    {
        ui->lineEdit_Stock->setText(QString::number(nStock));
    }
    else
    {
        ui->lineEdit_Stock->setText("");
    }

    int nMinPerOrder = ui->widgetCalendar->getPrice(item->getText()).nMinPerOrder;
    if(nMinPerOrder > 0)
    {
        ui->lineEdit_MinPerOrder->setText(QString::number(nMinPerOrder));
    }
    else
    {
        ui->lineEdit_MinPerOrder->setText("");
    }

    int nMaxPerOrder = ui->widgetCalendar->getPrice(item->getText()).nMaxPerOrder;
    if(nMaxPerOrder > 0)
    {
        ui->lineEdit_MaxPerOrder->setText(QString::number(nMaxPerOrder));
    }
    else
    {
        ui->lineEdit_MaxPerOrder->setText("");
    }
}

void PriceEditor::adultPriceLienEditChanged(QString strAdultPrice)
{
    for(int i=1; i<7; i++)
    {
        for(int j=0; j<7; j++)
        {
            if(ui->widgetCalendar->m_pCalendarItemArray[i][j]->isChecked() && ""!=ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText())
            {
                ui->widgetCalendar->m_pCalendarItemArray[i][j]->setAdultPrice(strAdultPrice);
                ui->widgetCalendar->updateTicketAdultPrice(ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText(), strAdultPrice.toInt());

                qDebug() << "price:" << ui->widgetCalendar->getHelpPriceInfo(ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText()).nTicketAdultPrice;
                qDebug() << "day:" << ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText();
                int nAdultPrice = strAdultPrice.toInt() + ui->widgetCalendar->getHelpPriceInfo(ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText()).nTicketAdultPrice;
                ui->lineEdit_AdultTotal->setText(QString::number(nAdultPrice));
            }
        }
    }
}
void PriceEditor::childPriceLienEditChanged(QString strChildPrice)
{
    for(int i=1; i<7; i++)
    {
        for(int j=0; j<7; j++)
        {
            if(ui->widgetCalendar->m_pCalendarItemArray[i][j]->isChecked() && ""!=ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText())
            {
                ui->widgetCalendar->m_pCalendarItemArray[i][j]->setChildPrice(strChildPrice);
                ui->widgetCalendar->updateTicketChildPrice(ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText(), strChildPrice.toInt());

                int nChildPrice = strChildPrice.toInt() + ui->widgetCalendar->getHelpPriceInfo(ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText()).nTicketChildPrice;
                ui->lineEdit_ChildTotal->setText(QString::number(nChildPrice));
            }
        }
    }
}

void PriceEditor::singleRoomLienEditChanged(QString strSingleRoom)
{
    for(int i=1; i<7; i++)
    {
        for(int j=0; j<7; j++)
        {
            if(ui->widgetCalendar->m_pCalendarItemArray[i][j]->isChecked() && ""!=ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText())
            {
                ui->widgetCalendar->m_pCalendarItemArray[i][j]->setSingleRoom(strSingleRoom);
                ui->widgetCalendar->updateSingleRoomPrice(ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText(), strSingleRoom.toInt());
            }
        }
    }
}

void PriceEditor::retailPriceLineEditChanged(QString strRetailPrice)
{
    for(int i=1; i<7; i++)
    {
        for(int j=0; j<7; j++)
        {
            if(ui->widgetCalendar->m_pCalendarItemArray[i][j]->isChecked() && ""!=ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText())
            {
                ui->widgetCalendar->updateRetailPrice(ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText(), strRetailPrice.toInt());
            }
        }
    }
}

void PriceEditor::stockLineEditChanged(QString strStock)
{
    for(int i=1; i<7; i++)
    {
        for(int j=0; j<7; j++)
        {
            if(ui->widgetCalendar->m_pCalendarItemArray[i][j]->isChecked() && ""!=ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText())
            {
                ui->widgetCalendar->updateStock(ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText(), strStock.toInt());
            }
        }
    }
}

void PriceEditor::minPerOrderLineEditChanged(QString strMinPerOrder)
{
    for(int i=1; i<7; i++)
    {
        for(int j=0; j<7; j++)
        {
            if(ui->widgetCalendar->m_pCalendarItemArray[i][j]->isChecked() && ""!=ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText())
            {
                ui->widgetCalendar->updateMinPerOrder(ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText(), strMinPerOrder.toInt());
            }
        }
    }
}

void PriceEditor::maxPerOrderLineEditChanged(QString strMaxPerOrder)
{
    for(int i=1; i<7; i++)
    {
        for(int j=0; j<7; j++)
        {
            if(ui->widgetCalendar->m_pCalendarItemArray[i][j]->isChecked() && ""!=ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText())
            {
                ui->widgetCalendar->updateMaxPerOrder(ui->widgetCalendar->m_pCalendarItemArray[i][j]->getText(), strMaxPerOrder.toInt());
            }
        }
    }
}

void PriceEditor::setPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> > &mapTicketPriceInfo)
{
    ui->widgetCalendar->setPriceInfo(mapTicketPriceInfo);
}

void PriceEditor::setHelpPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> > &mapTicketPriceInfo)
{
    ui->widgetCalendar->setHelpPriceInfo(mapTicketPriceInfo);
}

void PriceEditor::getPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> >&mapTicketPriceInfo)
{
    ui->widgetCalendar->getPriceInfo(mapTicketPriceInfo);
}

void PriceEditor::clearUpdateFlag()
{
    ui->widgetCalendar->clearUpdateFlag();
}

Calendar* PriceEditor::getCalendar()
{
    return ui->widgetCalendar;
}

void PriceEditor::show()
{
    QDate date = QDate::currentDate();
    ui->widgetCalendar->setSelectMode(SELECTMODE_SINGLE);
    ui->widgetCalendar->PrintCalendar(date.year(), date.month());
    QWidget::show();
}

void PriceEditor::on_pushButton_back_clicked()
{
    hide();
}
