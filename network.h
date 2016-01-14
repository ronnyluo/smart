#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QNetworkAccessManager>
#include "captchadialog.h"
#include "define.h"
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QTimer>


class MainWindow;
/*
class HttpWorker : public QObject
{
    Q_OBJECT
    QThread workerThread;

public slots:
    void doWork(const QString & url, const QString & postData);

signals:
    void resultReady(const QString &result);
};

class HttpController : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    HttpController()
    {
        HttpWorker *worker = new HttpWorker;
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &HttpController::send, worker, &HttpWorker::doWork);
        connect(worker, &HttpWorker::resultReady, this, &HttpController::handleResults);
        workerThread.start();
    }
    ~HttpController()
    {
        workerThread.quit();
        workerThread.wait();
    }
public slots:
    virtual void handleResults(const QString & result) = 0;
signals:
    void send(const QString & url, const QString & postData);
};

class UpdateAirTicketController : public HttpController
{
    Q_OBJECT
public slots:
    virtual void handleResults(const QString & result);
    void updateTicket(const QString & postData);
};


class UpdatePickServiceController : public HttpController
{
    Q_OBJECT
public slots:
     virtual void handleResults(const QString & result);
     void updatePickService(const QString & postData);
};
*/
class MainWindow;

class QunerHttp : public QObject
{
    Q_OBJECT
public:
    QunerHttp(const QString & sUserName, const QString & sPassword, const QString& strChannelName, MainWindow * parent);
    virtual ~QunerHttp();
    QString GetUserName() {return m_sUserName;}
    QString GetChannelName(){return m_sChannelName;}

    void setUserName(const QString & sUserName);
    void setPassword(const QString & sPassword);

    void updateQunarPrice(QVector<QunarPriceInfo>& vecQunerPriceInfo);
    void login();
    void setQunarPrice4Update(QVector<QunarPriceInfo>& vecQunerPriceInfo);

private slots:
    void replyLogin();
    void replyReqSecApi();
    void sslErrors(const QList<QSslError> &errors);
    void replyReqQunerHome();
    void replyNeedCaptcha();
    void replyGetVcode();
    void replySetQunarPrice();
    void getVcode(const QString & code);
    void refreshVcode();
    void tryUpdatePriceToQunar();

private:
    void reqQunerHome();
    void loginQuner(const QString & answer, const QString & cookie, const QString & code);
    void reqSecApi();
    void getAnswer(QString& jsFunc, QString& answer, QString& cookie);
    void getAnswerV1(QString& jsFunc, QString& answer);
    void getCookie(QString & jsCode, QString & cookie);
    void reqVcode();
    void setQunarPrice(const QunarPriceInfo & qunarPriceInfo);

signals:
    void netlog(const QString & log);

private:
    bool m_bNeedCaptcha;
    QString m_sAnswer;
    QString m_sCookie;
    QString m_sCode;
    CaptchaDialog * m_pCaptchaDialog;
    QString m_sUserName;
    QString m_sPassword;
    QNetworkAccessManager *m_pNetworkManager;
    MainWindow * m_pMainWindow;
    QTextStream  m_stream;
    QFile * m_pFile;
    QString m_sChannelName;
    QMap<QNetworkReply*, QunarPriceInfo> m_mapQunarPriceInfo;
    QVector<QunarPriceInfo> m_vecTryQunarPriceInfo; //重试队列
    QVector<QunarPriceInfo> m_vecQunarPriceInfo;  //需要更新的原始队列
    QVector<QunarPriceInfo> m_vecQunarPriceInfoBatch; //批量更新的队列

    QTimer * m_pTimer;
    int m_iTotal;
    int m_iSuccess;
    int m_iFailed;
};

#endif // NETWORK_H
