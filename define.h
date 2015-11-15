#ifndef DEFINE_H
#define DEFINE_H
#include <QString>
#include <QMap>

//调试时使用
const QString QSS_CONFIG_FILE_PATH = "../smart/stylesheet.txt";
const QString PICTURE_FILE_PATH = "../smart/picture.png";


//编译发布时使用
//const QString QSS_CONFIG_FILE_PATH = "stylesheet.txt";
//const QString PICTURE_FILE_PATH = "picture.png";

//票价信息
//1机票票价：nTicketAdultPrice（成人票价），nTicketChildPrice（儿童票价）
//2地接服务票价：nTicketAdultPrice（成人票价），nTicketChildPrice（儿童票价）,nSigleRoomSpread（单房差）
typedef struct TicketPriceInfo
{
    int nTicketAdultPrice;               //成人票价
    int nTicketChildPrice;               //儿童票价
    int nSigleRoomSpread;                    //单房差价
}TicketPriceInfoStruct;

//机票产品信息
typedef struct TicketInfo
{
    QString strTicketDeparture;                      //机票出发地
    QString strTicketDestination;                    //机票目的地
    QString strTicketDays;                           //机票往返天数
    QString strTicketNo;                             //机票产品编号
    QString strTicketName;                           //机票代码，名称
    //QVector<TicketPriceInfo> vecTicketPriceInfo;     //该机票的票价信息
    QMap<QString, QMap<QString, TicketPriceInfo> > mapTicketPriceInfo; //该机票的票价信息;第一个key为年月,第二个key为日
}TicketInfoStruct;

//地接模块

//地接产品信息
typedef struct PickServiceInfo
{
    QString strDeparture;                      //地接服务出发地
    QString strDestination;                    //地接服务目的地
    QString strDays;                           //地接服务往返天数
    QString strNo;                             //地接服务产品编号（系统生成）
    QString strName;                           //地接服务名称
    QString strMissionNo;                      //地接服务团号
    QString strTicketNo;                       //地接服务关联的机票产品编号
    QMap<QString, QMap<QString, TicketPriceInfo> > mapTicketPriceInfo; //该机票的票价信息;第一个key为年月,第二个key为日
}PickServiceInfoStruct;

#endif // DEFINE_H
