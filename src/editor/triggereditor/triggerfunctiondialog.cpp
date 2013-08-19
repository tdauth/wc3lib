/*
 *
 */
#include <QtGui>

#include <vector>

#include <boost/algorithm/string.hpp>

#include "triggerfunctiondialog.hpp"

namespace wc3lib
{

namespace editor
{

TriggerFunctionDialog::TriggerFunctionDialog(map::TriggerData *triggerData, map::TriggerStrings *triggerStrings, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_type(map::TriggerFunction::Type::Event), m_triggerData(triggerData), m_triggerStrings(triggerStrings)
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
			this->m_categoryComboBox->addItem(ref.second->displayText().c_str(), ref.second->name().c_str());
		}
	}
	
	this->changeCategory(0);

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
	
	if (this->m_functionComboBox->count() > 0) {
		this->changeFunction(0);
	}
}

template<class Functions>
void TriggerFunctionDialog::showFunction(const QString &code, const Functions &functions, const map::TriggerStrings::Entries &entries)
{
	map::TriggerStrings::Entries::const_iterator iterator = entries.find(code.toUtf8().constData());
	
	if (iterator != entries.end()) {
		QString text;
		
		if (!iterator->second->name().empty()) {
			text += iterator->second->name().c_str();
			text += "\n";
		}
		
		typedef std::vector<string> SplitVector;
		SplitVector vector; // #2: Search for tokens
		boost::algorithm::split(vector, iterator->second->layout(), boost::is_any_of(","), boost::token_compress_on);
		
		map::TriggerData::Functions::const_iterator functionIterator = functions.find(code.toUtf8().constData());
		
		if (functionIterator != functions.end() && !vector.empty()) {
			std::size_t i = 0;
			
			BOOST_FOREACH (SplitVector::reference ref, vector) {
				if (ref[0] == '~') {
					if (functionIterator->second->types().size() > i) {
						ref = boost::apply_visitor(map::TriggerData::FunctionArgumentVisitor(), functionIterator->second->types()[i]);
						++i;
					} else {
						qDebug() << "Invalid argument index " << i;
					}
				} else {
					ref = ref.substr(1, ref.length() - 2); // cut quotes
				}
			}
			
			text += "\n";
			
			BOOST_FOREACH (SplitVector::reference ref, vector) {
				text += ref.c_str();
			}
		}
		
		if (!iterator->second->hint().empty()) {
			text += "\n";
			text += iterator->second->hint().c_str();
		}
		
		this->m_descriptionLabel->setText(text);
		
	} else {
		qDebug() << "Did not find function " << code;
	}
}

void TriggerFunctionDialog::changeFunction(int index)
{
	const QString code = this->m_functionComboBox->itemText(index);
	
	switch (type()) {
		case map::TriggerFunction::Type::Event:
		{
			this->showFunction(code, this->triggerData()->events(), this->triggerStrings()->events());
			break;
		}
		
		case map::TriggerFunction::Type::Condition:
		{
			this->showFunction(code, this->triggerData()->conditions(), this->triggerStrings()->conditions());
			break;
		}
		
		case map::TriggerFunction::Type::Action:
		{
			this->showFunction(code, this->triggerData()->actions(), this->triggerStrings()->actions());
			break;
		}
		
		case map::TriggerFunction::Type::Call:
		{
			this->showFunction(code, this->triggerData()->calls(), this->triggerStrings()->calls());
			break;
		}
	}
	
	//map::TriggerData::Functions::const_iterator iterator = this->triggerData()->events().find(code);
	
		
}

}

}
