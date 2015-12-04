#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "define.h"
#include <QListWidgetItem>
#include "priceeditor.h"
#include <QtNetwork/QNetworkReply>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonUpdate_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonCancel_clicked();
    void on_pushButtonPrice_clicked();
    void ticketInfoChanged();
    void fillTicketInfo(TicketInfo& ticketInfo);
    void ticketItemClicked(QListWidgetItem* listWidgetItem);
    void ticketCurrentItemChanged(QListWidgetItem *currentItme, QListWidgetItem *);
    void findTicketInfoChanged(QString strFindName);
    void updatePriceInfoSlot(QMap<QString, QMap<QString, TicketPriceInfo> >& mapPriceInfo);

    void on_pushButton_ServiceUpdate_clicked();
    void on_pushButton_ServiceDelete_clicked();
    void on_pushButton_ServiceCancel_clicked();
    void on_pushButton_ServicePrice_clicked();
    void serviceInfoChanged();
    void findServiceInfoChanged(QString strFindName);
    void fillPickServiceInfo(PickServiceInfo& pickServiceInfo);
    void serviceCurrentItemChanged(QListWidgetItem *currentItme, QListWidgetItem *);
    void serviceItemClicked(QListWidgetItem* listWidgetItem);
    void updateServicePriceInfoSlot(QMap<QString, QMap<QString, TicketPriceInfo> >& mapPriceInfo);

    void replyFinishedForTicket(QNetworkReply* pNetworkReply);
    void replyFinishedForPickService(QNetworkReply* pNetworkReply);

    void replyFinishedForLoadTicket(QNetworkReply* pNetworkReply);
    void replyFinishedForLoadPickService(QNetworkReply* pNetworkReply);

private:
    void addItemToTicketList(TicketInfo& ticketInfo);
    bool hasSameTicketNo(QString ticketNo);
    bool hasSameServiceNo(QString serviceNo);
    void updateTicketUI(TicketInfo & ticketInfo);
    void clearTicketUI();

    void addItemToServiceList(PickServiceInfo& serviceInfo);
    void clearServiceUI();
    void updateServiceUI(PickServiceInfo& serviceInfo);
    void sendTicket(const TicketInfo & ticketInfo);
    void sendPickService(const PickServiceInfo & pickServiceInfo);
    void loadTicket();
    void loadPickServce();
private:
    Ui::MainWindow *ui;
    QVector<TicketInfo> m_vecTicketInfo;
    QVector<TicketInfo> m_vecUpdateTicketInfo;             //需要向后台更新的机票信息
    PriceEditor *m_pPriceEditor;
    QVector<PickServiceInfo> m_vecPickServiceInfo;
    QVector<PickServiceInfo> m_vecUpdatePickServiceInfo;    //需要向后台更新的地接服务信息
    PriceEditor *m_pPickServicePriceEditor;
};

#endif // MAINWINDOW_H
