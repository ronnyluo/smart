#include "dialogwebview.h"
#include "ui_dialogwebview.h"
#include <QWebElement>
#include <QWebFrame>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QDebug>

DialogWebview::DialogWebview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWebview)
{
    ui->setupUi(this);
}

DialogWebview::~DialogWebview()
{
    delete ui;
}

void DialogWebview::initialize(const QString sName, const QString sPassword)
{
    m_sName = sName;
    m_sPassword = sPassword;
}

void DialogWebview::setUrl(const QUrl & url)
{
    ui->webView->setUrl(url);
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    m_strLog.clear();
}

void DialogWebview::on_pushButton_clicked()
{
    QNetworkAccessManager * pManager = ui->webView->page()->networkAccessManager();
    QNetworkCookieJar * pCookieJar = pManager->cookieJar();

    QList<QNetworkCookie> cookies =  pCookieJar->cookiesForUrl(
                QUrl("https://tb2cadmin.qunar.com/supplier/productTeamOperation.do"));
    for (int i = 0; i < cookies.size(); i++)
    {
        qDebug() << "cookie[" << i << "]=" << QString(cookies[i].toRawForm()) << endl;
    }
    emit startUpdate(pCookieJar);
}

void DialogWebview::loadFinished(bool bOk)
{
    if (!bOk)
    {
       return;
    }
    QWebFrame *frame = ui->webView->page()->mainFrame();
//! [select elements]
    QWebElement document = frame->documentElement();
    QWebElement username = document.findFirst("input[type=text][name=username]");
    if (!username.isNull())
    {
        username.setAttribute("value", m_sName);
    }
    QWebElement password = document.findFirst("input[type=password][name=password]");
    if (!password.isNull())
    {
        password.removeAttribute("autocomplete");
        //qDebug() << "bf autocomplete=" << password.attribute("autocomplete") << endl;
        //password.setAttribute("autocomplete", "on");
        password.setAttribute("value", m_sPassword);
        qDebug() << "password innerxml=" << password.toInnerXml() << endl;
        qDebug() << "password outerxml=" << password.toOuterXml() << endl;
        qDebug() << "password value=" << password.attribute("value") << endl;
        qDebug() << "af autocomplete=" << password.attribute("autocomplete") << endl;
    }
    disconnect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    emit loginSuccess();
}

void DialogWebview::netlog(const QString & log)
{
    if (!m_strLog.isEmpty())
    {
        m_strLog.append("\n");
    }
    m_strLog.append(log);
    ui->textBrowser->setPlainText(m_strLog);
}

void DialogWebview::clearLog()
{
    ui->textBrowser->clear();
}
