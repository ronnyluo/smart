#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QListWidget>
#include "priceeditor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    resize(QSize(900, 600));
    ui->listWidget_Ticket->setAlternatingRowColors(true);
    ui->lineEdit_Departure->installEventFilter(this);
    ui->lineEdit_Destination->installEventFilter(this);
    ui->lineEdit_Day->installEventFilter(this);
    ui->lineEdit_Day->setValidator(new QIntValidator(1, 1000, ui->lineEdit_Day));
    ui->lineEdit_No->setReadOnly(true);
    ui->lineEdit_Name->setReadOnly(true);

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

    //显示首页图片
    QImage *image=new QImage(PICTURE_FILE_PATH);
    ui->label_Picture->setPixmap(QPixmap::fromImage(*image));

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
