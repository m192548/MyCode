#ifndef MYCALENDARWIDGET_H
#define MYCALENDARWIDGET_H
#include<QCalendarWidget>
#include <QPaintEvent>
#include <QLabel>
#include <QPainter>
class MyCalendarWidget : public QCalendarWidget
{
    Q_OBJECT

public:
    explicit MyCalendarWidget(QWidget *parent = nullptr);

    // 设置开始日期
    void setStartDate(const QDate &date);
    // 设置结束日期
    void setEndDate(const QDate &date);
    // 获取开始日期
    QDate getStartDate() const;

    // 获取结束日期
    QDate getEndDate()const;

signals:
    // 日期范围改变信号
    void dateRangeChanged(const QDate &start, const QDate &end);

protected:
    // 重写绘图事件以自定义日期高亮
     void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const override;
private slots:
    // 处理日期点击事件
    void onDateClicked(const QDate &date);
private:
    QDate startDate;
    QDate endDate;
};


#endif // MYCALENDARWIDGET_H
