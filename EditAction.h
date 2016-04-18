#ifndef EDITACTION_H
#define EDITACTION_H
#include"ObjectMap.h"

class ImageEdit;
class EditAction
{
public:
    EditAction(long _editID,ImageEdit* _parent):
        editID(_editID),parent(_parent){}
    virtual~EditAction(){}
    virtual void redo()noexcept=0;
    virtual void undo()noexcept=0;

    const long editID;
    ImageEdit*const parent;
};

class FillBalloon:public EditAction
{
public:
    FillBalloon(ImageEdit* parent,const Balloon* balloon, long editID);

    void redo()noexcept;
    void undo()noexcept;

private:
    const std::vector<QPoint>&  pointList;
    std::vector<QRgb> pointColors;
};

class CreateBalloons:public EditAction
{
public:
    CreateBalloons(ImageEdit* parent, const QImage& image, const ObjectMap::Config& _config,long editID);
    void redo() noexcept;
    void undo() noexcept;

    ObjectMap::Config config;
    ObjectMap objectMap;

private:
    CreateBalloons*const oldBalloonInfo;
};
#endif // EDITACTION_H
