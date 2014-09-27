/***************************************************************************
 *   Copyright (C) 2013 by Tamino Dauth                                    *
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

#include <KMessageBox>

#include "triggerfunctiondialog.hpp"
#include "triggereditor.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

TriggerFunctionDialog::TriggerFunctionDialog(TriggerEditor *triggerEditor, map::TriggerData *triggerData, map::TriggerStrings *triggerStrings, map::Triggers *triggers, QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
, m_triggerEditor(triggerEditor)
, m_triggerData(triggerData)
, m_triggerStrings(triggerStrings)
, m_triggers(triggers)
, m_type(map::TriggerFunction::Type::Event)
, m_function(0)
, m_freeFunction(false)
, m_argumentIndex(0)
, m_subDialog(0)
{
	setupUi(this);

	connect(m_dialogButtonBox, SIGNAL(accepted()), this, SLOT(acceptOnlyValid()));
	connect(m_dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(m_categoryComboBox, SIGNAL(activated(int)), this, SLOT(changeCategory(int)));
	connect(m_functionComboBox, SIGNAL(activated(int)), this, SLOT(changeFunctionToNew(int)));
	connect(m_descriptionLabel, SIGNAL(linkActivated(QString)), this, SLOT(editParameter(QString)));

	connect(m_variablesPushButton, SIGNAL(clicked(bool)), this, SLOT(editVariables(bool)));
}

TriggerFunctionDialog::~TriggerFunctionDialog()
{
	if (m_freeFunction && m_function != 0) {
		delete m_function;
	}
}

void TriggerFunctionDialog::fillCall(int argumentIndex)
{
	this->m_argumentIndex = argumentIndex;

	this->fill(map::TriggerFunction::Type::Call);
}

void TriggerFunctionDialog::fillCall(map::TriggerFunction *function, int argumentIndex)
{
	this->m_argumentIndex = argumentIndex;

	this->fill(function);
}

void TriggerFunctionDialog::fill(map::TriggerFunction::Type type)
{
	m_function = 0;

	fillInternal(type);
}

void TriggerFunctionDialog::fill(map::TriggerFunction *function)
{
	this->fill(function->type());

	// evaluate parameter type!
	if (type() == map::TriggerFunction::Type::Call)
	{
		qDebug() << "TODO: support call view!";
	}

	qDebug() << "Initial name: " << function->name().c_str();

	m_function = function;
	const int index = this->m_functionComboBox->findData(QVariant(function->name().c_str()));

	if (index != -1)
	{
		this->m_functionComboBox->setCurrentIndex(index);
		this->changeFunction(index);
	}
	else
	{
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

void TriggerFunctionDialog::fill(map::TriggerFunction::Type type, map::TriggerData::Function* function)
{
	this->fillInternal(type);

	const int index = this->m_functionComboBox->findData(function->code().c_str());

	if (index != -1)
	{
		this->m_functionComboBox->setCurrentIndex(index);
		changeFunction(index); // update parameters and description

		// TODO
		// set parameter values
	}
}

bool TriggerFunctionDialog::isValid() const
{
	if (m_function == 0)
	{
		return false;
	}

	foreach (map::TriggerFunction::Parameters::const_reference ref, m_function->parameters())
	{
		if (ref.type() == map::TriggerFunctionParameter::Type::Jass && ref.value().empty())
		{
			return false;
		}
	}

	return true;
}

void TriggerFunctionDialog::apply(map::TriggerFunctionParameter *parameter)
{
	if (m_radioButtonPreset->isChecked())
	{
		parameter->setType(map::TriggerFunctionParameter::Type::Preset);
		const int index = this->m_presetComboBox->currentIndex();
		parameter->setValue(this->m_presetComboBox->itemData(index).toByteArray().constData());
	} else if (m_radioButtonVariable->isChecked())
	{
		parameter->setType(map::TriggerFunctionParameter::Type::Variable);
		const int index = this->m_variablesComboBox->currentIndex();
		parameter->setValue(this->m_variablesComboBox->itemData(index).toByteArray().constData());
	} else if (m_radioButtonFunction->isChecked())
	{
		std::auto_ptr<map::TriggerFunction> function(new map::TriggerFunction());
		//this->apply(function.get());
		// TODO apply function argument!
		parameter->functions().push_back(function);
	}
	else if (m_radioButtonImported->isChecked())
	{ // TODO Frozen Throne only?
		parameter->setType(map::TriggerFunctionParameter::Type::Jass);
		const int index = this->m_importedComboBox->currentIndex();
		parameter->setValue(this->m_importedComboBox->itemData(index).toByteArray().constData());
	}
	else if (m_radioButtonValue->isChecked())
	{
		parameter->setType(map::TriggerFunctionParameter::Type::Jass);

		if (m_textEdit->isVisible())
		{ // TODO store strings in strings file of map
			parameter->setValue(m_textEdit->toPlainText().toStdString());
		}
		else if (m_numInput->isVisible())
		{
			parameter->setValue((QString("%1").arg(m_numInput->value())).toStdString());
		}
		else if (m_numInputDouble->isVisible())
		{
			parameter->setValue((QString("%1").arg(m_numInputDouble->value())).toStdString());
		}
	}
}

void TriggerFunctionDialog::apply(map::TriggerFunction* function)
{
	const int index = this->m_functionComboBox->currentIndex();
	function->setName(this->m_functionComboBox->itemData(index).toByteArray().constData());
	function->setType(m_type);

	if (this->function() != 0)
	{
		function->setIsEnabled(this->function()->isEnabled());
	}
	else
	{
		function->setIsEnabled(true);
	}
}

string TriggerFunctionDialog::functionName() const
{
	return this->m_functionComboBox->itemData(this->m_functionComboBox->currentIndex()).toString().toStdString();
}

template<class Functions>
void TriggerFunctionDialog::filterFunctionsByCategory(int index, const Functions &functions, const map::TriggerStrings::Entries &entries)
{
	/*
	 * TODO order alphabetically by category etc.
	 */
	QMap<QString, QString> result;

	foreach (typename Functions::const_reference ref, functions)
	{
		const map::TriggerData::Category *category = ref.second->category();
		const string categoryName = category->name();

		if (index == 0 || (categoryName == this->m_categoryComboBox->itemData(index).toString().toStdString()))
		{
			string code = ref.first;
			map::TriggerStrings::Entries::const_iterator iterator = entries.find(code);
			const QString categoryText = this->triggerEditor()->source()->sharedData()->tr(category->displayText().c_str());

			// show category for index 0 - "All"
			if (iterator == entries.end())
			{
				const QString name = index == 0 && category->displayName() ? QString(tr("%1 - %2")).arg(categoryText).arg(code.c_str()) : code.c_str();
				result.insert(name, code.c_str());
				qDebug() << "Did not find \"" << code.c_str() << "\" in trigger strings!";
			}
			else
			{
				const string functionName = TriggerEditor::cutQuotes(iterator->second->name());
				const QString name = index == 0 && category->displayName() ? QString(tr("%1 - %2")).arg(categoryText).arg(functionName.c_str()) : functionName.c_str();
				result.insert(name, code.c_str());
			}
		}
	}

	for (QMap<QString, QString>::const_iterator iterator = result.begin(); iterator != result.end(); ++iterator)
	{
		this->m_functionComboBox->addItem(iterator.key(), iterator.value());
	}
}

void TriggerFunctionDialog::acceptOnlyValid()
{
	if (!isValid())
	{
		KMessageBox::error(this, tr("Function is not valid!"));

		return;
	}

	this->accept();
}

void TriggerFunctionDialog::changeCategory(int index)
{
	this->m_functionComboBox->clear();
	map::TriggerStrings::Entries::const_iterator iterator;

	switch (type())
	{
		case map::TriggerFunction::Type::Event:
		{
			this->filterFunctionsByCategory(index, this->triggerData()->events(), this->triggerStrings()->events());

			break;
		}

		case map::TriggerFunction::Type::Condition:
		{
			this->filterFunctionsByCategory(index, this->triggerData()->conditions(), this->triggerStrings()->conditions());

			break;
		}

		case map::TriggerFunction::Type::Action:
		{
			this->filterFunctionsByCategory(index, this->triggerData()->actions(), this->triggerStrings()->actions());

			break;
		}

		case map::TriggerFunction::Type::Call:
		{
			this->filterFunctionsByCategory(index, this->triggerData()->calls(), this->triggerStrings()->calls());

			break;
		}
	}

	if (this->m_functionComboBox->count() > 0)
	{
		this->changeFunction(0);
	}
}

void TriggerFunctionDialog::fillInternal(map::TriggerFunction::Type type)
{
	this->m_presetComboBox->clear();
	this->m_variablesComboBox->clear();
	this->m_categoryComboBox->clear();
	this->m_functionComboBox->clear();
	this->m_descriptionLabel->clear();
	this->m_valueLabel->clear();

	m_variablesComboBox->setEnabled(true);

	m_radioButtonPreset->setChecked(false);
	m_radioButtonVariable->setChecked(false);
	m_radioButtonFunction->setChecked(false);
	m_radioButtonImported->setChecked(false);
	m_radioButtonValue->setChecked(false);

	m_type = type;

	if (this->type() != map::TriggerFunction::Type::Call)
	{
		m_radioButtonPreset->setVisible(false);
		m_presetComboBox->setVisible(false);

		m_radioButtonFunction->setChecked(true);
		m_categoryLabel->setVisible(true);
		m_categoryComboBox->setVisible(true);
		m_functionComboBox->setVisible(true);
		m_descriptionTitleLabel->setVisible(true);
		m_descriptionFrame->setVisible(true);
		m_descriptionLabel->setVisible(true);

		// disable all items which are only usable for calls!
		m_radioButtonVariable->setVisible(false);
		m_variablesComboBox->setVisible(false);
		m_variablesSelectPushButton->setVisible(false);
		m_variablesPushButton->setVisible(false);

		m_radioButtonFunction->setVisible(false);

		m_radioButtonImported->setVisible(false);
		m_importedComboBox->setVisible(false);

		m_radioButtonValue->setVisible(false);
		m_booleanFrame->setVisible(false);
		m_lineEdit->setVisible(false);
		m_textEdit->setVisible(false);
		m_valuePushButton->setVisible(false);
		m_numInput->setVisible(false);
		m_numInputDouble->setVisible(false);
		m_valueLabel->setVisible(false);

		this->m_categoryComboBox->addItem(tr("All"), "All");

		foreach (map::TriggerData::Categories::const_reference ref, triggerData()->categories())
		{
			if (ref.second->displayName())
			{
				const QString categoryText = this->triggerEditor()->source()->sharedData()->tr(ref.second->displayText().c_str());
				this->m_categoryComboBox->addItem(categoryText, ref.second->name().c_str());
			}
		}

		this->changeCategory(0);
	// Calls have extended widgets
	}
	else
	{
		m_radioButtonPreset->setVisible(true);
		m_presetComboBox->setVisible(true);

		//m_radioButtonFunction->setChecked(true);
		m_categoryLabel->setVisible(false);
		m_categoryComboBox->setVisible(false);
		m_functionComboBox->setVisible(true);
		m_descriptionTitleLabel->setVisible(true);
		m_descriptionFrame->setVisible(true);
		m_descriptionLabel->setVisible(true);

		m_radioButtonVariable->setVisible(true);
		m_variablesComboBox->setVisible(true);
		m_variablesSelectPushButton->setVisible(true);
		m_variablesPushButton->setVisible(true);

		m_radioButtonFunction->setVisible(true);

		m_radioButtonImported->setVisible(true);
		m_importedComboBox->setVisible(true);

		m_radioButtonValue->setVisible(true);

		// fill variables
		if (this->isSubDialog())
		{
			map::TriggerFunction *function = this->parentDialog()->function();

			if (function != 0)
			{
				string type = argumentType(function, m_argumentIndex);

				if (type == "Null")
				{ // special type for set variable, use type of variable
					if (m_argumentIndex - 1 < 0)
					{
						qDebug() << "Invalid usage of Null in function " << function->name().c_str();
					}
					else
					{
						type = argumentType(function, m_argumentIndex - 1);
						qDebug() << "var type: " << type.c_str();
					}
				}

				if (!type.empty())
				{
					qDebug() << "Call type: " << type.c_str();

					map::TriggerData::Types::const_iterator typeIterator = this->triggerData()->types().find(type);
					string baseType;

					if (typeIterator == this->triggerData()->types().end())
					{
						qDebug() << "Did not find type " << type.c_str() << " in TriggerData.txt";
					}
					else
					{
						const map::TriggerData::Type *type = typeIterator->second;

						qDebug() << "Type: " << type;
						qDebug() << "Base Type: " << type->baseType();

						if (type->baseType() != 0)
						{ // TODO get all base types until the last recursively
							baseType = type->baseType()->name();
						}
					}

					m_booleanFrame->hide();
					m_lineEdit->hide();
					m_numInput->hide();
					m_textEdit->hide();
					m_numInputDouble->hide();
					m_valuePushButton->hide();

					if (type != "AnyGlobal")
					{ // is not custom variable
						if (type == "real" || baseType == "real")
						{
							m_numInputDouble->show();
						}
						else if (type == "integer"|| baseType == "integer")
						{
							m_numInput->show();
						}
						else if (type == "StringExt")
						{
							m_textEdit->show();
						}
						else if (type == "string" || baseType == "string")
						{
							m_lineEdit->show();
						}
						else if (type == "boolean" || baseType == "boolean")
						{
							m_booleanFrame->show();
						}
					}

					if (m_booleanFrame->isHidden() &&
						m_lineEdit->isHidden() &&
						m_numInput->isHidden() &&
						m_textEdit->isHidden() &&
						m_numInputDouble->isHidden()) {
						m_radioButtonValue->hide();
					}

					// show all presets of type
					if (type != "AnyGlobal")
					{ // is not custom variable
						foreach (map::TriggerData::Parameters::const_reference ref, this->triggerData()->parameters())
						{
							if (ref.second->type()->name() == type || (!baseType.empty() && ref.second->type()->name() == baseType))
							{
								const QString presetDescription = this->triggerEditor()->source()->sharedData()->tr(ref.second->displayText().c_str());
								m_presetComboBox->addItem(presetDescription, ref.second->name().c_str());
							}
						}
					}

					if (m_presetComboBox->count() == 0)
					{
						m_presetComboBox->hide();
						m_radioButtonPreset->hide();
					}

					// show all variables of type
					qDebug() << "Triggers: " << this->triggers();
					qDebug() << "Variables: " << (&this->triggers()->variables());
					qDebug() << "Variables size: " << this->triggers()->variables().size();

					foreach (map::Triggers::Variables::const_reference ref, this->triggers()->variables())
					{
						if (ref.type() == type || type == "AnyGlobal" || (!baseType.empty() && ref.type() == baseType))
						{
							m_variablesComboBox->addItem(ref.name().c_str());
						}
					}

					if (m_variablesComboBox->count() == 0)
					{
						m_variablesComboBox->setEnabled(false);
					}

					if (type != "AnyGlobal")
					{
						// show all functions of type
						foreach (map::TriggerData::Calls::const_reference ref, this->triggerData()->calls())
						{
							const string returnType = boost::apply_visitor(map::TriggerData::FunctionArgumentVisitor(), ref.second->returnType());

							if (returnType == type || (!baseType.empty() && returnType == baseType))
							{
								//ref.second->code
								try
								{
									map::TriggerStrings::Entries::const_iterator iterator = TriggerEditor::triggerFunctionEntry(triggerStrings(), ref.second->code(), map::TriggerFunction::Type::Call);

									m_functionComboBox->addItem(TriggerEditor::cutQuotes(iterator->second->name()).c_str(), ref.second->code().c_str());
								}
								catch (std::runtime_error &e)
								{
									qDebug() << e.what();
									m_functionComboBox->addItem(ref.second->code().c_str(), ref.second->code().c_str());
								}
							}
						}
					}

					if (m_functionComboBox->count() == 0)
					{
						m_functionComboBox->hide();
						m_radioButtonFunction->hide();
					}
				}
				else
				{
					qDebug() << "Empty type!";
				}
			}
			else
			{
				qDebug() << "No parent function!";
			}
		}
	}

	switch (this->type())
	{
		case map::TriggerFunction::Type::Event:
		{
			setWindowTitle(tr("Configure Event"));
			m_categoryLabel->setText(tr("Event Type"));
			m_descriptionTitleLabel->setText(tr("Event Text"));

			break;
		}

		case map::TriggerFunction::Type::Condition:
		{
			setWindowTitle(tr("Configure Condition"));
			m_categoryLabel->setText(tr("Condition Type"));
			m_descriptionTitleLabel->setText(tr("Condition Text"));

			break;
		}

		case map::TriggerFunction::Type::Action:
		{
			setWindowTitle(tr("Configure Action"));
			m_categoryLabel->setText(tr("Action Type"));
			m_descriptionTitleLabel->setText(tr("Action Text"));

			break;
		}
	}
}


template<class Functions>
void TriggerFunctionDialog::showFunction(const QString &code, const Functions &functions, const map::TriggerStrings::Entries &entries)
{
	map::TriggerStrings::Entries::const_iterator iterator = entries.find(code.toStdString());

	if (iterator != entries.end())
	{
		QString text = TriggerEditor::triggerFunctionText(this->triggerEditor()->source()->sharedData().get(), triggerData(), triggerStrings(), code, function(), functions, entries, true, true);

		this->m_descriptionLabel->setText(text);

	}
	else
	{
		qDebug() << "Did not find function " << code;
	}
}

template<class Functions>
typename Functions::const_iterator TriggerFunctionDialog::triggerFunction(const string& code, const Functions& functions) const
{
	return functions.find(code);
}

void TriggerFunctionDialog::changeFunctionToNew(int index)
{
	// select a new function
	if (m_freeFunction && m_function != 0)
	{
		delete m_function;
	}

	qDebug() << "New function!";
	m_function = new map::TriggerFunction();
	m_freeFunction = true;
	m_function->setType(m_type);
	m_function->setName(m_functionComboBox->itemData(index).toByteArray().constData());
	m_function->setIsEnabled(true);

	try
	{
		TriggerEditor::fillNewTriggerFunctionParameters(triggerData(), m_function);
	}
	catch (std::runtime_error &e)
	{
		qDebug() << e.what();
	}

	changeFunction(index);
}

void TriggerFunctionDialog::changeFunction(int index)
{
	const QString code = this->m_functionComboBox->itemData(index).toString();

	qDebug() << "Current function " << m_function;

	switch (type())
	{
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

	bool ok = false;
	const int index = parameter.toInt(&ok);

	if (!ok)
	{
		qDebug() << "Invalid index " << parameter;

		return;
	}

	qDebug() << "Editing parameter " << index << " for real";

	const string &code = this->m_functionComboBox->itemData(this->m_functionComboBox->currentIndex()).toString().toStdString();
	// TODO handle if function == 0

	//if (function() != 0) { // actual function
		map::TriggerFunctionParameter *functionParameter = &function()->parameters().at(index);

		switch (functionParameter->type()) {
			case map::TriggerFunctionParameter::Type::Preset:
			{
				qDebug() << "Preset";
				subDialog()->fillCall(index);

				subDialog()->m_radioButtonVariable->setChecked(true);

				const int comboIndex = subDialog()->m_presetComboBox->findData(functionParameter->value().c_str());

				if (comboIndex == -1) {
					qDebug() << "Invalid index for preset \"" << functionParameter->value().c_str() << "\"";
				}

				subDialog()->m_radioButtonPreset->setChecked(true);
				subDialog()->m_presetComboBox->setCurrentIndex(comboIndex);
				subDialog()->show();

				break;
			}

			case map::TriggerFunctionParameter::Type::Variable:
			{
				qDebug() << "Variable: " << functionParameter->value().c_str();

				subDialog()->fillCall(index);
				subDialog()->m_radioButtonVariable->setChecked(true);
				subDialog()->m_variablesComboBox->setCurrentItem(functionParameter->value().c_str());
				subDialog()->show();

				break;
			}

			case map::TriggerFunctionParameter::Type::Function:
			{
				qDebug() << "Function";
				//this->function()->parameters().at(i)

				//map::TriggerData::Calls::const_iterator functionIterator = triggerData()->calls().find(parameter->value());

				//if (functionIterator != triggerData()->calls().end()) {
				//		map::TriggerData::Type *type = boost::get<map::TriggerData::Type*>(functionIterator->second->types()[index]);

				subDialog()->fillCall(&functionParameter->functions().at(0), index);
				subDialog()->m_radioButtonFunction->setChecked(true);
				subDialog()->show();

				//}

				break;
			}

			case map::TriggerFunctionParameter::Type::Jass:
			{
				qDebug() << "JASS";

				subDialog()->fillCall(index);

				// TODO move to fillCall?
				const string type = argumentType(function(), index);

				map::TriggerData::Types::const_iterator typeIterator = this->triggerData()->types().find(type);
				string baseType;

				if (typeIterator == this->triggerData()->types().end()) {
					qDebug() << "Did not find type " << type.c_str() << " in TriggerData.txt";
				} else {
					const map::TriggerData::Type *type = typeIterator->second;

					qDebug() << "Type: " << type;
					qDebug() << "Base Type: " << type->baseType();

					if (type->baseType() != 0) { // TODO get all base types until the last recursively
						baseType = type->baseType()->name();
					}
				}

				map::TriggerData::Calls::const_iterator iterator = this->triggerData()->calls().find(type);

				if (type == "real" || baseType == "real") {
					bool ok = false;
					const double value = QString(functionParameter->value().c_str()).toDouble(&ok);

					if (!ok) {
						qDebug() << "Invalid double: \"" << functionParameter->value().c_str();
					} else {
						subDialog()->m_numInputDouble->setValue(value);
					}

					if (iterator != this->triggerData()->calls().end() && iterator->second->limits().size() > index) {
						const map::TriggerData::Call::Limit &limit = iterator->second->limits()[index];
						bool hasMinium = false;
						bool hasMaximum = false;
						const double minimum = TriggerEditor::triggerFunctionLimitDoubleMinimum(limit, &hasMinium);
						const double maximum = TriggerEditor::triggerFunctionLimitDoubleMaximum(limit, &hasMaximum);
						QString labelText;

						if (hasMinium) {
							subDialog()->m_numInputDouble->setMinimum(minimum);
							labelText += QString("Minimum: %1").arg(minimum);
						}

						if (hasMaximum) {
							if (!labelText.isEmpty()) {
								labelText += QString(", Maximum: %1").arg(maximum);
							} else {
								labelText += QString("Maximum: %1").arg(maximum);
							}

							subDialog()->m_numInputDouble->setMaximum(maximum);
						}

						m_valueLabel->setText(labelText);
					}
				} else if (type == "integer" || baseType == "integer") {
					bool ok = false;
					const int value = QString(functionParameter->value().c_str()).toInt(&ok);

					if (!ok) {
						qDebug() << "Invalid integer: \"" << functionParameter->value().c_str();
					} else {
						subDialog()->m_numInput->setValue(value);
					}

					if (iterator != this->triggerData()->calls().end() && iterator->second->limits().size() > index) {
						const map::TriggerData::Call::Limit &limit = iterator->second->limits()[index];
						bool hasMinium = false;
						bool hasMaximum = false;
						const int minimum = TriggerEditor::triggerFunctionLimitIntMinimum(limit, &hasMinium);
						const int maximum = TriggerEditor::triggerFunctionLimitIntMaximum(limit, &hasMaximum);
						QString labelText;

						if (hasMinium) {
							subDialog()->m_numInput->setMinimum(minimum);
							labelText += QString("Minimum: %1").arg(minimum);
						}

						if (hasMaximum) {
							if (!labelText.isEmpty()) {
								labelText += QString(", Maximum: %1").arg(maximum);
							} else {
								labelText += QString("Maximum: %1").arg(maximum);
							}

							subDialog()->m_numInput->setMaximum(maximum);
						}

						m_valueLabel->setText(labelText);
					}
				} else if (type == "boolean" || baseType == "boolean") {
					bool value = false;

					try {
						value = boost::lexical_cast<bool>(functionParameter->value());
					} catch (boost::bad_lexical_cast &e) {
						qDebug() << e.what();
					}

					subDialog()->m_radioButtonTrue->setChecked(value);
					subDialog()->m_radioButtonFalse->setChecked(!value);
				// StringExt needs whole text edit please!
				} else if (type == "StringExt") {
					subDialog()->m_textEdit->setText(functionParameter->value().c_str());
				} else if (type == "string" || baseType == "string") {
					subDialog()->m_lineEdit->setText(functionParameter->value().c_str());
				}

				subDialog()->m_radioButtonValue->setChecked(true);

				subDialog()->show();

				break;
			}
		}
	/*
	} else { // newly selected function
	}
	*/
}

void TriggerFunctionDialog::editVariables(bool)
{
	this->triggerEditor()->showVariables();
	// TODO connect variable changes with updating combo box!
}

string TriggerFunctionDialog::argumentType(const map::TriggerFunction *function, int32 argumentIndex) const
{
	string result = "";

	switch (function->type()) {
		case map::TriggerFunction::Type::Event:
		{
			map::TriggerData::Functions::const_iterator iterator = this->triggerData()->events().find(function->name());

			if (iterator == this->triggerData()->events().end()) {
				return "";
			}

			result = boost::apply_visitor(map::TriggerData::FunctionArgumentVisitor(), iterator->second->types().at(argumentIndex));

			break;
		}

		case map::TriggerFunction::Type::Condition:
		{
			map::TriggerData::Functions::const_iterator iterator = this->triggerData()->conditions().find(function->name());

			if (iterator == this->triggerData()->conditions().end()) {
				return "";
			}

			result = boost::apply_visitor(map::TriggerData::FunctionArgumentVisitor(), iterator->second->types().at(argumentIndex));

			break;
		}

		case map::TriggerFunction::Type::Action:
		{
			map::TriggerData::Functions::const_iterator iterator = this->triggerData()->actions().find(function->name());

			if (iterator == this->triggerData()->actions().end()) {
				return "";
			}

			result = boost::apply_visitor(map::TriggerData::FunctionArgumentVisitor(), iterator->second->types().at(argumentIndex));

			break;
		}

		case map::TriggerFunction::Type::Call:
		{
			map::TriggerData::Calls::const_iterator iterator = this->triggerData()->calls().find(function->name());

			if (iterator == this->triggerData()->calls().end()) {
				return "";
			}

			result = boost::apply_visitor(map::TriggerData::FunctionArgumentVisitor(), iterator->second->types().at(argumentIndex));

			break;
		}
	}

	return result;
}

}

}
