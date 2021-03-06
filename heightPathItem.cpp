/*
    Slot track and model railway editor by Milan Skipala
    Copyright (C) 2014 Milan Skipala

    This file is a part of Rail & Slot Editor.
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

#include <QFormLayout>
#include <QGraphicsWidget>
#include <QSpinBox>
#include "heightPathItem.h"
#include "globalVariables.h"
#include "mathFunctions.h"

HeightPathItem::HeightPathItem(ModelItem *item, QGraphicsItem *parent) : QObject(), QGraphicsPathItem(parent)
{
    this->parentItem=item;
    this->slotTrackDialog=NULL;
    this->latSpinBox=NULL;
    this->longSpinBox=NULL;
    this->lastLatSBValue=new int;
    *this->lastLatSBValue=0;
    this->lastLongSBValue=0;
    this->slotTrackDialog = this->initSlotTrackDialog();
    this->latAngle = new qreal;
    *this->latAngle = 0;


}

HeightPathItem::HeightPathItem(ModelItem *item, const QPainterPath &path, QGraphicsItem *parent) : QObject(), QGraphicsPathItem(path,parent)
{
    this->parentItem=item;
    this->slotTrackDialog=NULL;
    this->latSpinBox=NULL;
    this->longSpinBox=NULL;
    this->lastLatSBValue=new int;
    *this->lastLatSBValue=0;
    this->lastLongSBValue=0;
    this->slotTrackDialog = this->initSlotTrackDialog();
    this->latAngle = new qreal;
    *this->latAngle = 0;
}

HeightPathItem::HeightPathItem(const HeightPathItem &hpi): QObject(), QGraphicsPathItem()
{
    this->parentItem=hpi.parentItem;
    this->slotTrackDialog=hpi.slotTrackDialog;
    this->latSpinBox=hpi.latSpinBox;
    this->longSpinBox=hpi.longSpinBox;
    this->lastLongSBValue=hpi.lastLongSBValue;
    this->lastLatSBValue=hpi.lastLatSBValue;
    this->latAngle = hpi.latAngle;
}

QDialog * HeightPathItem::initSlotTrackDialog()
{
    QDialog * pointer = NULL;

    if (app!=NULL && this->parentItem->get2DModelNoText()!=NULL && this->slotTrackDialog==NULL)
    {
        pointer = new QDialog(app->getWindow()->getWorkspaceWidget());
        if (app->getUserPreferences()->getLocale()->startsWith("EN"))
            pointer->setWindowTitle("Adjust height");
        else
            pointer->setWindowTitle("Upravit výšku");
        QFormLayout * layout = new QFormLayout(pointer);
        QGraphicsView * preview = new QGraphicsView(pointer);
        preview->setScene(new QGraphicsScene);
        //scene is the new parent of 2D Model thus it disappears from workspace scene
        QGraphicsPathItem * qgpi = new QGraphicsPathItem();
        qgpi->setPath(this->parentItem->get2DModelNoText()->path());
        QGraphicsPathItem * qgpiPath = new QGraphicsPathItem(qgpi);
        qgpiPath->setPath(((QGraphicsPathItem *)(this->parentItem->get2DModelNoText()->childItems().first()))->path());

        qgpiPath->setPen(((QGraphicsPathItem *)(this->parentItem->get2DModelNoText()->childItems().first()))->pen());

        qgpi->setBrush(this->parentItem->get2DModelNoText()->brush());
        qgpi->setPen(this->parentItem->get2DModelNoText()->pen());
        qgpi->moveBy(this->parentItem->get2DModelNoText()->scenePos().x(),this->parentItem->get2DModelNoText()->scenePos().y());
        preview->scene()->addItem(qgpi);
        preview->scale(0.3,0.3);

        layout->addRow(preview);

        QLabel * partNoLabel = new QLabel(*this->parentItem->getPartNo());
        if (app->getUserPreferences()->getLocale()->startsWith("EN"))
            layout->addRow("Part number",partNoLabel);
        else
            layout->addRow("Číslo dílu",partNoLabel);

        this->longSpinBox = new QSpinBox(this->slotTrackDialog);

        this->longSpinBox->setValue(this->lastLongSBValue);
        this->longSpinBox->setMaximum(512);
        this->longSpinBox->setMinimum(-512);
        if (app->getUserPreferences()->getLocale()->startsWith("EN"))
            layout->addRow("Longitudinal climb",this->longSpinBox);
        else
            layout->addRow("Podélné stoupání",this->longSpinBox);

        if (this->parentItem->getSlotTrackInfo()->getNumberOfLanes()>1)
        {
            this->latSpinBox = new QSpinBox(this->slotTrackDialog);
            this->latSpinBox->setValue(*this->lastLatSBValue);

            this->latSpinBox->setMaximum(512);
            this->latSpinBox->setMinimum(-512);
            if (app->getUserPreferences()->getLocale()->startsWith("EN"))
                layout->addRow("Lateral climb",this->latSpinBox);
            else
                layout->addRow("Boční převýšení",this->latSpinBox);
        }

        QPushButton * accept = new QPushButton("OK",pointer);
        QPushButton * cancel = new QPushButton("Cancel",pointer);
        if (!app->getUserPreferences()->getLocale()->startsWith("EN"))
            cancel->setText("Storno");

        connect(accept,SIGNAL(clicked()),this,SLOT(adjustHeightOfParentItem()));
        connect(cancel,SIGNAL(clicked()),pointer,SLOT(close()));

        layout->addRow(accept);
        layout->addRow(cancel);

        pointer->setLayout(layout);
    }
    return pointer;
}

void HeightPathItem::setAngle(qreal angle)
{

    qreal latClimb = 0;
    if ((this->parentItem->getType()==S1 ||
        this->parentItem->getType()==J5 ||
        this->parentItem->getType()==X1 ||
        this->parentItem->getType()==X2 ||
        this->parentItem->getType()==JM ||
        this->parentItem->getType()==HE ||
        this->parentItem->getType()==HS))
        latClimb = sin(angle*PI/180)*(2*abs(this->parentItem->getRadius()));

    else
        latClimb = sin(angle*PI/180)*(abs(this->parentItem->getRadius()-this->parentItem->getSecondRadius()));

    *this->latAngle=angle;
    *this->lastLatSBValue=latClimb;
    if (this->parentItem->getSlotTrackInfo()->getNumberOfLanes()>1)
        this->latSpinBox->setValue(latClimb);
}

qreal HeightPathItem::getAngle() const
{
    return *this->latAngle;
}


void HeightPathItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    QGraphicsPathItem::mousePressEvent(event);
}

void HeightPathItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPathItem::mouseDoubleClickEvent(event);
    if (this->boundingRect().contains(event->pos()) || this->parentItem->get2DModelNoText()->boundingRect().contains(event->pos()))
        this->slotTrackDialog->exec();
}

void HeightPathItem::adjustHeightOfParentItem()
{
    //get longitudinal climb

    int longClimb = this->longSpinBox->value()-this->lastLongSBValue;
    ModelItem * mi = this->parentItem;

    int index = 0;
    //get lateral climb if there are 2 lanes or more
    if (this->parentItem->getSlotTrackInfo()->getNumberOfLanes()>=2)
    {


        int latClimb = this->latSpinBox->value();

        QPointF point;
        QPointF point2(abs(this->parentItem->getRadius())-this->parentItem->getSlotTrackInfo()->getFstLaneDist(),0);
        qreal angle;



        if ((this->parentItem->getType()==S1 ||
            this->parentItem->getType()==J5 ||

            this->parentItem->getType()==X2 ||
            this->parentItem->getType()==JM ||
            this->parentItem->getType()==HE ||
            this->parentItem->getType()==HS ||
            this->parentItem->getType()==HC
             ))
        {
            point2=QPointF(abs(this->parentItem->getRadius()*2)-this->parentItem->getSlotTrackInfo()->getFstLaneDist(),0);
            angle = 180/PI*asin(latClimb/(2*abs(this->parentItem->getRadius())))-*this->latAngle;
        }
        else if (this->parentItem->getType()==X1)
        {
            SlotTrackInfo * info = this->parentItem->getSlotTrackInfo();
            qreal width = info->getFstLaneDist()*1+info->getLanesGauge()*(info->getNumberOfLanes()-1);
            point2=QPointF(abs(width),0);

            angle = 180/PI*asin(latClimb/(abs(width+info->getFstLaneDist())))-*this->latAngle;
        }
        else
        {
            point2.setX(point2.x()-abs(this->parentItem->getSecondRadius()));
            angle = 180/PI*asin(latClimb/(abs(this->parentItem->getRadius()-this->parentItem->getSecondRadius())))-*this->latAngle;


        }

        point = point2;

        if (!this->slotTrackDialog->isVisible())
        {
            *this->latAngle=(int)*this->latAngle;
            angle = *this->latAngle;
        }

        if (((int)angle-(int)*this->latAngle)!=0)
            *this->latAngle=angle+*this->latAngle;

        if (!this->slotTrackDialog->isVisible() || (this->latSpinBox->value()-*this->lastLatSBValue!=0))//(((int)angle-(int)*this->latAngle)!=0)//
        {
            if ((this->parentItem->getType()==HE)
             || (this->parentItem->getType()==HS)
             || (this->parentItem->getType()==HC))
            {
                //used on the right side when item is HS,HC
                //used on the left side when item is HE
                QPointF pointShortDistance(point2.x()-this->parentItem->getSlotTrackInfo()->getLanesGaugeEnd(),point2.y());

                QPointF pointShortDistance2(point2.x()-this->parentItem->getSlotTrackInfo()->getLanesGaugeEnd(),point2.y());

                if (this->parentItem->getType()==HC)
                {
                    pointShortDistance=QPointF(abs(this->parentItem->getRadius())+this->parentItem->getSlotTrackInfo()->getLanesGaugeEnd()*(this->parentItem->getSlotTrackInfo()->getNumberOfLanes()/2-0.5),point2.y());
                    pointShortDistance2=QPointF(pointShortDistance);
                }

                for (unsigned int i = 0; i < this->parentItem->getSlotTrackInfo()->getNumberOfLanes()*2; i+=2)
                {
                    rotatePoint(&point,*this->latAngle-angle);
                    rotatePoint(&pointShortDistance,*this->latAngle-angle);
                    qreal yOld = point.y();
                    qreal yOldShortDistance = pointShortDistance.y();
                    rotatePoint(&point,angle);
                    rotatePoint(&pointShortDistance,angle);

                        if (this->parentItem->getSlotTrackInfo()->getNumberOfLanes()>1)
                        {
                            *this->lastLatSBValue=this->latSpinBox->value();
                        }

                        if (this->parentItem->getType()==HE)
                        {
                            this->parentItem->adjustHeightProfile((int)pointShortDistance.y()-yOldShortDistance,this->parentItem->getEndPoint(i));
                            this->parentItem->adjustHeightProfile((int)point.y()-yOld,this->parentItem->getEndPoint(i+1));
                        }
                        else
                        {
                            this->parentItem->adjustHeightProfile((int)point.y()-yOld,this->parentItem->getEndPoint(i));
                            this->parentItem->adjustHeightProfile((int)pointShortDistance.y()-yOldShortDistance,this->parentItem->getEndPoint(i+1));
                        }




                    point2.setX(point2.x()-this->parentItem->getSlotTrackInfo()->getLanesGauge());
                    point=point2;

                    pointShortDistance2.setX(pointShortDistance2.x()-this->parentItem->getSlotTrackInfo()->getLanesGaugeEnd());
                    pointShortDistance=pointShortDistance2;


                }
            }
            else if (this->parentItem->getType()==C1 || this->parentItem->getType()==C2) //banked curve is not here intentionaly
            {
                for (unsigned int i = 0; i < this->parentItem->getSlotTrackInfo()->getNumberOfLanes()*2; i+=2)
                {
                    qreal currentItemAngle = this->parentItem->getTurnAngle(1)-this->parentItem->getTurnAngle(0);
                    if (this->parentItem->leftRightDifference180(0,1))
                        currentItemAngle+=180;
                    QPointF otherSidePoint(point2.x()+this->parentItem->getSecondRadius(),0);
                    rotatePoint(&otherSidePoint,currentItemAngle);
                    otherSidePoint.setX((otherSidePoint.x()-this->parentItem->getSecondRadius()));
                    otherSidePoint.setY(0);


                    rotatePoint(&point,*this->latAngle-angle);
                    rotatePoint(&otherSidePoint,*this->latAngle-angle);
                    qreal yOld = point.y();
                    qreal yOldOther = otherSidePoint.y();

                    rotatePoint(&point,angle);
                    rotatePoint(&otherSidePoint,angle);

                    if (this->parentItem->getSlotTrackInfo()->getNumberOfLanes()>1)
                    {
                        *this->lastLatSBValue=this->latSpinBox->value();
                    }
                    this->parentItem->adjustHeightProfile((int)point.y()-yOld,this->parentItem->getEndPoint(i));
                    this->parentItem->adjustHeightProfile((int)otherSidePoint.y()-yOldOther,this->parentItem->getEndPoint(i+1));



                    point2.setX(point2.x()-this->parentItem->getSlotTrackInfo()->getLanesGauge());
                    point=point2;
                }
            }
            else
            {
                for (unsigned int i = 0; i < this->parentItem->getSlotTrackInfo()->getNumberOfLanes()*2; i+=2)
                {
                    rotatePoint(&point,*this->latAngle-angle);
                    qreal yOld = point.y();
                    rotatePoint(&point,angle);

                        if (this->parentItem->getSlotTrackInfo()->getNumberOfLanes()>1)
                        {
                            *this->lastLatSBValue=this->latSpinBox->value();
                        }
                        this->parentItem->adjustHeightProfile((int)point.y()-yOld,this->parentItem->getEndPoint(i));
                        this->parentItem->adjustHeightProfile((int)point.y()-yOld,this->parentItem->getEndPoint(i+1));

                    point2.setX(point2.x()-this->parentItem->getSlotTrackInfo()->getLanesGauge());
                    point=point2;
                }
            }

            //here shouldnt have been used "else if"
            if (this->parentItem->getType()==X1)
            {
                point2 = QPointF(abs(this->parentItem->getSlotTrackInfo()->getLanesGauge()*(this->parentItem->getSlotTrackInfo()->getNumberOfLanes()-1))
                                 +this->parentItem->getSlotTrackInfo()->getFstLaneDist()*3,0);


                for (unsigned int i = this->parentItem->getSlotTrackInfo()->getNumberOfLanes()*2; i < this->parentItem->getSlotTrackInfo()->getNumberOfLanes()*4; i++)
                {
                    if (i%2==0)
                        point = point2;
                    else
                        point = QPointF(-(abs(2*this->parentItem->getSecondRadius())-point2.x()),0);
                    rotatePoint(&point,*this->latAngle-angle);
                    qreal yOld = point.y();
                    rotatePoint(&point,angle);

                        if (this->parentItem->getSlotTrackInfo()->getNumberOfLanes()>1)
                        {
                            *this->lastLatSBValue=this->latSpinBox->value();
                        }
                        this->parentItem->adjustHeightProfile((int)point.y()-yOld,this->parentItem->getEndPoint(i));
                }
            }
        }



        QPointF ptDebug(156,0);
        rotatePoint(&ptDebug,angle);

    }

    index = 1;
    //adjust height by long. climb
    while (mi->getEndPoint(index)!=NULL)
    {
        mi->adjustHeightProfile(longClimb,mi->getEndPoint(index));
        index+=2;
    }



    this->lastLongSBValue=this->longSpinBox->value();
    this->slotTrackDialog->close();

}


