#ifndef IMAGEEDIT_H
#define IMAGEEDIT_H
#include<QWidget>
#include<QPainter>
#include<QMouseEvent>

#include<memory>
#include<stdexcept>

#include"EditAction.h"

void errorHandler(QWidget* parent,const std::exception& error);

class ConfigWidget;
class MainWindow;
class ImageEdit:public QWidget
{
    friend class CreateBalloons;
    friend class FillBalloon;
public:
    ImageEdit(const ObjectMap::Config& config,ConfigWidget* _configWidget,MainWindow* _mainWindow,QWidget* parent);

    //property
    long editID()const noexcept{return editHistory[undoPos]->editID;}
    const ObjectMap::Config& config()const noexcept{return balloonsInfo->config;}
    ObjectMap& objectMap()noexcept{return balloonsInfo->objectMap;}
    const QImage& image()const noexcept{return _image;}
    bool canUndo()const{return undoPos;}
    bool canRedo()const{return undoPos+1<editHistory.size();}

    //change methods
    void setImage(QImage&& image);
    void setConfig(const ObjectMap::Config& config){
        addAction(std::make_unique<CreateBalloons>(this,_image,config,editID()),undoPos+1);
    }
    void redo()noexcept;
    void undo()noexcept;
private:
    void paintEvent(QPaintEvent*){QPainter(this).drawImage(0,0,showImage);}
    void mouseMoveEvent(QMouseEvent* ev){setActiveBalloon(objectMap().balloon(ev->pos()));}
    void mousePressEvent(QMouseEvent* ev);
    void leaveEvent(QEvent*){resetActiveBalloon();}

    void setActiveBalloon(const Balloon* balloon,bool forceUpdate=false)noexcept;
    void resetActiveBalloon()noexcept{setActiveBalloon(nullptr,true);}
    void addAction(std::unique_ptr<EditAction> action,int pos);

    QImage _image,showImage;
    std::vector<std::unique_ptr<EditAction> > editHistory;
    CreateBalloons* balloonsInfo=nullptr;
    size_t undoPos=0;
    long nextEditID=0;
    const Balloon* activeBalloon=nullptr;
    ConfigWidget* configWidget=nullptr;
    MainWindow* mainWindow=nullptr;
};

#endif // IMAGEEDIT_H
