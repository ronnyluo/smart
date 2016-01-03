#include "network.h"
#include <QSslConfiguration>
#include <QSslSocket>
#include <QSsl>
#include <QJSEngine>
#include <QJSValue>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QMessageBox>
#include "mainwindow.h"

/*
void HttpWorker::doWork(const QString & url, const QString & postData)
{
    QString result;
    emit resultReady(result);
}

void UpdateAirTicketController::handleResults(const QString &result)
{
    return ;
}

void UpdateAirTicketController::updateTicket(const QString &postData)
{
    return ;
}

void UpdatePickServiceController::handleResults(const QString &result)
{
    return ;
}

void UpdatePickServiceController::updatePickService(const QString &postData)
{
    return ;
}
*/


QunerHttp::QunerHttp(const QString & sUserName, const QString & sPassword, const QString &strChannelName, MainWindow * parent)
    : m_sUserName(sUserName),
      m_sPassword(sPassword),
      m_sChannelName(strChannelName)
{
    m_pNetworkManager = new QNetworkAccessManager(this);
    m_pCaptchaDialog = new CaptchaDialog(parent);
    connect(m_pCaptchaDialog, SIGNAL(signalVcode(QString)), this, SLOT(getVcode(QString)));
    connect(m_pCaptchaDialog, SIGNAL(signalRefreshVcode()), this, SLOT(refreshVcode()));
    m_pMainWindow = parent;

    m_pFile = new QFile("debuglog.txt");
    m_pFile->open(QIODevice::WriteOnly | QIODevice::Append);
    m_stream.setDevice(m_pFile);
}


QunerHttp::~QunerHttp()
{
    delete m_pFile;
}

void QunerHttp::setUserName(const QString & sUserName)
{
   m_sUserName = sUserName;
}

void QunerHttp::setPassword(const QString & sPassword)
{
   m_sPassword = sPassword;
}

void QunerHttp::reqSecApi()
{
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_0);

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                             "application/x-www-form-urlencoded;charset=UTF-8");
    QString time = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
    networkRequest.setUrl(
                QUrl("https://secapi.qunar.com/api/noCaptcha/get.json?callback=Query1720694714388697577_"
                     + time + "&_=" + time));
    networkRequest.setSslConfiguration(config);
    QNetworkReply *pNetworkReply =  m_pNetworkManager->get(networkRequest);

    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyReqSecApi()));
}

void QunerHttp::loginQuner(const QString & answer, const QString & cookie, const QString & code)
{
    qDebug() << "answer=" << answer;
    qDebug() << "cookie=" << cookie;
    qDebug() << "code=" << code;

    QByteArray postData;
    postData.append("loginType=0&ret=https://tb2cadmin.qunar.com/&username=" +
                m_sUserName + "&password=" + m_sPassword + "&remember=0&vcode="
                + code + "&answer=" + answer);

    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_0);

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                             "application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, postData.length());
    networkRequest.setUrl(QUrl("https://user.qunar.com/passport/loginx.jsp"));
    networkRequest.setSslConfiguration(config);

    QNetworkCookieJar * pNetworkCookieJar =  m_pNetworkManager->cookieJar();
    QList<QNetworkCookie> listCookie = QNetworkCookie::parseCookies(cookie.toUtf8());
    for (int i = 0; i < listCookie.size(); i++)
    {
        listCookie[i].setDomain(".qunar.com");
        pNetworkCookieJar->insertCookie(listCookie[i]);
        qDebug() << "insert cookie=" << QString(listCookie[i].toRawForm()) << endl;
    }
    QList<QNetworkCookie> cookies =  pNetworkCookieJar->cookiesForUrl(
                QUrl("https://user.qunar.com/passport/loginx.jsp"));
    for (int i = 0; i < cookies.size(); i++)
    {
        qDebug() << "cookie[" << i << "]=" << QString(cookies[i].toRawForm()) << endl;
    }
    m_pNetworkManager->setCookieJar(pNetworkCookieJar);
    QNetworkReply *pNetworkReply = m_pNetworkManager->post(networkRequest, postData);

    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyLogin()));

    connect(pNetworkReply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(sslErrors(QList<QSslError>)));
}
void QunerHttp::sslErrors(const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty())
            errorString += ", ";
        errorString += error.errorString();
    }

    if (QMessageBox::warning(NULL, tr("HTTP"),
                             tr("One or more SSL errors has occurred: %1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        //reply->ignoreSslErrors();
    }
}

void QunerHttp::replyLogin()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        qDebug() << "status=" << status.toInt() << ",login back=" << QString(bytes) << endl;
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonObject object = document.object();
        if (false == object["ret"].toBool())
        {
            qDebug() << "errmsg=" << object["errmsg"].toString() << endl;
            //处理错误
            QMessageBox::information(NULL, QString("错误"),
                         ", login error:" + object["errmsg"].toString());

        }
        else
        {
            updateQunarPrice(m_vecQunerPriceInfo);
            m_vecQunerPriceInfo.clear();
        }
    }
    else
    {
        bool flag = QSslSocket::supportsSsl();
        //处理错误
        QMessageBox::information(NULL, QString("错误"),
                                 pNetworkReply->errorString() +
                                 ",support ssl:" + QString(flag?"true":"false") +
                                 ",ssl version:" + QSslSocket::sslLibraryVersionString());
    }

    pNetworkReply->deleteLater();
}


void QunerHttp::replyReqSecApi()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        QString code(bytes);
        qDebug() << "status=" + status.toString() << ",content=" << code << endl;
        getAnswerV1(code, m_sAnswer);
        getCookie(code, m_sCookie);

        //启动登录流程
        loginQuner(m_sAnswer, m_sCookie, m_sCode);
    }
    else
    {
        bool flag = QSslSocket::supportsSsl();
        //处理错误
        QMessageBox::information(NULL, QString("错误"),
                                 pNetworkReply->errorString() +
                                 ",support ssl:" + QString(flag?"true":"false") +
                                 ",ssl version:" + QSslSocket::sslLibraryVersionString());
    }
    pNetworkReply->deleteLater();
}

void QunerHttp::reqQunerHome()
{
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_0);

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                             "application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setUrl(
                QUrl("https://user.qunar.com/passport/login.jsp?ret=http%3A%2F%2Ftb2cadmin.qunar.com%2F"));
    networkRequest.setSslConfiguration(config);
    QNetworkReply *pNetworkReply = m_pNetworkManager->get(networkRequest);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyReqQunerHome()));

    networkRequest.setUrl(QUrl("https://user.qunar.com/webApi/isNeedCaptcha.jsp?username=" + m_sUserName));
    pNetworkReply = m_pNetworkManager->get(networkRequest);
    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyNeedCaptcha()));
}

void QunerHttp::replyNeedCaptcha()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        qDebug() << "QunerHome statue=" << status.toInt() << ",content=" << QString(bytes) << endl;
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonObject object = document.object();
        if ("success" == object["errmsg"].toString())
        {
            m_bNeedCaptcha = object["data"].toBool();
            qDebug() << "NeedCaptcha=" << (m_bNeedCaptcha ? "true" : "false") << endl;
            reqVcode();
        }
        else
        {
            //处理错误
            QMessageBox::information(NULL, QString("错误"),
                         ",NeedCaptcha error:" + QString(object["errmsg"].toString()));
        }
    }
    else
    {
        bool flag = QSslSocket::supportsSsl();
        //处理错误
        QMessageBox::information(NULL, QString("错误"),
                                 pNetworkReply->errorString() +
                                 ",support ssl:" + QString(flag?"true":"false") +
                                 ",ssl version:" + QSslSocket::sslLibraryVersionString());
    }

    pNetworkReply->deleteLater();

}

void QunerHttp::replyReqQunerHome()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        qDebug() << "QunerHome statue=" << status.toInt() << ",content=" << QString(bytes) << endl;
    }
    else
    {
        bool flag = QSslSocket::supportsSsl();
        //处理错误
        QMessageBox::information(NULL, QString("错误"),
                                 pNetworkReply->errorString() +
                                 ",support ssl:" + QString(flag?"true":"false") +
                                 ",ssl version:" + QSslSocket::sslLibraryVersionString());
    }
    pNetworkReply->deleteLater();
}

void QunerHttp::reqVcode()
{
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_0);

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "image/jpeg");
    networkRequest.setUrl(
                QUrl("https://user.qunar.com/captcha/api/image?k={en7mni(z&p=ucenter_login&c=ef7d278eca6d25aa6aec7272d57f0a9a"));
    networkRequest.setSslConfiguration(config);
    QNetworkReply *pNetworkReply =  m_pNetworkManager->get(networkRequest);

    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replyGetVcode()));
}

void QunerHttp::replyGetVcode()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        qDebug() << "QunerHome statue=" << status.toInt() << ",content=" << QString(bytes) << endl;
        //需要验证码
        if (m_bNeedCaptcha)
        {
            m_pCaptchaDialog->init(bytes);
            m_pCaptchaDialog->show();
        }
        else
        {
            //请求answer
            reqSecApi();
        }

    }
    else
    {
        bool flag = QSslSocket::supportsSsl();
        //处理错误
        QMessageBox::information(NULL, QString("错误"),
                                 pNetworkReply->errorString() +
                                 ",support ssl:" + QString(flag?"true":"false") +
                                 ",ssl version:" + QSslSocket::sslLibraryVersionString());
    }
    pNetworkReply->deleteLater();
}


void QunerHttp::getVcode(const QString & code)
{
    m_sCode = code;
    //请求answer
    reqSecApi();
}


void QunerHttp::getAnswerV1(QString& jsFunc, QString& answer)
{
    /*.版本 2
    window函数名 ＝ 文本_取出中间文本 (参数, “(function(”, “)”)
    临时文本 ＝ 文本_替换 (参数, , , , “(function(” ＋ window函数名 ＋ “){”, “”, “})(window);”, “”, , , , , )
    临时文本 ＝ #常量2 ＋ window函数名 ＋ “=window;” ＋ 临时文本
    临时文本1 ＝ 文本_取出中间文本 (参数, “(''));”, “;”) ＋ “;”
    临时文本 ＝ 子文本替换 (临时文本, 临时文本1, “”, , , 真)
    加密的函数名 ＝ 文本_取出中间文本 (参数, “g,''));”, “(”)
    ' 调试输出 (加密的函数名)
    aa的串串 ＝ 文本_取出中间文本 (参数, “g,''));” ＋ 加密的函数名 ＋ “(”, “);”)
    ' 调试输出 (aa的串串)
    最终加密函数名 ＝ 文本_取出中间文本 (参数, “)))));},”, “=”)

    局部_随机常量3 ＝ 子文本替换 (#常量3, “[5位1]”, 文本_取随机数字 (5), , , 真)
    局部_随机常量3 ＝ 子文本替换 (局部_随机常量3, “[6位2]”, 文本_取随机数字 (6), , , 真)
    局部_随机常量3 ＝ 子文本替换 (局部_随机常量3, “[6位3]”, 文本_取随机数字 (6), , , 真)
    最终加密函数 ＝ 临时文本 ＋ 局部_随机常量3

    bbb的串串 ＝ 文本_取出中间文本 (参数, “))]);}”, “);”)
    bbb的串串 ＝ 文本_取右边 (bbb的串串, “))](”)

    最终加密函数 ＝ 文本_替换 (最终加密函数, , , , “7777777”, “aaa=” ＋ aa的串串 ＋ “;”, “6666666”, “bbb=” ＋ bbb的串串 ＋ “;”, “88888888”, 最终加密函数名, , )

    结果 ＝ JS调试 (最终加密函数, “test”, “”)
    */



    QString winFunName = getMidStr(jsFunc, "(function(", ")");
    QString tmp = jsFunc;
    tmp.replace("(function(" + winFunName + "){", "").replace("})(window);", "");
    tmp = cs2 + winFunName + "=window;" + tmp;
    qDebug() << "tmp=" << tmp << endl;
    QString tmp1 = getMidStr(jsFunc, "(''));", ";");
    qDebug() << "tmp1=" << tmp1 << endl;
    tmp = tmp.replace(tmp1, "");
    qDebug() << "tmp=" << tmp << endl;
    QString encryFunName = getMidStr(jsFunc,  "g,''));", "(");
    qDebug() << "encryFunName=" << encryFunName << endl;
    QString aa = getMidStr(jsFunc,  "g,''));" + encryFunName + "(", ");");
    qDebug() << "aa=" << aa << endl;
    QString lastEncryFunName = getMidStr(jsFunc, ")))));},", "=");
    qDebug() << "lastEncryFunName=" << lastEncryFunName << endl;

    QString local3 = cs3;
    local3 = local3.replace("[5W1]", getRandomDigit(5));
    local3 = local3.replace("[6W2]", getRandomDigit(6));
    local3 = local3.replace("[6W3]", getRandomDigit(6));
    QString finalEncryFun =  tmp + local3;
    qDebug() << "finalEncryFun=" << finalEncryFun << endl;

    /**
    bbb的串串 ＝ 文本_取出中间文本 (参数, “))]);}”, “);”)
    bbb的串串 ＝ 文本_取右边 (bbb的串串, “))](”)

    最终加密函数 ＝ 文本_替换 (最终加密函数, , , , “7777777”, “aaa=” ＋ aa的串串 ＋ “;”, “6666666”, “bbb=” ＋ bbb的串串 ＋ “;”, “88888888”, 最终加密函数名, , )
    **/
    QString bbb = getMidStr(jsFunc, "))]);}", ");");
    bbb = getRightStr(bbb, "))](");
    qDebug() << "bbb=" << bbb << endl;
    finalEncryFun.replace("7777777", "aaa=" + aa  + ";");
    finalEncryFun.replace("6666666", "bbb=" + bbb + ";");
    finalEncryFun.replace("88888888", lastEncryFunName);
    qDebug() << "finalEncryFun=" << finalEncryFun << endl;

    QJSEngine jsEngine;
    QJSValue error = jsEngine.evaluate(finalEncryFun);
    if (error.isError())
    {
       qDebug() << error.property("lineNumber").toInt() << ":" << error.toString() << endl;
    }
    QJSValue global = jsEngine.globalObject();
    if (global.hasProperty("test"))
    {
        QJSValue jsAnswerFun = global.property("test");
        QJSValue answerValue = jsAnswerFun.call();
        answer = answerValue.toString();
        qDebug() << "answer=" << answer << endl;
    }
}

void QunerHttp::getCookie(QString & jsCode, QString & cookie)
{
    /*.版本 2
    window函数名 ＝ 文本_取出中间文本 (参数, “(function(”, “)”)
    临时文本 ＝ 文本_替换 (参数, , , , “(function(” ＋ window函数名 ＋ “){”, “”, “})(window);”, “”, , , , , )
    临时文本 ＝ #常量2 ＋ window函数名 ＋ “=window;” ＋ 临时文本

    临时文本1 ＝ 文本_取出中间文本 (参数, “(''));”, “;”) ＋ “;”
    临时文本 ＝ 子文本替换 (临时文本, 临时文本1, “”, , , 真)
    加密的函数名 ＝ 文本_取出中间文本 (参数, “g,''));”, “(”)
    ' 调试输出 (加密的函数名)
    aa的临时串串 ＝ 文本_取出中间文本 (参数, “);}” ＋ window函数名 ＋ “[”, “=”)
    aa的串串 ＝ 文本_取出中间文本 (参数, “);}” ＋ window函数名 ＋ “[” ＋ aa的临时串串 ＋ “=”, “;”)
    ' 调试输出 (aa的串串)
    最终加密函数名 ＝ 文本_取出中间文本 (参数, “)))));},”, “=”)
    最终加密函数 ＝ 临时文本 ＋ #常量6
    最终加密函数 ＝ 文本_替换 (最终加密函数, , , , “88888888”, aa的串串, )
    结果 ＝ JS调试 (最终加密函数, “test”, “”)
    */
    QString winFunName = getMidStr(jsCode, "(function(", ")");
    QString tmp = jsCode;
    tmp.replace("(function(" + winFunName + "){", "").replace("})(window);", "");
    tmp = cs2 + winFunName + "=window;" + tmp;
    QString tmp1 = getMidStr(jsCode, "(''));", ";") + ";";
    tmp = tmp.replace(tmp1, "");
    QString encryFunName = getMidStr(jsCode, "g,''));", "(");
    qDebug() << "encryFunName=" << encryFunName << endl;
    QString aaTmp = getMidStr(jsCode, ");}" + winFunName + "[", "=");
    QString aa = getMidStr(jsCode, ");}" + winFunName + "[" + aaTmp + "=", ";");
    qDebug() << "aa=" << aa << endl;
    QString finalFunName = getMidStr(jsCode, ")))));},", "=");

    QString finalFun = tmp + cs6;
    finalFun = finalFun.replace("88888888", aa);

    QJSEngine jsEngine;
    QJSValue error = jsEngine.evaluate(finalFun);
    qDebug() << "finalfun=" << finalFun << endl;
    if (error.isError())
    {
       qDebug() << error.property("lineNumber").toInt() << ":" << error.toString() << endl;
    }
    QJSValue global = jsEngine.globalObject();
    if (global.hasProperty("test"))
    {
        QJSValue func = global.property("test");
        QJSValue result = func.call();
        cookie = result.toString();
        qDebug() << "cookie=" << cookie << endl;
    }
}

void QunerHttp::getAnswer(QString& jsFunc, QString& answer, QString& cookie)
{
    /*
    函数名 ＝ 文本_取左边指定个数 (返回文本, “=('')['\x”, 1)
    参数 [1] ＝ 文本_取出中间文本 (返回文本, “parseInt(”, “),cs=”)  ' parseInt
    参数 [2] ＝ 函数名 ＋ 文本_取右边 (文本_取出中间文本 (返回文本, “/[^a-zA-Z0-9]/g,''));”, “);})(window);”, ), “a”)  ' key1
    参数 [3] ＝ 文本_取右边 (文本_取出中间文本 (文本_取出中间文本 (返回文本, “;}” ＋
            文本_取左边指定个数 (返回文本, “=[new Date()”, 1) ＋ “[”, “;try{”), 函数名 ＋ “(”, “));”), “](”) ＋ “)”  ' key2
    参数 [4] ＝ 函数名 ＋ 文本_取出中间文本 (返回文本, “]=” ＋ 函数名, “;return re;”)  ' change
    js代码 ＝ 文本_取出中间文本 (返回文本, “){”, “;var ”) ＋ “;”
    js代码 ＝ js代码 ＋ #常量4
    .计次循环首 (4, i)
        js代码2 ＝ 子文本替换 (js代码, “[代码]”, 参数 [i], , , 真)
        js.执行 (js代码2)
        参数 [i] ＝ js.运行 (“aaa”)
    .计次循环尾 ()
    js代码 ＝ 子文本替换 (#js, “[参数1]”, 参数 [1], , , 真)
    js代码 ＝ 子文本替换 (js代码, “[参数3]”, 参数 [3], , , 真)
    js.执行 (js代码)
    answer ＝ js.运行 (“getanswer”, 参数 [2])
    */
    QString funcName = jsFunc.left(jsFunc.indexOf("=('')['\\x")).right(1);
    QString param[4];
    int firstIndex = jsFunc.indexOf("parseInt(") + QString("parseInt(").length();
    param[0] = jsFunc.mid(firstIndex, jsFunc.indexOf("),cs=") - firstIndex);
    qDebug() << param[0] << endl;
    firstIndex = jsFunc.indexOf("/[^a-zA-Z0-9]/g,''));") + QString("/[^a-zA-Z0-9]/g,''));").length();
    param[1] = jsFunc.mid(firstIndex, jsFunc.indexOf(");})(window);") - firstIndex);
    param[1] = funcName + param[1].mid(param[1].indexOf(funcName) + 1);
    qDebug() << param[1] << endl;
    QString tmp = ";}" + jsFunc.left(jsFunc.indexOf("=[new Date()")).right(1) + "[";
    firstIndex = jsFunc.indexOf(tmp) + tmp.length();
    param[2] = jsFunc.mid(firstIndex, jsFunc.indexOf(";try{") - firstIndex);
    firstIndex = param[2].indexOf(funcName + "(") + QString(funcName + "(").length();
    param[2] = param[2].mid(firstIndex, param[2].indexOf("));") - firstIndex);
    param[2] = param[2].mid(param[2].indexOf("](") + QString("](").length()) + ")";
    qDebug() << param[2] << endl;
    firstIndex = jsFunc.indexOf("]=" + funcName) + QString("]=" + funcName).length();
    param[3] = jsFunc.mid(firstIndex, jsFunc.indexOf(";return re;") - firstIndex);
    param[3] = funcName + param[3];
    qDebug() << param[3] << endl;
    firstIndex = jsFunc.indexOf("){") + QString("){").length();
    QString code = jsFunc.mid(firstIndex, jsFunc.indexOf(";var ") - firstIndex) + ";";
    code = code + "function aaa(){ return [@@];}";
    qDebug() << code << endl;

    for (int i = 0; i < 4; i++)
    {
        QJSEngine jsEngine;
        QString runCode = code;
        runCode.replace("[@@]", param[i]);
        qDebug() << "i=" << i << ",code=" << code << endl;
        qDebug() << "i=" << i << ",runCode=" << runCode << endl;
        jsEngine.evaluate(runCode);
        QJSValue jsGloble = jsEngine.globalObject();
        if (jsGloble.hasProperty("aaa"))
        {
            qDebug() << "has property aaa" << endl;
        }
        QJSValue aaaFunc = jsGloble.property("aaa");
        QJSValue jsValue = aaaFunc.call();
        param[i] = jsValue.toString();
        qDebug() << "param[" << i << "]=" << param[i] <<  endl;
    }
    QJSEngine jsEngine;
    QString jsCode = "var count=2; \n\
    var length=1; \n\
    var c = function (a, b) { \n\
            return a - 2 + b; \n\
        }, d = function (a, b) { \n\
            return (a - 45) / b; \n\
        }, e = function (a, b) { \n\
            return (a ^ 77) / b; \n\
        }, f = function (a, b) { \n\
            return a / 18 - b; \n\
        }, g = function (a, b) { \n\
            return (a / 72) ^ b; \n\
        }, h = function (a, b) { \n\
            return (a ^ 87) / b; \n\
        }, i = function (a, b) { \n\
            return (a ^ 62) + b; \n\
        }, k = function (a, b) { \n\
            return a / 60 + b; \n\
        }, l = function (a, b) { \n\
            return a + 56 - b; \n\
        }, m = function (a, b) { \n\
            return (a ^ 80) - b; \n\
        }, n = function (a, b) { \n\
            return (a / 98) ^ b; \n\
        }, b = ('')['\\x63\\x6f\\x6e\\x73\\x74\\x72\\x75\\x63\\x74\\x6f\\x72']['\\x66\\x72\\x6f\\x6d\\x43\\x68\\x61\\x72\\x43\\x6f\\x64\\x65']; \n\
        var r, x \n\
 \n\
        var w = []; \n\
        //z.push(new Date().getTime()); \n\
        q = function (aa, bb) { \n\
            var hh = 0, ch, le; \n\
            if (aa.length== 0) \n\
                return hh; \n\
            for (var ii = 0, le = aa.length; ii < le; ii++) { \n\
                ch = aa.charCodeAt(ii); \n\
                hh = ((hh << 5) - hh) + ch; \n\
                hh |= 0; \n\
            } \n\
            return hh; \n\
        }, p = function (aa, bb) { \n\
            var rem, bs, h1, h1b, c1, c1b, c2, c2b, k1, ii; \n\
            rem = aa.length & 3; \n\
            bs = aa.length - rem; \n\
            h1 = bb; \n\
            c1 = 0xcc9e2d51; \n\
            c2 = 0x1b873593; \n\
            ii = 0; \n\
            while (ii < bs) { \n\
                k1 = ((aa.charCodeAt(ii) & 0xff)) | ((aa.charCodeAt(++ii) & 0xff) << 8) | ((aa.charCodeAt(++ii) & 0xff) << 16) | ((aa.charCodeAt(++ii) & 0xff) << 24); \n\
                ++ii; \n\
                k1 = ((((k1 & 0xffff) * c1) + ((((k1 >>> 16) * c1) & 0xffff) << 16))) & 0xffffffff; \n\
                k1 = (k1 << 15) | (k1 >>> 17); \n\
                k1 = ((((k1 & 0xffff) * c2) + ((((k1 >>> 16) * c2) & 0xffff) << 16))) & 0xffffffff; \n\
                h1 ^= k1; \n\
                h1 = (h1 << 13) | (h1 >>> 19); \n\
                h1b = ((((h1 & 0xffff) * 5) + ((((h1 >>> 16) * 5) & 0xffff) << 16))) & 0xffffffff; \n\
                h1 = (((h1b & 0xffff) + 0x6b64) + ((((h1b >>> 16) + 0xe654) & 0xffff) << 16)); \n\
            } \n\
            k1 = 0; \n\
            switch (rem) { \n\
                case 3: \n\
                    k1 ^= (aa.charCodeAt(ii + 2) & 0xff) << 16; \n\
                case 2: \n\
                    k1 ^= (aa.charCodeAt(ii + 1) & 0xff) << 8; \n\
                case 1: \n\
                    k1 ^= (aa.charCodeAt(ii) & 0xff); \n\
                    k1 = (((k1 & 0xffff) * c1) + ((((k1 >>> 16) * c1) & 0xffff) << 16)) & 0xffffffff; \n\
                    k1 = (k1 << 15) | (k1 >>> 17); \n\
                    k1 = (((k1 & 0xffff) * c2) + ((((k1 >>> 16) * c2) & 0xffff) << 16)) & 0xffffffff; \n\
                    h1 ^= k1; \n\
            } \n\
            h1 ^= aa.length; \n\
            h1 ^= h1 >>> 16; \n\
            h1 = (((h1 & 0xffff) * 0x85ebca6b) + ((((h1 >>> 16) * 0x85ebca6b) & 0xffff) << 16)) & 0xffffffff; \n\
            h1 ^= h1 >>> 13; \n\
            h1 = ((((h1 & 0xffff) * 0xc2b2ae35) + ((((h1 >>> 16) * 0xc2b2ae35) & 0xffff) << 16))) & 0xffffffff; \n\
            h1 ^= h1 >>> 16; \n\
            return h1 >>> 0; \n\
        }, \n\
    y = function (aa, bb) \n\
    { \n\
 \n\
        }, t = function (aa, bb) { \n\
            var re = ''; \n\
            var mm = aa.length % 3; \n\
            switch (mm) { \n\
                case 0: \n\
                    break; \n\
                case 1: \n\
                    aa += '\\x00\\x00'; \n\
                    break; \n\
                case 2: \n\
                    aa += '\\x00'; \n\
                    break; \n\
            } \n\
            var pos = 0, bb = parseInt(\"[param1]\"), cs =\"abcdefghijklmnopqrstuvwxyz_1234567890-ABCDEFGHIJKLMNOPQRSTUVWXYZ\"; \n\
            for (var ii = aa.length - 1; ii >= 0; ii -= 3) { \n\
                var nn = aa.charCodeAt(ii) ^ (bb >>> (8 * (pos++ % 4)) & 0xFF); \n\
                nn |= (aa.charCodeAt(ii - 1) ^ (bb >>> (8 * (pos++ % 4)) & 0xFF)) << 8; \n\
                nn |= (aa.charCodeAt(ii - 2) ^ (bb >>> (8 * (pos++ % 4)) & 0xFF)) << 16; \n\
                re += cs.charAt(nn & 0x3F); \n\
                re += cs.charAt(nn >>> 6 & 0x3F); \n\
                re += cs.charAt(nn >>> 12 & 0x3F); \n\
                re += cs.charAt(nn >>> 18 & 0x3F); \n\
            } \n\
            //u[document][cookie] =\" _challenge=c411d0de-f639-4add-8c00-fc9afe042a59;domain=qunar.com;path=/\"; \n\
            return re; \n\
        }; \n\
    s = function (aa, bb) { \n\
            z.push(aa); \n\
        }, \n\
    v = function (aa, bb) { \n\
            if (\"function\" === typeof (this[aa])) { \n\
                this[aa].apply(this, bb); \n\
            } else { \n\
                this[aa] = bb; \n\
            } \n\
            return v.bind(this); \n\
        }; \n\
      /* if (!count) { \n\
            u[_nc] = { count: 1 }; \n\
        }*/ \n\
    function getanswer(key) \n\
    { \n\
    for (var ii = 0; ii < length; ii++) { \n\
            w.push(\"Shockwave Flash\"); \n\
        } \n\
       var z=[]; \n\
    z.push([param3]); \n\
    z.push(\"12nezle\"); \n\
    z.push(\"yynsm9\"); \n\
    z.push(\"Mozilla50WindowsNT63WOW64AppleWebKit53736KHTMLlikeGeckoChrome310165063Safari53736115Browser526\"); \n\
    z.push(\"nw18gp018go\"); \n\
    z.push(new Date().getTime()); \n\
    z.push(3); \n\
 \n\
 \n\
    return t(key+(new Date().getTime())+\":\"+z.join(\":\")).toString(); \n\
 \n\
    } \n\
    ";
    jsCode.replace("[param1]", param[0]).replace("[param3]", param[2]);
    QJSValue error = jsEngine.evaluate(jsCode);
    if (error.isError())
    {
       qDebug() << error.property("lineNumber").toInt() << ":" << error.toString() << endl;
    }
    QJSValue jsCodeGlobe = jsEngine.globalObject();
    if (jsCodeGlobe.hasProperty("getanswer"))
    {
        QJSValue jsAnswerFun = jsCodeGlobe.property("getanswer");
        QJSValueList args;
        args << param[1];
        QJSValue answerValue = jsAnswerFun.call(args);
        answer = answerValue.toString();
        qDebug() << "answer=" << answer << endl;
    }
    cookie = param[3].left(param[3].indexOf("domain"));
    qDebug() << "cookie=" << param[3] << endl;
}

void QunerHttp::setQunarPrice4Update(QVector<QunarPriceInfo> &vecQunerPriceInfo)
{
    m_vecQunerPriceInfo = vecQunerPriceInfo;
}

void QunerHttp::updateQunarPrice(QVector<QunarPriceInfo>& vecQunerPriceInfo)
{
    for (int i = 0; i < vecQunerPriceInfo.size(); i++)
    {
        setQunarPrice(vecQunerPriceInfo[i].toPostForm().toUtf8());
    }
    //Test
    /*
    QunarPriceInfo price;
    price.adult_price = "4260";
    price.child_price = "2700";
    price.count = "20";
    price.dateString = "2015-12-20";
    price.market_price = "6250";
    price.pId = "2533826371";
    price.room_send_price = "1860";
    price.min_buy_count = "2";
    price.max_buy_count = "20";
    setQunarPrice(price.toPostForm().toUtf8());
    */

}

void QunerHttp::setQunarPrice(const QByteArray & post)
{
    m_stream << "setQunarPrice=" << QString(post) << endl;
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_0);

    QNetworkRequest networkRequest;
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                             "application/x-www-form-urlencoded;charset=UTF-8");
    networkRequest.setHeader(QNetworkRequest::ContentLengthHeader, post.length());
    networkRequest.setUrl(
                QUrl("https://tb2cadmin.qunar.com/supplier/productTeamOperation.do?method=operatProductTeams&op=update"));
    networkRequest.setSslConfiguration(config);

    QList<QNetworkCookie> cookies =  m_pNetworkManager->cookieJar()->cookiesForUrl(
                QUrl("https://tb2cadmin.qunar.com/supplier/productTeamOperation.do?method=operatProductTeams&op=update"));
    for (int i = 0; i < cookies.size(); i++)
    {
        qDebug() << "cookie[" << i << "]=" << QString(cookies[i].toRawForm()) << endl;
    }
    QNetworkReply *pNetworkReply = m_pNetworkManager->post(networkRequest, post);

    connect(pNetworkReply, SIGNAL(finished()), this, SLOT(replySetQunarPrice()));

    connect(pNetworkReply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(sslErrors(QList<QSslError>)));
}

void QunerHttp::replySetQunarPrice()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());

    //获取响应的信息，状态码为200表示正常
    QVariant status = pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(pNetworkReply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = pNetworkReply->readAll();  //获取字节
        qDebug() << "SetQunarPrice statue=" << status.toInt() << ",content=" << QString(bytes) << endl;
        QJsonDocument document = QJsonDocument::fromJson(bytes);
        QJsonObject object = document.object();
        if (object["ret"].toInt() <= 0)
        {
            qDebug() << "errmsg=" << object["message"].toString() << endl;
            //处理错误
            QMessageBox::information(NULL, QString("更新价格异常"), object["message"].toString());

        }
        m_stream << "setQunarPrice=" << QString(bytes) << endl;

    }
    else
    {
        bool flag = QSslSocket::supportsSsl();
        //处理错误
        QMessageBox::information(NULL, QString("错误"),
                                 pNetworkReply->errorString() +
                                 ",support ssl:" + QString(flag?"true":"false") +
                                 ",ssl version:" + QSslSocket::sslLibraryVersionString());
    }

    pNetworkReply->deleteLater();
}


void QunerHttp::login()
{
    //启动登录流程
    reqQunerHome();
}
void QunerHttp::refreshVcode()
{
    reqVcode();
}
