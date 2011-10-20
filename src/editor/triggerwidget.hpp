/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_TRIGGERWIDGET_HPP
#define WC3LIB_EDITOR_TRIGGERWIDGET_HPP

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>

#include "ui/ui_triggertopwidget.h"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

class TriggerWidget : public QWidget, protected Ui::TriggerTopWidget
{
	Q_OBJECT

	public slots:
		void enableTrigger(bool enable);
		void setTriggerInitiallyOn(bool on);
		void updateTriggerComment();

	public:
		typedef QMap<QTreeWidgetItem*, map::TriggerFunction*> Functions;

		TriggerWidget(class TriggerEditor *triggerEditor);

		map::Trigger* trigger() const;
		QTreeWidget* functionsTreeWidget() const;
		QTreeWidgetItem* eventsItem() const;
		QTreeWidgetItem* conditionsItem() const;
		QTreeWidgetItem* actionsItem() const;
		const Functions& functions() const;

		void showTrigger(map::Trigger *trigger);

	protected slots:
		void itemDoubleClicked(QTreeWidgetItem *item, int column);

	protected:
		void setTrigger(map::Trigger *trigger);
		Functions& functions();

	private:
		class TriggerEditor *m_triggerEditor;
		map::Trigger *m_trigger;
		QTreeWidget *m_functionsTreeWidget;
		QTreeWidgetItem *m_eventsItem;
		QTreeWidgetItem *m_conditionsItem;
		QTreeWidgetItem *m_actionsItem;
		Functions m_functions;
};

inline map::Trigger* TriggerWidget::trigger() const
{
	return this->m_trigger;
}

inline QTreeWidget* TriggerWidget::functionsTreeWidget() const
{
	return m_functionsTreeWidget;
}

inline QTreeWidgetItem* TriggerWidget::eventsItem() const
{
	return m_eventsItem;
}

inline QTreeWidgetItem* TriggerWidget::conditionsItem() const
{
	return m_conditionsItem;
}

inline QTreeWidgetItem* TriggerWidget::actionsItem() const
{
	return m_actionsItem;
}

inline const TriggerWidget::Functions& TriggerWidget::functions() const
{
	return m_functions;
}

inline void TriggerWidget::setTrigger(map::Trigger *trigger)
{
	m_trigger = trigger;
}

inline TriggerWidget::Functions& TriggerWidget::functions()
{
	return m_functions;
}


}

}

#endif
