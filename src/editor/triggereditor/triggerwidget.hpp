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

#ifndef WC3LIB_EDITOR_TRIGGEREDITOR_TRIGGERWIDGET_HPP
#define WC3LIB_EDITOR_TRIGGEREDITOR_TRIGGERWIDGET_HPP

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>

#include "ui_triggertopwidget.h"
#include "../../map.hpp"
#include "triggereditor.hpp"
#include "triggerfunctiondialog.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief Widget for displaying and editing common triggers and custom text triggers.
 * For both types, common triggers (with functions) and custom text triggers (with script code) there is a top widget which provides you a text edit for the trigger's comment and some buttons for enabling or disabling the trigger (\ref enableTrigger()) and setting it initially on (\ref setTriggerInitiallyOn()).
 * The difference between common triggers and custom text triggers is that common triggers have functions displayed in a tree widget (\ref functionsTreeWidget()) separated into events, conditions and actions. All functions of the shown trigger do have exactly one corresponding tree widget item in the tree widget which can be accessed via \ref functions().
 * In contrast, custom text triggers do only need a text edit for their script code which can be accessed via \ref textEdit().
 * \sa TriggerEditor
 */
class TriggerWidget : public QWidget, protected Ui::TriggerTopWidget
{
	Q_OBJECT

	public slots:
		void enableTrigger(bool enable);
		void setTriggerInitiallyOn(bool on);
		void updateTriggerComment();

		void newEvent();
		void newCondition();
		void newAction();

	public:
		typedef QMap<QTreeWidgetItem*, map::TriggerFunction*> Functions;

		TriggerWidget(class TriggerEditor *triggerEditor);

		TriggerEditor* triggerEditor() const;
		map::Trigger* trigger() const;
		/**
		 * Tree widget is required for non custom text triggers (\ref map::Trigger::isCustomText()) which do have trigger functions (\ref map::Trigger::functions()).
		 * Each function has its corresponding tree widget item which can be accessed via \ref functions().
		 * Additionally, there are top level tree widget items for separating functions into events, conditions and actions (\ref map::Trigger::TriggerFunction::Type).
		 * \sa rootItem()
		 * \sa eventsItem()
		 * \sa conditionsItem()
		 * \sa actionsItem()
		 * \sa functions()
		 */
		QTreeWidget* functionsTreeWidget() const;
		/**
		 * The root item has the same format as the entry in the left-side tree widget (\ref TriggerEditor::rootItem())
		 */
		QTreeWidgetItem* rootItem() const;
		QTreeWidgetItem* eventsItem() const;
		QTreeWidgetItem* conditionsItem() const;
		QTreeWidgetItem* actionsItem() const;
		const Functions& functions() const;
		/**
		 * Text edit is required for custom text triggers (\ref map::Trigger::isCustomText()).
		 * Otherwise it is hidden.
		 * \sa functionsTreeWidget()
		 */
		KTextEdit* textEdit() const;

		TriggerFunctionDialog* functionDialog() const;

		QString triggerFunctionName(map::TriggerFunction *triggerFunction) const;
		void showTrigger(map::Trigger *trigger, const string &customText);

	protected slots:
		void itemDoubleClicked(QTreeWidgetItem *item, int column);

	protected:
		void setTrigger(map::Trigger *trigger);
		Functions& functions();

	private:
		QTreeWidgetItem* addTreeItem(map::TriggerFunction *function);
		void refreshBasicTreeItems();

		TriggerEditor *m_triggerEditor;
		map::Trigger *m_trigger;
		QTreeWidget *m_functionsTreeWidget;
		QTreeWidgetItem *m_rootItem;
		QTreeWidgetItem *m_eventsItem;
		QTreeWidgetItem *m_conditionsItem;
		QTreeWidgetItem *m_actionsItem;
		Functions m_functions;
		KTextEdit *m_textEdit;

		class TriggerFunctionDialog *m_functionDialog;
};

inline TriggerEditor* TriggerWidget::triggerEditor() const
{
	return m_triggerEditor;
}

inline map::Trigger* TriggerWidget::trigger() const
{
	return this->m_trigger;
}

inline QTreeWidget* TriggerWidget::functionsTreeWidget() const
{
	return m_functionsTreeWidget;
}

inline QTreeWidgetItem* TriggerWidget::rootItem() const
{
	return m_rootItem;
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

inline KTextEdit* TriggerWidget::textEdit() const
{
	return m_textEdit;
}

inline TriggerFunctionDialog* TriggerWidget::functionDialog() const
{
	if (m_functionDialog == 0 && triggerEditor()->source() != 0) {
		const_cast<TriggerWidget*>(this)->m_functionDialog = new TriggerFunctionDialog(triggerEditor(), triggerEditor()->source()->sharedData()->triggerData().get(), triggerEditor()->source()->sharedData()->triggerStrings().get(), this->triggerEditor()->triggers(), const_cast<TriggerWidget*>(this));
	}

	return m_functionDialog;
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
