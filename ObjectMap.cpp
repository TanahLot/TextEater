#include"ObjectMap.h"

void ImageObject::push(const QPoint &point)
{
    minx=std::min(minx,point.x());
    miny=std::min(miny,point.y());
    maxx=std::max(maxx,point.x());
    maxy=std::max(maxy,point.y());
    _pointList.emplace_back(point);
}

ObjectMap::ObjectMap(const QImage &src, const Config &config):
    map(src.width()*src.height(),PixelInfo(nullptr,white)),
    width(src.width()),
    height(src.height())
{
    std::list<ImageObject> blackObjects;
    createMonochromeImage(src,config.whiteLevel);
    findObjects(black,blackObjects);
    removeSmallObjects(blackObjects,config.maxTextWidth,config.maxTextHeight);
    findObjects(white,balloonList);
    removeBlackObjects(blackObjects);
}

void ObjectMap::createMonochromeImage(const QImage &src, int whiteLevel)
{
    for(int x=0; x < width; ++x)
        for(int y=0; y < height; ++y)
            if(qGray(src.pixel(x,y)) < whiteLevel)
            {
                for(int nearx=x-1; nearx <= x+1; ++nearx)
                    for(int neary=y-1; neary <= y+1; ++neary)
                        if(isValidPixel(nearx,neary))
                            color(nearx,neary)=black;
            }
}

void ObjectMap::findObjects(Color objectColor, std::list<ImageObject> &out)
{
    //non recursive algorithm, for avoid stack overflow
    std::vector<QPoint> stack;

    //search first point of object
    for(int x=0; x < width; ++x)
        for(int y=0; y < height; ++y)
            if(color(x,y) == objectColor && !object(x,y))
            {
                //create new object
                out.emplace_back();
                ImageObject& newObject=out.back();

                //find all points of new object
                stack.emplace_back(x,y);
                while(!stack.empty())
                {
                    QPoint point=stack.back();
                    stack.pop_back();

                    if(isValidPixel(point) && color(point) == objectColor && !object(point))
                    {
                        newObject.push(point);
                        object(point)=&newObject;
                        stack.emplace_back(point.x()+1,point.y());
                        stack.emplace_back(point.x()-1,point.y());
                        stack.emplace_back(point.x(),point.y()+1);
                        stack.emplace_back(point.x(),point.y()-1);
                    }
                }
            }
}

void ObjectMap::removeSmallObjects(std::list<ImageObject> &blackObjects, int maxObjectWidth, int maxObjectHeight)
{
    for(auto&object:blackObjects)
    {
        const bool isSmallObject=object.width() <= maxObjectWidth &&
                object.height() <= maxObjectHeight;
        const bool touchHorizontalBorder=!object.minx || object.maxx == width-1;
        const bool touchVerticalBorder=!object.miny || object.maxy == height-1;

        if(isSmallObject && !touchHorizontalBorder && !touchVerticalBorder)
        {
            for(auto&point:object.pointList())
                pixel(point)=PixelInfo(nullptr,white);
        }else
            object.toRemove=true;
    }
}

void ObjectMap::removeBlackObjects(const std::list<ImageObject> &blackObjects)
{
    for(auto&object:blackObjects)
        if(object.toRemove)
            for(auto&point:object.pointList())
                pixel(point)=PixelInfo(nullptr,white);
}
