#ifndef SINGLETONDB_H
#define SINGLETONDB_H
#include "model/sqlite3.h"
#include <QDebug>
#include <QString>
#include <QVector>
class SingletonDB
{
public:
    static SingletonDB*getInstance();
    static void releaseInstance();
    int execsql(QString sql);
    int selectsql(QString sql, char **&qres,int &row,int &col);

private:
    SingletonDB();
    ~SingletonDB();
    static SingletonDB*instance;
    sqlite3*pdb;
    class GC
      {
          public:~GC()
          {
              // 可以在这里销毁所有的资源，例如：db 连接、文件句柄等
              if (instance != nullptr) {
                  qDebug()<< "Here destroy the m_pSingleton..." << endl;
                  delete instance;
                  instance = nullptr;
              }
          }
          static GC gc; // 用于释放单例
      };
};

#endif // SINGLETONDB_H
