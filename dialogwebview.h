#ifndef DIALOGWEBVIEW_H
#define DIALOGWEBVIEW_H

#include <QDialog>
#include <QNetworkCookieJar>

namespace Ui {
class DialogWebview;
}

class DialogWebview : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWebview(QWidget *parent = 0);
    ~DialogWebview();
    void initialize(const QString sName, const QString sPassword);

    void setUrl(const QUrl & url);

private slots:
    void on_pushButton_clicked();
    void loadFinished(bool bOk);
    void netlog(const QString & log);
signals:
    void startUpdate(QNetworkCookieJar * pCookieJar);
    void loginSuccess();
private:
    QString m_sName;
    QString m_sPassword;
    Ui::DialogWebview *ui;
    QString  m_strLog;

};

#endif // DIALOGWEBVIEW_H
