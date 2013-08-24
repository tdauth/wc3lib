/*
 *
 */
#include <QtGui>

#include <vector>

#include <boost/algorithm/string.hpp>
#include <KMessageBox>

#include "triggerfunctiondialog.hpp"
#include "triggereditor.hpp"

namespace wc3lib
{

namespace editor
{

TriggerFunctionDialog::TriggerFunctionDialog(map::TriggerData *triggerData, map::TriggerStrings *triggerStrings, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f), m_type(map::TriggerFunction::Type::Event), m_triggerData(triggerData), m_triggerStrings(triggerStrings), m_function(0), m_subDialog(0)
{
	setupUi(this);
	
	connect(m_dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(m_categoryComboBox, SIGNAL(activated(int)), this, SLOT(changeCategory(int)));
	connect(m_functionComboBox, SIGNAL(activated(int)), this, SLOT(changeFunction(int)));
	connect(m_descriptionLabel, SIGNAL(linkActivated(QString)), this, SLOT(editParameter(QString)));
}

void TriggerFunctionDialog::fill(BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type) {
	m_function = 0;
	
	fillInternal(type);
}

void TriggerFunctionDialog::fill(map::TriggerFunction *function)
{
	this->fill(function->type());
	
	// evaluate parameter type!
	if (type() == map::TriggerFunction::Type::Call) {
		qDebug() << "TODO: support call view!";
	}
	
	qDebug() << "Initial name: " << function->name().c_str();
	
	m_function = function;
	const int index = this->m_functionComboBox->findData(QVariant(function->name().c_str()));
	
	if (index != -1) {
		this->m_functionComboBox->setCurrentIndex(index);
		this->changeFunction(index);
	} else {
		qDebug() << "Couldn't find function " << function->name().c_str();
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

string TriggerFunctionDialog::functionName() const
{
	return this->m_functionComboBox->itemData(this->m_functionComboBox->currentIndex()).toString().toUtf8().constData();
}

template<class Functions>
void TriggerFunctionDialog::filterFunctionsByCategory(int index, const Functions &functions, const map::TriggerStrings::Entries &entries) {
	foreach (typename Functions::const_reference ref, functions) {
		if (index == 0 || (ref.second->category()->name() == this->m_categoryComboBox->itemData(index).toString().toUtf8().constData())) {
			string code = ref.first;
			map::TriggerStrings::Entries::const_iterator iterator = entries.find(code);
			
			if (iterator == entries.end()) {
				this->m_functionComboBox->addItem(code.c_str(), code.c_str());
				qDebug() << "Did not find \"" << code.c_str() << "\" in trigger strings!";
			} else {
				this->m_functionComboBox->addItem(TriggerEditor::cutQuotes(iterator->second->name()).c_str(), code.c_str());
			}
		}
	}
}

void TriggerFunctionDialog::changeCategory(int index)
{
	this->m_functionComboBox->clear();
	map::TriggerStrings::Entries::const_iterator iterator;

	switch (type()) {
		case map::TriggerFunction::Type::Event:
			this->filterFunctionsByCategory(index, this->triggerData()->events(), this->triggerStrings()->events());
			
			break;
		
		case map::TriggerFunction::Type::Condition:
			this->filterFunctionsByCategory(index, this->triggerData()->conditions(), this->triggerStrings()->conditions());
			
			break;
		
		case map::TriggerFunction::Type::Action:
			this->filterFunctionsByCategory(index, this->triggerData()->actions(), this->triggerStrings()->actions());
			
			break;
		
		case map::TriggerFunction::Type::Call:
			this->filterFunctionsByCategory(index, this->triggerData()->calls(), this->triggerStrings()->calls());
			
			break;
	}
	
	if (this->m_functionComboBox->count() > 0) {
		this->changeFunction(0);
	}
}

void TriggerFunctionDialog::fillInternal(BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type)
{
	this->m_categoryComboBox->clear();
	this->m_functionComboBox->clear();
	this->m_descriptionLabel->clear();
	
	m_type = type;
	
	if (this->type() != map::TriggerFunction::Type::Call) {
		m_checkBoxFunction->setChecked(true);
		m_categoryComboBox->setVisible(true);
		m_functionComboBox->setVisible(true);
		m_textGroupBox->setVisible(true);
		m_descriptionLabel->setVisible(true);
		
		// disable all items which are only usable for calls!
		m_checkBoxVariable->setVisible(false);
		m_variablesComboBox->setVisible(false);
		m_variablesPushButton->setVisible(false);
		
		m_checkBoxFunction->setVisible(false);
		
		m_checkBoxImported->setVisible(false);
		m_importedComboBox->setVisible(false);
		
		m_checkBoxValue->setVisible(false);
		m_textEdit->setVisible(false);
		m_valuePushButton->setVisible(false);
		m_numInput->setVisible(false);
		m_numInputDouble->setVisible(false);
		m_valueLabel->setVisible(false);
	
		this->m_categoryComboBox->addItem(tr("All"), "All");
		
		foreach (map::TriggerData::Categories::const_reference ref, triggerData()->categories()) {
			if (ref.second->displayName()) {
				this->m_categoryComboBox->addItem(ref.second->displayText().c_str(), ref.second->name().c_str());
			}
		}
		
		this->changeCategory(0);
	}
	
	switch (this->type()) {
		case map::TriggerFunction::Type::Event:
			setWindowTitle(tr("Configure Event"));
			m_categoryLabel->setText(tr("Event Type"));
			m_textGroupBox->setTitle(tr("Event Text"));
			
			break;
		
		case map::TriggerFunction::Type::Condition:
			setWindowTitle(tr("Configure Condition"));
			m_categoryLabel->setText(tr("Condition Type"));
			m_textGroupBox->setTitle(tr("Condition Text"));
			
			break;
		
		case map::TriggerFunction::Type::Action:
			setWindowTitle(tr("Configure Action"));
			m_categoryLabel->setText(tr("Action Type"));
			m_textGroupBox->setTitle(tr("Action Text"));
			
			break;
	}
}


QString TriggerFunctionDialog::triggerFunction(const map::TriggerFunction* triggerFunction) const
{
	QString result = triggerFunction->name().c_str();
	result += "(";
	
	foreach (map::TriggerFunction::Parameters::const_reference ref, triggerFunction->parameters()) {
		result += triggerFunctionParameter(&ref);
	}
	
	result += ")";
	
	return result;
}

QString TriggerFunctionDialog::triggerFunctionParameter(const map::TriggerFunctionParameter *parameter) const
{
	QString result;
	
	switch (parameter->type()) {
		case map::TriggerFunctionParameter::Type::Function:
			foreach (map::TriggerFunctionParameter::Functions::const_reference ref, parameter->functions()) {
				result += triggerFunction(&ref); // call recursively all encapsulated calls
			}
			
			break;
		
		case map::TriggerFunctionParameter::Type::Jass:
			result = parameter->value().c_str();
			
			break;
		
		case map::TriggerFunctionParameter::Type::Preset:
		{
			map::TriggerData::Parameters::const_iterator iterator = triggerData()->parameters().find(parameter->value());
			
			if (iterator != triggerData()->parameters().end()) {
				result = iterator->second->name().c_str();
			} else {
				qDebug() << "Unable to find preset " << parameter->value().c_str() << " in TriggerData.txt";
				result = parameter->value().c_str();
			}
		}
		
		case map::TriggerFunctionParameter::Type::Variable:
			result = parameter->value().c_str();
			
			break;
	}
	
	return result;
}


template<class Functions>
void TriggerFunctionDialog::showFunction(const QString &code, const Functions &functions, const map::TriggerStrings::Entries &entries)
{
	map::TriggerStrings::Entries::const_iterator iterator = entries.find(code.toUtf8().constData());
	
	if (iterator != entries.end()) {
		QString text;
		
		if (!iterator->second->name().empty()) {
			text += iterator->second->name().c_str();
			text += "<br>";
		}
		
		typedef std::vector<string> SplitVector;
		SplitVector vector; // #2: Search for tokens
		boost::algorithm::split(vector, iterator->second->layout(), boost::is_any_of(","), boost::token_compress_on);
		
		map::TriggerData::Functions::const_iterator functionIterator = functions.find(code.toUtf8().constData());

		if (functionIterator != functions.end() && !vector.empty()) {
			std::size_t i = 0;
			
			BOOST_FOREACH (SplitVector::reference ref, vector) {
				if (ref[0] == '~') {
					// encapsulated calls!
					if (function() != 0) {
						if (function()->parameters().size() > i) {
							ref = boost::str(boost::format("<a href=\"%1%\">%2%</a>") % i % triggerFunctionParameter(&function()->parameters().at(i)).toUtf8().constData());
						} else {
							qDebug() << "Invalid parameter index " << i;
						}
					} else {
						if (functionIterator->second->types().size() > i) {
							ref = boost::str(boost::format("<a href=\"%1%\">%2%</a>") % i % boost::apply_visitor(map::TriggerData::FunctionArgumentVisitor(), functionIterator->second->types()[i]));
						} else {
							qDebug() << "Invalid argument index " << i;
						}
					}
				} else {
					ref = TriggerEditor::cutQuotes(ref);
				}
			}
			
			text += tr("Layout:<br>");
			
			BOOST_FOREACH (SplitVector::reference ref, vector) {
				text += ref.c_str();
			}
		}
		
		if (!iterator->second->hint().empty()) {
			text += "<br>";
			text + tr("Hint:<br>");
			text += iterator->second->hint().c_str();
		}
		
		this->m_descriptionLabel->setText(text);
		
	} else {
		qDebug() << "Did not find function " << code;
	}
}

template<class Functions>
typename Functions::const_iterator TriggerFunctionDialog::triggerFunction(const string& code, const Functions& functions) const
{
	return functions.find(code);
}

map::TriggerStrings::Entries::const_iterator TriggerFunctionDialog::triggerFunctionEntry(const string& code, BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type) const
{
	map::TriggerStrings::Entries::const_iterator iterator;
	
	switch (type) {
		case map::TriggerFunction::Type::Event:
			iterator = triggerStrings()->events().find(code);
			
			if (iterator == triggerStrings()->events().end()) {
				throw std::runtime_error(_("No corresponding TriggerStrings.txt entry."));
			}
			
			break;
			
		case map::TriggerFunction::Type::Condition:
			iterator = triggerStrings()->conditions().find(code);
			
			if (iterator == triggerStrings()->conditions().end()) {
				throw std::runtime_error(_("No corresponding TriggerStrings.txt entry."));
			}
			
			break;
			
		case map::TriggerFunction::Type::Action:
			iterator = triggerStrings()->actions().find(code);
			
			if (iterator == triggerStrings()->actions().end()) {
				throw std::runtime_error(_("No corresponding TriggerStrings.txt entry."));
			}
			
			break;
			
		case map::TriggerFunction::Type::Call:
			iterator = triggerStrings()->calls().find(code);
			
			if (iterator == triggerStrings()->calls().end()) {
				throw std::runtime_error(_("No corresponding TriggerStrings.txt entry."));
			}
			
			break;
	}
	
	return iterator;
}

void TriggerFunctionDialog::changeFunction(int index)
{
	const QString code = this->m_functionComboBox->itemData(index).toString();
	
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

void TriggerFunctionDialog::editParameter(const QString &parameter)
{
	qDebug() << "Editing parameter: " << parameter;
	/*
	int index = 0;
	bool ok = false;
	parameter.toInt(&ok, index);
	
	if (!ok) {
		qDebug() << "Invalid index " << parameter;
		
		return;
	}
	
	const string &code = this->m_functionComboBox->itemData(this->m_functionComboBox->currentIndex()).toString().toUtf8().constData();
	
	switch (function()->parameters().at(i).type()) {
		case map::TriggerFunctionParameter::Type::Preset:
		{
			break;
		}
		
		case map::TriggerFunctionParameter::Type::Variable:
		{
			break;
		}
		
		case map::TriggerFunctionParameter::Type::Function:
		{
			map::TriggerData::Functions::const_iterator functionIterator = functions.find(code);
			
			if (functionIterator != functions.end()) {
					map::TriggerData::Type *type = boost::get<map::TriggerData::Type*>(functionIterator->second->types()[index]);
					
					m_subDialog = new TriggerFunctionDialog(this->triggerData(), this->triggerStrings(), this);
					
					
					m_subDialog->fill(map::TriggerFunction::Type::Call, );
					
					m_subDialog->show();
					
			}
			
			break;
		}
		
		case map::TriggerFunctionParameter::Type::Jass:
		{
			break;
		}
	}
	*/
}

}

}
