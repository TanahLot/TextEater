#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H
#include<QWidget>
#include<QFormLayout>
#include<QLineEdit>
#include<QIntValidator>

#include"ObjectMap.h"

class ImageEdit;
class ConfigWidget:public QWidget
{
    Q_OBJECT
    friend class ImageEdit;
public:
    ConfigWidget(QWidget*parent):
        QWidget(parent),
        layout(new QFormLayout(this))
    {
        maxTextWidth=newIntField(tr("max text width"));
        maxTextHeight=newIntField(tr("max text height"));
        whiteLevel=newIntField(tr("white level"),255);
    }

    void setConfig(const ObjectMap::Config& config)
    {
        maxTextWidth->setText(QString::number(config.maxTextWidth));
        maxTextHeight->setText(QString::number(config.maxTextHeight));
        whiteLevel->setText(QString::number(config.whiteLevel));
    }

private slots:
    void returnPressed();

private:
    QLineEdit* newIntField(const QString& label, int maxValue=std::numeric_limits<int>::max())
    {
        QLineEdit*line=new QLineEdit(this);
        connect(line,SIGNAL(returnPressed()),SLOT(returnPressed()));
        line->setValidator(new QIntValidator(0,maxValue,line));
        layout->addRow(label,line);
        return line;
    }

    QFormLayout* layout;
    QLineEdit* maxTextWidth;
    QLineEdit* maxTextHeight;
    QLineEdit* whiteLevel;
    ImageEdit* imageEdit=nullptr;
};

#endif // CONFIGWIDGET_H
