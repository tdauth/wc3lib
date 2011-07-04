/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "renderstatswidget.hpp"
#include "modelview.hpp"

namespace wc3lib
{

namespace editor
{

RenderStatsWidget::RenderStatsWidget(class ModelView *modelView, QWidget *parent) : QWidget(parent), m_modelView(modelView)
{
	setupUi(this);
	connect(modelView, SIGNAL(onRendered()), this, SLOT(update()));
}


void RenderStatsWidget::update()
{
	const Ogre::RenderTarget *renderTarget = this->modelView()->viewPort()->getTarget();
	this->m_bestFrameTimeLabel->setText(tr("Best frame time: %1").arg(renderTarget->getBestFrameTime()));
	this->m_worstFrameTimeLabel->setText(tr("Worst frame time: %1").arg(renderTarget->getWorstFrameTime()));
	this->m_triangleCountLabel->setText(tr("Triangle count: %1").arg(renderTarget->getTriangleCount()));
	this->m_lastFpsLabel->setText(tr("Last FPS: %1").arg(renderTarget->getLastFPS()));
	this->m_averageFpsLabel->setText(tr("Average FPS: %1").arg(renderTarget->getAverageFPS()));
	this->m_bestFpsLabel->setText(tr("Best FPS: %1").arg(renderTarget->getBestFPS()));
	this->m_worstFpsLabel->setText(tr("Worst FPS: %1").arg(renderTarget->getWorstFPS()));
}

#include "moc_renderstatswidget.cpp"

}

}
