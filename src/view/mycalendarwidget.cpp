#include "mycalendarwidget.h"
#include <QDebug>
MyCalendarWidget::MyCalendarWidget(QWidget *parent)
    : QCalendarWidget(parent)
{
    startDate = QDate();
    endDate = QDate();
    setGridVisible(true);
    setSelectionMode(QCalendarWidget::SingleSelection);
    connect(this, &QCalendarWidget::clicked, this, &MyCalendarWidget::onDateClicked);
}

void MyCalendarWidget::setStartDate(const QDate &date)
{
    startDate = date;
}

void MyCalendarWidget::setEndDate(const QDate &date) {
    endDate = date;
}

QDate MyCalendarWidget::getStartDate() const
{
    return startDate;
}

QDate MyCalendarWidget::getEndDate() const
{
    return endDate;
}

void MyCalendarWidget::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    QCalendarWidget::paintCell(painter, rect, date);
    if (startDate.isValid() && endDate.isValid()&&date>=startDate&&date<=endDate)
    {
        painter->fillRect(rect,QColor(242,168,106));
        painter->drawText(rect,Qt::AlignCenter,QString::number(date.day()));
    }

}

void MyCalendarWidget::onDateClicked(const QDate &date)
{
    //开始日期无效(没有设置开始日期和结束日期)
    if (!startDate.isValid())
    {
        startDate = date;
        endDate = QDate();
    }
    else
    {
        if(!endDate.isValid())
        {
             endDate = date;
        }
        else
        {
            if(date>endDate)endDate=date;
            else if(date>startDate&&date<endDate)endDate=date;
            else if(date<startDate)
            {
                startDate=date;
                endDate=QDate();
            }
            else if(date==startDate)
            {
                startDate=QDate();
                endDate=QDate();
            }

        }
    }

    qDebug()<<"startDate:"<<startDate<<",endDate:"<<endDate;
    updateCells();

}
