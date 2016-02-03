#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "define.h"
#include <QListWidgetItem>
#include "priceeditor.h"
#include <QtNetwork/QNetworkReply>
#include <QTableWidgetItem>
#include <QtNetwork/QNetworkAccessManager>
#include "captchadialog.h"
#include "network.h"

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

    void replyFinishedForTicket();
    void replyFinishedForPickService();
    void replyUpdateChannel();
    void replyUpdateChannelRelation();


    void replyFinishedForLoadTicket();
    void replyFinishedForLoadPickService();
    void replyLoadChannel();
    void replyLoadChannelRelation();

    void replyDeleteChannel();
    void replyDeleteChannelRelation();

    void on_pushButton_ChannelUpdate_clicked();
    void on_pushButton_ChannelDel_clicked();
    void on_pushButton_ChannelCancel_clicked();
    void channelCurrentItemClicked(QTableWidgetItem *tableWidgetItem);
    void channelRelationCurrentItemClicked(QTableWidgetItem *tableWidgetItem);
    void channelServiceListCurrentItemClicked(QTableWidgetItem *tableWidgetItem);
    void findChannelServiceChanged(QString strFindName);

    void tabWidgetCurrentChanged(int index);
    void channelListCurrentItemChanged(QListWidgetItem *currentItem, QListWidgetItem *);

    void on_pushButton_ChannelRelationUpdate_clicked();
    void on_pushButton_ChannelRelationDel_clicked();
    void on_pushButton_ChannelRelationCancel_clicked();

    void saveSignalsSlot(PriceEditorMode eMode);

private:
    void addItemToTicketList(TicketInfo& ticketInfo);
    bool hasSameTicketNo(QString ticketNo);
    bool hasSameServiceNo(QString serviceNo);
    void updateTicketUI(TicketInfo & ticketInfo);
    void clearTicketUI();

    void addItemToServiceList(PickServiceInfo& serviceInfo);
    void clearServiceUI();
    void updateServiceUI(PickServiceInfo& serviceInfo);

    void addItemToChannelServiceList(QVector<PickServiceInfo> &vecPickServiceInfo);

    void updateTicket(const TicketInfo & ticketInfo);
    void updatePickService(const PickServiceInfo & pickServiceInfo);
    void updateChannel(const ChannelInfo & channelInfo);
    void updateChannelRelation(const ChannelRelationInfo & channelRelationInfo);

    void loadTicket();
    void loadPickServce();
    void loadChannel();
    void loadChannelRelation();

    void deleteChannel(const QString & strChannelName);
    void deleteChannelRelation(const QString & strChannelName, const QString & strProductId);

    //机票信息UI
    void updateTicketUI();

    //地接信息UI
    void updatePickServiceUI();

    //渠道管理
    void updateChannelListUI();

    //渠道关联
    void updateChannelRelationUI();
    void updateChannelRelationDetailUI(QString strChannelName);

    void clearChannelRelationDetailUI();


    void initUI();
    void getAnswer(QString& jsFunc, QString& answer, QString& cookie);

    //获取更新去哪儿网的数据
    void getPriceInfo4Qunaer(TicketInfo& ticketInfo, PickServiceInfo &pickServiceInfo, QMap<QString, QVector<QunarPriceInfo> > &mapQunarPriceInfo);

    //更新去哪儿网数据
    void update2Qunaer(QMap<QString, QVector<QunarPriceInfo> > mapQunarPriceInfo);

    void checkTicketInfo(QMap<QString, QMap<QString, TicketPriceInfo> > &mapTicketPriceInfo);

private:
    Ui::MainWindow *ui;
    QVector<TicketInfo> m_vecTicketInfo;
    QVector<TicketInfo> m_vecUpdateTicketInfo;             //需要向后台更新的机票信息
    PriceEditor *m_pPriceEditor;
    QVector<PickServiceInfo> m_vecPickServiceInfo;
    QVector<PickServiceInfo> m_vecUpdatePickServiceInfo;    //需要向后台更新的地接服务信息
    PriceEditor *m_pPickServicePriceEditor;

    QVector<ChannelInfo> m_vecChannelInfo;                  //渠道信息
    QMap<QString, QVector<ChannelRelationInfo> > m_mapChannelRelationInfo;  //渠道关联信息，key为渠道名称
    QNetworkAccessManager *m_pAssitNetworkManager;
    QVector<QunerHttp*> m_vecQunerHttPtr;
};

#endif // MAINWINDOW_H
