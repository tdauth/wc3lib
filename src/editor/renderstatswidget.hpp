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

#ifndef WC3LIB_EDITOR_RENDERSTATSWIDGET_HPP
#define WC3LIB_EDITOR_RENDERSTATSWIDGET_HPP

#include <QWidget>

#include "ui_renderstatswidget.h"

namespace wc3lib
{

namespace editor
{

/**
* Simple widget which is related to a specific \ref ModelView instance.
* Shows some render statistics of its model view's current render target.
* Is updated automatically each time the model view renders a frame.
*/
class RenderStatsWidget : public QWidget, protected Ui::RenderStatsWidget
{
	Q_OBJECT

	public:
		RenderStatsWidget(class ModelView *modelView, QWidget *parent = 0);

		class ModelView* modelView() const;

	public slots:
		void update();

	protected:
		class ModelView *m_modelView;
};


inline class ModelView* RenderStatsWidget::modelView() const
{
	return m_modelView;
}


}

}

#endif
