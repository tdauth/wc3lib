/*
 *
 */
#include <QtGui>

#include "triggerfunctiondialog.hpp"

namespace wc3lib
{

namespace editor
{

TriggerFunctionDialog::TriggerFunctionDialog(map::TriggerData *triggerData, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_type(map::TriggerFunction::Type::Event), m_triggerData(triggerData)
{
	setupUi(this);
	
	connect(m_dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(m_categoryComboBox, SIGNAL(activated(int)), this, SLOT(changeCategory(int)));
	connect(m_functionComboBox, SIGNAL(activated(int)), this, SLOT(changeFunction(int)));
}

void TriggerFunctionDialog::fill(BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type)
{
	this->m_categoryComboBox->clear();
	m_type = type;
	
	this->m_categoryComboBox->addItem(tr("All"), "All");
	
	foreach (map::TriggerData::Categories::const_reference ref, triggerData()->categories()) {
		if (ref.second->displayName()) {
			this->m_categoryComboBox->addItem(ref.second->displayText().c_str(), ref.second->displayText().c_str());
		}
	}

	/*
	if (item->parent() == eventsItem()) {
		foreach (map::TriggerData::Eve::const_reference ref, triggerData->categories()) {
		if (ref.displayText()) {
			dialog->m_categoryComboBox->addItem(ref.displayText().c_str(), ref.displayName().c_str());
		}
	}
	*/
}

void TriggerFunctionDialog::changeCategory(int index)
{
	this->m_functionComboBox->clear();

	switch (type()) {
		case map::TriggerFunction::Type::Event:
			
			foreach (map::TriggerData::Functions::const_reference ref, triggerData()->events()) {
				if (index == 0 || (ref.second->category()->name() == this->m_categoryComboBox->itemData(index).toString().toUtf8().constData())) {
					this->m_functionComboBox->addItem(ref.second->code().c_str());
				}
			}
			
			break;
			
		case map::TriggerFunction::Type::Condition:
			
			foreach (map::TriggerData::Functions::const_reference ref, triggerData()->conditions()) {
				if (index == 0 || (ref.second->category()->name() == this->m_categoryComboBox->itemData(index).toString().toUtf8().constData())) {
					this->m_functionComboBox->addItem(ref.second->code().c_str());
				}
			}
			
			break;
		
		case map::TriggerFunction::Type::Action:
			
			foreach (map::TriggerData::Functions::const_reference ref, triggerData()->actions()) {
				if (index == 0 || (ref.second->category()->name() == this->m_categoryComboBox->itemData(index).toString().toUtf8().constData())) {
					this->m_functionComboBox->addItem(ref.second->code().c_str());
				}
			}
			
			break;
		
		case map::TriggerFunction::Type::Call:
			
			foreach (map::TriggerData::Calls::const_reference ref, triggerData()->calls()) {
				if (index == 0 || (ref.second->category()->name() == this->m_categoryComboBox->itemData(index).toString().toUtf8().constData())) {
					this->m_functionComboBox->addItem(ref.second->code().c_str());
				}
			}
			
			break;
	}
}

void TriggerFunctionDialog::changeFunction(int index)
{
	switch (type()) {
		case map::TriggerFunction::Type::Event:
		{
			map::TriggerData::Functions::const_iterator iterator = this->triggerData()->events().find(this->m_functionComboBox->itemText(index).toUtf8().constData());
			
			if (iterator != this->triggerData()->events().end()) {
				QString text;
				
				foreach (map::TriggerData::Function::ArgumentTypes::const_reference ref, iterator->second->types()) {
					text += "Argument";
					//text += ref.
				}
				
			} else {
				qDebug() << "Did not find function " << this->m_functionComboBox->itemText(index);
			}
			
			break;
		}
	}
		
}

}

}
