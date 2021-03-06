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

#include "graphicsScene.h"
#include "globalVariables.h"
#include "mathFunctions.h"


GraphicsScene::GraphicsScene(QMenu * contextMenuItem, QMenu * contextMenuScene, qreal x, qreal y, qreal width, qreal height, QObject *parent) : QGraphicsScene(x,y,width,height,parent)
{
    //this->mouseMode=false;
    this->contextMenuItem=contextMenuItem;
    this->contextMenuScene=contextMenuScene;
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    /*if (this->mouseMode)
        QGraphicsScene::mousePressEvent(event);
    else*/
    {
        if (app->getWindow()->getWorkspaceWidget()->getHeightProfileMode())
        {
            QPointF * pos = new QPointF(event->scenePos());
            ModelItem* item = app->getWindow()->getWorkspaceWidget()->findItemByApproxPos(pos,NULL);
            if (item!=NULL)
            {
                app->getWindow()->getWorkspaceWidget()->setActiveItem(item);
                app->getWindow()->getWorkspaceWidget()->setActiveFragment(NULL);
                app->getWindow()->getWorkspaceWidget()->setActiveEndPoint(pos);
                app->getWindow()->getWorkspaceWidget()->selectItem(item);
                app->getWindow()->getWorkspaceWidget()->setActiveItem(item);
            }
            else
                delete pos;
        }
        else
        {
            if (!app->getWindow()->getWorkspaceWidget()->getSelectTwoPointsBend() && !app->getWindow()->getWorkspaceWidget()->getSelectTwoPointsComplete())
            {
                QPointF * pos = new QPointF(event->scenePos());
                ModelFragment* frag = app->getWindow()->getWorkspaceWidget()->findFragmentByApproxPos(pos);
                if (frag!=NULL)
                {
                    app->getWindow()->getWorkspaceWidget()->setActiveFragment(frag);
                    app->getWindow()->getWorkspaceWidget()->setActiveEndPoint(pos);
                    app->getWindow()->getWorkspaceWidget()->setActiveItem(NULL);
                    app->getWindow()->getWorkspaceWidget()->selectItem(frag->findEndPointItem(pos));
                }
                else if (this->itemAt(event->scenePos(),QTransform())!=NULL)
                {
                    QRectF r(event->scenePos().x()-16,event->scenePos().y()-16,32,32);
                    QList<QGraphicsItem*> items = this->items(r);
                    for (int i = 0; i < items.count();i++)
                    {
                        if (items.at(i)->type()==(QGraphicsItem::UserType+2))
                        {
                            ModelItem * mi = ((GraphicsPathItemModelItem*)items.at(i))->getParentItem();
                            if (mi->getSlotTrackInfo()!=NULL)
                            {
                                for (int j = 0; j < mi->getSlotTrackInfo()->getBorderEndPoints()->count();j++)
                                {
                                    QPointF ePos = event->scenePos();
                                    if (pointsAreCloseEnough(&ePos,mi->getSlotTrackInfo()->getBorderEndPoints()->at(j),mi->getProdLine()->getScaleEnum()/4.0))
                                    {
                                        if (mi->getSlotTrackInfo()->getBorders()->at(j)!=NULL)
                                            break;

                                        *pos=*mi->getSlotTrackInfo()->getBorderEndPoints()->at(j);
                                        app->getWindow()->getWorkspaceWidget()->setActiveFragment(mi->getParentFragment());
                                        app->getWindow()->getWorkspaceWidget()->setActiveEndPoint(pos);
                                        //app->getWindow()->getWorkspaceWidget()->setActiveItem(mi);
                                        app->getWindow()->getWorkspaceWidget()->selectItem(mi);
                                        i=items.count();
                                        break;
                                    }
                                }
                            }
                        }
                        else if (items.at(i)->type()==(QGraphicsItem::UserType+3))
                        {
                            app->getWindow()->getWorkspaceWidget()->setActiveFragment(NULL);
                            app->getWindow()->getWorkspaceWidget()->getSelection()->clear();
                            app->getWindow()->getWorkspaceWidget()->selectBorder(((GraphicsPathItemBorderItem*)items.at(i))->getParentItem());
                        }
                    }

                }
                else
                {
                    app->getWindow()->getWorkspaceWidget()->getSelection()->clear();
                    delete pos;
                }
            }
            else if (app->getWindow()->getWorkspaceWidget()->getSelectTwoPointsBend())
            {
                QPointF * pos = new QPointF(event->scenePos());
                if (app->getWindow()->getWorkspaceWidget()->getActiveFragment()==NULL)
                {
                    ModelFragment* frag = app->getWindow()->getWorkspaceWidget()->findFragmentByApproxPos(pos);
                    if (frag!=NULL)
                    {
                        app->getWindow()->getWorkspaceWidget()->setActiveFragment(frag);
                        app->getWindow()->getWorkspaceWidget()->setActiveEndPoint(pos);
                        app->getWindow()->getWorkspaceWidget()->setActiveItem(NULL);
                        app->getWindow()->getWorkspaceWidget()->selectItem(frag->findEndPointItem(pos));
                    }
                    else
                        delete pos;
                }
                else
                {
                    ModelFragment* frag = app->getWindow()->getWorkspaceWidget()->findFragmentByApproxPos(pos);
                    if (frag!=NULL && frag==app->getWindow()->getWorkspaceWidget()->getActiveFragment())
                        app->getWindow()->getWorkspaceWidget()->bendAndClose(frag,app->getWindow()->getWorkspaceWidget()->getActiveEndPoint(),pos);
                    else if (frag!=NULL)
                        app->getWindow()->getWorkspaceWidget()->bendAndClose(app->getWindow()->getWorkspaceWidget()->getActiveFragment(),frag,app->getWindow()->getWorkspaceWidget()->getActiveEndPoint(),pos);
                }
            }
            else if (app->getWindow()->getWorkspaceWidget()->getSelectTwoPointsComplete())
            {
                QPointF * pos = new QPointF(event->scenePos());
                if (app->getWindow()->getWorkspaceWidget()->getActiveFragment()==NULL)
                {
                    ModelFragment* frag = app->getWindow()->getWorkspaceWidget()->findFragmentByApproxPos(pos);
                    if (frag!=NULL)
                    {
                        app->getWindow()->getWorkspaceWidget()->setActiveFragment(frag);
                        app->getWindow()->getWorkspaceWidget()->setActiveEndPoint(pos);
                        app->getWindow()->getWorkspaceWidget()->setActiveItem(NULL);
                        app->getWindow()->getWorkspaceWidget()->selectItem(frag->findEndPointItem(pos));
                    }
                    else
                        delete pos;
                }
                else
                {
                    ModelFragment* frag = app->getWindow()->getWorkspaceWidget()->findFragmentByApproxPos(pos);
                    if (frag!=NULL && frag==app->getWindow()->getWorkspaceWidget()->getActiveFragment())
                        app->getWindow()->getWorkspaceWidget()->completeFragment(frag,app->getWindow()->getWorkspaceWidget()->getActiveEndPoint(),pos);
                    else if (frag!=NULL)
                        app->getWindow()->getWorkspaceWidget()->completeFragment(frag,app->getWindow()->getWorkspaceWidget()->getActiveEndPoint(),pos);
                }
            }
        }
    }

    QGraphicsScene::mousePressEvent(event);
}

void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{

    QPointF * newPoint = new QPointF(event->scenePos());
    app->getWindow()->getWorkspaceWidget()->setActiveFragment(NULL);
    app->getWindow()->getWorkspaceWidget()->setActiveEndPoint(newPoint);

    QGraphicsScene::mouseDoubleClickEvent(event);

}

void GraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (this->itemAt(event->scenePos(),QTransform())!=NULL)
    {
        if (this->itemAt(event->scenePos(),QTransform())->type()==QGraphicsItem::UserType+2)
            QGraphicsScene::contextMenuEvent(event);
        else
            this->contextMenuScene->exec(event->screenPos());
    }
    else
        this->contextMenuScene->exec(event->screenPos());
}


