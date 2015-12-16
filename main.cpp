#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include "define.h"
#include <QDir>
#include <QDebug>
#include <QSize>
#include <QtGlobal>
#include <QDateTime>


/**
 * @brief setStyleSheet
 */
void setStyleSheet(QApplication &app)
{
    QFile qssFile(QSS_CONFIG_FILE_PATH);
    if(qssFile.open(QFile::ReadOnly))
    {
        app.setStyleSheet(qssFile.readAll());
    }
    else
    {
        QMessageBox::information(NULL, QString("title"), QString("qss open error!"));
    }
}

int main(int argc, char *argv[])
{
    QApplication::addLibraryPath("./plugins");
    QApplication a(argc, argv);

    qDebug() << QDir::currentPath();
    //QMessageBox::information(NULL, QString("title"), QDir::currentPath());
    setStyleSheet(a);

    MainWindow w;
    w.show();

    QDateTime dateTime;
    qsrand(dateTime.toTime_t());
    return a.exec();
}
