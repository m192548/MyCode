#include "photolistwidget.h"

PhotoListWidget::PhotoListWidget(QWidget *parent) : QWidget(parent)
{
    initUi();
    initQSS();
    initConnect();
    initWin();

}
PhotoListWidget::~PhotoListWidget()
{

}
void PhotoListWidget::initUi()
{
    //左右窗口
    PhotoAndRightWidget=new QWidget(this);
    PhotoAndRightWidgetLayout=new QHBoxLayout(PhotoAndRightWidget);
    //显示9张图片的窗口
    photosWidget=new QWidget(PhotoAndRightWidget);
    photosWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    photosWidgetLayout=new QGridLayout(photosWidget);
    photoLab00=new QLabel("图片1",photosWidget);
    photoLab00->setObjectName("photoLab00");
    photoLab00->setAlignment(Qt::AlignCenter);//文字居中显示
    photoLab00->setMinimumSize(240, 180);
    photoLab00->installEventFilter(this);
    photoLab00->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    photoLab01=new QLabel("图片2",photosWidget);
    photoLab01->setObjectName("photoLab01");
    photoLab01->setAlignment(Qt::AlignCenter);//文字居中显示
    photoLab01->setMinimumSize(240, 180);
    photoLab01->installEventFilter(this);
    photoLab01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    photoLab02=new QLabel("图片3",photosWidget);
    photoLab02->setObjectName("photoLab02");
    photoLab02->setAlignment(Qt::AlignCenter);//文字居中显示
    photoLab02->setMinimumSize(240, 180);
    photoLab02->installEventFilter(this);
    photoLab02->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    photoLab10=new QLabel("图片4",photosWidget);
    photoLab10->setObjectName("photoLab10");
    photoLab10->setAlignment(Qt::AlignCenter);//文字居中显示
    photoLab10->setMinimumSize(240, 180);
    photoLab10->installEventFilter(this);
    photoLab10->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    photoLab11=new QLabel("图片5",photosWidget);
    photoLab11->setObjectName("photoLab11");
    photoLab11->setAlignment(Qt::AlignCenter);//文字居中显示
    photoLab11->setMinimumSize(240, 180);
    photoLab11->installEventFilter(this);
    photoLab11->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    photoLab12=new QLabel("图片6",photosWidget);
    photoLab12->setObjectName("photoLab12");
    photoLab12->setAlignment(Qt::AlignCenter);//文字居中显示
    photoLab12->setMinimumSize(240, 180);
    photoLab12->installEventFilter(this);
    photoLab12->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    photoLab20=new QLabel("图片7",photosWidget);
    photoLab20->setObjectName("photoLab20");
    photoLab20->setAlignment(Qt::AlignCenter);//文字居中显示
    photoLab20->setMinimumSize(240, 180);
    photoLab20->installEventFilter(this);
    photoLab20->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    photoLab21=new QLabel("图片8",photosWidget);
    photoLab21->setObjectName("photoLab21");
    photoLab21->setAlignment(Qt::AlignCenter);//文字居中显示
    photoLab21->setMinimumSize(240, 180);
    photoLab21->installEventFilter(this);
    photoLab21->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    photoLab22=new QLabel("图片9",photosWidget);
    photoLab22->setObjectName("photoLab22");
    photoLab22->setAlignment(Qt::AlignCenter);//文字居中显示
    photoLab22->setMinimumSize(240, 180);
    photoLab22->installEventFilter(this);
    photoLab22->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    photosWidgetLayout->addWidget(photoLab00,0,0);
    photosWidgetLayout->addWidget(photoLab01,0,1);
    photosWidgetLayout->addWidget(photoLab02,0,2);
    photosWidgetLayout->addWidget(photoLab10,1,0);
    photosWidgetLayout->addWidget(photoLab11,1,1);
    photosWidgetLayout->addWidget(photoLab12,1,2);
    photosWidgetLayout->addWidget(photoLab20,2,0);
    photosWidgetLayout->addWidget(photoLab21,2,1);
    photosWidgetLayout->addWidget(photoLab22,2,2);
    photosWidgetLayout->setSpacing(0);
    photosWidgetLayout->setMargin(0);
    //右侧视图:显示当前图片信息
    rightWidget=new QWidget(PhotoAndRightWidget);
    rightWidget->setFixedWidth(200);
    rightWidget->setObjectName("rightWidget");
    rightWidgetLayout=new QGridLayout(rightWidget);
    title_photoNameLab=new QLabel("图片名称:",rightWidget);
    title_photoByteLab=new QLabel("图片大小:",rightWidget);
    title_photoSizeLab=new QLabel("图片尺寸:",rightWidget);
    photoNameLab=new QLabel("name",rightWidget);
    photoByteLab=new QLabel("0kb",rightWidget);
    photoSizeLab=new QLabel("0x0",rightWidget);
    placeholderLab=new QLabel(rightWidget);

    title_photoNameLab->setFixedSize(100,30);
    title_photoByteLab->setFixedSize(100,30);
    title_photoSizeLab->setFixedSize(100,30);
    photoNameLab->setFixedSize(100,30);
    photoByteLab->setFixedSize(100,30);
    photoSizeLab->setFixedSize(100,30);

    rightWidgetLayout->addWidget(title_photoNameLab,0,0);
    rightWidgetLayout->addWidget(title_photoByteLab,1,0);
    rightWidgetLayout->addWidget(title_photoSizeLab,2,0);
    rightWidgetLayout->addWidget(photoNameLab,0,1);
    rightWidgetLayout->addWidget(photoByteLab,1,1);
    rightWidgetLayout->addWidget(photoSizeLab,2,1);
    rightWidgetLayout->addWidget(placeholderLab,3,0);
    rightWidgetLayout->setRowStretch(0,1);
    rightWidgetLayout->setRowStretch(1,1);
    rightWidgetLayout->setRowStretch(2,1);
    rightWidgetLayout->setRowStretch(3,8);
    rightWidgetLayout->setSpacing(0);
    rightWidgetLayout->setMargin(3);
    //把图片窗口和右边窗口添加到窗口
    PhotoAndRightWidgetLayout->addWidget(photosWidget);
    PhotoAndRightWidgetLayout->addWidget(rightWidget);
    PhotoAndRightWidgetLayout->setSpacing(0);
    PhotoAndRightWidgetLayout->setMargin(0);

    //底部视图
    bottomWidget=new QWidget(this);
    bottomWidget->resize(this->width(),40);
    bottomWidgetLayout=new QHBoxLayout(bottomWidget);
    prePageBtn=new QPushButton("<",bottomWidget);
    nextPageBtn=new QPushButton(">",bottomWidget);
    countLabel=new QLabel("0/0",bottomWidget);
    bottomWidgetLayout->addStretch();
    bottomWidgetLayout->addWidget(prePageBtn);
    bottomWidgetLayout->addWidget(nextPageBtn);
    bottomWidgetLayout->addWidget(countLabel);
    bottomWidgetLayout->addStretch();
    bottomWidgetLayout->addStretch();


    //把窗口添加到总布局
    totalWidgetLayout=new QVBoxLayout(this);
    totalWidgetLayout->addWidget(PhotoAndRightWidget);
    totalWidgetLayout->addWidget(bottomWidget);
    totalWidgetLayout->setSpacing(0);
    totalWidgetLayout->setMargin(0);




}
void PhotoListWidget::initConnect()
{

}
void PhotoListWidget::initWin()
{

}
void PhotoListWidget::initQSS()
{
    //加载外部qss文件
    QFile file(":/qss/photolistwidget.qss");//资源文件路径
    if(file.open(QFile::ReadOnly))
    {
        QString styleSheet=QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    }
}
void PhotoListWidget::findPhotos()
{

}
void PhotoListWidget::setphotoDir(const QString& photoDir)
{

}
void PhotoListWidget::onPrePageBtnClicked()
{

}
void PhotoListWidget::onNextPageBtnClicked()
{

}
