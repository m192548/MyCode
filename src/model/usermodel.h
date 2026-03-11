#ifndef USERMODEL_H
#define USERMODEL_H
#include <QString>
#include <QCryptographicHash>
//用户模型层,处理用户相关业务逻辑
class UserModel
{
public:
    static UserModel*getinstance();
    int selectUserByAccount(QString account);//查找用户是否重名 return 0用户不存在 1用户存在
    int insertUser(QString account,QString name,QString pwd);//插入用户 return 0 添加用户成功 else添加失败
    int existUserByNamePwd(QString account,QString pwd,QString& name);//查找用户是否存在
private:
    UserModel();
    static UserModel*instance;
    // 对明文密码进行MD5加密
    QString encryptPwd(const QString &plainPwd) {
        // 将QString转换为UTF-8格式的字节数组
        QByteArray bytePwd = plainPwd.toUtf8();
        // 计算MD5哈希值
        QByteArray md5Pwd = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
        // 转换为十六进制字符串（32位）
        return md5Pwd.toHex();
    }

};

#endif // USERMODEL_H
