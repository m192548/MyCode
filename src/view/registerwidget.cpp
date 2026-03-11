#include "registerwidget.h"

RegisterWidget::RegisterWidget(QWidget *parent) : QWidget(parent)
{
    this->initUI();
    this->initWin();
    this->initconnect();
    this->initQss();
}

void RegisterWidget::initUI()
{
    //正则表达式
    QRegExp regExp("[A-Za-z0-9_]+");
    QRegExpValidator *validator=new QRegExpValidator(regExp,this);
    this->titlelab=new QLabel("用户注册",this);//标题设置
    this->titlelab->setStyleSheet("font-size:45px;color:white");
    this->titlelab->setGeometry(430,70,300,100);
    //用户账号标签设置
    this->useraccountlab=new QLabel("账号:",this);
    this->useraccountlab->setGeometry(300,180,130,50);
    //用户账号编辑框设置
    this->useraccountedit=new QLineEdit(this);
    this->useraccountedit->setGeometry(370,185,330,35);
    //用户昵称标签设置
    this->usernamelab=new QLabel("昵称:",this);
    this->usernamelab->setGeometry(300,250,130,50);
    //用户昵称编辑框设置
    this->usernameedit=new QLineEdit(this);
    this->usernameedit->setGeometry(370,255,330,35);
    //用户密码标签设置
    this->userpwdlab=new QLabel("密码:",this);
    this->userpwdlab->setGeometry(300,320,130,50); 
    //用户密码编辑框设置
    this->userpwdedit=new QLineEdit(this);
    this->userpwdedit->setGeometry(370,325,330,35);
    this->userpwdedit->setValidator(validator);//只允许输入数字 字母 下划线
    this->userpwdedit->setEchoMode(QLineEdit::Password);
    //密码明密文显示
    this->eyesbtn=new QPushButton(this);
    this->eyesbtn->setGeometry(670,325,30,35);
    this->eyesbtn->setStyleSheet("border:none;");
    this->eyesbtn->setIcon(QIcon(":/images/eyesclose.png"));
    this->eyesbtn->setIconSize(QSize(20,20));
    pwdstatus=false;//初始为密文
    //用户确认密码标签设置
    this->conpwdlab=new QLabel("确认密码:",this);
    this->conpwdlab->setGeometry(300,390,130,50);
    //用户确认密码编辑框设置
    this->conpwdedit=new QLineEdit(this);
    this->conpwdedit->setGeometry(430,395,270,35);
    this->conpwdedit->setValidator(validator);//只允许输入数字 字母 下划线
    this->conpwdedit->setEchoMode(QLineEdit::Password);//初始密文显示
    //密码明密文显示
    this->eyebtn=new QPushButton(this);
    this->eyebtn->setGeometry(670,395,30,35);
    this->eyebtn->setStyleSheet("border:none;");
    this->eyebtn->setIcon(QIcon(":/images/eyesclose.png"));
    this->eyebtn->setIconSize(QSize(20,20));
    status=false;//初始为密文
    //确定按钮设置
    this->doregisterbtn=new QPushButton("确定",this);
    this->doregisterbtn->setGeometry(300,490,100,50);
    //返回按钮设置
    this->returnbtn=new QPushButton("取消",this);
    this->returnbtn->setGeometry(600,490,100,50);
}

void RegisterWidget::initconnect()
{
    connect(this->doregisterbtn,SIGNAL(clicked()),this,SLOT(doRegister()));
    connect(this->returnbtn,SIGNAL(clicked()),this,SIGNAL(backtologin()));
    connect(this->eyesbtn,SIGNAL(clicked()),this,SLOT(changepwdstatus()));
    connect(this->eyebtn,SIGNAL(clicked()),this,SLOT(changeconpwdstatus()));
}

void RegisterWidget::initWin()
{
    this->setWindowTitle("注册界面");
    this->setWindowIcon(QIcon(":/images/icon.png"));
    this->resize(1042,600);
}

void RegisterWidget::initQss()
{
    //加载外部qss文件
    QFile file(":register.qss");//资源文件路径
    if(file.open(QFile::ReadOnly))
    {
        QString styleSheet=QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

}
void RegisterWidget::paintEvent(QPaintEvent *event)
{
    //背景自适应
    QPainter paint(this);
    paint.drawPixmap(this->rect(),QPixmap(":/images/background2.png"));
}

void RegisterWidget::clearForm()
{
    //清空所有编辑框内容
    this->useraccountedit->clear();
    this->userpwdedit->clear();
    this->conpwdedit->clear();
}


void RegisterWidget::doRegister()
{
    //1.获取表单信息
    QString account = this->useraccountedit->text();
    QString username=this->usernameedit->text();
    QString password=this->userpwdedit->text();
    QString confirmPwd=this->conpwdedit->text();
    //2.表单验证
    if(username.isEmpty()||password.isEmpty()||account.isEmpty()){
        QMessageBox::warning(this,"注册提示","用户名或账号或密码不能为空");
        return;
    }
    if(username.length()<6||account.length()<6){
        QMessageBox::warning(this,"注册提示","用户名或账号不能小于6位数");
        return;
    }
    if(password.length()<8){
        QMessageBox::warning(this,"注册提示","密码不能小于8位数");
        return;
    }
    if(password!=confirmPwd){
        QMessageBox::warning(this,"注册提示","两次输入的密码不一致,请重新输入!");
         return;
    }
    int res=UserController::getinstance()->doRegister(account,username,password);
    if(res==0){
        QMessageBox::information(this,"注册界面","注册成功");
        this->hide(); // 隐藏注册界面
        emit backtologin();//发射返回登录的信号
    }else if(res==1){
        QMessageBox::warning(this,"注册界面","已存在该用户,注册失败!");
    }else{
        QMessageBox::warning(this,"注册界面","服务器异常请稍后重试!");
    }
}

void RegisterWidget::doreturn()
{

}

void RegisterWidget::changepwdstatus()
{
    if(!pwdstatus){
       this->userpwdedit->setEchoMode(QLineEdit::Normal);//明文显示
        this->eyesbtn->setIcon(QIcon(":/images/eyesopen.png"));//按钮图标切换

    }else{
        this->userpwdedit->setEchoMode(QLineEdit::Password);//密文显示
        this->eyesbtn->setIcon(QIcon(":/images/eyesclose.png"));//按钮图标切换
    }
    pwdstatus=!pwdstatus;//按钮状态切换
}

void RegisterWidget::changeconpwdstatus()
{
    if(!status){
       this->conpwdedit->setEchoMode(QLineEdit::Normal);//明文显示
        this->eyebtn->setIcon(QIcon(":/images/eyesopen.png"));//按钮图标切换

    }else{
        this->conpwdedit->setEchoMode(QLineEdit::Password);//密文显示
        this->eyebtn->setIcon(QIcon(":/images/eyesclose.png"));//按钮图标切换
    }
    status=!status;//按钮状态切换
}
