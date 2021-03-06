#ifndef DEFINE_H
#define DEFINE_H
#include <QString>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextCodec>
#include <QtGlobal>
#include <QVector>

//调试时使用
//const QString QSS_CONFIG_FILE_PATH = "../smart/stylesheet.txt";
//const QString PICTURE_FILE_PATH = "../smart/picture.png";

//编译发布时使用
const QString QSS_CONFIG_FILE_PATH = "stylesheet.txt";
const QString PICTURE_FILE_PATH = "picture.png";

//const QString SERVER_DOMAIN = "http://58.251.206.224:3000";
//const QString SERVER_DOMAIN = "http://nicecook.jios.org:10001";
//const QString SERVER_DOMAIN = "http://120.25.217.221:8080";
//const QString COMPANY_NAME = "测试客户端";

//南京远界国际旅行社有限公司
const QString SERVER_DOMAIN = "http://121.42.158.27";
const QString COMPANY_NAME = "南京远界国际旅行社有限公司";


#define ITEM_NUM_OF_PAGE 50

//票价信息
//1机票票价：nTicketAdultPrice（成人票价），nTicketChildPrice（儿童票价）
//2地接服务票价：nTicketAdultPrice（成人票价），nTicketChildPrice（儿童票价）,nSigleRoomSpread（单房差）
typedef struct TicketPriceInfo
{
    int nTicketAdultPrice;               //成人票价
    int nTicketChildPrice;               //儿童票价
    int nSigleRoomSpread;                //单房差价
    int nTicketRetailPrice;              //门市价
    int nTicketStock;                    //库存
    int nMinPerOrder;                    //每单最小
    int nMaxPerOrder;                    //每单最多
    bool bUpdate;                        //是否需要向去哪儿网更新
    QJsonObject& writeTo(QJsonObject & jsonObject)const
    {
        jsonObject.insert(QString("TicketAdultPrice"), QJsonValue(nTicketAdultPrice));
        jsonObject.insert(QString("TicketChildPrice"), QJsonValue(nTicketChildPrice));
        jsonObject.insert(QString("SigleRoomSpread"), QJsonValue(nSigleRoomSpread));
        jsonObject.insert(QString("TicketRetailPrice"), QJsonValue(nTicketRetailPrice));
        jsonObject.insert(QString("TicketStock"), QJsonValue(nTicketStock));
        jsonObject.insert(QString("MinPerOrder"), QJsonValue(nMinPerOrder));
        jsonObject.insert(QString("MaxPerOrder"), QJsonValue(nMaxPerOrder));
        return jsonObject;
    }
    void readFrom(QJsonObject & jsonObject)
    {
        QJsonValue jsonValue = jsonObject.value(QString("TicketAdultPrice"));
        if (!jsonValue.isUndefined())
        {
            nTicketAdultPrice = jsonValue.toDouble();
        }
        jsonValue = jsonObject.value(QString("TicketChildPrice"));
        if (!jsonValue.isUndefined())
        {
            nTicketChildPrice = jsonValue.toDouble();
        }
        jsonValue = jsonObject.value(QString("SigleRoomSpread"));
        if (!jsonValue.isUndefined())
        {
            nSigleRoomSpread = jsonValue.toDouble();
        }
        jsonValue = jsonObject.value(QString("TicketRetailPrice"));
        if (!jsonValue.isUndefined())
        {
            nTicketRetailPrice = jsonValue.toDouble();
        }
        jsonValue = jsonObject.value(QString("TicketStock"));
        if (!jsonValue.isUndefined())
        {
            nTicketStock = jsonValue.toDouble();
        }
        jsonValue = jsonObject.value(QString("MinPerOrder"));
        if (!jsonValue.isUndefined())
        {
            nMinPerOrder = jsonValue.toDouble();
        }
        jsonValue = jsonObject.value(QString("MaxPerOrder"));
        if (!jsonValue.isUndefined())
        {
            nMaxPerOrder = jsonValue.toDouble();
        }
        return ;
    }
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
    QJsonObject& writeTo(QJsonObject & jsonObject)const
    {
        jsonObject.insert(QString("TicketDeparture"), QJsonValue(strTicketDeparture));
        jsonObject.insert(QString("TicketDestination"), QJsonValue(strTicketDestination));
        jsonObject.insert(QString("TicketDays"), QJsonValue(strTicketDays));
        jsonObject.insert(QString("TicketNo"), QJsonValue(strTicketNo));
        jsonObject.insert(QString("TicketName"), QJsonValue(strTicketName));
        QJsonObject joMap;
        for (QMap<QString, QMap<QString, TicketPriceInfo> >::ConstIterator it = mapTicketPriceInfo.begin();
             it != mapTicketPriceInfo.end(); ++it)
        {
            QJsonObject joOuter;
            for (QMap<QString, TicketPriceInfo>::ConstIterator itInner = it.value().begin();
                 itInner != it.value().end(); ++itInner)
            {
                QJsonObject joInner;
                joOuter.insert(itInner.key(), itInner->writeTo(joInner));
            }
            joMap.insert(it.key(), joOuter);
        }
        jsonObject.insert("TicketPriceInfo", joMap);
        return jsonObject;
    }
    void readFrom(QJsonObject & jsonObject)
    {
        QJsonValue jsonValue = jsonObject.value(QString("TicketDeparture"));
        if (!jsonValue.isUndefined())
        {
            strTicketDeparture = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("TicketDestination"));
        if (!jsonValue.isUndefined())
        {
            strTicketDestination = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("TicketDays"));
        if (!jsonValue.isUndefined())
        {
            strTicketDays = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("TicketNo"));
        if (!jsonValue.isUndefined())
        {
            strTicketNo = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("TicketName"));
        if (!jsonValue.isUndefined())
        {
            strTicketName = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("TicketPriceInfo"));
        if (jsonValue.isUndefined())
        {
            return;
        }
        QJsonObject joTpDmap = jsonValue.toObject();
        for (QJsonObject::Iterator it = joTpDmap.begin(); it != joTpDmap.end(); ++it)
        {
            QJsonObject joTpmap = it.value().toObject();
            for (QJsonObject::Iterator itInner = joTpmap.begin(); itInner != joTpmap.end(); ++itInner)
            {
                QJsonObject joPt = itInner.value().toObject();
                mapTicketPriceInfo[it.key()][itInner.key()].readFrom(joPt);
            }
        }
    }
}TicketInfoStruct;

//地接模块

//地接产品信息，团号和机票产品编号废弃，有产品信息去关联
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
    QJsonObject& writeTo(QJsonObject & jsonObject)const
    {
        jsonObject.insert(QString("Departure"), QJsonValue(strDeparture));
        jsonObject.insert(QString("Destination"), QJsonValue(strDestination));
        jsonObject.insert(QString("Days"), QJsonValue(strDays));
        jsonObject.insert(QString("No"), QJsonValue(strNo));
        jsonObject.insert(QString("Name"), QJsonValue(strName));
        jsonObject.insert(QString("MissionNo"), QJsonValue(strMissionNo));
        jsonObject.insert(QString("TicketNo"), QJsonValue(strTicketNo));
        QJsonObject joMap;
        for (QMap<QString, QMap<QString, TicketPriceInfo> >::ConstIterator it = mapTicketPriceInfo.begin();
             it != mapTicketPriceInfo.end(); ++it)
        {
            QJsonObject joOuter;
            for (QMap<QString, TicketPriceInfo>::ConstIterator itInner = it.value().begin();
                 itInner != it.value().end(); ++itInner)
            {
                QJsonObject joInner;
                joOuter.insert(itInner.key(), itInner.value().writeTo(joInner));
            }
            joMap.insert(it.key(), joOuter);
        }
        jsonObject.insert("TicketPriceInfo", joMap);
        return jsonObject;
    }
    void readFrom(QJsonObject & jsonObject)
    {
        QJsonValue jsonValue = jsonObject.value(QString("Departure"));
        if (!jsonValue.isUndefined())
        {
            strDeparture = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("Destination"));
        if (!jsonValue.isUndefined())
        {
            strDestination = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("Days"));
        if (!jsonValue.isUndefined())
        {
            strDays = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("No"));
        if (!jsonValue.isUndefined())
        {
            strNo = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("Name"));
        if (!jsonValue.isUndefined())
        {
            strName = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("MissionNo"));
        if (!jsonValue.isUndefined())
        {
            strMissionNo = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("TicketNo"));
        if (!jsonValue.isUndefined())
        {
            strTicketNo = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("TicketPriceInfo"));
        if (jsonValue.isUndefined())
        {
            return;
        }
        QJsonObject joTpDmap = jsonValue.toObject();
        for (QJsonObject::Iterator it = joTpDmap.begin(); it != joTpDmap.end(); ++it)
        {
            QJsonObject joTpmap = it.value().toObject();
            for (QJsonObject::Iterator itInner = joTpmap.begin(); itInner != joTpmap.end(); ++itInner)
            {
                QJsonObject joTp = itInner.value().toObject();
                mapTicketPriceInfo[it.key()][itInner.key()].readFrom(joTp);
            }
        }
    }
}PickServiceInfoStruct;

//产品信息:主要用于关联机票信息，地接信息，团号，完整路线信息
typedef struct ProductInfo
{
    QString strDeparture;                      //产品出发地
    QString strDestination;                    //产品目的地
    QString strDays;                           //产品往返天数
    QString strNo;                             //产品编号（系统生成）
    QString strName;                           //产品名称
    QString strMissionNo;                      //产品服务团号
    QString strTicketNo;                       //产品关联的机票编号
    QString strServiceNo;                      //产品关联的地接编号
    QJsonObject& writeTo(QJsonObject & jsonObject)const
    {
        jsonObject.insert(QString("Departure"), QJsonValue(strDeparture));
        jsonObject.insert(QString("Destination"), QJsonValue(strDestination));
        jsonObject.insert(QString("Days"), QJsonValue(strDays));
        jsonObject.insert(QString("No"), QJsonValue(strNo));
        jsonObject.insert(QString("Name"), QJsonValue(strName));
        jsonObject.insert(QString("MissionNo"), QJsonValue(strMissionNo));
        jsonObject.insert(QString("TicketNo"), QJsonValue(strTicketNo));
        jsonObject.insert(QString("ServiceNo"), QJsonValue(strServiceNo));
        return jsonObject;
    }
    void readFrom(QJsonObject & jsonObject)
    {
        QJsonValue jsonValue = jsonObject.value(QString("Departure"));
        if (!jsonValue.isUndefined())
        {
            strDeparture = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("Destination"));
        if (!jsonValue.isUndefined())
        {
            strDestination = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("Days"));
        if (!jsonValue.isUndefined())
        {
            strDays = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("No"));
        if (!jsonValue.isUndefined())
        {
            strNo = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("Name"));
        if (!jsonValue.isUndefined())
        {
            strName = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("MissionNo"));
        if (!jsonValue.isUndefined())
        {
            strMissionNo = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("TicketNo"));
        if (!jsonValue.isUndefined())
        {
            strTicketNo = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("ServiceNo"));
        if (!jsonValue.isUndefined())
        {
            strServiceNo = jsonValue.toString();
        }
    }
}ProductInfoStruct;

//渠道管理
typedef struct ChannelInfo
{
    QString strChannelName;      //渠道名称
    QString strAccount;          //该渠道关联店铺账号
    QString strPassword;         //该渠道关联店铺账号密码
    QJsonObject& writeTo(QJsonObject & jsonObject)const
    {
        jsonObject.insert(QString("ChannelName"), QJsonValue(strChannelName));
        jsonObject.insert(QString("Account"), QJsonValue(strAccount));
        jsonObject.insert(QString("Password"), QJsonValue(strPassword));
        return jsonObject;
    }
    void readFrom(QJsonObject & jsonObject)
    {
        QJsonValue jsonValue = jsonObject.value(QString("ChannelName"));
        if (!jsonValue.isUndefined())
        {
            strChannelName = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("Account"));
        if (!jsonValue.isUndefined())
        {
            strAccount = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("Password"));
        if (!jsonValue.isUndefined())
        {
            strPassword = jsonValue.toString();
        }
        return ;
    }
}ChannelInfoStruct;

//渠道关联信息
typedef struct ChannelRelationInfo
{
    QString strPickServiceId;    //地接产品ID
    QString strShopProductId;    //该渠道关联店铺里产品ID
    QString strShopProductName;  //该渠道关联店铺里产品名称
    QString strChannelName;      //渠道名称

    QJsonObject& writeTo(QJsonObject & jsonObject)const
    {
        jsonObject.insert(QString("PickServiceId"), QJsonValue(strPickServiceId));
        jsonObject.insert(QString("ShopProductId"), QJsonValue(strShopProductId));
        jsonObject.insert(QString("ShopProductName"), QJsonValue(strShopProductName));
        jsonObject.insert(QString("ChannelName"), QJsonValue(strChannelName));
        return jsonObject;
    }
    void readFrom(QJsonObject & jsonObject)
    {
        QJsonValue jsonValue = jsonObject.value(QString("PickServiceId"));
        if (!jsonValue.isUndefined())
        {
            strPickServiceId = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("ShopProductId"));
        if (!jsonValue.isUndefined())
        {
            strShopProductId = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("ShopProductName"));
        if (!jsonValue.isUndefined())
        {
            strShopProductName = jsonValue.toString();
        }
        jsonValue = jsonObject.value(QString("ChannelName"));
        if (!jsonValue.isUndefined())
        {
            strChannelName = jsonValue.toString();
        }
        return ;
    }

}ChannelRelationInfoStruct;

//价格更新
typedef struct tagQunarPriceInfo
{		/*
        market_price:6520
        adult_price:4260
        room_send_price:1860
        room_type:2
        count:20
        min_buy_count:1
        max_buy_count:10
        profit:0
        price_desc:
        dateString:2015-12-27
        pId:2533826371
        is_child_price:on
        child_price:2700
        child_price_desc:
        is_taocan:off
        */
    QString market_price; //门市价
    QString adult_price;  //成人价
    QString room_send_price; //单房差
    QString count; //库存
    QString dateString; //日期
    QString pId; //产品id
    QString child_price; //儿童价
    QString min_buy_count; //每单至少人数
    QString max_buy_count; //每单至多人数

    QString toPostForm() const
    {
        return "market_price=" + market_price + "&adult_price=" +
                adult_price + "&room_send_price=" + room_send_price +
                "&count=" + count + "&dateString=" + dateString +
                "&pId=" + pId + "&child_price=" + child_price +
                "&min_buy_count=" + min_buy_count + "&max_buy_count=" +
                max_buy_count;
    }
}QunarPriceInfo;

typedef struct tagQunarPriceInfoBatch
{		/*
        market_price:6520
        adult_price:4260
        room_send_price:1860
        room_type:2
        count:20
        min_buy_count:1
        max_buy_count:10
        profit:0
        price_desc:
        dateString:2015-12-27
        pId:2533826371
        is_child_price:on
        child_price:2700
        child_price_desc:
        is_taocan:off
        */
    QString market_price; //门市价
    QString adult_price;  //成人价
    QString room_send_price; //单房差
    QString count; //库存
    QVector<QString> vecDateString; //日期
    QString pId; //产品id
    QString child_price; //儿童价
    QString min_buy_count; //每单至少人数
    QString max_buy_count; //每单至多人数

    QString toPostForm()
    {
        QString dateString;
        for (int i = 0; i < vecDateString.size(); i++)
        {
            if (dateString.isEmpty())
            {
                dateString.append(vecDateString[i]);
            }
            else
            {

                dateString.append(",").append(vecDateString[i]);
            }
        }
        return "market_price=" + market_price + "&adult_price=" +
                adult_price + "&room_send_price=" + room_send_price +
                "&count=" + count + "&dateString=" + dateString +
                "&pId=" + pId + "&child_price=" + child_price +
                "&min_buy_count=" + min_buy_count + "&max_buy_count=" +
                max_buy_count;
    }
}QunarPriceInfoBatch;

QString getRandomDigit(int iNum);
QString getMidStr(QString & origin, const QString & first, const QString & last);
QString getLeftStr(QString & origin, const QString & start);
QString getRightStr(QString & origin, const QString & start);

const QString cs2 = "window={ \n\
                  navigator:{plugins:[0],userAgent:\"Mozilla/5.0 (Windows NT 6.1; Trident/7.0; SLCC2; .NET CLR 2.0.50727; .NET CLR  3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C; .NET4.0E; rv:11.0) like Gecko\"}, \n\
                  screen:{availHeight:\"222\",availWidth:\"1024\",height:\"652\",width:\"1024\",colorDepth:\"24\"}, \n\
                  document:{cookie:\"\"} \n\
                   };";

const QString cs3 = "function test(aa1, bb1) {  \n\
                    7777777 \n\
                    6666666 \n\
                    cc = 88888888(aaa+'1442394301406:'+bbb+':hntub3:t20zrv:Mozilla50WindowsNT61Trident70SLCC2NETCLR2[5W1]NETCLR3[6W2]NETCLR3[6W3]MediaCenterPC60NET40CNET40Erv110likeGecko:nw140p0140o:1442397677436:14'); \n\
                    return cc; \n\
                    }";

const QString cs6 = "function test(aa1, bb1) { \n\
        cc = 88888888; \n\
        return cc; \n\
        }";
#endif // DEFINE_H
