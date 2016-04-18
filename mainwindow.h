#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QMainWindow>
#include<QCloseEvent>
#include<QAction>
#include<QSettings>
#include<QSplitter>
#include<QDir>

#include"ConfigWidget.h"
#include"ImageEdit.h"

class q_runtime_error:public std::runtime_error
{
public:
    q_runtime_error(const QString& error):std::runtime_error(error.toUtf8().data()){}
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& fileName,QWidget *parent = 0);

    void imageChangedEvent();

private slots:
    void redo(){editWidget->redo();}
    void undo(){editWidget->undo();}
    void open();
    void saveAs();
    void save();
    void prev(){nextPrev(-1);}
    void next(){nextPrev(1);}
    void about();

private:
    void createWidgets();
    void createShortcuts();
    void createToolbar();
    void createMenu();

    void closeEvent(QCloseEvent* ev);

    void open(const QString& fileName);
    void updateTitle(){setWindowTitle(QString(isChanged()?"* ":"")+"Text Eater - "+currentFileName);}
    void nextPrev(int step);
    void baseSave(const QString& fileName);
    bool forceSave();
    QString fileDialogNameFilter()const;

    bool isChanged()const{return saveID!=editWidget->editID();}

    QSettings settings;
    QString currentFileName,lastFolder;
    QImage::Format format;
    long saveID=0;
    QSplitter* splitter=nullptr;
    ImageEdit* editWidget=nullptr;
    QAction* undoAction=nullptr;
    QAction* redoAction=nullptr;
};

#endif // MAINWINDOW_H
