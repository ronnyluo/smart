#ifndef CALENDARITEM_H
#define CALENDARITEM_H

#include <QWidget>

namespace Ui {
class CalendarItem;
}

class CalendarItem : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarItem(QWidget *parent = 0);
    ~CalendarItem();

    void setText(QString strText);
    void setAdultPrice(QString strAdultPrice);
    void setChildPrice(QString strChildPrice);
    void setCheck(bool bCheck);
    void setSingleRoom(QString strSingleRoom);

    QString getText();
    QString getAdultPrice();
    QString getChildPrice();
    QString getSingleRoom();

    bool isChecked();
    void showCheckBox();
    void hideCheckBox();

private:
    bool eventFilter(QObject *target, QEvent *event);

signals:
    void calendarItemCheckSignal(CalendarItem* item, bool bCheck);
private:
    Ui::CalendarItem *ui;
};

#endif // CALENDARITEM_H
