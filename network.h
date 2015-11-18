#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QThread>

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


#endif // NETWORK_H
