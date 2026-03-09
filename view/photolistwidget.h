#ifndef PHOTOLISTWIDGET_H
#define PHOTOLISTWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
class PhotoListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PhotoListWidget(QWidget *parent = nullptr);
    ~PhotoListWidget();
    void initConnect();
    void initUi();
    void initQSS();
    void initWin();
    void findPhotos();
    void setphotoDir(const QString& photoDir);

signals:

public slots:
    void onPrePageBtnClicked();
    void onNextPageBtnClicked();

private:
    QWidget* photosWidget;
    QWidget* rightWidget;
    QWidget* bottomWidget;
    QWidget* PhotoAndRightWidget;

    QGridLayout* photosWidgetLayout;
    QGridLayout* rightWidgetLayout;
    QHBoxLayout* bottomWidgetLayout;
    QHBoxLayout* PhotoAndRightWidgetLayout;
    QVBoxLayout* totalWidgetLayout;


    QPushButton* prePageBtn,*nextPageBtn;
    QLabel* countLabel;
    QLabel* title_photoNameLab,*title_photoByteLab,*title_photoSizeLab;
    QLabel* photoNameLab,*photoByteLab,*photoSizeLab;
    QLabel* placeholderLab;//占位标签，用来伸缩空间
    QLabel* photoLab00,*photoLab01,*photoLab02;
    QLabel* photoLab10,*photoLab11,*photoLab12;
    QLabel* photoLab20,*photoLab21,*photoLab22;

private:
    QList<QFileInfo> photoInfos;
    int currentPage;        //当前页
    int currentPhoto;       //当前页中的当前照片
    QString photoDir;
};

#endif // PHOTOLISTWIDGET_H
