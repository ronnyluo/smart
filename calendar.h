#ifndef CALENDAR_H
#define CALENDAR_H

#include <QWidget>
#include "calendaritem.h"
#include "define.h"

namespace Ui {
class Calendar;
}

enum SelectMode
{
    SELECTMODE_SINGLE = 0,    //单选模式
    SELECTMODE_MULTI = 1      //多选模式，包括全选
};

class Calendar : public QWidget
{
    Q_OBJECT

public:
    explicit Calendar(QWidget *parent = 0);
    ~Calendar();
    void setPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> > &mapTicketPriceInfo);
    void getPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> > &mapTicketPriceInfo);
    void updateTicketAdultPrice(QString strDay, int nAdultPrice);
    void updateTicketChildPrice(QString strDay, int nChildPrice);
    void updateSingleRoomPrice(QString strDay, int nSingleRoomPrice);
    void PrintCalendar(int nYear, int nMonth);
    SelectMode getSelectMode() {return m_selectMode;}
    void setSelectMode(SelectMode mode);
    void setHelpPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> > &mapTicketPriceInfo);
    TicketPriceInfo getHelpPriceInfo(QString strDay);
    void updateRetailPrice(QString strDay, int nRetailPrice);
    void updateStock(QString strDay, int nStock);
    void updateMinPerOrder(QString strDay, int nMinPerOrder);
    void updateMaxPerOrder(QString strDay, int nMaxPerOrder);
    TicketPriceInfo getPrice(QString strDay);
    void clearUpdateFlag();


private:
    bool IsLeapYear(int year);
    int NumLeapYear( int year);
    int DaysOfMonth(int nYear, int nMonth);
    int getDefaultSelectDay(int nYear, int nMonth);

signals:
    void updatePriceInfoSignal(QMap<QString, QMap<QString, TicketPriceInfo> >&);

private slots:
    void on_pushButton_PreMonth_clicked();
    void on_pushButton_NextMonth_clicked();
    void on_pushButton_SelectMode_clicked();
    void on_pushButton_SelectAll_clicked();

private:
    Ui::Calendar *ui;
    int m_nYear;
    int m_nMonth;

public:
    CalendarItem* m_pCalendarItemArray[7][7];
    QMap<QString, QMap<QString, TicketPriceInfo> > m_mapTicketPriceInfo; //该机票的票价信息;第一个key为年月/地接服务的票价
    QMap<QString, QMap<QString, TicketPriceInfo> > m_mapHelpPriceInfo;   //地接价格编辑时，需要传递关联的机票价格，用于计算总价格
    SelectMode m_selectMode;
};

#endif // CALENDAR_H
