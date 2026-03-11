#include "usermodel.h"
#include "model/singletondb.h"
#include <QDateTime>
UserModel *UserModel::instance=nullptr;
UserModel *UserModel::getinstance()
{
    if(UserModel::instance==nullptr){
        UserModel::instance=new UserModel();
    }
    return instance;
}

int UserModel::selectUserByAccount(QString account)
{
    QString sql=QString("select *from tbl_user where user_account='%1';").arg(account);
    qDebug()<<"UserModel::selectUserByName-11"<<sql;
    char **qres=nullptr;
    int row=0,col=0;
    int res=SingletonDB::getInstance()->selectsql(sql,qres,row,col);
    if(res!=0){
        return -1;
    }
    return row;//1-已存在,2-不存在
}

int UserModel::insertUser(QString account,QString name, QString pwd)
{
    //QString nowtime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    // 对明文密码进行MD5加密
    QString encryptedPwd = encryptPwd(pwd);
    QString sql=QString("insert into tbl_user(user_account,user_name,user_pwd) values('%1','%2','%3');")
            .arg(account).arg(name).arg(encryptedPwd);
    qDebug()<<"UserModel::insertUser-31"<<sql;
    return SingletonDB::getInstance()->execsql(sql);

}

int UserModel::existUserByNamePwd(QString account,QString pwd,QString& name)
{
    // 对输入的明文密码进行MD5加密
    QString encryptedPwd = encryptPwd(pwd);
     QString sql=QString("SELECT *from tbl_user WHERE user_account='%1' and user_pwd='%2';")
             .arg(account).arg(pwd);
     qDebug()<<"UserModel::existUserByNamePwd"<<sql;
     char**qres=nullptr;
     int row=0,col=0;
     int res = SingletonDB::getInstance()->selectsql(sql, qres, row, col);
     if (res != 0) {

             return -1; //数据库操作失败

         }
         if (row > 0) {
             name=QString(qres[7]);
             return 0;  // 有匹配记录，登录成功

         } else {

            return 1; // 无匹配记录（用户名或密码错误)

         }


}

UserModel::UserModel()
{

}
