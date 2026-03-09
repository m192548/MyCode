#include "usercontroller.h"
#include "model/usermodel.h"
#include <QString>
UserController*UserController::instance=nullptr;
UserController *UserController::getinstance()
{
    if(UserController::instance==nullptr){
        UserController::instance=new UserController;
    }
    return UserController::instance;
}

int UserController::doRegister(QString account,QString name, QString pwd)
{
    int res=UserModel::getinstance()->selectUserByAccount(account);//0--用户不存在
    if(res==0){
        return UserModel::getinstance()->insertUser(account,name,pwd);
    }
    return res;
}

int UserController::doLogin(QString account, QString pwd)
{
     int loginresult=UserModel::getinstance()->existUserByNamePwd(account, pwd,currentusername);//登录验证
     //0--登录成功
     if(loginresult==0){
        // 登录成功时保存用户名
     }else{
         currentusername.clear();
     }
     return loginresult;

}

QString UserController::getcurrentusername()
{
    return currentusername;
}

UserController::UserController()
{

}
