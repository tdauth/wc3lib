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

#include <QtGui>

#include "triggerwidget.hpp"

namespace wc3lib
{

namespace editor
{

void TriggerWidget::enableTrigger(bool enable)
{
	// TODO change background in tree widget of trigger editor
	if (trigger() != 0) {
		trigger()->setEnabled(enable);
	}
}

void TriggerWidget::setTriggerInitiallyOn(bool on)
{
	if (trigger() != 0) {
		// TODO change icon in tree widget of trigger editor
		trigger()->setInitiallyOn(on);
	}
}

void TriggerWidget::updateTriggerComment()
{
	if (trigger() != 0) {
		trigger()->setDescription(m_commentTextEdit->toPlainText().toUtf8().constData());
	}
}

void TriggerWidget::newEvent()
{
	functionDialog()->fill(map::TriggerFunction::Type::Event);
	functionDialog()->show();

	if (functionDialog()->result() == QDialog::Accepted) {
		std::auto_ptr<map::TriggerFunction> function(new map::TriggerFunction());
		function->setType(map::TriggerFunction::Type::Event);
		function->setName(functionDialog()->functionName());
		function->setIsEnabled(true);

		// TODO set parameters

		QTreeWidgetItem *item = addTreeItem(function.get());

		this->trigger()->functions().push_back(function);

		item->setSelected(true);
		functionsTreeWidget()->scrollToItem(item);
	}
}

void TriggerWidget::newCondition()
{
	functionDialog()->fill(map::TriggerFunction::Type::Condition);
	functionDialog()->show();

	if (functionDialog()->result() == QDialog::Accepted) {
		std::auto_ptr<map::TriggerFunction> function(new map::TriggerFunction());
		function->setType(map::TriggerFunction::Type::Condition);
		function->setName(functionDialog()->functionName());
		function->setIsEnabled(true);

		// TODO set parameters

		QTreeWidgetItem *item = addTreeItem(function.get());

		this->trigger()->functions().push_back(function);

		item->setSelected(true);
		functionsTreeWidget()->scrollToItem(item);
	}
}

void TriggerWidget::newAction()
{
	functionDialog()->fill(map::TriggerFunction::Type::Action);
	functionDialog()->show();

	if (functionDialog()->result() == QDialog::Accepted) {
		std::auto_ptr<map::TriggerFunction> function(new map::TriggerFunction());
		function->setType(map::TriggerFunction::Type::Action);
		function->setName(functionDialog()->functionName());
		function->setIsEnabled(true);

		// TODO set parameters

		QTreeWidgetItem *item = addTreeItem(function.get());

		this->trigger()->functions().push_back(function);

		item->setSelected(true);
		functionsTreeWidget()->scrollToItem(item);
	}
}


TriggerWidget::TriggerWidget(class TriggerEditor *triggerEditor) : m_triggerEditor(triggerEditor), m_trigger(0), m_functionsTreeWidget(new QTreeWidget(this)), m_textEdit(new KTextEdit(this)), m_functionDialog(0), QWidget(triggerEditor)
{
	Ui::TriggerTopWidget::setupUi(this);
	m_functionsTreeWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_triggerFunctionsWidget->layout()->addWidget(functionsTreeWidget());
	// TODO set icons and text from source!
	// TODO make collapsable by function?
	functionsTreeWidget()->setHeaderHidden(true);
	functionsTreeWidget()->setSelectionMode(QAbstractItemView::SingleSelection);
	functionsTreeWidget()->setSelectionBehavior(QAbstractItemView::SelectItems);
	functionsTreeWidget()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	functionsTreeWidget()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	refreshBasicTreeItems();

	connect(functionsTreeWidget(), SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));

	textEdit()->hide();
}

QString TriggerWidget::triggerFunctionName(map::TriggerFunction* triggerFunction) const
{
	if (this->triggerEditor()->source() != 0) {
		map::TriggerStrings *triggerStrings = this->triggerEditor()->source()->sharedData()->triggerStrings().get();

		if (triggerStrings != 0) {
			map::TriggerStrings::Entries::const_iterator iterator = triggerStrings->entries(triggerFunction->type()).find(triggerFunction->name());

			if (iterator != triggerStrings->entries(triggerFunction->type()).end()) {
				return TriggerEditor::cutQuotes(iterator->second->name()).c_str();
			}
		}
	}

	return triggerFunction->name().c_str();
}

void TriggerWidget::showTrigger(map::Trigger* trigger, const string &customText)
{
	if (this->trigger() != 0)
	{
		setTrigger(0);
		functionsTreeWidget()->clear();
		this->refreshBasicTreeItems();
		functions().clear();
	}

	m_enabledCheckBox->setChecked(trigger->isEnabled());
	m_onAtBeginningCheckBox->setChecked(trigger->isInitiallyOn());
	m_commentTextEdit->setText(trigger->description().c_str());

	if (trigger->isCustomText())
	{
		qDebug() << "Custom text!";
		functionsTreeWidget()->hide();
		textEdit()->show();
		textEdit()->setText(customText.c_str());
	}
	else
	{
		qDebug() << "NO Custom text!";
		textEdit()->hide();
		functionsTreeWidget()->show();
		rootItem()->setText(0, trigger->name().c_str());

		// TODO set icon and background color

		if (trigger->isInitiallyOn()) {
			rootItem()->setForeground(0, QColor(Qt::black));
		}
		else {
			rootItem()->setForeground(0, QColor(Qt::gray));
			//rootItem()->setIcon(0, );
		}

		for (int32 i = 0; i < trigger->functions().size(); ++i)
		{
			map::TriggerFunction *function = &trigger->functions()[i];

			addTreeItem(function);
		}

		functionsTreeWidget()->expandAll(); // make all visible
	}

	setTrigger(trigger);
}

void TriggerWidget::itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item == rootItem() || item == eventsItem() || item == conditionsItem() || item == actionsItem())
	{
		functionsTreeWidget()->collapseItem(item);

		return;
	}

	Functions::iterator iterator = functions().find(item);

	// TODO open parameters window
	if (iterator != functions().end()) {
		qDebug() << "Got trigger function: " << (*iterator)->name().c_str();

		functionDialog()->fill(this->functions()[item]);

		functionDialog()->show();
	}
}

QTreeWidgetItem* TriggerWidget::addTreeItem(map::TriggerFunction* function)
{
	QTreeWidgetItem *item = new QTreeWidgetItem();

	if (!function->isEnabled()) {
		item->setBackgroundColor(0, QColor(Qt::gray));
	}

	// TODO set icon and mark as enabled or not using ->isEnabled()
	// TODO show hard coded parameters
	functions().insert(item, function);
	const map::TriggerData *triggerData = triggerEditor()->source()->sharedData()->triggerData().get();
	const map::TriggerStrings *triggerStrings = triggerEditor()->source()->sharedData()->triggerStrings().get();

	switch (function->type())
	{
		case map::TriggerFunction::Type::Event:
		{
			const QString text = TriggerEditor::triggerFunctionText(triggerData, triggerStrings, function->name().c_str(), function, triggerData->events(), triggerStrings->events(), false, false, true);

			item->setText(0, text);
			eventsItem()->addChild(item);

			break;
		}

		case map::TriggerFunction::Type::Condition:
		{
			const QString text = TriggerEditor::triggerFunctionText(triggerData, triggerStrings, function->name().c_str(), function, triggerData->conditions(), triggerStrings->conditions(), false, false, true);

			item->setText(0, text);
			conditionsItem()->addChild(item);

			break;
		}

		case map::TriggerFunction::Type::Action:
		{
			const QString text = TriggerEditor::triggerFunctionText(triggerData, triggerStrings, function->name().c_str(), function, triggerData->actions(), triggerStrings->actions(), false, false, true);

			item->setText(0, text);
			actionsItem()->addChild(item);

			break;
		}
	}

	return item;
}

void TriggerWidget::refreshBasicTreeItems()
{
	m_rootItem = new QTreeWidgetItem(functionsTreeWidget());
	m_eventsItem = new QTreeWidgetItem(rootItem());
	m_conditionsItem = new QTreeWidgetItem(rootItem());
	m_actionsItem = new QTreeWidgetItem(rootItem());

	eventsItem()->setText(0, tr("Events"));
	conditionsItem()->setText(0, tr("Conditions"));
	actionsItem()->setText(0, tr("Actions"));

	connect(m_enabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableTrigger(bool)));
	connect(m_onAtBeginningCheckBox, SIGNAL(toggled(bool)), this, SLOT(setTriggerInitiallyOn(bool)));
	connect(m_commentTextEdit, SIGNAL(textChanged()), this, SLOT(updateTriggerComment()));
}

#include "moc_triggerwidget.cpp"

}

}
