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
    m_iLoadPage = 0;
    ui->setupUi(this);
    setWindowTitle(COMPANY_NAME);
    m_pAssitNetworkManager = new QNetworkAccessManager(this);

    loadTicket();


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
    connect(m_pPriceEditor, SIGNAL(saveSignals(PriceEditorMode)), this, SLOT(saveSignalsSlot(PriceEditorMode)));
    connect(m_pPriceEditor->getCalendar(), SIGNAL(updatePriceInfoSignal(QMap<QString, QMap<QString, TicketPriceInfo> >&)), this, SLOT(updatePriceInfoSlot(QMap<QString, QMap<QString, TicketPriceInfo> >&)));

    //地接模块UI
    ui->listWidget_Service->setAlternatingRowColors(true);
    ui->lineEdit_ServiceDeparture->installEventFilter(this);
    ui->lineEdit_ServiceDestination->installEventFilter(this);
    ui->lineEdit_ServiceDays->installEventFilter(this);
    ui->lineEdit_ServiceDays->setValidator(new QIntValidator(1, 1000, ui->lineEdit_ServiceDays));
    //ui->lineEdit_ServiceName->setReadOnly(true);
    ui->lineEdit_ServiceNo->setReadOnly(true);

    //产品模块
    ui->lineEdit_ProductNo->setReadOnly(true);
    connect(ui->lineEdit_ProductFind, SIGNAL(textChanged(QString)), this, SLOT(findProductInfoChanged(QString)));

    connect(ui->listWidget_Service, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(serviceCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));
    connect(ui->listWidget_Service, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(serviceItemClicked(QListWidgetItem *)));
    connect(ui->lineEdit_ServiceDeparture, SIGNAL(textChanged(QString)), this, SLOT(serviceInfoChanged()));
    connect(ui->lineEdit_ServiceDestination, SIGNAL(textChanged(QString)), this, SLOT(serviceInfoChanged()));
    connect(ui->lineEdit_ServiceDays, SIGNAL(textChanged(QString)), this, SLOT(serviceInfoChanged()));
    connect(ui->lineEdit_ServiceFind, SIGNAL(textChanged(QString)), this, SLOT(findServiceInfoChanged(QString)));
    connect(ui->lineEdit_ChannelServiceSearch, SIGNAL(textChanged(QString)), this, SLOT(findChannelServiceChanged(QString)));
    m_pPickServicePriceEditor = new PriceEditor(this, PRICE_EDITOR_PICK_SERVICE);
    m_pPickServicePriceEditor->hide();
    connect(m_pPickServicePriceEditor, SIGNAL(saveSignals(PriceEditorMode)), this, SLOT(saveSignalsSlot(PriceEditorMode)));
    connect(m_pPickServicePriceEditor->getCalendar(), SIGNAL(updatePriceInfoSignal(QMap<QString, QMap<QString, TicketPriceInfo> >&)), this, SLOT(updateServicePriceInfoSlot(QMap<QString, QMap<QString, TicketPriceInfo> >&)));

    //渠道模块
    connect(ui->tableWidget_Channel, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(channelCurrentItemClicked(QTableWidgetItem *)));

    connect(ui->listWidget_ChannelList, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(channelListCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));

    connect(ui->tableWidget_ChannelRelation, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(channelRelationCurrentItemClicked(QTableWidgetItem *)));

    connect(ui->tableWidget_ChannelServiceList, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(channelServiceListCurrentItemClicked(QTableWidgetItem *)));

    connect(ui->tableWidget_ProductList, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(productListCurrentItemClicked(QTableWidgetItem *)));

    //渠道暂时不需要搜索功能
    ui->label_ChannelFind->hide();
    ui->lineEdit_ChannelFind->hide();

    //显示首页图片
    QImage *image=new QImage(PICTURE_FILE_PATH);
    ui->label_Picture->setPixmap(QPixmap::fromImage(*image));

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
    updateChannelListUI();
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
    case 4:
        updateChannelListUI();
        break;
    case 5:
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
    //QString strServiceName = strDeparture + strDestination + strDays + "天往返";
    //qDebug() << strServiceName;
    //ui->lineEdit_ServiceName->setText(strServiceName);
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
        if(m_vecPickServiceInfo[i].strName.contains(strFindName, Qt::CaseInsensitive)
                || m_vecPickServiceInfo[i].strMissionNo.contains(strFindName, Qt::CaseInsensitive)
                || m_vecPickServiceInfo[i].strNo.contains(strFindName, Qt::CaseInsensitive))
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

void MainWindow::findChannelServiceChanged(QString strFindName)
{
    QVector<ProductInfo> vecFindProductInfo;
    for(int i=0; i<m_vecProductInfo.size(); i++)
    {
        if(m_vecProductInfo[i].strName.contains(strFindName, Qt::CaseInsensitive)
                || m_vecProductInfo[i].strMissionNo.contains(strFindName, Qt::CaseInsensitive)
                || m_vecProductInfo[i].strNo.contains(strFindName, Qt::CaseInsensitive))
        {
            vecFindProductInfo.push_back(m_vecProductInfo[i]);
        }
    }
    qDebug() << vecFindProductInfo.size();

    //删除所有的Item
    addItemToChannelServiceList(vecFindProductInfo);
}

void MainWindow::findProductInfoChanged(QString strFindName)
{
    QVector<ProductInfo> vecFindProductInfo;
    for(int i=0; i<m_vecProductInfo.size(); i++)
    {
        if(m_vecProductInfo[i].strName.contains(strFindName, Qt::CaseInsensitive)
                || m_vecProductInfo[i].strNo.contains(strFindName, Qt::CaseInsensitive)
                || m_vecProductInfo[i].strServiceNo.contains(strFindName, Qt::CaseInsensitive)
                || m_vecProductInfo[i].strTicketNo.contains(strFindName, Qt::CaseInsensitive)
                || m_vecProductInfo[i].strMissionNo.contains(strFindName, Qt::CaseInsensitive))
        {
            vecFindProductInfo.push_back(m_vecProductInfo[i]);
        }
    }

    qDebug() << vecFindProductInfo.size();
    updateProductList(vecFindProductInfo);
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
        else
        {
            return;
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
        else
        {
            return;
        }
    }
    updateTicket(tmpTicketInfo);

    QMap<QString, QVector<QunarPriceInfo> > mapQunarPriceInfo;

    for(int i=0; i<m_vecProductInfo.size(); i++)
    {
        if(m_vecProductInfo[i].strTicketNo == tmpTicketInfo.strTicketNo)
        {
            for(int j=0; j<m_vecPickServiceInfo.size(); j++)
            {
                if(m_vecPickServiceInfo[j].strNo == m_vecProductInfo[i].strServiceNo)
                {
                    QMap<QString, QVector<QunarPriceInfo> > mapTmpQunarPriceInfo;
                    getPriceInfo4Qunaer(tmpTicketInfo, m_vecPickServiceInfo[j], m_vecProductInfo[i], mapTmpQunarPriceInfo);
                    for(QMap<QString, QVector<QunarPriceInfo> >::iterator iter = mapTmpQunarPriceInfo.begin();
                        iter != mapTmpQunarPriceInfo.end(); iter++)
                    {
                        QVector<QunarPriceInfo> &vecQunarPriceInfo = mapQunarPriceInfo[iter.key()];
                        QVector<QunarPriceInfo> &vecTmpQuanrPriceInfo = iter.value();
                        for(int index=0; index<vecTmpQuanrPriceInfo.size(); index++)
                        {
                            vecQunarPriceInfo.push_back(vecTmpQuanrPriceInfo[index]);
                        }
                    }
                }
            }
        }
    }
    /*for(int i=0; i<m_vecPickServiceInfo.size(); i++)
    {
        if(m_vecPickServiceInfo[i].strTicketNo == tmpTicketInfo.strTicketNo)
        {
            QMap<QString, QVector<QunarPriceInfo> > mapTmpQunarPriceInfo;
            getPriceInfo4Qunaer(tmpTicketInfo, m_vecPickServiceInfo[i], mapTmpQunarPriceInfo);
            for(QMap<QString, QVector<QunarPriceInfo> >::iterator iter = mapTmpQunarPriceInfo.begin();
                iter != mapTmpQunarPriceInfo.end(); iter++)
            {
                QVector<QunarPriceInfo> &vecQunarPriceInfo = mapQunarPriceInfo[iter.key()];
                QVector<QunarPriceInfo> &vecTmpQuanrPriceInfo = iter.value();
                for(int index=0; index<vecTmpQuanrPriceInfo.size(); index++)
                {
                    vecQunarPriceInfo.push_back(vecTmpQuanrPriceInfo[index]);
                }
            }
        }
    }*/
    update2Qunaer(mapQunarPriceInfo);

    m_pPriceEditor->clearUpdateFlag();

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

bool MainWindow::hasSameProductNo(QString productNo)
{
    bool bHasSame = false;
    for(int i=0; i<m_vecProductInfo.size(); i++)
    {
        if(productNo == m_vecProductInfo[i].strNo)
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
    if(/*NULL==tmpPickServiceInfo.strDeparture
      || NULL == tmpPickServiceInfo.strDestination
      || NULL == tmpPickServiceInfo.strDays
      || */NULL == tmpPickServiceInfo.strName)
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
        else
        {
            return;
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
        else
        {
            return;
        }
    }
    updatePickService(tmpPickServiceInfo);

    QMap<QString, QVector<QunarPriceInfo> > mapQunarPriceInfo;

    for(int i=0; i<m_vecProductInfo.size(); i++)
    {
        if(m_vecProductInfo[i].strServiceNo == tmpPickServiceInfo.strNo)
        {
            for(int j=0; j<m_vecTicketInfo.size(); j++)
            {
                if(m_vecTicketInfo[j].strTicketNo == m_vecProductInfo[i].strTicketNo)
                {
                    QMap<QString, QVector<QunarPriceInfo> > mapTmpQunarPriceInfo;
                    getPriceInfo4Qunaer(m_vecTicketInfo[j], tmpPickServiceInfo, m_vecProductInfo[i], mapTmpQunarPriceInfo);
                    for(QMap<QString, QVector<QunarPriceInfo> >::iterator iter = mapTmpQunarPriceInfo.begin();
                        iter != mapTmpQunarPriceInfo.end(); iter++)
                    {
                        QVector<QunarPriceInfo> &vecQunarPriceInfo = mapQunarPriceInfo[iter.key()];
                        QVector<QunarPriceInfo> &vecTmpQuanrPriceInfo = iter.value();
                        for(int index=0; index<vecTmpQuanrPriceInfo.size(); index++)
                        {
                            vecQunarPriceInfo.push_back(vecTmpQuanrPriceInfo[index]);
                        }
                    }
                }
            }
        }
    }
    /*for(int i=0; i<m_vecTicketInfo.size(); i++)
    {
        if(tmpPickServiceInfo.strTicketNo == m_vecTicketInfo[i].strTicketNo)
        {
            QMap<QString, QVector<QunarPriceInfo> > mapTmpQunarPriceInfo;
            getPriceInfo4Qunaer(m_vecTicketInfo[i], tmpPickServiceInfo, mapTmpQunarPriceInfo);
            for(QMap<QString, QVector<QunarPriceInfo> >::iterator iter = mapTmpQunarPriceInfo.begin();
                iter != mapTmpQunarPriceInfo.end(); iter++)
            {
                QVector<QunarPriceInfo> &vecQunarPriceInfo = mapQunarPriceInfo[iter.key()];
                QVector<QunarPriceInfo> &vecTmpQuanrPriceInfo = iter.value();
                for(int index=0; index<vecTmpQuanrPriceInfo.size(); index++)
                {
                    vecQunarPriceInfo.push_back(vecTmpQuanrPriceInfo[index]);
                }
            }
        }
    }*/
    update2Qunaer(mapQunarPriceInfo);
    m_pPickServicePriceEditor->clearUpdateFlag();
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
            break;
        }
    }

    m_pPickServicePriceEditor->show();
}

void MainWindow::updateTicket(const TicketInfo & ticketInfo)
{
    if (ticketInfo.strTicketNo.isEmpty())
    {
        QMessageBox::information(NULL, QString("错误"), QString("机票ID不能为空"));
        return ;
    }
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

void MainWindow::updatePickService(const PickServiceInfo & pickServiceInfo)
{
    if (pickServiceInfo.strNo.isEmpty())
    {
        QMessageBox::information(NULL, QString("错误"), QString("地接ID不能为空"));
        return ;
    }
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
    postData.append("op=load&page=").append(QByteArray::number(m_iLoadPage));

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/airticket.cgi"));

    QNetworkReply *pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyFinishedForLoadTicket()));
}

void MainWindow::loadPickServce()
{
    QByteArray postData;
    postData.append("op=load&page=").append(QByteArray::number(m_iLoadPage));

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
            checkTicketInfo(ticketInfo.mapTicketPriceInfo);
            m_vecTicketInfo.push_back(ticketInfo);
        }
        m_iLoadPage++;
        if (jsonArray.size() < ITEM_NUM_OF_PAGE)
        {
            updateTicketUI();
            m_iLoadPage = 0;
            loadPickServce();
        }
        else
        {
            loadTicket();
        }
    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    qDebug() << "m_vecTicketInfo.szie():" << m_vecTicketInfo.size();
    pNetworkReply->deleteLater();
}

//校验删除一个星期之前的过期数据
void MainWindow::checkTicketInfo(QMap<QString, QMap<QString, TicketPriceInfo> > &mapTicketPriceInfo)
{
    for(QMap<QString, QMap<QString, TicketPriceInfo> >::iterator iterMapTicketPriceInfo = mapTicketPriceInfo.begin();
        iterMapTicketPriceInfo!=mapTicketPriceInfo.end(); )
    {
        bool bEarse = false;
        QString strYearMonth = iterMapTicketPriceInfo.key();
        if(strYearMonth.size() >= 4)
        {
            int nYear = strYearMonth.left(4).toInt();
            QString strYearMonthBak = strYearMonth;
            int nMonth = strYearMonthBak.remove(0, 4).toInt();

            QDate date = QDate::currentDate();
            int nCurYear = date.year();
            int nCurMonth = date.month();
            if(nYear < nCurYear || ((nYear==nCurYear) && (nMonth<nCurMonth)))
            {
                bEarse = true;
            }
        }
        else
        {
            bEarse = true;
        }

        if(true == bEarse)
        {
            iterMapTicketPriceInfo = mapTicketPriceInfo.erase(iterMapTicketPriceInfo);
        }
        else
        {
            iterMapTicketPriceInfo ++;
        }
    }

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
            checkTicketInfo(pickServiceInfo.mapTicketPriceInfo);
            m_vecPickServiceInfo.push_back(pickServiceInfo);
        }
        m_iLoadPage++;
        if (jsonArray.size() < ITEM_NUM_OF_PAGE)
        {
            m_iLoadPage = 0;
            updatePickServiceUI();
            loadChannel();
        }
        else
        {
            loadPickServce();
        }
    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    qDebug() << "m_vecPickServiceInfo.size()" << m_vecPickServiceInfo.size();
    pNetworkReply->deleteLater();
}

void MainWindow::updateChannel(const ChannelInfo & channelInfo)
{
    if (channelInfo.strChannelName.isEmpty())
    {
        QMessageBox::information(NULL, QString("错误"), QString("渠道名不能为空"));
        return ;
    }
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

void MainWindow::updateChannelRelation(const ChannelRelationInfo & channelRelationInfo)
{
    if (channelRelationInfo.strChannelName.isEmpty())
    {
        QMessageBox::information(NULL, QString("错误"), QString("渠道名不能为空"));
        return ;
    }
    if (channelRelationInfo.strShopProductId.isEmpty())
    {
        QMessageBox::information(NULL, QString("错误"), QString("产品ID不能为空"));
        return ;
    }
    QJsonObject jsonObject;
    channelRelationInfo.writeTo(jsonObject);
    QJsonDocument jsonDocument(jsonObject);
    QByteArray postData;
    postData.append("op=update&");
    postData.append("channel_name=").append(channelRelationInfo.strChannelName);
    postData.append("&product_id=").append(channelRelationInfo.strShopProductId);
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
    postData.append("op=load&page=").append(QByteArray::number(m_iLoadPage));

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
    postData.append("op=load&page=").append(QByteArray::number(m_iLoadPage));

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/channelrelation.cgi"));

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
        m_iLoadPage++;
        if (jsonArray.size() < ITEM_NUM_OF_PAGE)
        {
            m_iLoadPage = 0;
            updateChannelListUI();
            loadChannelRelation();
        }
        else
        {
           loadChannel();
        }

    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    for(int i=0; i<m_vecChannelInfo.size(); i++)
    {
        m_vecQunerHttPtr.push_back(new QunerHttp(m_vecChannelInfo[i].strAccount, m_vecChannelInfo[i].strPassword, m_vecChannelInfo[i].strChannelName, this));
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
        m_iLoadPage++;
        if (jsonArray.size() < ITEM_NUM_OF_PAGE)
        {
            m_iLoadPage = 0;
            loadProduct();
        }
        else
        {
            loadChannelRelation();
        }
        //updateChannelRelationDetailUI(strChannelName);
    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    pNetworkReply->deleteLater();
}

void MainWindow::updateTicketUI()
{
    for(int i=0; i<m_vecTicketInfo.size(); i++)
    {
        addItemToTicketList(m_vecTicketInfo[i]);
    }
}

void MainWindow::updatePickServiceUI()
{
    for(int i=0; i<m_vecPickServiceInfo.size(); i++)
    {
        addItemToServiceList(m_vecPickServiceInfo[i]);
    }
}

void MainWindow::updateChannelListUI()
{
    qDebug() << "updateChannelListUI";
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


void MainWindow::deleteChannel(const QString & strChannelName)
{
    if (strChannelName.isEmpty())
    {
        QMessageBox::information(NULL, QString("错误"), QString("渠道名称不能为空"));
        return ;
    }
    QByteArray postData;
    postData.append("op=delete&");
    postData.append("channel_name=").append(strChannelName);

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/channel.cgi"));

    QNetworkReply *pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyDeleteChannel()));

}

void MainWindow::deleteChannelRelation(const QString & strChannelName, const QString & strProductId)
{
    if (strChannelName.isEmpty())
    {
        QMessageBox::information(NULL, QString("错误"), QString("渠道名称不能为空"));
        return ;
    }
    if (strProductId.isEmpty())
    {
        QMessageBox::information(NULL, QString("错误"), QString("关联的产品ID不能为空"));
        return ;
    }
    QByteArray postData;
    postData.append("op=delete&");
    postData.append("channel_name=").append(strChannelName);
    postData.append("&product_id=").append(strProductId);

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/channelrelation.cgi"));

    QNetworkReply *pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyDeleteChannelRelation()));
}


void MainWindow::replyDeleteChannel()
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
             QMessageBox::information(NULL, QString("提示"), QString("删除渠道信息成功"));
        }
        else
        {
            jsonValue = jsonObject.value(QString("msg"));
            if (jsonValue.isUndefined())
            {
                  QMessageBox::information(NULL, QString("错误"), QString("删除渠道信息错误!"));
            }
            else
            {
                 QMessageBox::information(NULL, QString("错误"), QString("删除渠道信息错误!").append(jsonValue.toString()));
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

void MainWindow::replyDeleteChannelRelation()
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
             QMessageBox::information(NULL, QString("提示"), QString("删除渠道关联信息成功"));
        }
        else
        {
            jsonValue = jsonObject.value(QString("msg"));
            if (jsonValue.isUndefined())
            {
                  QMessageBox::information(NULL, QString("错误"), QString("删除渠道关联信息错误!"));
            }
            else
            {
                 QMessageBox::information(NULL, QString("错误"), QString("删除渠道关联信息错误!").append(jsonValue.toString()));
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
    updateChannel(tmpChannelInfo);
    updateChannelListUI();
}

void MainWindow::on_pushButton_ChannelDel_clicked()
{
    QString strText = "是否删除" + ui->lineEdit_ChannelName->text() + "渠道信息？";
    QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
    QString strChannelName;
    if(QMessageBox::Yes == reply)
    {
        for(QVector<ChannelInfo>::iterator iter = m_vecChannelInfo.begin(); iter != m_vecChannelInfo.end(); iter++ )
        {
            if(ui->lineEdit_ChannelName->text() == iter->strChannelName)
            {
                strChannelName = iter->strChannelName;
                m_vecChannelInfo.erase(iter);
                break;
            }
        }
    }
    deleteChannel(strChannelName);
    updateChannelListUI();
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

void MainWindow::channelRelationCurrentItemClicked(QTableWidgetItem *tableWidgetItem)
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

    int currentRow = ui->tableWidget_ChannelRelation->row(tableWidgetItem);
    QVector<ChannelRelationInfo> vecChannelRealationInfo = m_mapChannelRelationInfo[strChannelName];
    if(currentRow < vecChannelRealationInfo.size())
    {
        ui->lineEdit_ChannelPickServiceID->setText(vecChannelRealationInfo[currentRow].strPickServiceId);
        ui->lineEdit_ShopProductID->setText(vecChannelRealationInfo[currentRow].strShopProductId);
        ui->lineEdit_ShopProductName->setText(vecChannelRealationInfo[currentRow].strShopProductName);
    }
}

void MainWindow::channelServiceListCurrentItemClicked(QTableWidgetItem *tableWidgetItem)
{
    /*int currentRow = ui->tableWidget_ChannelServiceList->row(tableWidgetItem);
    QString strServiceNo = ui->tableWidget_ChannelServiceList->item(currentRow, 0)->text();
    ui->lineEdit_ChannelPickServiceID->setText(strServiceNo);
    ui->lineEdit_ShopProductID->setText("");
    QString strServiceName = ui->tableWidget_ChannelServiceList->item(currentRow, 1)->text();
    ui->lineEdit_ShopProductName->setText(strServiceName);*/
}

void MainWindow::productListCurrentItemClicked(QTableWidgetItem *tableWidgetItem)
{
    if(m_vecProductInfo.size() == 0)
    {
        return;
    }

    int currentRow = ui->tableWidget_ProductList->row(tableWidgetItem);
    QString strProductNo = ui->tableWidget_ProductList->item(currentRow, 0)->text();
    int i=0;
    for(i=0; i<m_vecProductInfo.size(); i++)
    {
        if(strProductNo == m_vecProductInfo[i].strNo)
        {
            break;
        }
    }

    if(i != m_vecProductInfo.size())
    {
        ui->lineEdit_ProductDeparture->setText(m_vecProductInfo[i].strDeparture);
        ui->lineEdit_ProductDestination->setText(m_vecProductInfo[i].strDestination);
        ui->lineEdit_ProductDays->setText(m_vecProductInfo[i].strDays);
        ui->lineEdit_ProductMissionNo->setText(m_vecProductInfo[i].strMissionNo);
        ui->lineEdit_ProductName->setText(m_vecProductInfo[i].strName);
        ui->lineEdit_ProductNo->setText(m_vecProductInfo[i].strNo);
        ui->lineEdit_ProductServiceNo->setText(m_vecProductInfo[i].strServiceNo);
        ui->lineEdit_ProductTicketNo->setText(m_vecProductInfo[i].strTicketNo);
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

    //addItemToChannelServiceList(m_vecPickServiceInfo);
    addItemToChannelServiceList(m_vecProductInfo);
}

void MainWindow::addItemToChannelServiceList(QVector<PickServiceInfo> &vecPickServiceInfo)
{
    //渠道关联中的地接列表
    disconnect(ui->tableWidget_ChannelServiceList, SIGNAL(itemClicked(QTableWidgetItem *)), 0, 0);
    int row = ui->tableWidget_ChannelServiceList->rowCount();
    for(int i=0; i<row; i++)
    {
        ui->tableWidget_ChannelServiceList->removeRow(0);
    }

    ui->tableWidget_ChannelServiceList->setColumnCount(3);
    ui->tableWidget_ChannelServiceList->setRowCount(m_vecPickServiceInfo.size());
    ui->tableWidget_ChannelServiceList->verticalHeader()->setVisible(false);
    QStringList header;
    header << "地接产品ID" << "地接产品名称" << "团号";
    ui->tableWidget_ChannelServiceList->setHorizontalHeaderLabels(header);
    ui->tableWidget_ChannelServiceList->horizontalHeader()->resizeSections(QHeaderView::Stretch);

    connect(ui->tableWidget_ChannelServiceList, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(channelServiceListCurrentItemClicked(QTableWidgetItem *)));
    for(int i=0; i<vecPickServiceInfo.size(); i++)
    {
        QTableWidgetItem *itemPickServiceID = new QTableWidgetItem(vecPickServiceInfo[i].strNo);
        itemPickServiceID->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelServiceList->setItem(i, 0, itemPickServiceID);

        QTableWidgetItem *itemPickServiceName = new QTableWidgetItem(vecPickServiceInfo[i].strName);
        itemPickServiceName->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelServiceList->setItem(i, 1, itemPickServiceName);

        QTableWidgetItem *itemPickServiceMissionNo = new QTableWidgetItem(vecPickServiceInfo[i].strMissionNo);
        itemPickServiceMissionNo->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelServiceList->setItem(i, 2, itemPickServiceMissionNo);
    }
}

void MainWindow::addItemToChannelServiceList(QVector<ProductInfo> &vecProductInfo)
{
    //渠道关联中的地接列表
    disconnect(ui->tableWidget_ChannelServiceList, SIGNAL(itemClicked(QTableWidgetItem *)), 0, 0);
    int row = ui->tableWidget_ChannelServiceList->rowCount();
    for(int i=0; i<row; i++)
    {
        ui->tableWidget_ChannelServiceList->removeRow(0);
    }

    ui->tableWidget_ChannelServiceList->setColumnCount(5);
    ui->tableWidget_ChannelServiceList->setRowCount(vecProductInfo.size());
    ui->tableWidget_ChannelServiceList->verticalHeader()->setVisible(false);
    QStringList header;
    header << "产品信息编号" << "天数" << "团号" << "机票编号" << "地接编号";
    ui->tableWidget_ChannelServiceList->setHorizontalHeaderLabels(header);
    ui->tableWidget_ChannelServiceList->horizontalHeader()->resizeSections(QHeaderView::Stretch);

    connect(ui->tableWidget_ChannelServiceList, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(channelServiceListCurrentItemClicked(QTableWidgetItem *)));
    for(int i=0; i<vecProductInfo.size(); i++)
    {
        QTableWidgetItem *itemPickServiceID = new QTableWidgetItem(vecProductInfo[i].strNo);
        itemPickServiceID->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelServiceList->setItem(i, 0, itemPickServiceID);

        QTableWidgetItem *itemPickServiceName = new QTableWidgetItem(vecProductInfo[i].strDays);
        itemPickServiceName->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelServiceList->setItem(i, 1, itemPickServiceName);

        QTableWidgetItem *itemPickServiceMissionNo = new QTableWidgetItem(vecProductInfo[i].strMissionNo);
        itemPickServiceMissionNo->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelServiceList->setItem(i, 2, itemPickServiceMissionNo);

        QTableWidgetItem *itemTicketNo = new QTableWidgetItem(vecProductInfo[i].strTicketNo);
        itemTicketNo->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelServiceList->setItem(i, 3, itemTicketNo);

        QTableWidgetItem *itemPickNo = new QTableWidgetItem(vecProductInfo[i].strServiceNo);
        itemPickNo->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ChannelServiceList->setItem(i, 4, itemPickNo);
    }
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
    tmpChannelRelationInfo.strChannelName = strChannelName;
    updateChannelRelation(tmpChannelRelationInfo);
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
    deleteChannelRelation(strChannelName, tmpChannelRelationInfo.strShopProductId);
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

void MainWindow::getPriceInfo4Qunaer(TicketInfo &ticketInfo, PickServiceInfo &pickServiceInfo, ProductInfo &productInfo, QMap<QString, QVector<QunarPriceInfo> > &mapQunarPriceInfo)
{
    QVector<QunarPriceInfo> vecQunerPriceInfo;
    for(QMap<QString, QMap<QString, TicketPriceInfo> >::iterator iterMapTicketPriceInfo = ticketInfo.mapTicketPriceInfo.begin();
        iterMapTicketPriceInfo!=ticketInfo.mapTicketPriceInfo.end(); iterMapTicketPriceInfo++)
    {
        QString strYearMonth = iterMapTicketPriceInfo.key();
        if(strYearMonth.size() >= 4)
        {
            int nYear = strYearMonth.left(4).toInt();
            QString strYearMonthBak = strYearMonth;
            int nMonth = strYearMonthBak.remove(0, 4).toInt();
            if(nYear>2015 && nYear<=2020 && nMonth>0 && nMonth<=12)
            {
                QMap<QString, TicketPriceInfo> &tmpTicketPriceInfo = iterMapTicketPriceInfo.value();
                for(QMap<QString, TicketPriceInfo>::iterator iterTmpTicketPriceInfo=tmpTicketPriceInfo.begin();
                    iterTmpTicketPriceInfo!=tmpTicketPriceInfo.end(); iterTmpTicketPriceInfo++)
                {
                    if(iterTmpTicketPriceInfo.value().bUpdate == true)
                    {
                        QString strDay = iterTmpTicketPriceInfo.key();
                        int nDay = strDay.toInt();
                        if(nDay>0 && nDay <=31)
                        {
                            QDate tmpDay = QDate(nYear, nMonth, nDay);
                            QunarPriceInfo tmpQunarPriceInfo;
                            tmpQunarPriceInfo.dateString = tmpDay.toString("yyyy-MM-dd");
                            tmpQunarPriceInfo.adult_price = QString::number(ticketInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketAdultPrice + pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketAdultPrice);
                            tmpQunarPriceInfo.child_price = QString::number(ticketInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketChildPrice + pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketChildPrice);
                            tmpQunarPriceInfo.count = QString::number(pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketStock);
                            tmpQunarPriceInfo.market_price = QString::number(pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketRetailPrice);
                            tmpQunarPriceInfo.max_buy_count = QString::number(pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nMaxPerOrder);
                            tmpQunarPriceInfo.min_buy_count = QString::number(pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nMinPerOrder);
                            tmpQunarPriceInfo.room_send_price = QString::number(pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nSigleRoomSpread);
                            vecQunerPriceInfo.push_back(tmpQunarPriceInfo);
                        }
                    }
                }
            }
        }
    }

    for(QMap<QString, QMap<QString, TicketPriceInfo> >::iterator iterMapPickServicePriceInfo = pickServiceInfo.mapTicketPriceInfo.begin();
        iterMapPickServicePriceInfo!=pickServiceInfo.mapTicketPriceInfo.end(); iterMapPickServicePriceInfo++)
    {
        QString strYearMonth = iterMapPickServicePriceInfo.key();
        if(strYearMonth.size() >= 4)
        {
            int nYear = strYearMonth.left(4).toInt();
            QString strTmpMonth = strYearMonth;
            int nMonth = strTmpMonth.remove(0, 4).toInt();
            if(nYear>2015 && nYear<=2020 && nMonth>0 && nMonth<=12)
            {
                QMap<QString, TicketPriceInfo> &tmpPickServicePriceInfo = iterMapPickServicePriceInfo.value();
                for(QMap<QString, TicketPriceInfo>::iterator iterTmpPickServicePriceInfo=tmpPickServicePriceInfo.begin();
                    iterTmpPickServicePriceInfo!=tmpPickServicePriceInfo.end(); iterTmpPickServicePriceInfo++)
                {
                    if(iterTmpPickServicePriceInfo.value().bUpdate == true)
                    {
                        QString strDay = iterTmpPickServicePriceInfo.key();
                        int nDay = strDay.toInt();
                        if(nDay>0 && nDay <=31)
                        {
                            QDate tmpDay = QDate(nYear, nMonth, nDay);
                            QunarPriceInfo tmpQunarPriceInfo;
                            tmpQunarPriceInfo.dateString = tmpDay.toString("yyyy-MM-dd");
                            tmpQunarPriceInfo.adult_price = QString::number(ticketInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketAdultPrice + pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketAdultPrice);
                            tmpQunarPriceInfo.child_price = QString::number(ticketInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketChildPrice + pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketChildPrice);
                            tmpQunarPriceInfo.count = QString::number(pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketStock);
                            tmpQunarPriceInfo.market_price = QString::number(pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nTicketRetailPrice);
                            tmpQunarPriceInfo.max_buy_count = QString::number(pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nMaxPerOrder);
                            tmpQunarPriceInfo.min_buy_count = QString::number(pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nMinPerOrder);
                            tmpQunarPriceInfo.room_send_price = QString::number(pickServiceInfo.mapTicketPriceInfo[strYearMonth][strDay].nSigleRoomSpread);
                            vecQunerPriceInfo.push_back(tmpQunarPriceInfo);
                        }
                    }
                }
            }
        }
    }

    QMap<QString, QVector<ChannelRelationInfo> > mapChannelRelationInfoTmp;
    for(QMap<QString, QVector<ChannelRelationInfo> >::iterator iterMapChannelRelationInfo = m_mapChannelRelationInfo.begin();
        iterMapChannelRelationInfo!=m_mapChannelRelationInfo.end(); iterMapChannelRelationInfo++)
    {
        QVector<ChannelRelationInfo> vecTmpChannelRelationInfo;
        for(QVector<ChannelRelationInfo>::iterator iterVecChannelRelationInfo = iterMapChannelRelationInfo.value().begin();
            iterVecChannelRelationInfo != iterMapChannelRelationInfo.value().end(); iterVecChannelRelationInfo++)
        {
            if(iterVecChannelRelationInfo->strPickServiceId.simplified() == productInfo.strNo.simplified())
            {
                vecTmpChannelRelationInfo.push_back(*iterVecChannelRelationInfo);
            }
        }
        if(vecTmpChannelRelationInfo.size() != 0)
        {
            mapChannelRelationInfoTmp[iterMapChannelRelationInfo.key()] = vecTmpChannelRelationInfo;
        }
    }

    for(QMap<QString, QVector<ChannelRelationInfo> >::iterator iterTmp = mapChannelRelationInfoTmp.begin();
        iterTmp!=mapChannelRelationInfoTmp.end(); iterTmp++)
    {
        for(int i=0; i<m_vecChannelInfo.size(); i++)
        {
            //找到渠道名，找到HTTP对象进行登录
            if(m_vecChannelInfo[i].strChannelName == iterTmp.key())
            {
                /*for(int indexHttp=0; indexHttp!=m_vecQunerHttPtr.size(); indexHttp++)
                {
                    if(m_vecQunerHttPtr[indexHttp]->GetUserName() == m_vecChannelInfo[i].strAccount
                    && m_vecQunerHttPtr[indexHttp]->GetChannelName() == m_vecChannelInfo[i].strChannelName)
                    {*/
                        QVector<QunarPriceInfo> vecUpdatePriceInfo;
                        QVector<QunarPriceInfo> vecQunerPriceInfoTmp = vecQunerPriceInfo;
                        QVector<ChannelRelationInfo> vecTmpChannelRelationInfo = iterTmp.value();
                        for(int i=0; i<vecTmpChannelRelationInfo.size(); i++)
                        {
                            for(int index=0; index<vecQunerPriceInfoTmp.size(); index++)
                            {
                                vecQunerPriceInfoTmp[index].pId = vecTmpChannelRelationInfo[i].strShopProductId;
                                vecUpdatePriceInfo.push_back(vecQunerPriceInfoTmp[index]);
                            }
                        }
                        mapQunarPriceInfo[iterTmp.key()] = vecUpdatePriceInfo;
                        /*if(vecUpdatePriceInfo.size() != 0)
                        {
                            m_vecQunerHttPtr[indexHttp]->setQunarPrice4Update(vecUpdatePriceInfo);
                            qDebug() << "帐号：" << m_vecQunerHttPtr[indexHttp]->GetUserName()
                                     << " 密码：" << m_vecQunerHttPtr[indexHttp]->GetChannelName() << endl;
                            m_vecQunerHttPtr[indexHttp]->login();
                        }
                    }
                }*/
            }
        }
    }
}

void MainWindow::update2Qunaer(QMap<QString, QVector<QunarPriceInfo> > mapQunarPriceInfo)
{
    for(QMap<QString, QVector<QunarPriceInfo> >::iterator iter = mapQunarPriceInfo.begin();
        iter != mapQunarPriceInfo.end(); iter++)
    {
        for(int i=0; i<m_vecChannelInfo.size(); i++)
        {
            //找到渠道名，找到HTTP对象进行登录
            if(m_vecChannelInfo[i].strChannelName == iter.key())
            {
                for(int indexHttp=0; indexHttp!=m_vecQunerHttPtr.size(); indexHttp++)
                {
                    if(m_vecQunerHttPtr[indexHttp]->GetUserName() == m_vecChannelInfo[i].strAccount
                    && m_vecQunerHttPtr[indexHttp]->GetChannelName() == m_vecChannelInfo[i].strChannelName)
                    {
                        QVector<QunarPriceInfo> &vecUpdatePriceInfo = iter.value();
                        if(vecUpdatePriceInfo.size() != 0)
                        {
                            m_vecQunerHttPtr[indexHttp]->setQunarPrice4Update(vecUpdatePriceInfo);
                            qDebug() << "帐号：" << m_vecQunerHttPtr[indexHttp]->GetUserName()
                                     << " 密码：" << m_vecQunerHttPtr[indexHttp]->GetChannelName() << endl;
                            m_vecQunerHttPtr[indexHttp]->login();
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::saveSignalsSlot(PriceEditorMode eMode)
{
    if(PRICE_EDITOR_TICKET_MODE == eMode)
    {
        on_pushButtonUpdate_clicked();
    }
    else if(PRICE_EDITOR_PICK_SERVICE == eMode)
    {
        on_pushButton_ServiceUpdate_clicked();
    }
}

void MainWindow::fillProductInfo(ProductInfo &productInfo)
{
    productInfo.strDeparture = ui->lineEdit_ProductDeparture->text();
    productInfo.strDestination = ui->lineEdit_ProductDestination->text();
    productInfo.strDays = ui->lineEdit_ProductDays->text();
    productInfo.strMissionNo = ui->lineEdit_ProductMissionNo->text();
    productInfo.strName = ui->lineEdit_ProductName->text();
    productInfo.strNo = ui->lineEdit_ProductNo->text();
    productInfo.strTicketNo = ui->lineEdit_ProductTicketNo->text();
    productInfo.strServiceNo = ui->lineEdit_ProductServiceNo->text();
}

void MainWindow::on_pushButton_ProductUpdate_clicked()
{
    qDebug() << "on_pushButton_ProductUpdate_clicked";
    ProductInfo tmpProductInfo;
    fillProductInfo(tmpProductInfo);
    if(/*NULL==tmpProductInfo.strDeparture
      || NULL == tmpProductInfo.strDestination
      || NULL == tmpProductInfo.strDays
      ||*/ NULL == tmpProductInfo.strName
      || NULL == tmpProductInfo.strMissionNo
      || NULL == tmpProductInfo.strServiceNo
      || NULL == tmpProductInfo.strTicketNo)
    {
        QMessageBox::information(NULL, QString("提醒"), QString("请输入完整的产品信息!"));
        return;
    }

    /*if(!hasSameTicketNo(tmpProductInfo.strTicketNo))
    {
        QMessageBox::information(NULL, QString("提醒"), QString("请输入正确的机票编号!"));
        return;
    }

    if(!hasSameServiceNo(tmpProductInfo.strServiceNo))
    {
        QMessageBox::information(NULL, QString("提醒"), QString("请输入正确的地接编号!"));
        return;
    }*/

    bool bFind = false;
    int i=0;
    for(; i<m_vecProductInfo.size(); i++)
    {
        if(tmpProductInfo.strName == m_vecProductInfo[i].strName)
        {
            bFind = true;
            break;
        }
    }

    if(true == bFind)
    {
        QString strText = "是否修改" + tmpProductInfo.strName + "产品信息？";
        QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::Yes == reply)
        {
             m_vecProductInfo[i] = tmpProductInfo;
        }
        else
        {
            return;
        }
    }
    else
    {
        QString strText = "是否添加" + tmpProductInfo.strName + "产品信息？";
        QMessageBox::StandardButton reply = QMessageBox::question(this, "提醒", strText, QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::Yes == reply)
        {
            tmpProductInfo.strNo =  QString::number(QDateTime::currentDateTime().toTime_t(), 10);
            qDebug() << tmpProductInfo.strNo;
            if(hasSameProductNo(tmpProductInfo.strNo))
            {
                QMessageBox::information(NULL, QString("提醒"), QString("请不要在一秒中生成多个产品信息!"));
                return;
            }
            ui->lineEdit_ProductNo->setText(tmpProductInfo.strNo);
            m_vecProductInfo.push_back(tmpProductInfo);
            addItemToProductList(tmpProductInfo);
        }
        else
        {
            return;
        }
    }

    //向后台更新产品信息
    updateProduct(tmpProductInfo);

}

void MainWindow::on_pushButton_ProductCancel_clicked()
{
    qDebug() << "on_pushButton_ProductCancel_clicked";
    ui->lineEdit_ProductDeparture->setText("");
    ui->lineEdit_ProductDestination->setText("");
    ui->lineEdit_ProductDays->setText("");
    ui->lineEdit_ProductMissionNo->setText("");
    ui->lineEdit_ProductName->setText("");
    ui->lineEdit_ProductNo->setText("");
    ui->lineEdit_ProductServiceNo->setText("");
    ui->lineEdit_ProductTicketNo->setText("");
}

void MainWindow::addItemToProductList(ProductInfo &productInfo)
{
    ui->tableWidget_ProductList->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    int tmpRowCount = ui->tableWidget_ProductList->rowCount();
    ui->tableWidget_ProductList->setRowCount(tmpRowCount+1);

    QTableWidgetItem *itemProductID = new QTableWidgetItem(productInfo.strNo);
    itemProductID->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidget_ProductList->setItem(tmpRowCount, 0, itemProductID);

    QTableWidgetItem *itemProductName = new QTableWidgetItem(productInfo.strName);
    itemProductName->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidget_ProductList->setItem(tmpRowCount, 1, itemProductName);

    QTableWidgetItem *itemProductTicketNo = new QTableWidgetItem(productInfo.strTicketNo);
    itemProductTicketNo->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidget_ProductList->setItem(tmpRowCount, 2, itemProductTicketNo);

    QTableWidgetItem *itemProductServiceNo = new QTableWidgetItem(productInfo.strServiceNo);
    itemProductServiceNo->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidget_ProductList->setItem(tmpRowCount, 3, itemProductServiceNo);

    QTableWidgetItem *itemProductMissionNo = new QTableWidgetItem(productInfo.strMissionNo);
    itemProductMissionNo->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidget_ProductList->setItem(tmpRowCount, 4, itemProductMissionNo);
}

void MainWindow::updateProductList(QVector<ProductInfo> &vecProductInfo)
{
    qDebug() << ui->tableWidget_ProductList;
    int row = ui->tableWidget_ProductList->rowCount();
    for(int i=0; i<row; i++)
    {
        ui->tableWidget_ProductList->removeRow(0);
    }

    ui->tableWidget_ProductList->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    ui->tableWidget_ProductList->setColumnCount(5);
    ui->tableWidget_ProductList->setRowCount(vecProductInfo.size());
    ui->tableWidget_ProductList->verticalHeader()->setVisible(false);
    QStringList header;
    header << "产品编号" << "产品名称" << "关联机票编号" << "关联地接编号" << "团号";
    ui->tableWidget_ProductList->setHorizontalHeaderLabels(header);


    for(int i=0; i<vecProductInfo.size(); i++)
    {
        QTableWidgetItem *itemProductID = new QTableWidgetItem(vecProductInfo[i].strNo);
        itemProductID->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ProductList->setItem(i, 0, itemProductID);

        QTableWidgetItem *itemProductName = new QTableWidgetItem(vecProductInfo[i].strName);
        itemProductName->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ProductList->setItem(i, 1, itemProductName);

        QTableWidgetItem *itemProductTicketNo = new QTableWidgetItem(vecProductInfo[i].strTicketNo);
        itemProductTicketNo->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ProductList->setItem(i, 2, itemProductTicketNo);

        QTableWidgetItem *itemProductServiceNo = new QTableWidgetItem(vecProductInfo[i].strServiceNo);
        itemProductServiceNo->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ProductList->setItem(i, 3, itemProductServiceNo);

        QTableWidgetItem *itemProductMissionNo = new QTableWidgetItem(vecProductInfo[i].strMissionNo);
        itemProductMissionNo->setTextAlignment(Qt::AlignHCenter);
        ui->tableWidget_ProductList->setItem(i, 4, itemProductMissionNo);
    }
}

void MainWindow::loadProduct()
{
    QByteArray postData;
    postData.append("op=load&page=").append(QByteArray::number(m_iLoadPage));

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/product.cgi"));

    QNetworkReply *pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyFinishedForLoadProduct()));
}

void MainWindow::replyFinishedForLoadProduct()
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
            QString sProduct = (*it).toObject()["product_info"].toString();
            QJsonParseError jsonParseErr;
            QJsonDocument docTicket = QJsonDocument::fromJson(sProduct.toUtf8(), &jsonParseErr);
            QJsonObject jsonObject = docTicket.object();
            ProductInfo productInfo;
            productInfo.readFrom(jsonObject);
            m_vecProductInfo.push_back(productInfo);
        }

        m_iLoadPage++;
        if (jsonArray.size() >= ITEM_NUM_OF_PAGE)
        {
             loadProduct();
        }
    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }
    qDebug() << "m_vecProductInfo.szie():" << m_vecProductInfo.size();
    //后面放到load之后
    updateProductList(m_vecProductInfo);



    pNetworkReply->deleteLater();
}


void MainWindow::updateProduct(const ProductInfo & productInfo)
{
    if (productInfo.strNo.isEmpty())
    {
        QMessageBox::information(NULL, QString("错误"), QString("产品ID不能为空"));
        return ;
    }
    QJsonObject jsonObject;
    productInfo.writeTo(jsonObject);
    QJsonDocument jsonDocument(jsonObject);
    QByteArray postData;
    postData.append("op=update&");
    postData.append("product_no=").append(productInfo.strNo);
    postData.append("&product_info=").append(jsonDocument.toJson());

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl(SERVER_DOMAIN + "/product.cgi"));

    QNetworkReply* pNetworkReply = m_pAssitNetworkManager->post(networkRequest, postData);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyFinishedForProduct()));
}

void MainWindow::replyFinishedForProduct()
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
             QMessageBox::information(NULL, QString("提示"), QString("更新产品信息成功"));
        }
        else
        {
            jsonValue = jsonObject.value(QString("msg"));
            if (jsonValue.isUndefined())
            {
                  QMessageBox::information(NULL, QString("错误"), QString("更新产品信息错误!"));
            }
            else
            {
                 QMessageBox::information(NULL, QString("错误"), QString("更新产品信息错误!").append(jsonValue.toString()));
            }
        }

    }
    else
    {
        //处理错误
        QMessageBox::information(NULL, QString("错误"), status.toString());
    }

}
