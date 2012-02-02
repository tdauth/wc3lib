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
	trigger()->setEnabled(enable);
}

void TriggerWidget::setTriggerInitiallyOn(bool on)
{
	// TODO change icon in tree widget of trigger editor
	trigger()->setInitiallyOn(on);
}

void TriggerWidget::updateTriggerComment()
{
	trigger()->setDescription(m_commentTextEdit->toPlainText().toUtf8().constData());
}

TriggerWidget::TriggerWidget(class TriggerEditor *triggerEditor) : m_triggerEditor(triggerEditor), m_trigger(0), m_functionsTreeWidget(new QTreeWidget(this)), m_rootItem(new QTreeWidgetItem(functionsTreeWidget())), m_eventsItem(new QTreeWidgetItem(rootItem())), m_conditionsItem(new QTreeWidgetItem(rootItem())), m_actionsItem(new QTreeWidgetItem(rootItem())), m_textEdit(new KTextEdit(this)), QWidget(triggerEditor)
{
	Ui::TriggerTopWidget::setupUi(this);
	m_functionsLayout->addWidget(functionsTreeWidget());
	m_functionsLayout->addWidget(textEdit());
	// TODO set icons and text from source!
	// TODO make collapsable by function?
	functionsTreeWidget()->setHeaderHidden(true);
	eventsItem()->setText(0, tr("Events"));
	conditionsItem()->setText(0, tr("Conditions"));
	actionsItem()->setText(0, tr("Actions"));

	connect(m_enabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableTrigger(bool)));
	connect(m_onAtBeginningCheckBox, SIGNAL(toggled(bool)), this, SLOT(setTriggerInitiallyOn(bool)));
	connect(m_commentTextEdit, SIGNAL(textChanged()), this, SLOT(updateTriggerComment()));

	connect(functionsTreeWidget(), SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));

	textEdit()->hide();
}

void TriggerWidget::showTrigger(map::Trigger* trigger, const map::string &customText)
{
	if (this->trigger() != 0)
	{
		setTrigger(0);
		functionsTreeWidget()->clear();
		functions().clear();
	}

	m_enabledCheckBox->setChecked(trigger->isEnabled());
	m_onAtBeginningCheckBox->setChecked(trigger->isInitiallyOn());
	m_commentTextEdit->setText(trigger->description().c_str());

	if (trigger->isCustomText())
	{
		functionsTreeWidget()->hide();
		textEdit()->show();
		textEdit()->setText(customText.c_str());
	}
	else
	{
		textEdit()->hide();
		functionsTreeWidget()->show();
		rootItem()->setText(0, trigger->name().c_str());

		// TODO set icon and background color

		if (trigger->isInitiallyOn())
			rootItem()->setBackgroundColor(0, QColor(Qt::gray));
		else
			rootItem()->setBackgroundColor(0, QColor(Qt::white));

		for (map::int32 i = 0; i < trigger->functions().size(); ++i)
		{
			QTreeWidgetItem *item = new QTreeWidgetItem();
			item->setText(0, trigger->functions()[i]->name().c_str());
			// TODO set icon and mark as enabled or not using ->isEnabled()
			// TODO show hard coded parameters
			functions().insert(item, trigger->functions()[i].get());

			switch (trigger->functions()[i]->type())
			{
				case map::TriggerFunction::Type::Event:
					eventsItem()->addChild(item);

					break;

				case map::TriggerFunction::Type::Condition:
					conditionsItem()->addChild(item);

					break;

				case map::TriggerFunction::Type::Action:
					actionsItem()->addChild(item);

					break;
			}
		}
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
	if (iterator != functions().end())
		qDebug() << "Got trigger function: " << (*iterator)->name().c_str();
}

#include "moc_triggerwidget.cpp"

}

}
