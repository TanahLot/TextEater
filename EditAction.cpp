#include"mainwindow.h"

FillBalloon::FillBalloon(ImageEdit *parent, const Balloon *balloon, long editID):
    EditAction(editID,parent),
    pointList(balloon->pointList())
{
    pointColors.reserve(pointList.size());
    for(auto&point:pointList)
        pointColors.push_back(parent->_image.pixel(point));
}

void FillBalloon::redo() noexcept
{
    assert(pointList.size() == pointColors.size());

    for(auto&point:pointList)
        parent->_image.setPixel(point,qRgb(255,255,255));
    parent->setActiveBalloon(parent->activeBalloon,true);
    parent->mainWindow->imageChangedEvent();
}

void FillBalloon::undo() noexcept
{
    assert(pointList.size() == pointColors.size());

    auto pixelPos=pointColors.begin();
    for(auto&point:pointList)
        parent->_image.setPixel(point,*pixelPos++);
    parent->setActiveBalloon(parent->activeBalloon,true);
    parent->mainWindow->imageChangedEvent();
}

CreateBalloons::CreateBalloons(ImageEdit *parent, const QImage &image, const ObjectMap::Config &_config, long editID):
    EditAction(editID,parent),
    config(_config),
    objectMap(image,_config),
    oldBalloonInfo(parent->balloonsInfo){}

void CreateBalloons::redo() noexcept
{
    parent->balloonsInfo=this;
    parent->resetActiveBalloon();
    parent->configWidget->setConfig(config);
}

void CreateBalloons::undo() noexcept
{
    parent->balloonsInfo=oldBalloonInfo;
    parent->resetActiveBalloon();
    parent->configWidget->setConfig(oldBalloonInfo->config);
}
