#include "loginwidget.h"
#include "myview.h"
LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent)
{
    this->regWidget = new RegisterWidget();
    countTimer=new QTimer(this);//作为Qtimer的父对象
    countdowntime=0;
    srand(time(NULL));
    this->initWin();
    this->initUI();
    this->initQss();
    this->initconnect();
    pwdstatus=false;
}

void LoginWidget::initUI()
{
    //正则表达式
    QRegExp regExp("[A-Za-z0-9_]+");
    QRegExpValidator *validator=new QRegExpValidator(regExp,this);
    this->titlelab=new QLabel("用户登录",this);//标题设置
    this->titlelab->setStyleSheet("font-size:45px;color:white");
    this->titlelab->setGeometry(430,70,300,100);
    //用户账号标签设置
    this->useraccountlab=new QLabel("账号:",this);
    this->useraccountlab->setGeometry(300,180,130,50);
    //用户账号编辑框设置
    this->useraccountedit=new QLineEdit(this);
    this->useraccountedit->setGeometry(370,185,330,35);
    //用户密码标签设置
    this->userpwdlab=new QLabel("密码:",this);
    this->userpwdlab->setGeometry(300,250,130,50);
    //用户密码编辑框设置
    this->userpwdedit=new QLineEdit(this);
    this->userpwdedit->setGeometry(370,255,330,35);
    this->userpwdedit->setValidator(validator);//只允许输入数字 字母 下划线
    //用户明密文按钮显示
    this->eyesbtn=new QPushButton(this);
    this->eyesbtn->setGeometry(670,255,30,35);
    this->eyesbtn->setStyleSheet("border:none;");
    this->eyesbtn->setIcon(QIcon(":/images/eyesclose.png"));
    this->eyesbtn->setIconSize(QSize(20,20));
    pwdstatus=false;//初始为密文
    //验证码标签设置
    this->conpwdlab=new QLabel("验证码:",this);
    this->conpwdlab->setGeometry(300,320,130,50);
    //验证码编辑框设置
    this->conpwdedit=new QLineEdit(this);
    this->conpwdedit->setGeometry(400,325,150,50);
    this->conpwdedit->setValidator(validator);//只允许输入数字 字母 下划线
    //验证码按钮设置
    this->conpwdbtn=new QPushButton("获取验证码",this);
    this->conpwdbtn->setGeometry(600,325,150,50);
    //验证码倒计时标签
    this->countDownLabel = new QLabel("", this);
    this->countDownLabel->setGeometry(610, 300, 120, 30);
    this->countDownLabel->hide(); // 初始隐藏，点击按钮后显示
    //确定按钮设置
    this->dologinbtn=new QPushButton("确定",this);
    this->dologinbtn->setGeometry(250,430,100,50);
    //注册按钮设置
    this->goregisterbtn=new QPushButton("注册",this);
    this->goregisterbtn->setGeometry(450,430,100,50);
    //返回按钮设置
    this->returnbtn=new QPushButton("取消",this);
    this->returnbtn->setGeometry(650,430,100,50);


}

void LoginWidget::initconnect()
{
    connect(this->dologinbtn,SIGNAL(clicked()),this,SLOT(dologin()));//登录按钮槽函数
    connect(this->conpwdbtn,SIGNAL(clicked()),this,SLOT(getcode()));//验证码槽函数
    connect(this->countTimer, SIGNAL(timeout()), this, SLOT(updatecount()));//计时器
    connect(this->eyesbtn,SIGNAL(clicked()),this,SLOT(changepwdvisible()));//明密文切换槽函数
    connect(this->returnbtn,SIGNAL(clicked()),this,SLOT(doreturn()));//返回按钮槽函数
    connect(this->goregisterbtn,SIGNAL(clicked()),this,SLOT(gotoregister()));//点击按钮注册
    connect(this->regWidget,SIGNAL(backtologin()),this,SLOT(showlogin()));

}

void LoginWidget::initWin()
{
    this->setWindowTitle("登录界面");
    this->setWindowIcon(QIcon(":/images/icon.png"));
    this->resize(1042,600);
}

void LoginWidget::initQss()
{
    //加载外部qss文件
    QFile file(":/qss/login.qss");//资源文件路径
    if(file.open(QFile::ReadOnly))
    {
        QString styleSheet=QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }

}
void LoginWidget::clearForm()
{   //清空所有编辑框内容
    this->useraccountedit->clear();
    this->userpwdedit->clear();
    this->conpwdedit->clear();
}

void LoginWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    //背景自适应
    QPainter paint(this);
    paint.drawPixmap(this->rect(),QPixmap(":/images/background2.png"));
}

void LoginWidget::getcode()
{
    int randomCode=rand()%9000+1000;//随机生成验证码四位数
    this->conpwdbtn->setText(QString::number(randomCode));
    countdowntime=60;//倒计时1Min刷新验证码
    conpwdbtn->setEnabled(false);//按钮不可再点击
    countTimer->start(1000);
    countDownLabel->show();//生成验证码刷新时间标签
}

void LoginWidget::updatecount()
{
    countdowntime--;
    countDownLabel->setText(QString("%1s后重发").arg(countdowntime));//1s刷新验证码可获取时间
    if(countdowntime==0)
    { //计时器时间为0
        countTimer->stop();//停止计时器
        conpwdbtn->setText("获取验证码");//显示验证码标签
        conpwdbtn->setEnabled(true);//可重新获取验证码
    }
}

void LoginWidget::changepwdvisible()
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

void LoginWidget::gotoregister()
{
    this->hide();
    regWidget->show();
}

void LoginWidget::dologin()
{
    QString useraccount=this->useraccountedit->text();
    QString password=this->userpwdedit->text();
    QString concode=this->conpwdedit->text();
    QString inputcode=this->conpwdbtn->text();//获取编辑框的文本数据
    if(useraccount.isEmpty()||password.isEmpty()){
        QMessageBox::warning(this,"登录提示","用户名或密码不能为空!");
        return;
    }
    if(useraccount.length()<6||password.length()<6){
        QMessageBox::warning(this,"登录提示","用户名或密码不能小于六位数!");
        this->clearForm();
        return;
    }
    if(concode.isEmpty()){
        QMessageBox::warning(this,"登录提示","验证码不能为空!");
        return;
    }
    if(concode!=inputcode){
        QMessageBox::warning(this,"登录验证","验证码不一致,请重新输入!");
        this->clearForm();
        return;
    }
    int res= UserController::getinstance()->doLogin(useraccount,password);
    switch (res) {
         case 0: {
             // 登录成功：更新用户状态为在线
             QString sql = QString("UPDATE tbl_user SET user_status=1 WHERE user_account='%1';").arg(useraccount);
             int updateRes = SingletonDB::getInstance()->execsql(sql);
             if (updateRes == 0) {
                 QMessageBox::information(this, "登录界面", "登录成功!");
                 emit loginsuccess(UserController::getinstance()->getcurrentusername());//发送登录成功信号
                 clearForm();
             } else {
                 QMessageBox::warning(this, "登录界面", "服务器异常，请稍后重试!");
             }
             break;
         }
         case 1:
             QMessageBox::warning(this, "登录界面", "用户名或密码错误!");
             break;
         case -1:
             QMessageBox::warning(this, "登录界面", "数据库操作失败!");
             break;
         default:
             QMessageBox::warning(this, "登录界面", "未知错误!");
     }


}
void LoginWidget::doreturn()
{
    QMessageBox::information(this,"登录界面","返回成功!");
    emit returntoplay();
    this->close();

}
void LoginWidget::showlogin()
{
    this->regWidget->hide();
    this->show();
}


