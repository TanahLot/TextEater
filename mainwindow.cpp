#include "mainwindow.h"

#include<QScrollArea>
#include<QGridLayout>

#include<QStyle>
#include<QToolBar>
#include<QMenu>
#include<QMenuBar>

#include<QFileDialog>
#include<QMessageBox>

#include<QImageReader>
#include<QImageWriter>

#include<QShortcut>
#include<QKeySequence>
#include<QFileInfo>

MainWindow::MainWindow(const QString &fileName, QWidget *parent)
    : QMainWindow(parent),
      settings("Tanah Lot","Text Eater"),
      lastFolder(settings.value("lastFolder").toString())
{
    createWidgets();
    createShortcuts();
    createToolbar();
    createMenu();
    updateTitle();

    resize(settings.value("Window/Width",500).toInt(),
           settings.value("Window/Height",500).toInt());

    try
    {
        if(!fileName.isEmpty())
            open(fileName);
    }
    catch(const std::exception& error){errorHandler(this,error);}
}

void MainWindow::createWidgets()
{
    ObjectMap::Config config(settings.value("maxTextWidth",200).toInt(),
                             settings.value("maxTextHeight",200).toInt(),
                             settings.value("whiteLevel",128+64).toInt());

    //central widget
    QWidget* centralWidget=new QWidget(this);
    QGridLayout* baseLayout=new QGridLayout(centralWidget);
    splitter=new QSplitter(Qt::Horizontal,centralWidget);

    //left part
    ConfigWidget* configWidget=new ConfigWidget(centralWidget);
    splitter->addWidget(configWidget);
    splitter->setStretchFactor(0,0);

    //right part
    QScrollArea* scrollArea=new QScrollArea(splitter);
    editWidget=new ImageEdit(config,configWidget,this,scrollArea);
    scrollArea->setWidget(editWidget);
    splitter->addWidget(scrollArea);
    baseLayout->addWidget(splitter);
    splitter->setStretchFactor(1,1);

    if(settings.contains("Window/ConfigWidgetSize") && settings.contains("Window/ImageEditWidgetSize"))
        splitter->setSizes({settings.value("Window/ConfigWidgetSize",100).toInt(),
                            settings.value("Window/ImageEditWidgetSize",100).toInt()});

    setCentralWidget(centralWidget);
}

void MainWindow::createShortcuts()
{
    new QShortcut(QKeySequence("ctrl+y"),this,SLOT(redo()));
    new QShortcut(QKeySequence("ctrl+shift+z"),this,SLOT(redo()));
    new QShortcut(QKeySequence("ctrl+z"),this,SLOT(undo()));
}

void MainWindow::createToolbar()
{
    QToolBar* toolBar=new QToolBar(this);
    addToolBar(toolBar);
    toolBar->addAction(style()->standardIcon(QStyle::SP_DialogOpenButton),tr("Open"),this,SLOT(open()));
    toolBar->addAction(style()->standardIcon(QStyle::SP_DialogSaveButton),tr("Save"),this,SLOT(save()));
    toolBar->addAction(style()->standardIcon(QStyle::SP_ArrowLeft),tr("Prev"),this,SLOT(prev()));
    toolBar->addAction(style()->standardIcon(QStyle::SP_ArrowRight),tr("Next"),this,SLOT(next()));
}

void MainWindow::createMenu()
{
    QMenu* file=menuBar()->addMenu(tr("&File"));
    file->addAction(tr("&Open"),this,SLOT(open()));
    file->addAction(tr("&Save"),this,SLOT(save()));
    file->addAction(tr("S&ave as"),this,SLOT(saveAs()));

    QMenu* edit=menuBar()->addMenu(tr("&Edit"));
    undoAction=edit->addAction(tr("&Undo"),this,SLOT(undo()));
    redoAction=edit->addAction(tr("&Redo"),this,SLOT(redo()));
    undoAction->setDisabled(true);
    redoAction->setDisabled(true);

    menuBar()->addAction(tr("About"),this,SLOT(about()));
}


void MainWindow::imageChangedEvent()
{
    updateTitle();
    undoAction->setDisabled(!editWidget->canUndo());
    redoAction->setDisabled(!editWidget->canRedo());
}

QString MainWindow::fileDialogNameFilter()const
{
    QString filter=tr("Images (");
    QList<QByteArray> formats=QImageReader::supportedImageFormats();
    for(auto&format:formats)
        filter+="*."+format+" ";
    filter.chop(1);
    filter+=tr(");;All files (*.*)");
    return filter;
}

void MainWindow::open()
try
{
    QString fileName=QFileDialog::getOpenFileName(this,tr("Open image"),lastFolder,fileDialogNameFilter());
    if(!fileName.isNull())
        open(fileName);
}
catch(const std::exception& error){errorHandler(this,error);}

void MainWindow::saveAs()
try
{
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save image"),lastFolder,fileDialogNameFilter());
    if(!fileName.isNull())
        baseSave(fileName);
}
catch(const std::exception& error){errorHandler(this,error);}

void MainWindow::save()
try
{
    baseSave(currentFileName);
}
catch(const std::exception& error){errorHandler(this,error);}

void MainWindow::about()
{
    QMessageBox::about(this,tr("About"),
                       tr("Text Eater - program for remove text from balloons.<br>"
                          "Website and source - "
                          "<a href=\"https://github.com/textEater\">https://github.com/textEater</a>"));
}

void MainWindow::nextPrev(int step)
try
{
    //create nameFilters
    QList<QByteArray> formats=QImageReader::supportedImageFormats();
    QStringList nameFilters;
    for(auto& format:formats)
        nameFilters.push_back("*."+format);

    //get list of image files
    QDir dir(lastFolder);
    auto fileList=dir.entryInfoList(nameFilters,QDir::Files,QDir::Name);

    //find next or prev image
    if(!fileList.empty())
    {
        const int filePos=fileList.indexOf(currentFileName);
        auto& nextFile=filePos==-1?
                    fileList.front():
                    fileList[(fileList.size()+filePos+step)%fileList.size()];
        open(nextFile.absoluteFilePath());
    }
}
catch(const std::exception& error){errorHandler(this,error);}

void MainWindow::open(const QString &fileName)
{
    if(forceSave())
    {
        //read image
        QImageReader reader(fileName);
        reader.setDecideFormatFromContent(true);//for png files with jpg extension
        QImage image=reader.read();
        if(image.isNull())
            throw q_runtime_error(tr("Can't open ")+fileName+": "+reader.errorString());
        const QImage::Format newFormat=image.format();

        //convert to RGB32
        image=image.convertToFormat(QImage::Format_RGB32);
        if(image.isNull())
            throw q_runtime_error(tr("Can't convert image to RGB32 format"));

        //show new image
        editWidget->setImage(std::move(image));
        currentFileName=fileName;
        format=newFormat;
        saveID=editWidget->editID();
        updateTitle();

        //update config
        lastFolder=QFileInfo(fileName).absoluteDir().absolutePath();
        settings.setValue("lastFolder",lastFolder);
    }
}

void MainWindow::baseSave(const QString &fileName)
{
    QImageWriter writer(fileName);
    QImage image=editWidget->image().convertToFormat(format);
    if(image.isNull())
        throw std::bad_alloc();
    if(!writer.write(image))
        throw q_runtime_error("Can't save "+fileName+" file: "+writer.errorString());

    saveID=editWidget->editID();
    currentFileName=fileName;
    updateTitle();
}

bool MainWindow::forceSave()
{
    if(isChanged())
    {
        switch(QMessageBox::question(this,
                                     " ",
                                     tr("Save changes?"),
                                     QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel))
        {
        case QMessageBox::Save:
            baseSave(currentFileName);
            return true;
        case QMessageBox::Discard:
            return true;
        case QMessageBox::Cancel:
            return false;
        default:
            //WTF?
            assert(false);
            return false;
        }
    }else
        return true;
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    try
    {
        if(forceSave())
        {
            ev->accept();
            settings.setValue("maxTextWidth",editWidget->config().maxTextWidth);
            settings.setValue("maxTextHeight",editWidget->config().maxTextHeight);
            settings.setValue("whiteLevel",editWidget->config().whiteLevel);
            settings.setValue("Window/Width",width());
            settings.setValue("Window/Height",height());
            settings.setValue("Window/ConfigWidgetSize",splitter->sizes()[0]);
            settings.setValue("Window/ImageEditWidgetSize",splitter->sizes()[1]);
        }else
            ev->ignore();
    }
    catch(const std::exception& error)
    {
        errorHandler(this,error);
        ev->ignore();
    }
}
