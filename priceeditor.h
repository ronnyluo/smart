#ifndef PRICEEDITOR_H
#define PRICEEDITOR_H

#include <QWidget>
#include "calendaritem.h"
#include "define.h"
#include "calendar.h"

namespace Ui {
class PriceEditor;
}

enum PriceEditorMode
{
    PRICE_EDITOR_TICKET_MODE = 0,      //机票价格编辑
    PRICE_EDITOR_PICK_SERVICE = 1      //地接价格编辑
};

class PriceEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PriceEditor(QWidget *parent = 0, PriceEditorMode mode = PRICE_EDITOR_TICKET_MODE);
    ~PriceEditor();
    void setPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> >&mapTicketPriceInfo);
    void setHelpPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> >&mapTicketPriceInfo);
    void getPriceInfo(QMap<QString, QMap<QString, TicketPriceInfo> >&mapTicketPriceInfo);
    Calendar* getCalendar();
    void show();
    void clearUpdateFlag();

private slots:
    void adultPriceLienEditChanged(QString strAdultPrice);
    void childPriceLienEditChanged(QString strChildPrice);
    void calendarItemCheckSlot(CalendarItem* item, bool bCheck);
    void on_pushButton_back_clicked();
    void on_pushButton_Save_clicked();
    void singleRoomLienEditChanged(QString strSingleRoom);
    void retailPriceLineEditChanged(QString strRetailPrice);
    void stockLineEditChanged(QString strStock);
    void minPerOrderLineEditChanged(QString strMinPerOrder);
    void maxPerOrderLineEditChanged(QString strMaxPerOrder);

private:
    bool eventFilter(QObject *target, QEvent *event);
    void showPriceOnRightUI(CalendarItem* item);

signals:
    void saveSignals(PriceEditorMode editorMode);

private:
    Ui::PriceEditor *ui;
    PriceEditorMode m_mode;
};

#endif // PRICEEDITOR_H
