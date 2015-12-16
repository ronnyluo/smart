#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QThread>
#include <QNetworkAccessManager>
#include "captchadialog.h"


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
    QunerHttp(const QString & sUserName, const QString & sPassword, MainWindow * parent);
    void setUserName(const QString & sUserName);
    void setPassword(const QString & sPassword);

private slots:
    void replyLogin();
    void replyReqSecApi();
    void sslErrors(const QList<QSslError> &errors);
    void replyReqQunerHome();
    void replyNeedCaptcha();
    void replyGetVcode();
    void getVcode(const QString & code);

private:
    void reqQunerHome();
    void loginQuner(const QString & answer, const QString & cookie, const QString & code);
    void reqSecApi();
    void getAnswer(QString& jsFunc, QString& answer, QString& cookie);
    void getAnswerV1(QString& jsFunc, QString& answer);
    void getCookie(QString & jsCode, QString & cookie);
    void reqVcode();

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
};

#endif // NETWORK_H
