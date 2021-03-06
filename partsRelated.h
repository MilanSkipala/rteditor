/*
    Slot track and model railway editor by Milan Skipala
    Copyright (C) 2014 Milan Skipala

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PARTSRELATED_H
#define PARTSRELATED_H

#include "includeHeaders.h"
#include "graphicsScene.h"
#include "itemTypeEnums.h"
#include "scales.h"

class WorkspaceWidget;
class ProductLine;
class ModelItem;
class BorderItem;
class VegetationItem;
class GenericModelItem;
class ModelFragment;

class GraphicsPathItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
    QGraphicsPathItem * restrictedCountPath;
    GenericModelItem * parentItem;
    QMenu * contextMenuSBW;
    QMenu * contextMenuWSW;

    QDialog * dialog;
    QDialog * infoDialog;


public:
    GraphicsPathItem(GenericModelItem * item, QGraphicsItem * parent = 0);
    GraphicsPathItem(GenericModelItem * item, const QPainterPath & path, QGraphicsItem * parent = 0);
    ~GraphicsPathItem();
    bool contains( const QPointF & point ) const;
    QRectF boundingRect() const;
    QPainterPath shape() const;

    GenericModelItem * getParentItem();

    /**
      method changeCountPath deletes old QGraphicsPathItem representing count of available parts
      and creates new QGPI.
    */
    void changeCountPath(unsigned int count);

    /**
      initialize dialog using of which the count of available items can be changed
    */
    int initDialog();

    /**
      initialize context menus - 1 is used in SideBarWidget, 1 in WorkspaceWidget
    */
    int initMenus();

    //protected events
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    QGraphicsPathItem * getRestrictedCountPath() const;

private slots:
    void updateItem();

};

class GenericModelItem
{
    QString * partNo;
    QString * nameEn;
    QString * nameCs;
    int availableCount;
    GraphicsPathItem * model2D;
    GraphicsPathItem * model2DNoText;
    QWidget * parentWidget;
    ProductLine * prodLine;

public:
    GenericModelItem(QString &partNo, QString &nameEn, QString &nameCs, ProductLine * productLine, QWidget * parentWidget = NULL);
    virtual ~GenericModelItem();

    QString * getPartNo() const;
    QString * getNameEn() const;
    QString * getNameCs() const;

    GraphicsPathItem * get2DModel() const;
    int set2DModel(GraphicsPathItem * model);
    GraphicsPathItem * get2DModelNoText() const;
    int set2DModelNoText(GraphicsPathItem * model);

    /**
     * @brief generate2DModel - creates vector model of the item. The look of the model is determined from the item characteristic data (-> loaded from the database)
     * @param text - render partNo or not
     * @return
     */
    virtual int generate2DModel(bool text) = 0;

    /** following four methods are needed for correct function of "restricted inventory mode" */
    unsigned int getAvailableCount() const;
    void setAvailableCount(unsigned int count);
    void incrAvailableCount();
    void decrAvailableCount();

    QWidget * getParentWidget() const;
    ProductLine * getProdLine() const;

    virtual void rotate (qreal angle) = 0;
    virtual void rotate (qreal angle,QPointF * center, bool printCommand = false) = 0;

    virtual void moveBy(qreal dx, qreal dy) = 0;
};

class GraphicsPathItemModelItem : public GraphicsPathItem
{
    Q_OBJECT
    bool mousePressed;
    QPointF * offset;

    enum { Type = UserType + 2 };

public:
    GraphicsPathItemModelItem(ModelItem * item, QGraphicsItem * parent = 0);
    GraphicsPathItemModelItem(ModelItem * item, const QPainterPath & path, QGraphicsItem * parent = 0);
    ~GraphicsPathItemModelItem();

    ModelItem * getParentItem();
    int type() const;

    //protected events
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
      mouseDoubleClickEvent() calls creating of the new ModelItem instance which is based on the data of parentItem attribute
    */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

private slots:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    /**
      mouseMoveEvent() provides moving and rotation functionality
    */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

signals:
    void hpiDialogExec(QGraphicsSceneMouseEvent*evt);
};

class GraphicsPathItemBorderItem : public GraphicsPathItem
{
    Q_OBJECT
    bool mousePressed;

    enum { Type = UserType + 3 };

public:
    GraphicsPathItemBorderItem(BorderItem * item, QGraphicsItem * parent = 0);
    ~GraphicsPathItemBorderItem();


    BorderItem * getParentItem();

    int type() const;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    /**
      mouseDoubleClickEvent() calls creating of the new BorderItem instance which is based on the data of parentItem attribute
    */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    /**
      mouseMoveEvent() provides moving and rotation functionality
    */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);


};

class GraphicsPathItemVegetationItem : public GraphicsPathItem
{
    Q_OBJECT
    bool mousePressed;

    enum { Type = UserType + 4 };
public:
    GraphicsPathItemVegetationItem(VegetationItem * item, QGraphicsItem * parent = 0);
    ~GraphicsPathItemVegetationItem();
    VegetationItem *getParentItem();
    int type() const;
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

class BorderItem : public GenericModelItem
{
    qreal dAlpha;
    qreal radius;
    QList <QPointF*> *endPoints;
    QList <ModelItem*> *neighbours;

    ///This flag is used as "innerBorderFlag" for curves and as "endingBorderFlag" for straights
    bool innerBorder;
    bool deleteFlag;


public:
    BorderItem(QString &partNo, QString &nameEn, QString &nameCs, qreal dAlpha, qreal radius, QList<QPointF> &endPoints, bool innerBorder, ProductLine *prodLine, QWidget *parentWidg = 0);
    ~BorderItem();

    QPointF * getEndPoint(int index) const;
    int getEndPointsCount() const;
    qreal getAngle() const;
    qreal getRadius() const;
    bool getInnerBorderFlag() const;
    bool getDeleteFlag() const;

    GraphicsPathItemBorderItem * get2DModel() const;
    int set2DModel(GraphicsPathItemBorderItem * model);
    GraphicsPathItemBorderItem * get2DModelNoText() const;
    int set2DModelNoText(GraphicsPathItemBorderItem * model);

    int generate2DModel(bool text);


    ModelItem * getNeighbour(int index);
    ModelItem * getNeighbour(QPointF * pos);

    int setNeighbour(ModelItem * neighbour, int index);
    int setNeighbour(ModelItem * neighbour, QPointF * pos);

    void rotate (qreal angle);
    void rotate (qreal angle,QPointF * center, bool printCommand = false);


    void moveBy(qreal dx, qreal dy);

};

class SlotTrackInfo
{
    /**
     *slot track ModelItem logic:
     *-ModelItem.radius stores outer radius of model piece (including "road")
     *-ModelItem.radius2 stores inner radius of model piece (including "road")
     *-ModelItem.t stores type of part - curve/straight/curved crossing/straight crossing/straight chicane/some other special types (see ITEMTYPEENUMS_H)
     *-SlotTrackInfo stores numberOfLanes (1/2/4/6/8/any number should work with large enough radius)
     *-SlotTrackInfo.lanesGauge stores the distance between every two lanes, doesn't affect anything if numberOfLanes==1
     *-SlotTrackInfo.lanesGaugeEnd stores:
     *      -if ModelItem.t==HE || HS || HC then gauge of lanes at the end of the track piece
     *      -if ModelItem.t==JM then gap distance
     *      -otherwise - stores the same number as lanesGauge attribute
     *-SlotTrackInfo.fstLaneDist stores the distance of the first "outer-most" lane
*/

    ModelItem * parentItem;
    unsigned int numberOfLanes;
    qreal lanesGauge;
    qreal lanesGaugeEnd; ///is used only for HS/HE parts - hairpin start-end straight item
    qreal fstLaneDist;
    QList <BorderItem*> * borders;
    QList <QPointF*> * borderEndPoints;
    QList <QGraphicsEllipseItem*> * borderEndPointsGraphics;

public:
    ///SlotTrackInfo(ModelItem * item);
    SlotTrackInfo(ModelItem * item,unsigned int numberOfLanes,qreal lanesGauge,qreal lanesGaugeEnd,qreal fstLaneDist);
    SlotTrackInfo(const SlotTrackInfo & s);
    ~SlotTrackInfo();

    ModelItem * getParentItem() const;
    int setParentItem(ModelItem * item);

    unsigned int getNumberOfLanes() const;
    qreal getLanesGauge() const;
    qreal getLanesGaugeEnd() const;
    qreal getFstLaneDist() const;
    QList <BorderItem*> * getBorders() const;
    QList<QPointF *> *getBorderEndPoints() const;
    QList <QGraphicsEllipseItem*> * getBorderEndPointsGraphics() const;

    void addBorder(BorderItem * border);
    void removeBorder(BorderItem * border);

};

class ModelItem : public GenericModelItem
{
    ModelFragment * parentFragment;

    ItemType t;

    QList <QPointF*> * endPoints;
    QList <qreal> * endPointsAngles;
    QList <ModelItem*> * neighbours;
    QList <int> * endPointsHeight;
    QList <QGraphicsPathItem*> * endPointsHeightGraphics;

    qreal radius;
    qreal radius2; //is used only for curved turnouts (t==J1 || J2) and for slot track, otherwise it is 0
    qreal itemWidth;
    qreal itemHeight;

    qreal maxFlex;

    SlotTrackInfo * slotTrackInfo; //NULL for rail parts, contains info needed only when working with the slot track

    bool recursionStopper;
    bool recursionStopperAdj;
    bool deleteFlag;


public:
    ModelItem(QString & partNumber, QString & partNameEn, QString & partNameCs,
              QList<QPointF> & endPoints ,QList<qreal> angles, qreal turnRadius,
              qreal width, qreal height, ItemType type, ProductLine * prodLine, QWidget * parentWidg = 0);

    ~ModelItem();

    bool getDeleteFlag() const;
    void setDeleteFlag();

    QPointF * getEndPoint(int index = 1) const;
    QPointF * getEndPoint(ModelItem * neighbour) const;
    qreal getTurnAngle(int index = 1) const;
    qreal getTurnAngle(QPointF * pt) const;
    qreal getRadius() const;
    qreal getItemWidth() const;
    qreal getItemHeight() const;

    GraphicsPathItemModelItem * get2DModel() const;
    int set2DModel(GraphicsPathItemModelItem * model);
    GraphicsPathItemModelItem * get2DModelNoText() const;
    int set2DModelNoText(GraphicsPathItemModelItem * model);

    /**
     * @brief generate2DModel - generates vector model of the item
     * @param text - specifies whether the value of "partNo" attribute will be rendered
     * @return
     */
    int generate2DModel(bool text);

    ItemType getType() const;

    ModelFragment * getParentFragment() const;
    int setParentFragment(ModelFragment * frag);

    void rotate (qreal angle);
    void rotate (qreal angle,QPointF * center, bool printCommand = false);

    void moveBy(qreal dx, qreal dy);
    qreal getSecondRadius() const;

    void setSecondRadius (qreal radi2);

    /**
     * @brief adjustHeightProfile - normal behaviour is that it sets the height of this item and of its neighbour(s)
     * @param dz
     * @param point
     * @param printCommand
     * @param adjustOnlyThis - if true, adjusts only one point of this item - neither other points (i.e. for turnout parts) nor neighbours are affected
     * @return
     */
    int adjustHeightProfile(int dz, QPointF * point, bool printCommand = true, bool adjustOnlyThis = false);//, bool ignoreRecursionStopper = false);
    void updateEndPointsHeightGraphics(bool forceUpdate = false);
    int getHeightProfileAt(QPointF * point);
    void setLateralAngle(qreal angle);

    int getEndPointIndex(QPointF * pt);

    void setEndPointAngle(int index, qreal angle);
    void setEndPointAngle(QPointF * pt, qreal angle);

    SlotTrackInfo * getSlotTrackInfo();
    int setSlotTrackInfo(SlotTrackInfo * s);

    ModelItem * getNeighbour(int index);
    ModelItem * getNeighbour(QPointF * pos);

    /**
     * @brief setNeighbour sets the neighbour of this item, neighbour is NOT set if its delete flag == true
     * @param neighbour
     * @param index
     * @return
     */
    int setNeighbour(ModelItem * neighbour, int index);
    int setNeighbour(ModelItem * neighbour, QPointF * pos);

    /**
     leftRightDifference180 returns if there is 180 deg. difference between two points
     @param index1 index of the first point
     @param index2 index of the second point
     */
    bool leftRightDifference180 (int index1, int index2) const;
    /**
     leftRightDifference180 returns if there is 180 deg. difference between two points
     @param pt1 the first point
     @param pt2 the second point
     */
    bool leftRightDifference180 (QPointF * pt1, QPointF * pt2) const;

    qreal getMaxFlex() const;

};

class VegetationItem : public GenericModelItem
{
    QString * season;

    qreal itemWidth;
    qreal itemHeight;

    qreal currentRotation;

public:
    VegetationItem(QString& partNo, QString& nameEn, QString& nameCs, QString& season, qreal width, qreal height, ProductLine * prodLine, QWidget * parentWidget = 0);
    ~VegetationItem();

    qreal getItemWidth() const;
    qreal getItemHeight() const;
    qreal getRotation() const;
    void setRotation(qreal alpha);

    QString * getSeason() const;
    GraphicsPathItemVegetationItem *get2DModel() const;
    int set2DModel(GraphicsPathItemVegetationItem * model);
    GraphicsPathItemVegetationItem * get2DModelNoText() const;
    int set2DModelNoText(GraphicsPathItemVegetationItem *model);

    int generate2DModel(bool text);

    void rotate (qreal angle);
    void rotate (qreal angle,QPointF * center, bool printCommand= true);

    void moveBy(qreal dx, qreal dy);
    void moveBy(qreal dx, qreal dy, bool printCommand);

};

class ProductLine
{
    QList<ModelItem*> * items;
    QList<BorderItem*> * borderItems;
    QList<VegetationItem*> * vegetationItems;
    QString * name;
    QString * scale;
    ScaleEnum scaleE;
    QString * gauge;
    bool type; //true=rail false=slot track
    qreal maxTrackRadius;
    qreal minTrackRadius;
    qreal maxStraightLength;

    int lastFoundIndex; //this attribute improves findItemByPartNo() methods' performance
    //it will keep time complexity near O(1) when the inventory is being saved

public:
    ProductLine(QString &name,QString &scale, ScaleEnum s, QString &gauge, bool type, QList<ModelItem*> &items);
    ProductLine(QString &name,QString &scale, ScaleEnum s, QString &gauge, bool type);
    ~ProductLine();
    QString * getName() const;
    QString * getScale() const;
    ScaleEnum getScaleEnum() const;
    bool getType() const;
    QList<ModelItem*> * getItemsList() const;
    QList<BorderItem*> * getBorderItemsList() const;
    QList<VegetationItem*> * getVegetationItemsList() const;

    ModelItem * findItemByPartNo(QString * partNo);
    BorderItem * findBorderItemByPartNo(QString * partNo);
    VegetationItem * findVegetationItemByPartNo(QString * partNo);

    //int setItemsList(QList<ModelItem*>* list);
    int addItem(ModelItem* item);
    int addItem(BorderItem* item);
    int addItem(VegetationItem* item);

    qreal getMinRadius() const;
    qreal getMaxRadius() const;

    qreal getMaxStraight() const;

};

class ModelFragment
{
    QList <ModelItem*> * fragmentItems;
    QList<ProductLine*> * lines;
    //QTransform * transformMatrix;
    ////QPointF * startPoint;
    QList<QPointF *> *endPoints;
    QList<qreal> *endPointsAngles;
    QList<QGraphicsEllipseItem*> *endPointsGraphics;
    QList<ModelItem*> *endPointsItems;

    QDialog * infoDialog;

    int fragmentID;

public:
    ModelFragment(ModelItem* item);

    ~ModelFragment();

    void setID(int id);
    int getID() const;

    /**
     * @brief findEndPointItem - try to find the item which is the endPointItem of this track section
     * @param approxPos - is set to exact position if the item is found
     * @return - found item or NULL
     */
    ModelItem * findEndPointItem(QPointF * approxPos);

    QList <ModelItem*> * getFragmentItems() const;
    QList<ProductLine*> * getProductLines() const;
    QList<QPointF *> * getEndPoints() const;
    QList<QGraphicsEllipseItem*> * getEndPointsGraphics() const;
    QList<qreal> * getEndPointsAngles() const;
    QList<ModelItem*> * getEndPointsItems() const;

    /**
     * @brief addFragmentItem - insert item into this fragment. Item is connected by its endpoint at [0] and it is connected
     *                          at "point" parameter. Method moves and rotates the item, sets the neighbours and the height
     * @param item
     * @param point
     * @param insertionIndex
     * @return
     */
    int addFragmentItem(ModelItem* item, QPointF * point, int insertionIndex = 0);


    /**
     * @brief deleteFragmentItem - separate item from the rest of the track, then call rebuildFragment()
     *                              to build one or two new fragments
     * @param item
     * @param idList - is used in undo/redo functions - undo-redo-undo-redo... sequence wouldn't work
     *                  because there wouldnt be the same ids anymore - see WorkspaceWidget::commandExecution() for details
     * @return
     */
    int deleteFragmentItem(ModelItem * item, QList<int>* idList = NULL);

    /**
     * @brief addEndPoint - add endpoint into this modelFragment
     * @param pt
     * @param additionalInfo - if it is false, just coordinate is added
     * @param rotation
     * @param endPointItem
     * @return
     */
    int addEndPoint (QPointF* pt, bool additionalInfo = false, qreal rotation = 0, ModelItem * endPointItem = NULL);
    int removeEndPoint (QPointF *&pt);

    int setEndPointAngle(QPointF * pt, qreal angle);
    int setEndPointAngle(int index, qreal angle);

    /**
     * @brief showInfoDialog - shows dialog containing information about this track section
     * @return
     */
    int showInfoDialog();
    int initInfoDialog();

    /**
     * @brief moveBy - move all fragment items
     * @param dx
     * @param dy
     */
    void moveBy(qreal dx, qreal dy);

    /**
     * @brief rotate - rotate all fragment items
     * @param angle
     * @param center
     */
    void rotate(qreal angle, QPointF * center);

    /**
     * @brief updateEndPointsGraphics - position of endPointsGraphics' items is updated
     *
     */
    void updateEndPointsGraphics();

};

/**
 * @brief makeNewItem creates new instance of ModelItem based on the parameters and either creates new fragment or inserts the new instance
 * into the current active fragment
 * @param eventPos position of mouseEvent - decides whether the left or right turn/item will be created
 * @param gpi graphic representation of the original item
 * @param parentItem
 * @param toMake
 * @param key
 */
void makeNewItem(QPointF eventPos, GraphicsPathItemModelItem * gpi, ModelItem * parentItem, ModelItem * toMake, bool key);
void makeNewBorder(BorderItem *item);
VegetationItem * makeNewVegetation(VegetationItem * item);
void rebuildFragment(ModelItem * startItem, ModelFragment * fragment);


#endif // PARTSRELATED_H
