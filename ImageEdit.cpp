#include<QMessageBox>
#include<QCoreApplication>
#include"ImageEdit.h"
#include"ConfigWidget.h"

inline void errorHandler(QWidget* parent,const std::exception& error){
    QMessageBox::critical(parent,QCoreApplication::translate("errorHandler","Error"),QString::fromUtf8(error.what()));
}

ImageEdit::ImageEdit(const ObjectMap::Config &config,
                     ConfigWidget *_configWidget,
                     MainWindow *_mainWindow,
                     QWidget *parent):
    QWidget(parent),
    configWidget(_configWidget),
    mainWindow(_mainWindow)
{
    setMouseTracking(true);
    addAction(std::make_unique<CreateBalloons>(this,_image,config,nextEditID++),0);
    configWidget->imageEdit=this;
}

void ImageEdit::addAction(std::unique_ptr<EditAction> action, int pos)
{
    editHistory.resize(pos+1);
    editHistory.back()=std::move(action);
    undoPos=pos;

    editHistory.back()->redo();
}

void ImageEdit::setImage(QImage &&image)
{
    //editHistory have space to one value
    assert(!editHistory.empty());

    //this part can throw exception. In this case widget must remain unchanged
    QImage newShowImage=image.copy();//for avoid shallow copy
    auto firstAction=std::make_unique<CreateBalloons>(this,image,config(),nextEditID++);
    if(newShowImage.isNull())
        throw std::bad_alloc();

    //this part can't throw exception
    _image=std::move(image);
    showImage=std::move(newShowImage);
    activeBalloon=nullptr;
    addAction(std::move(firstAction),0);
    resize(_image.size());
}

void ImageEdit::redo()noexcept
{
    if(canRedo())
        editHistory[++undoPos]->redo();
}

void ImageEdit::undo()noexcept
{
    if(canUndo())
        editHistory[undoPos--]->undo();
}

void ImageEdit::setActiveBalloon(const Balloon *balloon, bool forceUpdate)noexcept
{
    if(activeBalloon != balloon || forceUpdate)
    {
        //for avoid std::bad_alloc from copy-on-write mechanism or from image().copy()
        for(int x=0;x<image().width();++x)
            for(int y=0;y<image().height();++y)
                showImage.setPixel(x,y,image().pixel(x,y));

        activeBalloon=balloon;
        if(balloon)
        {
            for(auto&point:balloon->pointList())
                if((point.x() ^ point.y()) & 1)
                    showImage.setPixel(point,qRgb(255,0,0));
        }
        update();
    }
}

void ImageEdit::mousePressEvent(QMouseEvent *ev)
{
    try
    {
        Balloon*const balloon=objectMap().balloon(ev->pos());
        if(balloon && !balloon->isFilled())
        {
            addAction(std::make_unique<FillBalloon>(this,balloon,nextEditID++),undoPos+1);
            setActiveBalloon(balloon,true);
        }
    }
    catch(const std::exception& error){errorHandler(this,error);}
}
