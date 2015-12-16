#ifndef CAPTCHADIALOG_H
#define CAPTCHADIALOG_H


#include <QDialog>

namespace Ui {
class CaptchaDialog;
}

class CaptchaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CaptchaDialog(QWidget *parent = 0);
    ~CaptchaDialog();

    void init(const QByteArray & codeBytes);

signals:
    void  signalVcode(const QString & code);

private slots:
    void on_pushButtonOk_clicked();
    void on_pushButtonCancel_clicked();

private:
    Ui::CaptchaDialog *ui;
};

#endif // CAPTCHADIALOG_H
