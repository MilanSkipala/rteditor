
#include "QFormLayout"
#include "QListWidget"
#include "QSpinBox"
#include "QTextEdit"
#include "application.h"
#include "partsRelated.h"
#include "globalVariables.h"
#include "itemTypeEnums.h"

ModelFragmentWidget::ModelFragmentWidget()
{
    this->endPoints = new QList<QPoint *>();
    this->endPointsAngles = new QList <qreal>();
    this->fragmentItems = new QList <ModelItem*>();
    this->lines = new QList <ProductLine*>();
    ////this->startPoint = NULL;
    ////this->startPointAngle = 0;
    this->infoDialog = NULL;


}

ModelFragmentWidget::ModelFragmentWidget(ModelItem* item, ProductLine* line, QPoint * startPt,QList<QPoint *> *endPts,QList<qreal> * endPtAngles)
{
    this->endPoints = endPts;
    this->endPoints->push_front(startPt);
    this->endPointsAngles = endPtAngles;
    this->endPointsAngles->push_front(0);
    this->fragmentItems = new QList <ModelItem*>();
    this->lines = new QList <ProductLine*>();
    ////this->startPoint = startPt;
    ////this->startPointAngle = 0;
    this->infoDialog = NULL;

    //ModelFragment constructor can't allocate new ModelItem (which will be then inserted in the scene) - it has to be done in the doubleclickevent
    //because of left/right turn rotation
    //ModelItem * it = new ModelItem(*item);
    this->transformMatrix = new QTransform();

    this->fragmentItems->push_back(item);
    this->lines->push_back(line);
    item->setParentFragment(this);

}


QList <ModelItem*> * ModelFragmentWidget::getFragmentItems() const
{
    return this->fragmentItems;
}

QList<ProductLine*> * ModelFragmentWidget::getProductLines() const
{
    return this->lines;
}

/** **
QPoint * ModelFragmentWidget::getStartPoint() const
{
    return this->startPoint;
}
*/
QList<QPoint *> * ModelFragmentWidget::getEndPoints() const
{
    return this->endPoints;
}

/** **
qreal ModelFragmentWidget::getStartPointAngle()
{
    return this->startPointAngle;
}
*/
QList<qreal> * ModelFragmentWidget::getEndPointsAngles() const
{
    return this->endPointsAngles;
}

int ModelFragmentWidget::addFragmentItems(QList <ModelItem*> * listToAppend)
{
    if (listToAppend==NULL)
        return 1;
    this->fragmentItems->append(*listToAppend);
    return 0;
}

int ModelFragmentWidget::addFragmentItem(ModelItem* item,QPoint * point)
{
    if (item == NULL)
        return 1;



    qreal dx = point->x()-6;
    qreal dy = point->y();
    item->get2DModelNoText()->moveBy(dx,dy);

    QTransform rot;
    QList<QPoint*>::Iterator iter = this->endPoints->begin();
    QPoint * pt = item->getEndPoint(0);
    int index = -1;
    while (iter!=this->endPoints->end())
    {
        index++;
        if (*pt == *(*iter))
            break;
        iter++;
    }
    qreal angle = this->endPointsAngles->at(index);

    //rotate model
    //rot.rotate(angle);
    //rot *=(*this->transformMatrix);
    //item->get2DModelNoText()->setTransform(rot);

    //rotate endPoint - use mathFunctions.h


    item->setParentFragment(this);
    this->fragmentItems->push_back(item);
    app->getWindow()->getWorkspaceWidget()->updateFragment(this);

    QPoint * newPt = new QPoint(*item->getEndPoint()+*point);
    app->getWindow()->getWorkspaceWidget()->setActiveEndPoint(newPt);
    return 0;
}

/** **
int ModelFragmentWidget::setStartPoint(QPoint * pt)
{
    if (pt==NULL)
        return 1;
    this->startPoint=pt;
    return 0;
}
*/

int ModelFragmentWidget::addEndPoints (QList<QPoint *> * listToAppend)
{
    if (listToAppend==NULL)
        return 1;
    this->endPoints->append(*listToAppend);
    return 0;
}

int ModelFragmentWidget::removeEndPoint (QPoint * pt)
{
    if (pt == NULL)
        return 1;
    bool success = this->endPoints->removeOne(pt);
    if (success)
        return 0;
    else
        return 2;
}

int ModelFragmentWidget::removeEndPoint (int index)
{
    if (index < 0 || index >= this->endPoints->size())
        return 1;
    this->endPoints->removeAt(index);
    return 0;
}

/** **
int ModelFragmentWidget::removeStartPoint()
{
    delete this->startPoint;
    this->startPoint=NULL;
    return 0;
}

int ModelFragmentWidget::setStartPointAngle(qreal angle)
{
    this->startPointAngle = angle;
    return 0;
}
*/

int ModelFragmentWidget::setEndPointAngle(QPoint * pt, qreal angle)
{
    if (pt==NULL)
        return 1;
    int index = this->endPoints->indexOf(pt);
    if (index==-1) //??
        return 2;
    //this->endPointsAngles->at(index)=angle;
    QList<qreal>::Iterator iter = this->endPointsAngles->begin();
    for (int i = 0; i <= index; i++)
        iter++;
    *iter=angle;

    return 0;
}

int ModelFragmentWidget::setEndPointAngle(int index, qreal angle)
{
    QList<qreal>::Iterator iter = this->endPointsAngles->begin();
    if (index < 0 || index >=this->endPointsAngles->size())
        return 1;
    for (int i = 0; i <= index; i++)
        iter++;
    *iter=angle;
    return 0;
}

int ModelFragmentWidget::showInfoDialog()
{
    int result = 0;
    if (this->infoDialog==NULL)
        result = this->initInfoDialog();

    if (result)
        return result;

    this->infoDialog->show();


    return result;
}

int ModelFragmentWidget::initInfoDialog()
{
    this->infoDialog = new QDialog(app->getWindow());
    QFormLayout * layout = new QFormLayout(this->infoDialog);
    this->infoDialog->setWindowTitle("Model fragment info");

    /*Model fragment info
     *parts count
     *parts listbox or something similar
     *parts are made by manufacturers:
     *list box or something similar
     *track length?? how to compute the distance? sum of all item's lengths
     */
    QString str (QString::number(this->fragmentItems->count()));
    QLabel * label = new QLabel(str, this->infoDialog);
    QListWidget * listWidgetI = new QListWidget(this->infoDialog);
    for (int i = 0; i < this->fragmentItems->count();i++)
    {
        QString str;
        str.append(*this->fragmentItems->at(i)->getPartNo());
        listWidgetI->addItem(str);
    }

    layout->insertRow(0,"Parts count:",label);
    layout->insertRow(1,"Parts used:",listWidgetI);

    QListWidget * listWidgetM = new QListWidget(this->infoDialog);
    for (int i = 0; i < this->fragmentItems->count(); i++)
    {
        listWidgetM->addItem(*this->fragmentItems->at(i)->getProdLine()->getName());
    }
    layout->insertRow(2,"Parts' manufacturer(s)",listWidgetM);

    this->infoDialog->setLayout(layout);
    return 0;
}

void ModelFragmentWidget::moveBy(qreal dx, qreal dy)
{
    for (int i = 0; i < this->fragmentItems->count(); i++)
    {
        this->fragmentItems->at(i)->get2DModelNoText()->moveBy(dx,dy);
    }
}

//ModelItem::ModelItem(QString * partNumber, QString * partNameEn, QString * partNameCs,
//                     QPoint * start, QPoint * end ,qreal degree, qreal turnRadius,
//                     void * appPointer, QWidget * parentWidg, QGraphicsItem * contourParent)

ModelItem::ModelItem(QString & partNumber, QString & partNameEn, QString & partNameCs,
                     QPoint & start, QPoint & end ,qreal degree,
                     qreal turnRadius, ProductLine * prodLine, QWidget * parentWidg)/// : QWidget (parentWidg)
{

    this->partNo = new QString(partNumber);
    this->nameEn = new QString(partNameEn);
    this->nameCs = new QString(partNameCs);
    ////this->startPoint = new QPoint(start);
    this->endPoints = new QList <QPoint*>();
    this->endPoints->append(new QPoint(start));
    this->endPoints->append(new QPoint(end));
    ////this->turnDegree=degree;
    this->endPointsAngles = new QList <qreal>();
    this->endPointsAngles->append(-degree/2.0);
    this->endPointsAngles->append(degree/2.0);

    this->radius=turnRadius;

    this->availableCount=10;
    this->prodLine=prodLine;
    this->parentFragment=NULL;
    this->parentWidget = parentWidg;

    this->contourModel=NULL;
    this->contourModelNoText=NULL;
    this->glModel=NULL;


    /////this->generate2DModel(true);
    /////this->generate2DModel(false);
    //this.generate3DModel();

    /*
    if (app_ptr->getUserPreferences()->getLocale()="Cs")
        this->label->setText(this->nameCs);
    else
        this->label->setText(this->nameEn);
*/

    this->t = T2;


}

ModelItem::ModelItem(ModelItem &orig)
{
    this->availableCount=orig.availableCount;
    this->contourModel= new GraphicsPathItem(&orig,orig.get2DModel()->path());
    this->contourModel->setFlag(QGraphicsItem::ItemIsMovable,false);
    this->contourModel->setFlag(QGraphicsItem::ItemIsSelectable);
    this->contourModel->setVisible(true);

    this->endPoints=orig.endPoints;
    this->glModel=orig.glModel;
    //this->label=unused
    this->nameCs=orig.nameCs;
    this->nameEn=orig.nameEn;

    //may cause errors
    this->parentWidget=app->getWindow()->getWorkspaceWidget();

    this->partNo=orig.partNo;
    this->prodLine=orig.prodLine;
    this->radius=orig.radius;
    ////this->startPoint=orig.startPoint;
    ////this->turnDegree=orig.turnDegree;
    this->endPointsAngles=orig.endPointsAngles;
}

QString * ModelItem::getPartNo() const
{
    return this->partNo;
}
QString * ModelItem::getNameEn() const
{
    return this->nameEn;
}
QString * ModelItem::getNameCs() const
{
    return this->nameCs;
}
/** **
QPoint * ModelItem::getStartPoint() const
{
    return this->startPoint;
}
*/
QPoint * ModelItem::getEndPoint(int index) const
{
    QPoint * pt = this->endPoints->at(index);
    return pt;
}

qreal ModelItem::getTurnDegree(int index) const
{
    ////return this->turnDegree;
    return this->endPointsAngles->at(index);
}
qreal ModelItem::getRadius() const
{
    return this->radius;
}

GraphicsPathItem * ModelItem::get2DModel() const
{
    return this->contourModel;
}

int ModelItem::set2DModel(GraphicsPathItem *model)
{
    if (model==NULL)
        return 1;
    this->contourModel=model;
    return 0;
}

GraphicsPathItem *ModelItem::get2DModelNoText() const
{
    return this->contourModelNoText;
}

int ModelItem::set2DModelNoText(GraphicsPathItem *model)
{
    if (model==NULL)
        return 1;
    this->contourModelNoText=model;
    return 0;
}

int ModelItem::generate2DModel(bool text)
{
    QRectF rectOuter;//(-this->radius+32,-this->radius+32,this->radius*2-64,this->radius*2-64);
    QRectF rectInner;//(-this->radius+24,-this->radius+24,this->radius*2-48,this->radius*2-48);

    if (this->radius>0)
    {
        rectOuter = QRectF(-this->radius+32,-this->radius+32,this->radius*2-64,this->radius*2-64);
        rectInner = QRectF(-this->radius+24,-this->radius+24,this->radius*2-48,this->radius*2-48);
    }
    else
    {
        rectOuter = QRectF(-this->radius-32,-this->radius-32,this->radius*2+64,this->radius*2+64);
        rectInner = QRectF(-this->radius-24,-this->radius-24,this->radius*2+48,this->radius*2+48);
    }
    QPainterPath itemPath;

    qreal startAngle = 90-(this->getTurnDegree());

    QFont font;
    font.setPixelSize(10);
    QString label(*this->partNo);
    label.append(" ");
    QString s("");
    if (this->getTurnDegree()!=0)
        s.setNum(2*this->getTurnDegree(),'f',1);
    else
        s.setNum(2*this->getTurnDegree(),'f',0);

    label.append(s);
    label.append("° ");
    s.setNum(this->radius);
    label.append(s);



    if (text)
        itemPath.addText(-(6*this->partNo->length())/2,48-this->radius,font,*this->partNo);

    if (this->getTurnDegree()!=0)
    {
        itemPath.arcMoveTo(rectOuter,startAngle);
        itemPath.arcTo(rectOuter,startAngle,2*this->getTurnDegree());
        itemPath.arcMoveTo(rectInner,startAngle);
        itemPath.arcTo(rectInner,startAngle,2*this->getTurnDegree());
    }
    else
    {
        QPolygon line;
        line << QPoint(-this->radius/2,-this->radius/2);
        line << QPoint(this->radius/2,-this->radius/2);
        QPolygon line2;
        line2 << QPoint(-this->radius/2,-this->radius/2+8);
        line2 << QPoint(this->radius/2,-this->radius/2+8);

        itemPath.addPolygon(line);
        itemPath.addPolygon(line2);
    }


    GraphicsPathItem * gpi = new GraphicsPathItem(this);
    gpi->setFlag(QGraphicsItem::ItemIsMovable,false);
    gpi->setFlag(QGraphicsItem::ItemIsSelectable,true);


    gpi->setPath(itemPath);
    gpi->changeCountPath(10,this->radius);
    gpi->setToolTip(label);

    ///GraphicsPathItem *gpiText = new GraphicsPathItem(gpi);
    ///gpiText->setPath(textPath);
    ///QBrush b = gpiText->brush();
    ///b.setColor(Qt::red);
    ///b.setStyle(Qt::SolidPattern);
    ///QPen p = gpiText->pen();
    ///p.setWidth(0);
    ///gpiText->setPen(p);
    ///gpiText->setBrush(b);

    ///gpiText->setVisible(false);



    if (text)
        this->contourModel=gpi;
    else
        this->contourModelNoText=gpi;
    return 0;
}

unsigned int ModelItem::getAvailableCount() const
{
    return this->availableCount;
}

void ModelItem::setAvailableCount(unsigned int count)
{
    this->availableCount=count;
}

void ModelItem::incrAvailableCount()
{
    ++this->availableCount;
}

void ModelItem::decrAvailableCount()
{
    if (this->availableCount>0)
        --this->availableCount;
}

ProductLine *ModelItem::getProdLine() const
{
    return this->prodLine;
}

ModelFragmentWidget *ModelItem::getParentFragment() const
{
    return this->parentFragment;
}

int ModelItem::setParentFragment(ModelFragmentWidget *frag)
{
    if (frag==NULL)
        return 1;
    this->parentFragment=frag;
    return 0;
}

QWidget *ModelItem::getParentWidget() const
{
    return this->parentWidget;
}

GraphicsPathItem::GraphicsPathItem(ModelItem * item, QGraphicsItem * parent) : QGraphicsPathItem(parent)
{
    //this->mouseMoveOffset = new QPoint(0,0);
    this->setFlag(QGraphicsItem::ItemIsMovable,true);
    this->setFlag(QGraphicsItem::ItemIsSelectable,true);
    this->restrictedCountPath = NULL;
    this->parentItem=item;
    this->dialog=NULL;
    this->contextMenuSBW = NULL;
    this->contextMenuWSW = NULL;
    this->mousePressed=false;

    //if (this->parentItem()==NULL)
    //    this->changeCountPath(10);
}
GraphicsPathItem::GraphicsPathItem(ModelItem * item, const QPainterPath & path, QGraphicsItem * parent) : QGraphicsPathItem(path,parent)
{
    this->restrictedCountPath = NULL;
    this->parentItem=item;
    this->dialog=NULL;
    this->contextMenuSBW = NULL;
    this->contextMenuWSW = NULL;
    this->mousePressed=false;



}



void GraphicsPathItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->mousePressed=true;
    QGraphicsPathItem::mousePressEvent(event);
}
void GraphicsPathItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->mousePressed=false;
    QGraphicsPathItem::mouseReleaseEvent(event);
}
void GraphicsPathItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (this->mousePressed && this->parentItem->getParentFragment()!=NULL)
    {
        QPointF diff = event->lastPos()-event->pos();
        this->parentItem->getParentFragment()->moveBy(-diff.x(),-diff.y());
    }
    QGraphicsPathItem::mouseMoveEvent(event);
}

bool GraphicsPathItem::contains(const QPoint &point) const
{
    return this->path().boundingRect().contains(point);
    //return QRectF(this->pos().x(),this->pos().y(),100,64).contains(point);

}

QRectF GraphicsPathItem::boundingRect() const
{
    //return QRectF(this->pos().x(),this->pos().y(),100,64);
    return this->path().controlPointRect();
}

QPainterPath GraphicsPathItem::shape() const
{
    QPainterPath * p = new QPainterPath();
    p->addRect(this->path().controlPointRect());
    //->addRect(this->pos().x(),this->pos().y(),100,64);
    return *p;
}

void GraphicsPathItem::changeCountPath(unsigned int count, qreal radius)
{
    GraphicsPathItem * newItem = new GraphicsPathItem(this->parentItem,this);

    QPainterPath pp;
    QRectF r(-50+5,5+38-(radius),15,13);
    pp.addRect(r);
    QFont font;
    font.setPixelSize(10);
    QString text;
    text.setNum(count);

    pp.addText(-49+5,5+49-(radius),font,text);
    QBrush b = newItem->brush();
    if (count==1)
        b.setColor(Qt::yellow);
    else if (count==0)
        b.setColor(Qt::red);
    else
        b.setColor(Qt::green);
    b.setStyle(Qt::SolidPattern);
    newItem->setBrush(b);
    newItem->setPath(pp);

    if (this->restrictedCountPath!=NULL)
        delete this->restrictedCountPath;

    this->restrictedCountPath=newItem;

}

int GraphicsPathItem::initDialog()
{
    this->dialog = new QDialog (app->getWindow());
    this->dialog->setWindowTitle("Change available count");
    QFormLayout * layout = new QFormLayout(this->dialog);

    QLabel * lineTextBox = new QLabel(*(this->parentItem->getProdLine()->getName()),this->dialog);
    layout->addRow("Manufacturer's name:", lineTextBox);
    lineTextBox->setFixedHeight(30);
    QLabel * partNoTextBox = new QLabel(*(this->parentItem->getPartNo()),this->dialog);
    partNoTextBox->setFixedHeight(30);
    layout->addRow("Part No:",partNoTextBox);


    //replace this with something better
    QSpinBox * countSpinBox = new QSpinBox(this->dialog);
    countSpinBox->setRange(0,999);
    countSpinBox->setValue(this->parentItem->getAvailableCount());


    layout->addRow("Available count:",countSpinBox);

    QPushButton * confirm = new QPushButton("Confirm",this->dialog);
    QPushButton * discard = new QPushButton("Discard",this->dialog);

    QWidget::connect(discard,SIGNAL(clicked()),this->dialog,SLOT(close()));
    connect(confirm,SIGNAL(clicked()),this,SLOT(updateItem()));

    layout->addRow(confirm,discard);

    this->dialog->setLayout(layout);

    return 0;
}

int GraphicsPathItem::initMenus()
{
    this->contextMenuSBW = new QMenu(app->getWindow()->getSideBarWidget());
    QAction * action = new QAction ("Change model part info",this->contextMenuSBW);
    this->contextMenuSBW->addAction(action);

    this->contextMenuWSW = new QMenu(app->getWindow()->getWorkspaceWidget());
    QAction * action2 = new QAction ("Fragment info",this->contextMenuWSW);
    this->contextMenuWSW->addAction(action2);
}

void GraphicsPathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (this->parentItem->getParentWidget()!=app->getWindow()->getWorkspaceWidget() && (app->getRestrictedInventoryMode() && this->restrictedCountPath!=NULL))
    {
        this->restrictedCountPath->setVisible(true);
    }
    else if ((!app->getRestrictedInventoryMode() || this->parentItem->getParentWidget()==app->getWindow()->getWorkspaceWidget()) && this->restrictedCountPath!=NULL)
        this->restrictedCountPath->setVisible(false);
    QGraphicsPathItem::paint(painter,option,widget);
}

void GraphicsPathItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    bool enable = true;

    /////
    ///refactor all of following if's
    /////
    if (this->parentItem->getParentWidget()!=app->getWindow()->getWorkspaceWidget())
    {
        if (app->getRestrictedInventoryMode())
        {
            if (this->parentItem->getAvailableCount()<1)
                enable = false;
            this->parentItem->decrAvailableCount();
            this->changeCountPath(this->parentItem->getAvailableCount(),this->parentItem->getRadius());
        }

        if (enable)
        {
            if (app->getWindow()->getWorkspaceWidget()->getActiveFragment()==NULL)
            {
                QList<qreal> * endDegrees = new QList<qreal>();
                if (event->scenePos().x()<this->pos().x()/2)
                    endDegrees->push_back(this->parentItem->getTurnDegree());
                else
                {
                    endDegrees->push_back(-this->parentItem->getTurnDegree());
                }
                endDegrees->push_front(this->parentItem->getTurnDegree(0));
                QList<QPoint*> * endPoints = new QList<QPoint*>();
                endPoints->push_back(this->parentItem->getEndPoint());


                ModelItem * it = NULL;

                if (event->scenePos().x()<this->pos().x()/2)
                {
                    it = new ModelItem(*this->parentItem->getPartNo(),*this->parentItem->getNameEn(),*this->parentItem->getNameCs(),*this->parentItem->getEndPoint(0),*this->parentItem->getEndPoint(),-2*this->parentItem->getTurnDegree(),-this->parentItem->getRadius(),this->parentItem->getProdLine(),app->getWindow()->getWorkspaceWidget());
                }
                else
                {
                    it = new ModelItem(*this->parentItem->getPartNo(),*this->parentItem->getNameEn(),*this->parentItem->getNameCs(),*this->parentItem->getEndPoint(0),*this->parentItem->getEndPoint(),2*this->parentItem->getTurnDegree(),this->parentItem->getRadius(),this->parentItem->getProdLine(),app->getWindow()->getWorkspaceWidget());
                }
                //no need to generate model with text, because text isn't needed in workspace scene
                it->generate2DModel(false);


                if (event->scenePos().x()<this->pos().x()/2)
                {
                    it->get2DModelNoText()->moveBy(0,it->getRadius()+64);
                    ///it->get2DModelNoText()->moveBy(0,-it->get2DModelNoText()->boundingRect().height()/4);
                }
                else
                {
                    it->get2DModelNoText()->moveBy(0,it->getRadius());
                    ///it->get2DModelNoText()->moveBy(0,it->get2DModelNoText()->boundingRect().height()/4);
                }
                //it->get2DModelNoText()->moveBy(0,-it->get2DModelNoText()->boundingRect().height()/4);

                ModelFragmentWidget * fragment = new ModelFragmentWidget(it,this->parentItem->getProdLine(),this->parentItem->getEndPoint(0), endPoints, endDegrees);
                app->getWindow()->getWorkspaceWidget()->addFragment(fragment);
                app->getWindow()->getWorkspaceWidget()->setActiveFragment(fragment);
                app->getWindow()->getWorkspaceWidget()->setActiveEndPoint(this->parentItem->getEndPoint());
            }
            ///else activePoint's fragment - addItem
            else
            {

                ModelItem * it = NULL;// new ModelItem(*this->parentItem->getPartNo(),*this->parentItem->getNameEn(),*this->parentItem->getNameCs(),*this->parentItem->getEndPoint(0),*this->parentItem->getEndPoint(),this->parentItem->getTurnDegree(),this->parentItem->getRadius(),this->parentItem->getProdLine(),app->getWindow()->getWorkspaceWidget());
                if (event->scenePos().x()<this->pos().x()/2)
                {
                    it = new ModelItem(*this->parentItem->getPartNo(),*this->parentItem->getNameEn(),*this->parentItem->getNameCs(),*this->parentItem->getEndPoint(0),*this->parentItem->getEndPoint(),-2*this->parentItem->getTurnDegree(),-this->parentItem->getRadius(),this->parentItem->getProdLine(),app->getWindow()->getWorkspaceWidget());
                }
                else
                {
                    it = new ModelItem(*this->parentItem->getPartNo(),*this->parentItem->getNameEn(),*this->parentItem->getNameCs(),*this->parentItem->getEndPoint(0),*this->parentItem->getEndPoint(),2*this->parentItem->getTurnDegree(),this->parentItem->getRadius(),this->parentItem->getProdLine(),app->getWindow()->getWorkspaceWidget());
                }

                //no need to generate model with text, because text isn't needed in workspace scene
                it->generate2DModel(false);

                if (event->scenePos().x()<this->pos().x()/2)
                {
                    //place length of the curve instead of 58
                   //it->get2DModelNoText()->moveBy(app->getWindow()->getWorkspaceWidget()->getActiveFragment()->getFragmentItems()->count()*58,it->getRadius()+64);
                    //move to the center of the scene
                    it->get2DModelNoText()->moveBy(0,it->getRadius()+64);
                    //move by quarter of item height (based on boundingRect)
                    ///it->get2DModelNoText()->moveBy(0,-it->get2DModelNoText()->boundingRect().height()/4);
                    //move to the activeEndPoint
                    //done in addFragmentItem
                }
                else
                {
                    //it->get2DModelNoText()->moveBy(app->getWindow()->getWorkspaceWidget()->getActiveFragment()->getFragmentItems()->count()*58,it->getRadius());
                    //move to the center of the scene
                    it->get2DModelNoText()->moveBy(0,it->getRadius());
                    //move by half of item height (based on boundingRect)
                    ///it->get2DModelNoText()->moveBy(0,it->get2DModelNoText()->boundingRect().height()/4);
                    //move to the activeEndPoint
                    //done in addFragmentItem
                }

                //it->get2DModelNoText()->moveBy(0,-it->get2DModelNoText()->boundingRect().height()/4);
                app->getWindow()->getWorkspaceWidget()->getActiveFragment()->addFragmentItem(it,app->getWindow()->getWorkspaceWidget()->getActiveEndPoint());

            }
        }
    }



    QGraphicsPathItem::mouseDoubleClickEvent(event);
}

void GraphicsPathItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{

    if (this->dialog==NULL)
        this->initDialog();


    if(this->contextMenuSBW==NULL && this->contextMenuWSW==NULL)
        this->initMenus();



    QPointF pt = event->screenPos();
    QAction * actionSelected = NULL;
    if (this->parentItem->getParentWidget()==app->getWindow()->getWorkspaceWidget())
        actionSelected = this->contextMenuWSW->exec(QPoint(pt.x(),pt.y()));
    else
        actionSelected = this->contextMenuSBW->exec(QPoint(pt.x(),pt.y()));

    if (actionSelected != NULL)
    {
        if (this->parentItem->getParentWidget()==app->getWindow()->getWorkspaceWidget())
        {
            if (this->parentItem->getParentFragment()!=NULL)
                this->parentItem->getParentFragment()->showInfoDialog();
        }
        else
            this->dialog->show();

    }
}

void GraphicsPathItem::updateItem()
{

    unsigned int count = ((QSpinBox*)this->dialog->layout()->itemAt(5)->widget())->value();
    this->parentItem->setAvailableCount(count);
    this->changeCountPath(count,this->parentItem->getRadius());
    this->dialog->close();
}




ProductLine::ProductLine(QString &name,QString &scale, QString &gauge, bool type, QList<ModelItem*> &items)
{
    this->name=new QString(name);
    this->scale=new QString(scale);
    this->type=type;
    this->gauge=new QString(gauge);
    this->items=new QList<ModelItem*>(items);
    this->maxTrackRadius=0;
    this->minTrackRadius=50000;

}
ProductLine::ProductLine(QString &name,QString &scale, QString &gauge, bool type)
{
    this->name=new QString(name);
    this->scale=new QString(scale);
    this->type=type;
    this->items = new QList<ModelItem*>();
    this->maxTrackRadius=0;
    this->minTrackRadius=50000;
}


QString * ProductLine::getName() const
{
    return this->name;
}

QString * ProductLine::getScale() const
{
    return this->scale;
}

bool ProductLine::getType() const
{
    return this->type;
}

QList<ModelItem*> * ProductLine::getItemsList() const
{
    return this->items;
}

int ProductLine::setItemsList(QList<ModelItem*>* list)
{
    return 0;
}

int ProductLine::addItem(ModelItem* item)
{
    if (item==NULL)
        return 1;
    this->items->push_back(item);
    if (item->getRadius()>this->maxTrackRadius)
        this->maxTrackRadius=item->getRadius();

    if(item->getRadius()<this->minTrackRadius)
        this->minTrackRadius=item->getRadius();

    return 0;
}

/*
int ProductLine::generate2DModels()
{
    QList<ModelItem*>::Iterator iter = this->items->begin();
    while (iter!=this->items->end())
    {
        /*
         *this cannot be used - doesn't preserve scale of one product line
         *

        qreal normalized = (this->maxTrackRadius-(*iter)->getRadius())/(this->maxTrackRadius-this->minTrackRadius);
        normalized = 1 - 0.2*normalized;
        int sizeOfItem = 32;
        QRectF rectOuter(-sizeOfItem*normalized,-sizeOfItem*normalized,sizeOfItem*normalized*2,sizeOfItem*normalized*2);
        sizeOfItem=24;
        QRectF rectInner(-sizeOfItem*normalized,-sizeOfItem*normalized,sizeOfItem*normalized*2,sizeOfItem*normalized*2);
        * /





        QRectF rectOuter(-(*iter)->getRadius()+32,-(*iter)->getRadius()+32,(*iter)->getRadius()*2-64,(*iter)->getRadius()*2-64);
        QRectF rectInner(-(*iter)->getRadius()+24,-(*iter)->getRadius()+24,(*iter)->getRadius()*2-48,(*iter)->getRadius()*2-48);


*/


/*
        QPainterPath itemPath;

        qreal startAngle = 90-((*iter)->getTurnDegree()/2);

        QFont font;
        font.setPixelSize(10);
        QString label(*(*iter)->getPartNo());
        label.append(" ");
        QString s("");
        if ((*iter)->getTurnDegree()!=0)
            s.setNum((*iter)->getTurnDegree(),'f',1);
        else
            s.setNum((*iter)->getTurnDegree(),'f',0);

        label.append(s);
        label.append("° ");
        s.setNum((*iter)->getRadius());
        label.append(s);



        ///QPainterPath textPath;
        itemPath.addText(-(6*(*iter)->getPartNo()->length())/2,48-(*iter)->getRadius(),font,*(*iter)->getPartNo());

        if ((*iter)->getTurnDegree()!=0)
        {
            itemPath.arcMoveTo(rectOuter,startAngle);
            itemPath.arcTo(rectOuter,startAngle,(*iter)->getTurnDegree());
            itemPath.arcMoveTo(rectInner,startAngle);
            itemPath.arcTo(rectInner,startAngle,(*iter)->getTurnDegree());
        }
        else
        {
            QPolygon line;
            line << QPoint(-(*iter)->getRadius()/2,-(*iter)->getRadius()/2);
            line << QPoint((*iter)->getRadius()/2,-(*iter)->getRadius()/2);
            QPolygon line2;
            line2 << QPoint(-(*iter)->getRadius()/2,-(*iter)->getRadius()/2+8);
            line2 << QPoint((*iter)->getRadius()/2,-(*iter)->getRadius()/2+8);

            itemPath.addPolygon(line);
            itemPath.addPolygon(line2);
        }



        GraphicsPathItem * gpi = new GraphicsPathItem(*iter);
        gpi->setFlag(QGraphicsItem::ItemIsMovable,true);
        gpi->setFlag(QGraphicsItem::ItemIsSelectable,true);


        gpi->setPath(itemPath);
        gpi->changeCountPath(10,(*iter)->getRadius());
        gpi->setToolTip(label);

        ///GraphicsPathItem *gpiText = new GraphicsPathItem(gpi);
        ///gpiText->setPath(textPath);
        ///QBrush b = gpiText->brush();
        ///b.setColor(Qt::red);
        ///b.setStyle(Qt::SolidPattern);
        ///QPen p = gpiText->pen();
        ///p.setWidth(0);
        ///gpiText->setPen(p);
        ///gpiText->setBrush(b);

        ///gpiText->setVisible(false);



        (*iter)->set2DModel(gpi);

        iter++;
    }
    return 0;
}
*/