#include "captchadialog.h"
#include "ui_captchadialog.h"
#include "mainwindow.h"
#include <QPicture>
#include <QPixmap>
#include <QMessageBox>

CaptchaDialog::CaptchaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaptchaDialog)
{
    ui->setupUi(this);
}

void CaptchaDialog::init(const QByteArray & codeBytes)
{
    ui->label_img->clear();
    ui->lineEdit->clear();

    QPixmap pixmap;
    pixmap.loadFromData(codeBytes);
    ui->label_img->setPixmap(pixmap.scaled(ui->label_img->size()));

    m_strLog.clear();
}

CaptchaDialog::~CaptchaDialog()
{
    delete ui;
}

void CaptchaDialog::on_pushButtonOk_clicked()
{
    QString text = ui->lineEdit->text();
    if (text.isEmpty())
    {
        QMessageBox::information(NULL, QString("提示"), "请输入验证码");
        return;
    }
    emit signalVcode(text);
}

void CaptchaDialog::on_pushButtonCancel_clicked()
{
    //this->hide();
}

void CaptchaDialog::on_pushButtonOk_refresh_clicked()
{
    emit signalRefreshVcode();
}

void CaptchaDialog::netlog(const QString & log)
{
    if (!m_strLog.isEmpty())
    {
        m_strLog.append("\n");
    }
    m_strLog.append(log);
    ui->textBrowser_log->setPlainText(m_strLog);
}
