#ifndef OBJECTMAP_H
#define OBJECTMAP_H
#include<vector>
#include<list>
#include<utility>
#include<limits>
#include<cassert>

#include<QImage>

class ObjectMap;
class ImageObject
{
    friend class ObjectMap;
public:
    const std::vector<QPoint>& pointList()const noexcept{return _pointList;}
    bool isFilled()const{return _isFilled;}
    void isFilled(bool value){_isFilled=value;}

private:
    void push(const QPoint& point);
    int width()const noexcept{return maxx-minx+1;}
    int height()const noexcept{return maxy-miny+1;}

    std::vector<QPoint> _pointList;
    int minx=std::numeric_limits<int>::max();
    int miny=std::numeric_limits<int>::max();
    int maxx=std::numeric_limits<int>::min();
    int maxy=std::numeric_limits<int>::min();
    bool _isFilled=false;
    bool toRemove=false;
};
typedef ImageObject Balloon;

class ObjectMap
{
public:
    struct Config
    {
        Config(int _maxTextWidth,int _maxTextHeight,int _whiteLevel)noexcept:
            maxTextWidth(_maxTextWidth),maxTextHeight(_maxTextHeight),whiteLevel(_whiteLevel){}

        int maxTextWidth,maxTextHeight,whiteLevel;
    };

    ObjectMap(const QImage& src, const Config& config);
    ObjectMap(const ObjectMap&)=delete;
    Balloon*balloon(const QPoint& point)noexcept{return isValidPixel(point)?object(point):nullptr;}

private:
    enum Color{white,black};
    typedef std::pair<ImageObject*,Color> PixelInfo;

    bool isValidPixel(int x,int y)const noexcept{return x>=0 && x<width && y>=0 && y<height;}
    bool isValidPixel(const QPoint& point)const noexcept{return isValidPixel(point.x(),point.y());}

    PixelInfo& pixel(int x,int y)noexcept{assert(isValidPixel(x,y));return map[x*height+y];}
    PixelInfo& pixel(const QPoint& point)noexcept{return pixel(point.x(),point.y());}
    const PixelInfo& pixel(int x,int y)const noexcept{assert(isValidPixel(x,y));return map[x*height+y];}
    const PixelInfo& pixel(const QPoint& point)const noexcept{return pixel(point.x(),point.y());}

    ImageObject*& object(int x,int y)noexcept{return pixel(x,y).first;}
    ImageObject*& object(const QPoint& point)noexcept{return pixel(point).first;}
    const ImageObject* object(int x,int y)const noexcept{return pixel(x,y).first;}
    const ImageObject* object(const QPoint& point)const noexcept{return pixel(point).first;}

    Color& color(int x,int y)noexcept{return pixel(x,y).second;}
    Color& color(const QPoint& point)noexcept{return pixel(point).second;}
    Color color(int x,int y)const noexcept{return pixel(x,y).second;}
    Color color(const QPoint& point)const noexcept{return pixel(point).second;}

    void createMonochromeImage(const QImage& src,int whiteLevel);
    void findObjects(Color objectColor,std::list<ImageObject>& out);
    void removeSmallObjects(std::list<ImageObject>& blackObjects, int maxObjectWidth, int maxObjectHeight);
    void removeBlackObjects(const std::list<ImageObject>& blackObjects);

    std::vector<PixelInfo> map;
    std::list<ImageObject> balloonList;
    const int width;
    const int height;
};
#endif // OBJECTMAP_H
