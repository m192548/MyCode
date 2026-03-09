#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFile>
#include <QTimer>
#include <QRegExp>
#include <QRegExpValidator>
#include <QMessageBox>
#include "model/singletondb.h"
#include "controller/usercontroller.h"
#include "view/registerwidget.h"
class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWidget(QWidget *parent = 0);
    void initUI();
    void initconnect();
    void initWin();
    void initQss();
    void clearForm();
    void paintEvent(QPaintEvent *event);

signals:
    void loginsuccess(QString username);// 新增：登录成功信号，携带用户名
    void returntoplay();
public slots:
    void dologin();
    void doreturn();
    void getcode();//获取验证码
    void updatecount();//更新验证码发送计时器
    void changepwdvisible();
    void gotoregister();
    void showlogin();

private:
    QLabel*titlelab,*useraccountlab,*userpwdlab,*conpwdlab,*countDownLabel;
    QPushButton*dologinbtn,*returnbtn,*conpwdbtn,*eyesbtn,*goregisterbtn;
    QLineEdit *useraccountedit,*userpwdedit,*conpwdedit;
    //倒计时指针
    QTimer*countTimer;
    int countdowntime;
    bool pwdstatus;//明文--true 密文--false
    //创建注册界面指针
    RegisterWidget*regWidget;
};

#endif // LOGINWIDGET_H
