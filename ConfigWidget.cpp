#include"ConfigWidget.h"
#include"ImageEdit.h"

void ConfigWidget::returnPressed()
{
    try
    {
        ObjectMap::Config config(maxTextWidth->text().toInt(),
                                 maxTextHeight->text().toInt(),
                                 whiteLevel->text().toInt());
        imageEdit->setConfig(config);
    }
    catch(const std::exception& error){errorHandler(this,error);}
}
