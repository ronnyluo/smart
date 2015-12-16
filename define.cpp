#include "define.h"

QString getRandomDigit(int iNum)
{
    QString randtext;
    for (int i = 0; i < iNum; i++)
    {
        randtext += QString::number(qrand()%9);
    }
    return randtext;
}
QString getMidStr(QString & origin, const QString & first, const QString & last)
{
    int index = origin.indexOf(first) + first.length();
    return origin.mid(index, origin.indexOf(last, index) - index);
}
QString getLeftStr(QString & origin, const QString & start)
{
    return origin.left(origin.indexOf(start));
}
QString getRightStr(QString & origin, const QString & start)
{
    return origin.mid(origin.indexOf(start) + start.length());
}

