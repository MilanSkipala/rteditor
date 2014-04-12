#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include "includeHeaders.h"
#include <QDialog>
#include <QListWidget>

#include "database.h"

class Database;

class SideBarWidget : public QFrame//QWidget
{
    Q_OBJECT

    QGridLayout * layout;
    QPushButton * addButton;
    QPushButton * delButton;
    QComboBox * prodLineCBox;
    Database * database;
    QMenu * contextMenu;
    QGraphicsScene * currentScene;
    QGraphicsView * graphicsView;

    QDialog * addDialog;
    QDialog * delDialog;
    QListWidget * listWidget;
    QListWidget * listWidgetD;

public:

    SideBarWidget(QString * lang, Database * db, QWidget * parent = 0);
    SideBarWidget(QString * lang, Database * db, QMenu * context, QWidget * parent = 0); ///DO NOT USE THIS ONE
    Database * getDatabase() const;

    QDialog * initAddDialog();
    QDialog * initDelDialog();


    QGraphicsScene * getCurrentScene() const;

    QComboBox *getProductLines();
    void resetSideBar();

    int setInventoryState(QTextStream & input);
    void printInventoryState(QTextStream & output);

protected:
    void paintEvent(QPaintEvent * evt);
    void contextMenuEvent(QContextMenuEvent * evt);
    void mousePressEvent(QMouseEvent * evt) ;
    void mouseMoveEvent(QMouseEvent * evt);

public slots:
    void showAddDialog();
    void showDelDialog();
    void closeAddDialog();
    void closeDelDialog();
    void importSelection();
    void deleteSelection();

};

#endif // SIDEBARWIDGET_H
