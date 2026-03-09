#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H
#include <QPainter>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QFile>
#include <QPixmap>
#include <QMessageBox>
#include "model/singletondb.h"
#include "controller/usercontroller.h"
class RegisterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterWidget(QWidget *parent = 0);
    void initUI();
    void initconnect();
    void initWin();
    void initQss();
    void paintEvent(QPaintEvent *event);
    void clearForm();
    void loadcameradevice();
signals:
    void backtologin();//返回登录窗口
public slots:
    void doRegister();
    void doreturn();
    void changepwdstatus();
    void changeconpwdstatus();

private:
    QLabel *titlelab,*useraccountlab,*usernamelab,*userpwdlab,*conpwdlab;
    QPushButton *doregisterbtn,*returnbtn,*eyesbtn,*eyebtn;
    QLineEdit *useraccountedit,*usernameedit,*userpwdedit,*conpwdedit;

    bool pwdstatus,status;
};

#endif // REGISTERWIDGET_H
