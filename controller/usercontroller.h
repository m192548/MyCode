#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H
#include <QString>
//1.构造函数私有化
//2.有一个静态的私有成员实例
//3.有一个公有的静态获取实例函数

class UserController
{
public:
    static UserController*getinstance();
    int doRegister(QString account,QString name,QString pwd);
    int doLogin(QString account, QString pwd);
    QString getcurrentusername();//获取当前登录的用户名
private:
    UserController();
    static UserController*instance;
    QString currentusername;
};

#endif // USERCONTROLLER_H
