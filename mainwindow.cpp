#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QListWidget>
#include "priceeditor.h"
#include <QtNetwork/QNetworkRequest>
#include <QTextCodec>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    m_pAssitNetworkManager = new QNetworkAccessManager(this);

    loadTicket();
    loadPickServce();
    ui->setupUi(this);
    m_vecQunerHttPtr.push_back(new QunerHttp("uotscjr3824", "hkjr84626200", this));

    resize(QSize(900, 600));
    ui->listWidget_Ticket->setAlternatingRowColors(true);
    ui->lineEdit_Departure->installEventFilter(this);
    ui->lineEdit_Destination->installEventFilter(this);
    ui->lineEdit_Day->installEventFilter(this);
    ui->lineEdit_Day->setValidator(new QIntValidator(1, 1000, ui->lineEdit_Day));
    ui->lineEdit_No->setReadOnly(true);
    ui->lineEdit_Name->setReadOnly(true);

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabWidgetCurrentChanged(int)));

    //connect(ui->listWidget_Ticket, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(ticketItemClicked(QListWidgetItem*)));
    connect(ui->listWidget_Ticket, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(ticketCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));
    connect(ui->listWidget_Ticket, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(ticketItemClicked(QListWidgetItem*)));
    connect(ui->lineEdit_Departure, SIGNAL(textChanged(QString)), this, SLOT(ticketInfoChanged()));
    connect(ui->lineEdit_Destination, SIGNAL(textChanged(QString)), this, SLOT(ticketInfoChanged()));
    connect(ui->lineEdit_Day, SIGNAL(textChanged(QString)), this, SLOT(ticketInfoChanged()));
    connect(ui->lineEdit_Find, SIGNAL(textChanged(QString)), this, SLOT(findTicketInfoChanged(QString)));

    m_pPriceEditor = new PriceEditor(this, PRICE_EDITOR_TICKET_MODE);
    m_pPriceEditor->hide();
    connect(m_pPriceEditor->getCalendar(), SIGNAL(updatePriceInfoSignal(QMap<QString, QMap<QString, TicketPriceInfo> >&)), this, SLOT(updatePriceInfoSlot(QMap<QString, QMap<QString, TicketPriceInfo> >&)));

    //地接模块UI
    ui->listWidget_Service->setAlternatingRowColors(true);
    ui->lineEdit_ServiceDeparture->installEventFilter(this);
    ui->lineEdit_ServiceDestination->installEventFilter(this);
    ui->lineEdit_ServiceDays->installEventFilter(this);
    ui->lineEdit_ServiceDays->setValidator(new QIntValidator(1, 1000, ui->lineEdit_ServiceDays));
    ui->lineEdit_ServiceName->setReadOnly(true);
    ui->lineEdit_ServiceNo->setReadOnly(true);

    connect(ui->listWidget_Service, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(serviceCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));
    connect(ui->listWidget_Service, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(serviceItemClicked(QListWidgetItem *)));
    connect(ui->lineEdit_ServiceDeparture, SIGNAL(textChanged(QString)), this, SLOT(serviceInfoChanged()));
    connect(ui->lineEdit_ServiceDestination, SIGNAL(textChanged(QString)), this, SLOT(serviceInfoChanged()));
    connect(ui->lineEdit_ServiceDays, SIGNAL(textChanged(QString)), this, SLOT(serviceInfoChanged()));
    connect(ui->lineEdit_ServiceFind, SIGNAL(textChanged(QString)), this, SLOT(findServiceInfoChanged(QString)));
    m_pPickServicePriceEditor = new PriceEditor(this, PRICE_EDITOR_PICK_SERVICE);
    m_pPickServicePriceEditor->hide();
    connect(m_pPickServicePriceEditor->getCalendar(), SIGNAL(updatePriceInfoSignal(QMap<QString, QMap<QString, TicketPriceInfo> >&)), this, SLOT(updateServicePriceInfoSlot(QMap<QString, QMap<QString, TicketPriceInfo> >&)));

    //渠道模块
    connect(ui->tableWidget_Channel, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(channelCurrentItemClicked(QTableWidgetItem *)));

    connect(ui->listWidget_ChannelList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(channelListCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));

    //渠道暂时不需要搜索功能
    ui->label_ChannelFind->hide();
    ui->lineEdit_ChannelFind->hide();

    //显示首页图片
    QImage *image=new QImage(PICTURE_FILE_PATH);
    ui->label_Picture->setPixmap(QPixmap::fromImage(*image));


    initUI();
    //reqQunerHome();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(NULL != m_pPriceEditor)
    {
        delete m_pPriceEditor;
    }
    if(NULL != m_pPickServicePriceEditor)
    {
        delete m_pPickServicePriceEditor;
    }
}

void MainWindow::initUI()
{
    updateChannelList();
}

void MainWindow::tabWidgetCurrentChanged(int index)
{
    switch (index) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        updateChannelList();
        break;
    case 4:
        updateChannelRelationUI();
        break;
    default:
        break;
    }
}

void MainWindow::ticketInfoChanged()
{
    qDebug() << "ticketInfoChanged";

    QString strDeparture = "**";
    QString strDestination = "**";
    QString strDays = "**";

    if(NULL != ui->lineEdit_Departure->text())
    {
        strDeparture = ui->lineEdit_Departure->text();
    }
    if(NULL != ui->lineEdit_Destination->text())
    {
        strDestination = ui->lineEdit_Destination->text();
    }
    if(NULL != ui->lineEdit_Day->text())
    {
        strDays = ui->lineEdit_Day->text();
    }

    qDebug() << "Departure:" << strDeparture;
    qDebug() << "Destination:" << strDestination;
    qDebug() << "Days:" << strDays;
    QString strTicketName = strDeparture + strDestination + strDays + "天往返";
    qDebug() << strTicketName;
    ui->lineEdit_Name->setText(strTicketName);
}

void MainWindow::serviceInfoChanged()
{
    qDebug() << "ticketInfoChanged";

    QString strDeparture = "**";
    QString strDestination = "**";
    QString strDays = "**";

    if(NULL != ui->lineEdit_ServiceDeparture->text())
    {
        strDeparture = ui->lineEdit_ServiceDeparture->text();
    }
    if(NULL != ui->lineEdit_ServiceDestination->text())
    {
        strDestination = ui->lineEdit_ServiceDestination->text();
    }
    if(NULL != ui->lineEdit_ServiceDays->text())
    {
        strDays = ui->lineEdit_ServiceDays->text();
    }

    qDebug() << "Departure:" << strDeparture;
    qDebug() << "Destination:" << strDestination;
    qDebug() << "Days:" << strDays;
    QString strServiceName = strDeparture + strDestination + strDays + "天往返";
    qDebug() << strServiceName;
    ui->lineEdit_ServiceName->setText(strServiceName);
}

void MainWindow::findTicketInfoChanged(QString strFindName)
{
    qDebug() << "findTicketInfoChanged";
    QVector<TicketInfo> vecFindTicketInfo;
    for(int i=0; i<m_vecTicketInfo.size(); i++)
    {
        if(m_vecTicketInfo[i].strTicketName.contains(strFindName))
        {
            vecFindTicketInfo.push_back(m_vecTicketInfo[i]);
        }
    }
    qDebug() << vecFindTicketInfo.size();

    //删除所有的Item
    disconnect(ui->listWidget_Ticket, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 0, 0);
    //disconnect(ui->listWidget_Ticket, SIGNAL(currentRowChanged(int)), 0, 0);
    qDebug() << "before delete" << ui->listWidget_Ticket->count();
    while(ui->listWidget_Ticket->count() != 0)
    {
        QListWidgetItem *deleteItem = ui->listWidget_Ticket->takeItem(0);
        if(NULL != deleteItem)
        {
            qDebug() << deleteItem->text();
            delete deleteItem;
            deleteItem = NULL;
        }
    }
    qDebug() << "after delete" << ui->listWidget_Ticket->count();
    connect(ui->listWidget_Ticket, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(ticketCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));
    for(int i=0; i<vecFindTicketInfo.size(); i++)
    {
        addItemToTicketList(vecFindTicketInfo[i]);
    }
    qDebug() << "before add" << ui->listWidget_Ticket->count();
}

void MainWindow::findServiceInfoChanged(QString strFindName)
{
    qDebug() << "findServiceInfoChanged";
    QVector<PickServiceInfo> vecFindPickServiceInfo;
    for(int i=0; i<m_vecPickServiceInfo.size(); i++)
    {
        if(m_vecPickServiceInfo[i].strName.contains(strFindName))
        {
            vecFindPickServiceInfo.push_back(m_vecPickServiceInfo[i]);
        }
    }
    qDebug() << vecFindPickServiceInfo.size();

    //删除所有的Item
    disconnect(ui->listWidget_Service, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 0, 0);
    //disconnect(ui->listWidget_Ticket, SIGNAL(currentRowChanged(int)), 0, 0);
    qDebug() << "before delete" << ui->listWidget_Service->count();
    while(ui->listWidget_Service->count() != 0)
    {
        QListWidgetItem *deleteItem = ui->listWidget_Service->takeItem(0);
        if(NULL != deleteItem)
        {
            qDebug() << deleteItem->text();
            delete deleteItem;
            deleteItem = NULL;
        }
    }
    qDebug() << "after delete" << ui->listWidget_Service->count();
    connect(ui->listWidget_Service, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(serviceCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));
    for(int i=0; i<vecFindPickServiceInfo.size(); i++)
    {
        addItemToServiceList(vecFindPickServiceInfo[i]);
    }
    qDebug() << "before add" << ui->listWidget_Service->count();
}

void MainWindow::on_pushButtonUpdate_clicked()
{
    qDebug() << "on_pushButtonUpdate_clicked";
    TicketInfo tmpTicketInfo;
    fillTicketInfo(tmpTicketInfo);
    if(NULL==tmpTicketInfo.strTicketDeparture
      || NULL == tmpTicketInfo.strTicketDestination
      || NULL == tmpTicketInfo.strTicketDays)
    {
        QMessageBox::information(NULL, QString("提醒"), QString("请输入完整的机票信息!"));
        return;
    }

    bool bFind = false;
    int i=0;
    for(; i<m_vecTicketInfo.size(); i++)
    {
        if(tmpTicketInfo.strTicketName == m_vecTicketInfo[i].strTicketName)
        {
            bFind = true;
            break;
        }
    }

    if(true == bFind)
    {
        QString strText = "是否修改" + tmpTicketInfo.strTicketName + "机票信息？";
        QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::Yes == reply)
        {
             m_vecTicketInfo[i] = tmpTicketInfo;
        }
    }
    else
    {
        QString strText = "是否添加" + tmpTicketInfo.strTicketName + "机票信息？";
        QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::Yes == reply)
        {
            tmpTicketInfo.strTicketNo =  QString::number(QDateTime::currentDateTime().toTime_t(), 10);
            qDebug() << tmpTicketInfo.strTicketNo;
            if(hasSameTicketNo(tmpTicketInfo.strTicketNo))
            {
                QMessageBox::information(NULL, QString("提醒"), QString("请不要在一秒中生成多个机票信息!"));
                return;
            }
            ui->lineEdit_No->setText(tmpTicketInfo.strTicketNo);
            m_vecTicketInfo.push_back(tmpTicketInfo);
            addItemToTicketList(tmpTicketInfo);
        }
    }
    sendTicket(tmpTicketInfo);
}

void MainWindow::on_pushButtonDelete_clicked()
{
    QListWidgetItem *currentItem = ui->listWidget_Ticket->currentItem();
    if(NULL != currentItem)
    {
        QString deleteTicketName = currentItem->text();
        QString strText = "是否删除" + deleteTicketName + "机票信息？";
        QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::Yes == reply)
        {
            for(QVector<TicketInfo>::iterator iter = m_vecTicketInfo.begin();
                iter != m_vecTicketInfo.end(); iter++)
            {
                if(deleteTicketName == iter->strTicketName)
                {
                    clearTicketUI();
                    m_vecTicketInfo.erase(iter);
                    qDebug() << "ui->listWidget_Ticket->currentRow():" << ui->listWidget_Ticket->currentRow();
                    /*QListWidgetItem *deleteItem = ui->listWidget_Ticket->item(ui->listWidget_Ticket->currentRow());
                    if(NULL != deleteItem)
                    {
                        qDebug() << "removeItemWidget";
                        ui->listWidget_Ticket->removeItemWidget(deleteItem);
                        delete deleteItem;
                        deleteItem = NULL;
                    }*/
                    QListWidgetItem *deleteItem = ui->listWidget_Ticket->takeItem(ui->listWidget_Ticket->currentRow());
                    if(NULL != deleteItem)
                    {
                        delete deleteItem;
                        deleteItem = NULL;
                    }
                    break;
                }
            }
        }
    }
}

void MainWindow::on_pushButtonPrice_clicked()
{
    for(int i=0; i<m_vecTicketInfo.size(); i++)
    {
        if(ui->lineEdit_Name->text() == m_vecTicketInfo[i].strTicketName)
        {
            qDebug() << "on_pushButtonPrice_clicked:" << ui->lineEdit_Name->text();
            m_pPriceEditor->setPriceInfo(m_vecTicketInfo[i].mapTicketPriceInfo);
            break;
        }
    }
    m_pPriceEditor->show();
}

void MainWindow::on_pushButtonCancel_clicked()
{
    clearTicketUI();
}

void MainWindow::clearTicketUI()
{
    ui->lineEdit_Departure->setText("");
    ui->lineEdit_Destination->setText("");
    ui->lineEdit_Day->setText("");
    ui->lineEdit_Name->setText("");
    ui->lineEdit_No->setText("");
}

void MainWindow::clearServiceUI()
{
    ui->lineEdit_ServiceDeparture->setText("");
    ui->lineEdit_ServiceDestination->setText("");
    ui->lineEdit_ServiceDays->setText("");
    ui->lineEdit_ServiceName->setText("");
    ui->lineEdit_ServiceNo->setText("");
    ui->lineEdit_ServiceMissionNo->setText("");
    ui->lineEdit_ServiceTicketNo->setText("");
}

bool MainWindow::hasSameTicketNo(QString ticketNo)
{
    bool bHasSame = false;
    for(int i=0; i<m_vecTicketInfo.size(); i++)
    {
        if(ticketNo == m_vecTicketInfo[i].strTicketNo)
        {
            bHasSame = true;
            break;
        }
    }
    return bHasSame;
}

bool MainWindow::hasSameServiceNo(QString serviceNo)
{
    bool bHasSame = false;
    for(int i=0; i<m_vecPickServiceInfo.size(); i++)
    {
        if(serviceNo == m_vecPickServiceInfo[i].strNo)
        {
            bHasSame = true;
            break;
        }
    }
    return bHasSame;
}

void MainWindow::fillTicketInfo(TicketInfo& ticketInfo)
{
    ticketInfo.strTicketDeparture = ui->lineEdit_Departure->text();
    ticketInfo.strTicketDestination = ui->lineEdit_Destination->text();
    ticketInfo.strTicketDays = ui->lineEdit_Day->text();
    ticketInfo.strTicketName = ui->lineEdit_Name->text();
    ticketInfo.strTicketNo = ui->lineEdit_No->text();

    //后面填充票价，出发日期不同票价不一样
    m_pPriceEditor->getPriceInfo(ticketInfo.mapTicketPriceInfo);

}

void MainWindow::fillPickServiceInfo(PickServiceInfo& pickServiceInfo)
{
    pickServiceInfo.strDeparture = ui->lineEdit_ServiceDeparture->text();
    pickServiceInfo.strDestination = ui->lineEdit_ServiceDestination->text();
    pickServiceInfo.strDays = ui->lineEdit_ServiceDays->text();
    pickServiceInfo.strMissionNo = ui->lineEdit_ServiceMissionNo->text();
    pickServiceInfo.strName = ui->lineEdit_ServiceName->text();
    pickServiceInfo.strNo = ui->lineEdit_ServiceNo->text();
    pickServiceInfo.strTicketNo = ui->lineEdit_ServiceTicketNo->text();

    //后面填充地接服务价格
    m_pPickServicePriceEditor->getPriceInfo(pickServiceInfo.mapTicketPriceInfo);
}

void MainWindow::addItemToTicketList(TicketInfo& ticketInfo)
{
    QString itemName = "default";
    if(NULL != ticketInfo.strTicketName)
    {
        itemName = ticketInfo.strTicketName;
    }
    QListWidgetItem *item = new QListWidgetItem(itemName);
    if(NULL == item)
    {
        qDebug() << "item pointer is NULL";
    }

    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    ui->listWidget_Ticket->addItem(item);
    ui->listWidget_Ticket->setCurrentItem(item);
}

void MainWindow::addItemToServiceList(PickServiceInfo& serviceInfo)
{
    QString itemName = "default";
    if(NULL != serviceInfo.strName)
    {
        itemName = serviceInfo.strName;
    }
    QListWidgetItem *item = new QListWidgetItem(itemName);
    if(NULL == item)
    {
        qDebug() << "item pointer is NULL";
        return;
    }

    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    ui->listWidget_Service->addItem(item);
    ui->listWidget_Service->setCurrentItem(item);
}

void MainWindow::ticketCurrentItemChanged(QListWidgetItem *currentItme, QListWidgetItem */*previousItem*/)
{
    qDebug() << "ticketCurrentItemChanged";
    ticketItemClicked(currentItme);
}

void MainWindow::serviceCurrentItemChanged(QListWidgetItem *currentItme, QListWidgetItem *)
{
    qDebug() << "serviceCurrentItemChanged";
    serviceItemClicked(currentItme);
}

void MainWindow::ticketItemClicked(QListWidgetItem* listWidgetItem)
{
    QString selectTicketName = listWidgetItem->text();
    bool bFind = false;
    TicketInfo tmpTicketInfo;
    for(int i=0; i<m_vecTicketInfo.size(); i++)
    {
        if(selectTicketName == m_vecTicketInfo[i].strTicketName)
        {
            bFind = true;
            tmpTicketInfo = m_vecTicketInfo[i];
        }
    }
    if(true == bFind)
    {
        updateTicketUI(tmpTicketInfo);
    }
}

void MainWindow::serviceItemClicked(QListWidgetItem* listWidgetItem)
{
    qDebug() << "serviceItemClicked";
    QString selectServiceName = listWidgetItem->text();
    bool bFind = false;
    PickServiceInfo tmpServiceInfo;
    for(int i=0; i<m_vecPickServiceInfo.size(); i++)
    {
        if(selectServiceName == m_vecPickServiceInfo[i].strName)
        {
            bFind = true;
            tmpServiceInfo = m_vecPickServiceInfo[i];
        }
    }
    if(true == bFind)
    {
        updateServiceUI(tmpServiceInfo);
    }
}

void MainWindow::updateTicketUI(TicketInfo & ticketInfo)
{
    ui->lineEdit_Departure->setText(ticketInfo.strTicketDeparture);
    ui->lineEdit_Destination->setText(ticketInfo.strTicketDestination);
    ui->lineEdit_Day->setText(ticketInfo.strTicketDays);
    ui->lineEdit_Name->setText(ticketInfo.strTicketName);
    ui->lineEdit_No->setText(ticketInfo.strTicketNo);

    //update票价在弹出票价编译对话框时更新
}

void MainWindow::updateServiceUI(PickServiceInfo& serviceInfo)
{
    ui->lineEdit_ServiceDeparture->setText(serviceInfo.strDeparture);
    ui->lineEdit_ServiceDestination->setText(serviceInfo.strDestination);
    ui->lineEdit_ServiceDays->setText(serviceInfo.strDays);
    ui->lineEdit_ServiceName->setText(serviceInfo.strName);
    ui->lineEdit_ServiceMissionNo->setText(serviceInfo.strMissionNo);
    ui->lineEdit_ServiceNo->setText(serviceInfo.strNo);
    ui->lineEdit_ServiceTicketNo->setText(serviceInfo.strTicketNo);
}

void MainWindow::updatePriceInfoSlot(QMap<QString, QMap<QString, TicketPriceInfo> >& mapPriceInfo)
{
    qDebug() << "updatePriceInfoSlot" << ui->lineEdit_Name->text();
    for(int i=0; i<m_vecTicketInfo.size(); i++)
    {
        if(ui->lineEdit_Name->text() == m_vecTicketInfo[i].strTicketName)
        {
            qDebug() << "updatePriceInfoSlot" << ui->lineEdit_Name->text();
            m_vecTicketInfo[i].mapTicketPriceInfo = mapPriceInfo;
            break;
        }
    }

    //需要想后台更新的机票信息
    for(int i=0; i<m_vecUpdateTicketInfo.size(); i++)
    {
        if(ui->lineEdit_Name->text() == m_vecUpdateTicketInfo[i].strTicketName)
        {
            m_vecUpdateTicketInfo[i].mapTicketPriceInfo = mapPriceInfo;
            break;
        }
    }
}

void MainWindow::updateServicePriceInfoSlot(QMap<QString, QMap<QString, TicketPriceInfo> >& mapPriceInfo)
{
    qDebug() << "updateServicePriceInfoSlot" << ui->lineEdit_ServiceName->text();
    for(int i=0; i<m_vecPickServiceInfo.size(); i++)
    {
        if(ui->lineEdit_ServiceName->text() == m_vecPickServiceInfo[i].strName)
        {
            qDebug() << "updateServicePriceInfoSlot" << ui->lineEdit_ServiceName->text();
            m_vecPickServiceInfo[i].mapTicketPriceInfo = mapPriceInfo;
            break;
        }
    }

    //需要想后台更新的地接服务信息
    for(int i=0; i<m_vecUpdatePickServiceInfo.size(); i++)
    {
        if(ui->lineEdit_ServiceName->text() == m_vecUpdatePickServiceInfo[i].strName)
        {
            m_vecUpdatePickServiceInfo[i].mapTicketPriceInfo = mapPriceInfo;
            break;
        }
    }
}

void MainWindow::on_pushButton_ServiceUpdate_clicked()
{
    qDebug() << "on_pushButton_ServiceUpdate_clicked";
    PickServiceInfo tmpPickServiceInfo;
    fillPickServiceInfo(tmpPickServiceInfo);
    if(NULL==tmpPickServiceInfo.strDeparture
      || NULL == tmpPickServiceInfo.strDestination
      || NULL == tmpPickServiceInfo.strDays)
    {
        QMessageBox::information(NULL, QString("提醒"), QString("请输入完整的地接服务信息!"));
        return;
    }

    bool bFind = false;
    int i=0;
    for(; i<m_vecPickServiceInfo.size(); i++)
    {
        if(tmpPickServiceInfo.strName == m_vecPickServiceInfo[i].strName)
        {
            bFind = true;
            break;
        }
    }

    if(true == bFind)
    {
        QString strText = "是否修改" + tmpPickServiceInfo.strName + "地接服务信息？";
        QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::Yes == reply)
        {
             m_vecPickServiceInfo[i] = tmpPickServiceInfo;
        }
    }
    else
    {
        QString strText = "是否添加" + tmpPickServiceInfo.strName + "地接服务信息？";
        QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::Yes == reply)
        {
            tmpPickServiceInfo.strNo =  QString::number(QDateTime::currentDateTime().toTime_t(), 10);
            qDebug() << tmpPickServiceInfo.strNo;
            if(hasSameServiceNo(tmpPickServiceInfo.strNo))
            {
                QMessageBox::information(NULL, QString("提醒"), QString("请不要在一秒中生成多个地接服务信息!"));
                return;
            }
            ui->lineEdit_ServiceNo->setText(tmpPickServiceInfo.strNo);
            m_vecPickServiceInfo.push_back(tmpPickServiceInfo);
            addItemToServiceList(tmpPickServiceInfo);
        }
    }
    sendPickService(tmpPickServiceInfo);
}

void MainWindow::on_pushButton_ServiceDelete_clicked()
{
    QListWidgetItem *currentItem = ui->listWidget_Service->currentItem();
    if(NULL != currentItem)
    {
        QString deleteServiceName = currentItem->text();
        QString strText = "是否删除" + deleteServiceName + "地接服务信息？";
        QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::Yes == reply)
        {
            for(QVector<PickServiceInfo>::iterator iter = m_vecPickServiceInfo.begin();
                iter != m_vecPickServiceInfo.end(); iter++)
            {
                if(deleteServiceName == iter->strName)
                {
                    clearServiceUI();
                    m_vecPickServiceInfo.erase(iter);
                    qDebug() << "ui->listWidget_Service->currentRow():" << ui->listWidget_Service->currentRow();
                    /*QListWidgetItem *deleteItem = ui->listWidget_Ticket->item(ui->listWidget_Ticket->currentRow());
                    if(NULL != deleteItem)
                    {
                        qDebug() << "removeItemWidget";
                        ui->listWidget_Ticket->removeItemWidget(deleteItem);
                        delete deleteItem;
                        deleteItem = NULL;
                    }*/
                    QListWidgetItem *deleteItem = ui->listWidget_Service->takeItem(ui->listWidget_Service->currentRow());
                    if(NULL != deleteItem)
                    {
                        delete deleteItem;
                        deleteItem = NULL;
                    }
                    break;
                }
            }
        }
    }
}

void MainWindow::on_pushButton_ServiceCancel_clicked()
{
    clearServiceUI();
}

void MainWindow::on_pushButton_ServicePrice_clicked()
{
    int i=0;
    for(; i<m_vecPickServiceInfo.size(); i++)
    {
        if(ui->lineEdit_ServiceName->text() == m_vecPickServiceInfo[i].strName)
        {
            qDebug() << "on_pushButton_ServicePrice_clicked:" << ui->lineEdit_ServiceName->text();
            m_pPickServicePriceEditor->setPriceInfo(m_vecPickServiceInfo[i].mapTicketPriceInfo);
            break;
        }
    }

    qDebug () << "setHelpPriceInfo" << m_vecTicketInfo.size();
    for(int j=0; j<m_vecTicketInfo.size(); j++)
    {
        //查到关联的机票产品信息
        qDebug() << "m_vecPickServiceInfo[i].strTicketNo:" << m_vecPickServiceInfo[i].strTicketNo;
        qDebug() << "m_vecTicketInfo[j].strTicketNo:" << m_vecTicketInfo[j].strTicketNo;
        qDebug() << "ui->lineEdit_ServiceTicketNo->text():" << ui->lineEdit_ServiceTicketNo->text();

        if(m_vecPickServiceInfo[i].strTicketNo == m_vecTicketInfo[j].strTicketNo
           || ui->lineEdit_ServiceTicketNo->text() == m_vecTicketInfo[j].strTicketNo)
        {
            qDebug () << "setHelpPriceInfo";
            m_pPickServicePriceEditor->setHelpPriceInfo(m_vecTicketInfo[j].mapTicketPriceInfo);
            qDebug() << "nTicketAdultPrice:" << m_vecTicketInfo[j].mapTicketPriceInfo["201511"]["8"].nTicketAdultPrice;
            qDebug() << "nTicketChildPrice:" << m_vecTicketInfo[j].mapTicketPriceInfo["201511"]["8"].nTicketChildPrice;
            break;
        }
    }
    m_pPickServicePriceEditor->show();
}

void MainWindow::sendTicket(const TicketInfo & ticketInfo)
{
    QJsonObject jsonObject;
    ticketInfo.writeTo(jsonObject);
    QJsonDocument jsonDocument(jsonObject);
    QByteArray postData;
    postData.append("op=update&");
    postData.append("ticket_num=").append(ticketInfo.strTicketNo);
    postData.append("&ticket_info=").append(jsonDocument.toJson());

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/airticket.cgi"));

    QNetworkReply* pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyFinishedForTicket()));
}

void MainWindow::sendPickService(const PickServiceInfo & pickServiceInfo)
{
    QJsonObject jsonObject;
    pickServiceInfo.writeTo(jsonObject);
    QJsonDocument jsonDocument(jsonObject);
    QByteArray postData;
    postData.append("op=update&");
    postData.append("service_num=").append(pickServiceInfo.strNo);
    postData.append("&service_info=").append(jsonDocument.toJson());

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/pickservice.cgi"));
    QNetworkReply* pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);

    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyFinishedForPickService()));


}


void MainWindow::replyFinishedForTicket()
{
    QNetworkReply *pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonObject jsonObject = document.object();
        QJsonValue jsonValue = jsonObject.value(QString("ret"));
        if (jsonValue.isUndefined())
        {
            QMessageBox::information(NULL, QString("错误"), QString("返回json错误!"));
        }
        if (0 == jsonValue.toDouble())
        {
             QMessageBox::information(NULL, QString("提示"), QString("更新机票信息成功"));
        }
        else
        {
            jsonValue = jsonObject.value(QString("msg"));
            if (jsonValue.isUndefined())
            {
                  QMessageBox::information(NULL, QString("错误"), QString("更新机票信息错误!"));
            }
            else
            {
                 QMessageBox::information(NULL, QString("错误"), QString("更新机票信息错误!").append(jsonValue.toString()));
            }
        }

    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }

}

void MainWindow::replyFinishedForPickService()
{
    QNetworkReply *pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonObject jsonObject = document.object();
        QJsonValue jsonValue = jsonObject.value(QString("ret"));
        if (jsonValue.isUndefined())
        {
            QMessageBox::information(NULL, QString("错误"), QString("返回json错误!"));
        }
        if (0 == jsonValue.toDouble())
        {
             QMessageBox::information(NULL, QString("提示"), QString("更新地接信息成功"));
        }
        else
        {
            jsonValue = jsonObject.value(QString("msg"));
            if (jsonValue.isUndefined())
            {
                  QMessageBox::information(NULL, QString("错误"), QString("更新地接信息错误!"));
            }
            else
            {
                 QMessageBox::information(NULL, QString("错误"), QString("更新地接信息错误!").append(jsonValue.toString()));
            }
        }

    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    pNetworkReply->deleteLater();
}

void MainWindow::loadTicket()
{
    QByteArray postData;
    postData.append("op=load&");

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/airticket.cgi"));

    QNetworkReply *pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()),
            this, SLOT(replyFinishedForLoadTicket()));
}

void MainWindow::loadPickServce()
{
    QByteArray postData;
    postData.append("op=load&");

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/pickservice.cgi"));
    QNetworkReply *pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);

    connect(pNetworkReply, SIGNAL(finished()),
            this, SLOT(replyFinishedForLoadPickService()));
}

void MainWindow::replyFinishedForLoadTicket()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonArray jsonArray = document.array();
        for (QJsonArray::iterator it = jsonArray.begin(); it != jsonArray.end(); ++it)
        {
            QString sTicket = (*it).toObject()["ticket_info"].toString();
            QJsonParseError jsonParseErr;
            QJsonDocument docTicket = QJsonDocument::fromJson(sTicket.toUtf8(), &jsonParseErr);
            QJsonObject jsonObject = docTicket.object();
            TicketInfo ticketInfo;
            ticketInfo.readFrom(jsonObject);
            m_vecTicketInfo.push_back(ticketInfo);
        }
    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    pNetworkReply->deleteLater();
}

void MainWindow::replyFinishedForLoadPickService()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonArray jsonArray = document.array();
        for (QJsonArray::iterator it = jsonArray.begin(); it != jsonArray.end(); ++it)
        {
            QString sService = (*it).toObject()["service_info"].toString();
            QJsonParseError jsonParseErr;
            QJsonDocument docService = QJsonDocument::fromJson(sService.toUtf8(), &jsonParseErr);
            QJsonObject jsonObject = docService.object();
            PickServiceInfo pickServiceInfo;
            pickServiceInfo.readFrom(jsonObject);
            m_vecPickServiceInfo.push_back(pickServiceInfo);
        }
    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    pNetworkReply->deleteLater();
}

void MainWindow::sendChannel(const ChannelInfo & channelInfo)
{
    QJsonObject jsonObject;
    channelInfo.writeTo(jsonObject);
    QJsonDocument jsonDocument(jsonObject);
    QByteArray postData;
    postData.append("op=update&");
    postData.append("channel_name=").append(channelInfo.strChannelName);
    postData.append("&channel_info=").append(jsonDocument.toJson());

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/channel.cgi"));
    QNetworkReply* pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyUpdateChannel()));
}

void MainWindow::sendChannelRelation(const ChannelRelationInfo & channelRelationInfo)
{
    QJsonObject jsonObject;
    channelRelationInfo.writeTo(jsonObject);
    QJsonDocument jsonDocument(jsonObject);
    QByteArray postData;
    postData.append("op=update&");
    postData.append("channel_name=").append(channelRelationInfo.strChannelName);
    postData.append("product_id=").append(channelRelationInfo.strShopProductId);
    postData.append("&channel_relation_info=").append(jsonDocument.toJson());

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/channelrelation.cgi"));
    QNetworkReply* pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyUpdateChannelRelation()));
}


void MainWindow::replyUpdateChannel()
{
    QNetworkReply *pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonObject jsonObject = document.object();
        QJsonValue jsonValue = jsonObject.value(QString("ret"));
        if (jsonValue.isUndefined())
        {
            QMessageBox::information(NULL, QString("错误"), QString("返回json错误!"));
        }
        if (0 == jsonValue.toDouble())
        {
             QMessageBox::information(NULL, QString("提示"), QString("更新渠道信息成功"));
        }
        else
        {
            jsonValue = jsonObject.value(QString("msg"));
            if (jsonValue.isUndefined())
            {
                  QMessageBox::information(NULL, QString("错误"), QString("更新渠道信息错误!"));
            }
            else
            {
                 QMessageBox::information(NULL, QString("错误"), QString("更新渠道信息错误!").append(jsonValue.toString()));
            }
        }

    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    pNetworkReply->deleteLater();
}

void MainWindow::replyUpdateChannelRelation()
{
    QNetworkReply *pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonObject jsonObject = document.object();
        QJsonValue jsonValue = jsonObject.value(QString("ret"));
        if (jsonValue.isUndefined())
        {
            QMessageBox::information(NULL, QString("错误"), QString("返回json错误!"));
        }
        if (0 == jsonValue.toDouble())
        {
             QMessageBox::information(NULL, QString("提示"), QString("更新渠道关联信息成功"));
        }
        else
        {
            jsonValue = jsonObject.value(QString("msg"));
            if (jsonValue.isUndefined())
            {
                  QMessageBox::information(NULL, QString("错误"), QString("更新渠道关联信息错误!"));
            }
            else
            {
                 QMessageBox::information(NULL, QString("错误"), QString("更新渠道关联信息错误!").append(jsonValue.toString()));
            }
        }

    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    pNetworkReply->deleteLater();
}


void MainWindow::loadChannel()
{
    QByteArray postData;
    postData.append("op=load&");

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/channel.cgi"));

    QNetworkReply *pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyLoadChannel()));
}

void MainWindow::loadChannelRelation()
{
    QByteArray postData;
    postData.append("op=load&");

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/channel.cgi"));

    QNetworkReply *pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyLoadChannelRelation()));
}

void MainWindow::replyLoadChannel()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonArray jsonArray = document.array();
        for (QJsonArray::iterator it = jsonArray.begin(); it != jsonArray.end(); ++it)
        {
            QString sChannel = (*it).toObject()["channel_info"].toString();
            QJsonParseError jsonParseErr;
            QJsonDocument docChannel = QJsonDocument::fromJson(sChannel.toUtf8(), &jsonParseErr);
            QJsonObject jsonObject = docChannel.object();
            ChannelInfo channelInfo;
            channelInfo.readFrom(jsonObject);
            m_vecChannelInfo.push_back(channelInfo);
        }
    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    pNetworkReply->deleteLater();

}

void MainWindow::replyLoadChannelRelation()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonArray jsonArray = document.array();
        for (QJsonArray::iterator it = jsonArray.begin(); it != jsonArray.end(); ++it)
        {
            QString sChannelRelation = (*it).toObject()["channel_relation_info"].toString();
            QJsonParseError jsonParseErr;
            QJsonDocument docChannelRelation = QJsonDocument::fromJson(sChannelRelation.toUtf8(), &jsonParseErr);
            QJsonObject jsonObject = docChannelRelation.object();
            ChannelRelationInfo channelRelationInfo;
            channelRelationInfo.readFrom(jsonObject);
            m_mapChannelRelationInfo[channelRelationInfo.strChannelName].push_back(channelRelationInfo);
        }
    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    pNetworkReply->deleteLater();
}

void MainWindow::updateChannelList()
{
    qDebug() << "updateChannelList";
    int row = ui->tableWidget_Channel->rowCount();
    for(int i=0; i<row; i++)
    {
        ui->tableWidget_Channel->removeRow(0);
    }

    ui->tableWidget_Channel->setColumnCount(3);
    ui->tableWidget_Channel->setRowCount(m_vecChannelInfo.size());
    ui->tableWidget_Channel->verticalHeader()->setVisible(false);
    //ui->tableWidget_Channel->horizontalHeader()->setVisible(false);
    QStringList header;
    header << "渠道名称" << "账号" << "密码";
    ui->tableWidget_Channel->setHorizontalHeaderLabels(header);
    ui->tableWidget_Channel->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    for(int i=0; i<m_vecChannelInfo.size(); i++)
    {
        QTableWidgetItem *itemName = new QTableWidgetItem(m_vecChannelInfo[i].strChannelName);
        itemName->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_Channel->setItem(i, 0, itemName);

        QTableWidgetItem *itemAccout = new QTableWidgetItem(m_vecChannelInfo[i].strAccount);
        itemAccout->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_Channel->setItem(i, 1, itemAccout);

        QTableWidgetItem *itemPassword = new QTableWidgetItem(m_vecChannelInfo[i].strPassword);
        itemPassword->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_Channel->setItem(i, 2, itemPassword);
    }
}

void MainWindow::on_pushButton_ChannelUpdate_clicked()
{
    qDebug() << "on_pushButton_ChannelUpdate_clicked";
    ChannelInfo tmpChannelInfo;
    tmpChannelInfo.strChannelName = ui->lineEdit_ChannelName->text();
    tmpChannelInfo.strAccount = ui->lineEdit_Account->text();
    tmpChannelInfo.strPassword = ui->lineEdit_Password->text();

    int i=0;
    for(; i<m_vecChannelInfo.size(); i++)
    {
        if(tmpChannelInfo.strChannelName == m_vecChannelInfo[i].strChannelName)
        {
            QString strText = "是否修改" + tmpChannelInfo.strChannelName + "渠道信息？";
            QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
            if(QMessageBox::Yes == reply)
            {
                 m_vecChannelInfo[i] = tmpChannelInfo;
            }
            break;
        }
    }

    if(i == m_vecChannelInfo.size())
    {
        QString strText = "是否添加" + tmpChannelInfo.strChannelName + "渠道信息？";
        QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::Yes == reply)
        {
            m_vecChannelInfo.push_back(tmpChannelInfo);
        }
    }

    updateChannelList();
}

void MainWindow::on_pushButton_ChannelDel_clicked()
{
    QString strText = "是否删除" + ui->lineEdit_ChannelName->text() + "渠道信息？";
    QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
    if(QMessageBox::Yes == reply)
    {
        for(QVector<ChannelInfo>::iterator iter = m_vecChannelInfo.begin(); iter != m_vecChannelInfo.end(); iter++ )
        {
            if(ui->lineEdit_ChannelName->text() == iter->strChannelName)
            {
                m_vecChannelInfo.erase(iter);
                break;
            }
        }
    }

    updateChannelList();
    ui->lineEdit_ChannelName->setText("");
    ui->lineEdit_Account->setText("");
    ui->lineEdit_Password->setText("");
}

void MainWindow::on_pushButton_ChannelCancel_clicked()
{
    ui->lineEdit_ChannelName->setText("");
    ui->lineEdit_Account->setText("");
    ui->lineEdit_Password->setText("");
}

void MainWindow::channelCurrentItemClicked(QTableWidgetItem *tableWidgetItem)
{
    int currentRow = ui->tableWidget_Channel->row(tableWidgetItem);
    if(currentRow < m_vecChannelInfo.size())
    {
        ui->lineEdit_ChannelName->setText(m_vecChannelInfo[currentRow].strChannelName);
        ui->lineEdit_Account->setText(m_vecChannelInfo[currentRow].strAccount);
        ui->lineEdit_Password->setText(m_vecChannelInfo[currentRow].strPassword);
    }
}

void MainWindow::updateChannelRelationUI()
{
    disconnect(ui->listWidget_ChannelList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 0, 0);
    ui->listWidget_ChannelList->clear();
    int count = ui->listWidget_ChannelList->count();
    for(int i=0; i<count; i++)
    {
        QListWidgetItem *deleteItem = ui->listWidget_Ticket->takeItem(0);
        if(NULL != deleteItem)
        {
            delete deleteItem;
            deleteItem = NULL;
        }
    }

    for(int i=0; i<m_vecChannelInfo.size(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(m_vecChannelInfo[i].strChannelName);
        if(NULL == item)
        {
            qDebug() << "item pointer is NULL";
            break;
        }

        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->listWidget_ChannelList->addItem(item);
        ui->listWidget_ChannelList->setCurrentItem(item);
    }
    connect(ui->listWidget_ChannelList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(channelListCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));
}

void MainWindow::updateChannelRelationDetailUI(QString strChannelName)
{
    QVector<ChannelRelationInfo> vecChannelRealationInfo = m_mapChannelRelationInfo[strChannelName];

    int row = ui->tableWidget_ChannelRelation->rowCount();
    for(int i=0; i<row; i++)
    {
        ui->tableWidget_ChannelRelation->removeRow(0);
    }

    ui->tableWidget_ChannelRelation->setColumnCount(3);
    ui->tableWidget_ChannelRelation->setRowCount(vecChannelRealationInfo.size());
    ui->tableWidget_Channel->verticalHeader()->setVisible(false);
    //ui->tableWidget_Channel->horizontalHeader()->setVisible(false);
    QStringList header;
    header << "地接产品ID" << "关联店铺产品ID" << "关联店铺产品名称";
    ui->tableWidget_ChannelRelation->setHorizontalHeaderLabels(header);
    ui->tableWidget_ChannelRelation->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    for(int i=0; i<vecChannelRealationInfo.size(); i++)
    {
        QTableWidgetItem *itemPickServiceID = new QTableWidgetItem(vecChannelRealationInfo[i].strPickServiceId);
        itemPickServiceID->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelRelation->setItem(i, 0, itemPickServiceID);

        QTableWidgetItem *itemShopProductId = new QTableWidgetItem(vecChannelRealationInfo[i].strShopProductId);
        itemShopProductId->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelRelation->setItem(i, 1, itemShopProductId);

        QTableWidgetItem *itemShopProductName = new QTableWidgetItem(vecChannelRealationInfo[i].strShopProductName);
        itemShopProductName->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelRelation->setItem(i, 2, itemShopProductName);
    }
}

void MainWindow::channelListCurrentItemChanged(QListWidgetItem *currentItem, QListWidgetItem *)
{
    QString strChannelName = currentItem->text();
    updateChannelRelationDetailUI(strChannelName);
    clearChannelRelationDetailUI();
}

void MainWindow::on_pushButton_ChannelRelationUpdate_clicked()
{
    QString strChannelName = "";
    if(ui->listWidget_ChannelList->currentItem())
    {
        strChannelName = ui->listWidget_ChannelList->currentItem()->text();
    }
    if(strChannelName == "")
    {
        QMessageBox::information(NULL, QString("提醒"), QString("请关联一个渠道!"));
        return;
    }

    QVector<ChannelRelationInfo>& vecChannelRealationInfo = m_mapChannelRelationInfo[strChannelName];

    ChannelRelationInfo tmpChannelRelationInfo;
    tmpChannelRelationInfo.strPickServiceId = ui->lineEdit_ChannelPickServiceID->text();
    tmpChannelRelationInfo.strShopProductId = ui->lineEdit_ShopProductID->text();
    tmpChannelRelationInfo.strShopProductName = ui->lineEdit_ShopProductName->text();
    if("" == tmpChannelRelationInfo.strPickServiceId)
    {
        QMessageBox::information(NULL, QString("提醒"), QString("请输入地接服务ID!"));
        return;
    }

    if("" == tmpChannelRelationInfo.strShopProductId)
    {
        QMessageBox::information(NULL, QString("提醒"), QString("请输入关联店铺产品ID!"));
        return;
    }

    int i=0;
    for(; i<vecChannelRealationInfo.size(); i++)
    {
        if(tmpChannelRelationInfo.strShopProductId == vecChannelRealationInfo[i].strShopProductId)
        {
            QString strText = "是否修改" + tmpChannelRelationInfo.strShopProductId + "关联信息？";
            QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
            if(QMessageBox::Yes == reply)
            {
                 vecChannelRealationInfo[i] = tmpChannelRelationInfo;
            }
            break;
        }
    }

    if(i == vecChannelRealationInfo.size())
    {
        QString strText = "是否添加" + tmpChannelRelationInfo.strShopProductId + "关联信息？";
        QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::Yes == reply)
        {
            vecChannelRealationInfo.push_back(tmpChannelRelationInfo);
        }
    }

    updateChannelRelationDetailUI(strChannelName);
}

void MainWindow::on_pushButton_ChannelRelationDel_clicked()
{
    QString strChannelName = "";
    if(ui->listWidget_ChannelList->currentItem())
    {
        strChannelName = ui->listWidget_ChannelList->currentItem()->text();
    }
    if(strChannelName == "")
    {
        QMessageBox::information(NULL, QString("提醒"), QString("请选择一个渠道!"));
        return;
    }

    QVector<ChannelRelationInfo>& vecChannelRealationInfo = m_mapChannelRelationInfo[strChannelName];

    ChannelRelationInfo tmpChannelRelationInfo;
    tmpChannelRelationInfo.strPickServiceId = ui->lineEdit_ChannelPickServiceID->text();
    tmpChannelRelationInfo.strShopProductId = ui->lineEdit_ShopProductID->text();
    tmpChannelRelationInfo.strShopProductName = ui->lineEdit_ShopProductName->text();

    QString strText = "是否删除" + tmpChannelRelationInfo.strShopProductId + "关联产品信息？";
    QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
    if(QMessageBox::Yes == reply)
    {
        for(QVector<ChannelRelationInfo>::iterator iter = vecChannelRealationInfo.begin();
            iter != vecChannelRealationInfo.end(); iter++)
        {
            if(tmpChannelRelationInfo.strShopProductId == iter->strShopProductId)
            {
                clearChannelRelationDetailUI();
                vecChannelRealationInfo.erase(iter);
                break;
            }
        }
    }

    updateChannelRelationDetailUI(strChannelName);
}

void MainWindow::on_pushButton_ChannelRelationCancel_clicked()
{
    clearChannelRelationDetailUI();
}

void MainWindow::clearChannelRelationDetailUI()
{
    ui->lineEdit_ChannelPickServiceID->clear();
    ui->lineEdit_ShopProductID->clear();
    ui->lineEdit_ShopProductName->clear();
}
