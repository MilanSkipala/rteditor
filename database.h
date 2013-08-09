#ifndef DATABASE_H
#define DATABASE_H

#include <QFile>
#include <QTextStream>
#include "includeHeaders.h"
#include "partsRelated.h"

class Database
{
    QMap<QString,ProductLine*> * productLines;
    QMap<QString,QGraphicsScene*> * scenes;
    QString *currentProductLine;
    QString *currentItem;
public:
    Database();

    ProductLine * findProductLineByName(QString &name) const;

    int setCurrentProductLine(QString &name);
    QString * getCurrentProductLinesName();
    ProductLine * getCurrentProductLine();
    QMap<QString,ProductLine*>::Iterator getDatabaseIterator();

    ModelItem * findModelItemByName(QString &manufactName, QString &partName) const;
    ModelItem * getNextModelItem(QString &manufactName) const;

    QGraphicsScene * findSceneByString(QString &scaleAndname) const;

    int addProductLine(ProductLine * manufact);
    int size();
    //int addModelItem(QString manufactName, ModelItem * itemToAdd);//ProductLine's name
    //int addModelItem(ModelItem *itemToAdd);


};

#endif // DATABASE_H
