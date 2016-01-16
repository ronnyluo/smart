#ifndef WEBWINDOW_H
#define WEBWINDOW_H

#include <QWidget>
#include <QUrl>

namespace Ui {
class webwindow;
}

class webwindow : public QWidget
{
    Q_OBJECT

public:
    explicit webwindow(QWidget *parent = 0);
    ~webwindow();

    void initialize(const QString sName, const QString sPassword);

    void setUrl(const QUrl & url);
private slots:
    void on_pushButton_ticket_clicked();
    void loadFinished(bool bOk);

private:
    Ui::webwindow *ui;
    QString m_sName;
    QString m_sPassword;
};

#endif // WEBWINDOW_H
