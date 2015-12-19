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
    this->hide();
    emit signalVcode(text);
}

void CaptchaDialog::on_pushButtonCancel_clicked()
{
    this->hide();
}

void CaptchaDialog::on_pushButtonOk_refresh_clicked()
{
    emit signalRefreshVcode();
}
