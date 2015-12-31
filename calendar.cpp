#include "calendar.h"
#include "ui_calendar.h"
#include <QDebug>
#include <QDate>

QString strWeekdays[7] = {"日", "一", "二", "三", "四", "五", "六"};
Calendar::Calendar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Calendar)
{
    ui->setupUi(this);
    CalendarItem* pCalendarItemArray[7][7] = {{(CalendarItem *)ui->widget_01, (CalendarItem *)ui->widget_02, (CalendarItem *)ui->widget_03, (CalendarItem *)ui->widget_04, (CalendarItem *)ui->widget_05, (CalendarItem *)ui->widget_06, (CalendarItem *)ui->widget_07},
                             {(CalendarItem *)ui->widget_11, (CalendarItem *)ui->widget_12, (CalendarItem *)ui->widget_13, (CalendarItem *)ui->widget_14, (CalendarItem *)ui->widget_15, (CalendarItem *)ui->widget_16, (CalendarItem *)ui->widget_17},
                             {(CalendarItem *)ui->widget_21, (CalendarItem *)ui->widget_22, (CalendarItem *)ui->widget_23, (CalendarItem *)ui->widget_24, (CalendarItem *)ui->widget_25, (CalendarItem *)ui->widget_26, (CalendarItem *)ui->widget_27},
                             {(CalendarItem *)ui->widget_31, (CalendarItem *)ui->widget_32, (CalendarItem *)ui->widget_33, (CalendarItem *)ui->widget_34, (CalendarItem *)ui->widget_35, (CalendarItem *)ui->widget_36, (CalendarItem *)ui->widget_37},
                             {(CalendarItem *)ui->widget_41, (CalendarItem *)ui->widget_42, (CalendarItem *)ui->widget_43, (CalendarItem *)ui->widget_44, (CalendarItem *)ui->widget_45, (CalendarItem *)ui->widget_46, (CalendarItem *)ui->widget_47},
                             {(CalendarItem *)ui->widget_51, (CalendarItem *)ui->widget_52, (CalendarItem *)ui->widget_53, (CalendarItem *)ui->widget_54, (CalendarItem *)ui->widget_55, (CalendarItem *)ui->widget_56, (CalendarItem *)ui->widget_57},
                             {(CalendarItem *)ui->widget_61, (CalendarItem *)ui->widget_62, (CalendarItem *)ui->widget_63, (CalendarItem *)ui->widget_64, (CalendarItem *)ui->widget_65, (CalendarItem *)ui->widget_66, (CalendarItem *)ui->widget_67}};
    qDebug() << "Calendar =======";
    for(int i=0; i<7; i++)
    {
        for(int j=0; j<7; j++)
        {
            m_pCalendarItemArray[i][j] = pCalendarItemArray[i][j];
            if(0 == i)
            {
                m_pCalendarItemArray[i][j]->setText(strWeekdays[j]);
            }
        }
    }

    QDate date = QDate::currentDate();
    m_nYear = date.year();
    m_nMonth = date.month();
    m_selectMode = SELECTMODE_SINGLE;
    ui->pushButton_SelectAll->hide();
    //PrintCalendar(m_nYear, m_nMonth);

}

Calendar::~Calendar()
{
    delete ui;
}

QString strDays[32] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11",
                      "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22",
                      "23", "24", "25", "26", "27", "28", "29", "30", "31"};

int fdofmonth[13];
//全局变量year为输入年数，fdmonth数组保存每月第一天是星期几
bool Calendar::IsLeapYear(int year)
{
    /*闰年的判断:(1)year被4整除,并且year不被100整除
    或(2)year被4整除,并且year被400整除*/
    if(year%4==0&&(year%100||year%400==0))
        return true ;
    else
        return false;
}

int Calendar::NumLeapYear( int year)//当前年数距离公元元年经历的闰年总数
{
    int count=0;
    for(int i=1;i<year;i++)
        if(IsLeapYear(i))
            count++;
    return count;
}
int Calendar::DaysOfMonth(int nYear, int nMonth)//每月有多少天
{
    switch (nMonth)
    {
    case 1 :
    case 3 :
    case 5 :
    case 7 :
    case 8 :
    case 10 :
    case 12 : return 31 ;
    case 4 :
    case 6 :
    case 9 :
    case 11 : return 30 ;
    case 2 : if (IsLeapYear(nYear))
                 return 29 ;
             else
                 return 28 ;
    }
    return 0 ;
}


void Calendar::PrintCalendar(int nYear, int nMonth)//打印nYear,nMonth的日历
{
    m_nYear = nYear;
    m_nMonth = nMonth;
    int days=DaysOfMonth(nYear, nMonth);
    long acdays=0;
    qDebug() << "Year:" << nYear << " Month:" << nMonth;
    //qDebug() <<" 日  一  二  三  四  五  六" << endl ; // 第一个修改的地方调整间距
    int rowIndex = 1;
    int columnIndex = 0;
    for(; rowIndex<7; rowIndex++)
    {
        for(columnIndex = 0; columnIndex<7; columnIndex++)
        {
            //qDebug() << "clear rowIndex:" << rowIndex << " columnIndex:" << columnIndex;
            m_pCalendarItemArray[rowIndex][columnIndex]->setText("");
            m_pCalendarItemArray[rowIndex][columnIndex]->setAdultPrice("");
            m_pCalendarItemArray[rowIndex][columnIndex]->setChildPrice("");
            m_pCalendarItemArray[rowIndex][columnIndex]->setSingleRoom("");
        }
    }
    qDebug() << "after rowIndex:" << rowIndex << " columnIndex:" << columnIndex;
    rowIndex = 1;
    columnIndex = -1;

    for(int i=1;i<13;i++)
    {
        acdays+=DaysOfMonth(nYear, i-1);
        fdofmonth[i]=(acdays+(nYear-1)*365+NumLeapYear(nYear))%7+1;
        //qDebug() << "i:" << i << " fdofmonth[i]:" << fdofmonth[i];
    }

    // 第二个修改的地方，原来为for ( int i = 0 ;i <=fdofmonth[m];i ++ )
    QString str = "";
    for ( int i = 0 ;i <fdofmonth[nMonth];i ++ )
    {
        if(fdofmonth[nMonth]%7==0 && fdofmonth[nMonth]!=0)
        {
            //rowIndex ++;
            columnIndex = 0;
            //qDebug() << str;
            str = "";
            break;
        }

        columnIndex ++;
        str += "  ";
    }

    int nDefaultSelectDay = getDefaultSelectDay(nYear, nMonth);
    for(int i=1;i<=days;i++)
    {
        if((i-1+fdofmonth[nMonth])%7==0)
        {
            qDebug() << str;
            str = strDays[i-1];
            rowIndex ++;
            columnIndex = 0;
        }
        else
        {
            str += "  " + strDays[i-1];
            columnIndex ++;
        }
        m_pCalendarItemArray[rowIndex][columnIndex]->setText(strDays[i]);
        TicketPriceInfo ticketInfo;
        ticketInfo = getPrice(strDays[i]);
        if(0 != ticketInfo.nTicketAdultPrice)
        {
            m_pCalendarItemArray[rowIndex][columnIndex]->setAdultPrice(QString::number(ticketInfo.nTicketAdultPrice));
        }
        else
        {
            m_pCalendarItemArray[rowIndex][columnIndex]->setAdultPrice("");
        }

        if(0 != ticketInfo.nTicketChildPrice)
        {
            m_pCalendarItemArray[rowIndex][columnIndex]->setChildPrice(QString::number(ticketInfo.nTicketChildPrice));
        }
        else
        {
            m_pCalendarItemArray[rowIndex][columnIndex]->setChildPrice("");
        }

        if(0 != ticketInfo.nSigleRoomSpread)
        {
            m_pCalendarItemArray[rowIndex][columnIndex]->setSingleRoom(QString::number(ticketInfo.nSigleRoomSpread));
        }
        else
        {
            m_pCalendarItemArray[rowIndex][columnIndex]->setSingleRoom("");
        }

        if(strDays[i] == QString::number(nDefaultSelectDay))
        {
            m_pCalendarItemArray[rowIndex][columnIndex]->setCheck(true);
        }
        else
        {
            m_pCalendarItemArray[rowIndex][columnIndex]->setCheck(false);
        }
    }
    for(int j=0; j<7; j++)
    {
        m_pCalendarItemArray[0][j]->setCheck(false);
    }

    QString title = QString::number(nYear) + "年" + QString::number(nMonth) + "月";
    ui->label_CurMonth->setText(title);
    int nPreMonth = (nMonth-1+12)%12==0 ? 12 : (nMonth-1+12)%12;
    int nNextMonth = (nMonth+1)%12==0 ? 12 : (nMonth+1)%12;
    ui->pushButton_PreMonth->setText(QString::number(nPreMonth) + "月");
    ui->pushButton_NextMonth->setText(QString::number(nNextMonth) + "月");
    qDebug() << str;

}

int Calendar::getDefaultSelectDay(int nYear, int nMonth)
{
    int nDefaultDay= 1;
    QDate date = QDate::currentDate();
    if(nYear==date.year() && nMonth==date.month())
    {
        nDefaultDay = date.day();
    }
    return nDefaultDay;
}

void Calendar::on_pushButton_PreMonth_clicked()
{
    if((m_nMonth-1+12)%12==0)
    {
        m_nMonth = 12;

        m_nYear --;
    }
    else
    {
        m_nMonth --;
    }
    PrintCalendar(m_nYear, m_nMonth);
}

void Calendar::on_pushButton_NextMonth_clicked()
{
    if(m_nMonth+1==13)
    {
        m_nMonth = 1;
        m_nYear ++;
    }
    else
    {
        m_nMonth ++;
    }
    PrintCalendar(m_nYear, m_nMonth);
}

void Calendar::setPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> > &mapTicketPriceInfo)
{
    m_mapTicketPriceInfo = mapTicketPriceInfo;
}

void Calendar::setHelpPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> > &mapTicketPriceInfo)
{
    m_mapHelpPriceInfo = mapTicketPriceInfo;
    qDebug() << "Calendar::setHelpPriceInfo:" << m_mapHelpPriceInfo["201511"]["8"].nTicketAdultPrice;
}

void Calendar::getPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> > &mapTicketPriceInfo)
{
    mapTicketPriceInfo = m_mapTicketPriceInfo;
}

TicketPriceInfo Calendar::getPrice(QString strDay)
{
    QMap<QString, TicketPriceInfo> mapMonthTicketPriceInfo = m_mapTicketPriceInfo[QString::number(m_nYear)+QString::number(m_nMonth)];
    return mapMonthTicketPriceInfo[strDay];
}

TicketPriceInfo Calendar::getHelpPriceInfo(QString strDay)
{
    QMap<QString, TicketPriceInfo> mapMonthHelpPriceInfo = m_mapHelpPriceInfo[QString::number(m_nYear)+QString::number(m_nMonth)];
    qDebug() << "nYear+nMonth:" << QString::number(m_nYear)+QString::number(m_nMonth);
    qDebug() << "strDay:" << strDay << " Adult:" << mapMonthHelpPriceInfo[strDay].nTicketAdultPrice;
    qDebug() << "Calendar::getHelpPriceInfo:" << m_mapHelpPriceInfo["201511"]["8"].nTicketAdultPrice;
    return mapMonthHelpPriceInfo[strDay];
}

void Calendar::updateTicketAdultPrice(QString strDay, int nAdultPrice)
{
    m_mapTicketPriceInfo[QString::number(m_nYear)+QString::number(m_nMonth)][strDay].nTicketAdultPrice = nAdultPrice;
    emit updatePriceInfoSignal(m_mapTicketPriceInfo);
}

void Calendar::updateTicketChildPrice(QString strDay, int nChildPrice)
{
    m_mapTicketPriceInfo[QString::number(m_nYear)+QString::number(m_nMonth)][strDay].nTicketChildPrice = nChildPrice;
    emit updatePriceInfoSignal(m_mapTicketPriceInfo);
}

void Calendar::updateSingleRoomPrice(QString strDay, int nSingleRoomPrice)
{
    m_mapTicketPriceInfo[QString::number(m_nYear)+QString::number(m_nMonth)][strDay].nSigleRoomSpread = nSingleRoomPrice;
    emit updatePriceInfoSignal(m_mapTicketPriceInfo);
}

void Calendar::updateRetailPrice(QString strDay, int nRetailPrice)
{
    m_mapTicketPriceInfo[QString::number(m_nYear)+QString::number(m_nMonth)][strDay].nTicketRetailPrice = nRetailPrice;
    emit updatePriceInfoSignal(m_mapTicketPriceInfo);
}

void Calendar::updateStock(QString strDay, int nStock)
{
    m_mapTicketPriceInfo[QString::number(m_nYear)+QString::number(m_nMonth)][strDay].nTicketStock = nStock;
    emit updatePriceInfoSignal(m_mapTicketPriceInfo);
}

void Calendar::updateMinPerOrder(QString strDay, int nMinPerOrder)
{
    m_mapTicketPriceInfo[QString::number(m_nYear)+QString::number(m_nMonth)][strDay].nMinPerOrder = nMinPerOrder;
    emit updatePriceInfoSignal(m_mapTicketPriceInfo);
}

void Calendar::updateMaxPerOrder(QString strDay, int nMaxPerOrder)
{
    m_mapTicketPriceInfo[QString::number(m_nYear)+QString::number(m_nMonth)][strDay].nMaxPerOrder = nMaxPerOrder;
    emit updatePriceInfoSignal(m_mapTicketPriceInfo);
}



void Calendar::on_pushButton_SelectMode_clicked()
{
    if(SELECTMODE_SINGLE == m_selectMode)
    {
        setSelectMode(SELECTMODE_MULTI);
    }
    else
    {
        setSelectMode(SELECTMODE_SINGLE);
    }
}

void Calendar::on_pushButton_SelectAll_clicked()
{
    if(SELECTMODE_MULTI == m_selectMode)
    {
        for(int i=0; i<7; i++)
        {
            for(int j=0; j<7; j++)
            {
                if("" != m_pCalendarItemArray[i][j]->getText() && !m_pCalendarItemArray[i][j]->isChecked())
                {
                    m_pCalendarItemArray[i][j]->setCheck(true);
                }
            }
        }
    }
}

void Calendar::setSelectMode(SelectMode mode)
{
    m_selectMode = mode;
    if(SELECTMODE_SINGLE == m_selectMode)
    {
        ui->pushButton_SelectMode->setText("切换到多选");
        ui->pushButton_SelectAll->hide();
        for(int i=0; i<7; i++)
        {
            for(int j=0; j<7; j++)
            {
                if("" != m_pCalendarItemArray[i][j]->getText())
                {
                    m_pCalendarItemArray[i][j]->hideCheckBox();
                }
                m_pCalendarItemArray[i][j]->setCheck(false);
            }
        }

    }
    else
    {
        ui->pushButton_SelectMode->setText("切换到单选");
        ui->pushButton_SelectAll->show();
        for(int i=0; i<7; i++)
        {
            for(int j=0; j<7; j++)
            {
                if("" != m_pCalendarItemArray[i][j]->getText())
                {
                    m_pCalendarItemArray[i][j]->showCheckBox();
                }
            }
        }
    }

}
