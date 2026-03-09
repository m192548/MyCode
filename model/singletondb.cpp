#include "model/singletondb.h"
#include <QApplication>
SingletonDB::GC SingletonDB::GC::gc;
SingletonDB*SingletonDB::instance =nullptr;
SingletonDB *SingletonDB::getInstance()
{
    if(SingletonDB::instance==nullptr){
        SingletonDB::instance=new SingletonDB();
        qDebug()<<"first create";
    }else{
        qDebug()<<"2-n create";
    }
    return SingletonDB::instance;
}

void SingletonDB::releaseInstance()
{
    if(SingletonDB::instance){
        delete SingletonDB::instance;
        SingletonDB::instance=nullptr;
        qDebug()<<"releaseinstance";
    }
}

int SingletonDB::execsql(QString sql)
{
    if (this->pdb == nullptr) {
         qDebug() << "execsql failed: 数据库未连接";
         return -1; // 返回错误码，告知调用者连接失败
     }

    char *errmsg=nullptr;
    int res=sqlite3_exec(this->pdb,sql.toUtf8(),nullptr,nullptr,&errmsg);
    if(res!=SQLITE_OK){
        qDebug()<<errmsg;
    }
    return res;
}

int SingletonDB::selectsql(QString sql, char **&qres, int &row, int &col)
{
    if (this->pdb == nullptr) {
         qDebug() << "execsql failed: 数据库未连接";
         return -1; // 返回错误码，告知调用者连接失败
     }

    char *errmsg=nullptr;
    int res=sqlite3_get_table(this->pdb,sql.toUtf8(),&qres,&row,&col,&errmsg);
    if(res == SQLITE_OK)
      {
          //qDebug()<<"do sql success"<<sql;
          return 0;
      }

        qDebug()<<errmsg;

    return res;
}

SingletonDB::SingletonDB()
{
    QString appPath=QApplication::applicationDirPath();
    QString dbPath=appPath+"/project.db";
    int res=sqlite3_open(dbPath.toStdString().c_str(),&pdb);
    if(res==SQLITE_OK){
        qDebug()<<"open db success";
    }else{
        qDebug()<<sqlite3_errcode(pdb);
        qDebug()<<sqlite3_errmsg(pdb);
        qDebug()<<"open db failed";
    }
}

SingletonDB::~SingletonDB()
{
    sqlite3_close(pdb);
    qDebug()<<"db release";
}
