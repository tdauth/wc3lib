/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#include <boost/filesystem/fstream.hpp>
#include <boost/cast.hpp>
#include <boost/format.hpp>

#include <QtGui>

#include <KFileDialog>
#include <KActionCollection>

#include "triggereditor.hpp"
#include "mapscriptwidget.hpp"
#include "triggerwidget.hpp"
#include "../editor.hpp"
#include "../map.hpp"
#include "variablesdialog.hpp"
#include "triggertreewidgetitem.hpp"
#include "../moduletoolbar.hpp"
#include "triggereditorconfig.h"
#include "../metadata.hpp"

namespace wc3lib
{

namespace editor
{

string TriggerEditor::cutQuotes(const string& value)
{
	return value.substr(1, value.length() - 2); // cut quotes
}

TriggerEditor::TriggerEditor(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f)
: Module(source, parent, f)
, m_triggers(0)
, m_customTextTriggers(0)
, m_freeTriggers(false)
, m_freeCustomTextTriggers(false)
, m_newMenu(0)
, m_treeWidget(new QTreeWidget(this))
, m_mapScriptWidget(new MapScriptWidget(this))
, m_triggerWidget(new TriggerWidget(this))
, m_variablesDialog(0)
, m_triggerActionCollection(0)
, m_newActionCollection(0)
{
	Module::setupUi();

	QSplitter *hSplitter = new QSplitter(Qt::Horizontal, this);
	hSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	treeWidget()->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	mapScriptWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	triggerWidget()->setupUi();
	triggerWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	hSplitter->addWidget(treeWidget());
	hSplitter->addWidget(mapScriptWidget());
	hSplitter->addWidget(triggerWidget());

	topLayout()->addWidget(hSplitter);
	treeWidget()->setHeaderHidden(true);
	treeWidget()->setSelectionMode(QAbstractItemView::SingleSelection);
	treeWidget()->setSelectionBehavior(QAbstractItemView::SelectItems);
	treeWidget()->setEditTriggers(QAbstractItemView::SelectedClicked);
	//centerLayout()->addWidget(triggerWidget());

	connect(treeWidget(), SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClicked(QTreeWidgetItem*,int)));

	triggerWidget()->setEnabled(false);
	mapScriptWidget()->hide();
	triggerWidget()->hide();
	triggerActionCollection()->action("savetriggers")->setEnabled(false);
	triggerActionCollection()->action("closetriggers")->setEnabled(false);
	triggerActionCollection()->action("closecustomtexttriggers")->setEnabled(false);
	triggerActionCollection()->action("closeall")->setEnabled(false);

	// TODO read settings
	QSettings settings("TriggerEditor");
	m_openDirectory = settings.value("OpenDirectory").toUrl();
}

TriggerEditor::~TriggerEditor()
{
	writeSettings();

	QSettings settings("TriggerEditor");
	settings.setValue("OpenDirectory", m_openDirectory);
}

bool TriggerEditor::configure()
{
	readSettings(); // fill sources first

	// Update required files if started as stand-alone module
	if (!hasEditor())
	{
		if (!this->source()->configure(this))
		{
			return false;
		}

		try
		{
			this->source()->sharedData()->refreshWorldEditorStrings(this);
			this->source()->sharedData()->refreshWorldEditData(this);
			this->source()->sharedData()->refreshTriggerData(this);
			this->source()->sharedData()->refreshTriggerStrings(this);
		}
		catch (const Exception &e)
		{
			QMessageBox::critical(this, tr("Error"), e.what());

			return false;
		}
	}

	retranslateUi();

	return true;
}

void TriggerEditor::retranslateUi()
{
	Module::retranslateUi();
	setWindowTitle(this->source()->sharedData()->tr("WESTRING_MODULE_SCRIPTS"));
	setWindowIcon(this->source()->sharedData()->worldEditDataIcon("ToolBarIcon_Module_Script", "WorldEditArt", this));
}

map::TriggerStrings::Entries::const_iterator TriggerEditor::triggerFunctionEntry(const map::TriggerStrings *triggerStrings, const string& code, map::TriggerFunction::Type type)
{
	map::TriggerStrings::Entries::const_iterator iterator;

	switch (type)
	{
		case map::TriggerFunction::Type::Event:
		{
			iterator = triggerStrings->events().find(code);

			if (iterator == triggerStrings->events().end())
			{
				throw std::runtime_error(boost::str(boost::format(_("No corresponding TriggerStrings.txt entry in events for \"%1%\".")) % code));
			}

			break;
		}

		case map::TriggerFunction::Type::Condition:
		{
			iterator = triggerStrings->conditions().find(code);

			if (iterator == triggerStrings->conditions().end())
			{
				throw std::runtime_error(boost::str(boost::format(_("No corresponding TriggerStrings.txt entry in conditions for \"%1%\".")) % code));
			}

			break;
		}

		case map::TriggerFunction::Type::Action:
		{
			iterator = triggerStrings->actions().find(code);

			if (iterator == triggerStrings->actions().end())
			{
				throw std::runtime_error(boost::str(boost::format(_("No corresponding TriggerStrings.txt entry in actions for \"%1%\".")) % code));
			}

			break;
		}

		case map::TriggerFunction::Type::Call:
		{
			iterator = triggerStrings->calls().find(code);

			if (iterator == triggerStrings->calls().end())
			{
				throw std::runtime_error(boost::str(boost::format(_("No corresponding TriggerStrings.txt entry in calls for \"%1%\".")) % code));
			}

			break;
		}
	}

	return iterator;
}

int TriggerEditor::triggerFunctionLimitIntMinimum(const map::TriggerData::Function::Limit& limit, bool *hasLimit)
{
	int result = -1;

	try
	{
		const int32 value = boost::get<int32>(limit.first);
		result = boost::numeric_cast<int>(value);

		if (hasLimit != 0)
		{
			*hasLimit = true;
		}
	}
	catch (std::exception &e)
	{
		/*
		 * parameters should only occur for defaults
		try {
			const map::TriggerData::Parameter *parameter = boost::get<map::TriggerData::Parameter*>(limit.first);
			parameter->
		} catch (std::exception &e) {
		}
		*/

		if (hasLimit != 0)
		{
			*hasLimit = false;
		}
	}

	return result;
}

int TriggerEditor::triggerFunctionLimitIntMaximum(const map::TriggerData::Function::Limit& limit, bool *hasLimit)
{
	int result = -1;

	try
	{
		const int32 value = boost::get<int32>(limit.second);
		result = boost::numeric_cast<int>(value);

		if (hasLimit != 0)
		{
			*hasLimit = true;
		}
	}
	catch (std::exception &e)
	{
		/*
		 * parameters should only occur for defaults
		try {
			const map::TriggerData::Parameter *parameter = boost::get<map::TriggerData::Parameter*>(limit.second);
			parameter->
		} catch (std::exception &e) {
		}
		*/

		if (hasLimit != 0)
		{
			*hasLimit = false;
		}
	}

	return result;
}

double TriggerEditor::triggerFunctionLimitDoubleMinimum(const map::TriggerData::Function::Limit& limit, bool *hasLimit)
{
	double result = -1;

	try
	{
		const float32 value = boost::get<float32>(limit.first);
		result = boost::numeric_cast<double>(value);

		if (hasLimit != 0)
		{
			*hasLimit = true;
		}
	}
	catch (std::exception &e)
	{
		/*
		 * parameters should only occur for defaults
		try {
			const map::TriggerData::Parameter *parameter = boost::get<map::TriggerData::Parameter*>(limit.first);
			parameter->
		} catch (std::exception &e) {
		}
		*/

		if (hasLimit != 0)
		{
			*hasLimit = false;
		}
	}

	return result;
}

double TriggerEditor::triggerFunctionLimitDoubleMaximum(const map::TriggerData::Function::Limit& limit, bool *hasLimit)
{
	double result = -1;

	try
	{
		const float32 value = boost::get<float32>(limit.second);
		result = boost::numeric_cast<double>(value);

		if (hasLimit != 0)
		{
			*hasLimit = true;
		}
	}
	catch (std::exception &e)
	{
		/*
		 * parameters should only occur for defaults
		try {
			const map::TriggerData::Parameter *parameter = boost::get<map::TriggerData::Parameter*>(limit.second);
			parameter->
		} catch (std::exception &e) {
		}
		*/

		if (hasLimit != 0)
		{
			*hasLimit = false;
		}
	}

	return result;
}

QString TriggerEditor::triggerFunction(WarcraftIIIShared *sharedData, const map::TriggerData *triggerData, const map::TriggerStrings *triggerStrings, const map::TriggerFunction* triggerFunction)
{
	QString result;

	try
	{
		map::TriggerStrings::Entries::const_iterator iterator = TriggerEditor::triggerFunctionEntry(triggerStrings, triggerFunction->name().c_str(), triggerFunction->type());
		result = QString("(") + TriggerEditor::cutQuotes(iterator->second->name()).c_str();
	}
	catch (std::runtime_error &e)
	{
		qDebug() << e.what();
		result = QString("(") + triggerFunction->name().c_str();
	}

	bool isFirst = true;

	foreach (map::TriggerFunction::Parameters::const_reference ref, triggerFunction->parameters())
	{
		/*
		 * Add spaces before non function calls.
		 */
		if (!isFirst && ref.type() != map::TriggerFunctionParameter::Type::Function)
		{
			result += " ";
		}

		result += triggerFunctionParameter(sharedData, triggerData, triggerStrings, &ref);
		isFirst = false;
	}

	result += ")";

	return result;
}

QString TriggerEditor::triggerFunctionParameter(WarcraftIIIShared *sharedData, const map::TriggerData *triggerData, const map::TriggerStrings *triggerStrings, const map::TriggerFunctionParameter *parameter)
{
	QString result;

	qDebug() << "Parameter value \"" << parameter->value().c_str() << "\"";

	switch (parameter->type())
	{
		case map::TriggerFunctionParameter::Type::Function:
		{
			foreach (map::TriggerFunctionParameter::Functions::const_reference ref, parameter->functions())
			{
				result += triggerFunction(sharedData, triggerData, triggerStrings, &ref); // call recursively all encapsulated calls
			}

			break;
		}

		case map::TriggerFunctionParameter::Type::Jass:
		{
			result = parameter->value().c_str();

			break;
		}

		case map::TriggerFunctionParameter::Type::Preset:
		{
			map::TriggerData::Parameters::const_iterator iterator = triggerData->parameters().find(parameter->value());

			if (iterator != triggerData->parameters().end())
			{
				qDebug() << "Using preset display text " << iterator->second->displayText().c_str();
				result = sharedData->tr(iterator->second->displayText().c_str());
			}
			else
			{
				qDebug() << "Unable to find preset " << parameter->value().c_str() << " in TriggerData.txt";
				result = parameter->value().c_str();
			}

			break;
		}

		case map::TriggerFunctionParameter::Type::Variable:
		{
			result = parameter->value().c_str();

			break;
		}
	}

	return result;
}

void TriggerEditor::fillNewTriggerFunctionParameters(const map::TriggerData *triggerData, map::TriggerFunction* function)
{
	assert(function->parameters().empty());

	const map::TriggerData::Function *triggerDataFunction = 0;

	switch (function->type())
	{
		case map::TriggerFunction::Type::Event:
		{
			qDebug() << "Searching events";

			map::TriggerData::Functions::const_iterator iterator = triggerData->events().find(function->name());

			if (iterator != triggerData->events().end())
			{
				triggerDataFunction = iterator->second;
			}
		}

		case map::TriggerFunction::Type::Condition:
		{
			qDebug() << "Searching conditions";

			map::TriggerData::Functions::const_iterator iterator = triggerData->conditions().find(function->name());

			if (iterator != triggerData->conditions().end())
			{
				triggerDataFunction = iterator->second;
			}
		}

		case map::TriggerFunction::Type::Action:
		{
			qDebug() << "Searching actions";

			map::TriggerData::Functions::const_iterator iterator = triggerData->actions().find(function->name());

			if (iterator != triggerData->actions().end())
			{
				triggerDataFunction = iterator->second;
			}
		}

		case map::TriggerFunction::Type::Call:
		{
			qDebug() << "Searching calls";

			map::TriggerData::Calls::const_iterator iterator = triggerData->calls().find(function->name());

			if (iterator != triggerData->calls().end())
			{
				triggerDataFunction = iterator->second;
			}
		}

	}

	if (triggerDataFunction == 0)
	{
		qDebug() << "Did not find function \"" << function->name().c_str() << "\"";

		throw std::runtime_error("");
	}

	for (std::size_t i = 0; i < triggerDataFunction->types().size(); ++i)
	{
		std::auto_ptr<map::TriggerFunctionParameter> functionParameter(new map::TriggerFunctionParameter());
		bool gotDefault = false;

		// show default if exists
		if (triggerDataFunction->defaults().size() > i)
		{
			string parameter = boost::apply_visitor(map::TriggerData::FunctionValueVisitor(), triggerDataFunction->defaults()[i]);

			// "_" means no default value
			if (parameter != "_")
			{
				if (!parameter.empty())
				{
					// defaults can be presets or calls
					if (QChar(parameter[0]).isLetter())
					{
						map::TriggerData::Parameters::const_iterator presetIterator = triggerData->parameters().find(parameter);

						if (presetIterator != triggerData->parameters().end())
						{
							functionParameter->setType(map::TriggerFunctionParameter::Type::Preset);
							functionParameter->setValue(parameter);

						// could be a call
						}
						else
						{
							map::TriggerData::Calls::const_iterator callIterator = triggerData->calls().find(parameter);

							if (callIterator != triggerData->calls().end())
							{
								functionParameter->setType(map::TriggerFunctionParameter::Type::Function);
								functionParameter->setValue(parameter);

								// TODO set inherited calls? possible for default values?
							}
						}
					// strings are enclosed by single quotes
					}
					else if (parameter[0] == '\"')
					{
						functionParameter->setType(map::TriggerFunctionParameter::Type::Jass);
						functionParameter->setValue(cutQuotes(parameter));
					}
				}

				gotDefault = true;
			}
		}

		// if it's been "_" or there is no default defined use the type's function
		if (!gotDefault)
		{
			functionParameter->setType(map::TriggerFunctionParameter::Type::Function);
			functionParameter->setValue("");
		}

		qDebug() << "Parameter of new function: \"" << functionParameter->value().c_str() << "\"";
		function->parameters().push_back(functionParameter);
	}
}

void TriggerEditor::openTriggers()
{
	KUrl url = KFileDialog::getOpenUrl(m_openDirectory, triggersFilter(), this, i18n("Open triggers"));

	if (url.isEmpty())
	{
		return;
	}

	m_openDirectory = url.directory();
	this->openTriggersUrl(url);
}

void TriggerEditor::openTriggersUrl(const KUrl &url)
{
	if (this->source()->sharedData()->triggerData().get() == 0)
	{
		QMessageBox::critical(this, tr("Error"), tr("No trigger data is loaded."));

		return;
	}

	if (this->source()->sharedData()->triggerStrings().get() == 0)
	{
		QMessageBox::critical(this, tr("Error"), tr("No trigger strings are loaded."));

		return;
	}

	QString target;

	if (!source()->download(url, target, this))
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to download triggers from \"%1\".").arg(url.toEncoded().constData()));

		return;
	}

	map::Triggers *triggers = 0;

	try
	{
		triggers = new map::Triggers();
		boost::filesystem::ifstream ifstream(target.toStdString(), std::ios::in | std::ios::binary);
		const map::TriggerData *triggerData = this->source()->sharedData()->triggerData().get();

		if (triggerData == 0)
		{
			throw std::runtime_error(_("No trigger data loaded."));
		}

		if (this->source()->sharedData()->triggerStrings().get() == 0)
		{
			throw std::runtime_error(_("No trigger strings loaded."));
		}

		triggers->read(ifstream, *triggerData);

		this->source()->removeTempFile(target);
	}
	catch (const std::exception &exception)
	{
		this->source()->removeTempFile(target);

		if (triggers != 0)
		{
			delete triggers;
			triggers = 0;
		}

		QMessageBox::critical(this, tr("Error"), tr("Unable to read triggers from file \"%1\".\nException: \"%2\".").arg(target).arg(exception.what()));

		return;
	}

	loadTriggers(triggers);
	setFreeTriggers(true); // for proper deletion
}

void TriggerEditor::openCustomTextTriggers()
{
	KUrl url = KFileDialog::getOpenUrl(m_openDirectory, customTextTriggersFilter(), this, i18n("Open custom text triggers"));

	if (url.isEmpty())
	{
		return;
	}

	m_openDirectory = url.directory();
	QString target;

	if (!source()->download(url, target, this))
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to download custom text triggers from \"%1\".").arg(url.toEncoded().constData()));

		return;
	}

	map::CustomTextTriggers *triggers = 0;

	try
	{
		triggers = new map::CustomTextTriggers();
		boost::filesystem::ifstream ifstream(target.toStdString(), std::ios::in | std::ios::binary);
		triggers->read(ifstream);
	}
	catch (std::exception &exception)
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to read custom text triggers from file \"%1\".\nException: \"%2\".").arg(target).arg(exception.what()));

		return;
	}

	loadCustomTextTriggers(triggers);
	setFreeCustomTextTriggers(true); // for proper deletion
}

void TriggerEditor::saveTriggers()
{
	if (triggers() == 0 && customTextTriggers() == 0)
	{
		return;
	}

	QString file = KFileDialog::getSaveFileName(KUrl(), triggersFilter(), this, tr("Save triggers"));

	if (file.isEmpty())
	{
		return;
	}

	ofstream of(file.toStdString(), std::ios::out | std::ios::binary);

	try
	{
		if (this->triggers() != 0)
		{
			this->triggers()->write(of);
		}
		else if (this->customTextTriggers() != 0)
		{
			this->customTextTriggers()->write(of);
		}
	}
	catch (Exception &e)
	{
		QMessageBox::critical(this, tr("Error"), tr("Error: \"%1\"").arg(e.what()));
	}
}

void TriggerEditor::closeTriggers()
{
	this->clear();
}

void TriggerEditor::closeCustomTextTriggers()
{
	if (customTextTriggers() != 0 && freeCustomTextTriggers())
	{
		delete customTextTriggers();
	}

	setCustomTextTriggers(0);
	// TODO update open trigger text (clear, warn?? consider situations when having a open map!!)

	triggerActionCollection()->action("closecustomtexttriggers")->setEnabled(false);

	if (triggers() == 0)
	{
		triggerActionCollection()->action("closeall")->setEnabled(false);
	}

	this->mapScriptWidget()->hide();
	this->triggerWidget()->clear();
	this->triggerWidget()->hide();
}

void TriggerEditor::closeAll()
{
	closeTriggers();
	closeCustomTextTriggers();
}

void TriggerEditor::resetTriggers()
{
	// TODO read default triggers from class \ref TriggerData.
}

void TriggerEditor::renameTrigger()
{
	treeWidget()->editItem(treeWidget()->currentItem());
}

void TriggerEditor::showVariables()
{
	if (variablesDialog() == 0)
	{
		m_variablesDialog = new VariablesDialog(this);
	}

	if (triggers() != 0)
	{
		variablesDialog()->showVariables(triggers());
	}

	variablesDialog()->show();
}

void TriggerEditor::convertToText()
{
	foreach (QTreeWidgetItem *item, treeWidget()->selectedItems())
	{
		if (triggerEntries().contains(item))
		{
			map::Trigger *trigger = boost::polymorphic_cast<TriggerTreeWidgetItem*>(item)->trigger();

			if (!trigger->isCustomText())
			{
				//
			}
		}
	}
}

void TriggerEditor::loadTriggers(map::Triggers *triggers)
{
	clear();
	m_rootItem= new QTreeWidgetItem(treeWidget());

	if (hasEditor())
	{
		/// \todo get w3x and w3m icon paths
		if (editor()->currentMap() != 0)
		{
			const KUrl src(editor()->currentMap()->isW3x() ? "" : "");
			QString file;
			rootItem()->setText(0, editor()->currentMap()->map()->info()->name().c_str());
			qDebug() << "Root item: " << editor()->currentMap()->map()->info()->name().c_str();

			if (source()->download(src, file, this))
			{
				rootItem()->setIcon(0, QIcon(file));
			}
		}
		else
		{
			rootItem()->setText(0, tr("Current map script"));
		}
	}
	else
	{
		rootItem()->setText(0, tr("Current map script"));
	}

	categories().resize(triggers->categories().size());

	if (!this->categories().isEmpty())
	{
		QIcon categoryIcon = this->source()->sharedData()->worldEditDataIcon("SEIcon_TriggerCategory", "WorldEditArt", this);

		for (int32 i = 0; i < this->categories().size(); ++i)
		{
			m_categoryIndices.insert(triggers->categories()[i].index(), i);
			categories()[i] = new QTreeWidgetItem(rootItem());
			//categories()[i]->setFlags(item->flags() | Qt::ItemIsEditable);
			categories()[i]->setText(0, triggers->categories()[i].name().c_str());
			categories()[i]->setIcon(0, categoryIcon);

			qDebug() << "Category: " << triggers->categories()[i].name().c_str();
		}
	}

	triggerEntries().resize(triggers->triggers().size());

	if (!triggerEntries().isEmpty())
	{
		for (int32 i = 0; i < this->triggerEntries().size(); ++i)
		{
			const int32 category = triggers->triggers()[i].category();
			TriggerTreeWidgetItem *item = 0;
			CategoryIndices::const_iterator iterator = m_categoryIndices.find(category);

			if (iterator != m_categoryIndices.end())
			{
				item = new TriggerTreeWidgetItem(&triggers->triggers()[i], categories()[iterator.value()]);
			}
			else
			{
				item = new TriggerTreeWidgetItem(&triggers->triggers()[i], rootItem());
				qDebug() << "Invalid category index " << category;
			}

			// TODO set icon

			if (triggers->triggers()[i].isInitiallyOn())
			{
				item->setForeground(0, QColor(Qt::black));
			}
			else
			{
				item->setForeground(0, QColor(Qt::gray));
			}

			item->setIcon(0, TriggerEditor::triggerIcon(this->source()->sharedData().get(), this, triggers->triggers()[i]));

			triggerEntries()[i] = item;
			triggerEntries()[i]->setText(0, triggers->triggers()[i].name().c_str());

			qDebug() << "Trigger: " << triggers->triggers()[i].name().c_str();
		}
	}

	m_triggers = triggers;
	triggerActionCollection()->action("savetriggers")->setEnabled(true);
	triggerActionCollection()->action("closetriggers")->setEnabled(true);
	triggerActionCollection()->action("closeall")->setEnabled(true);
}

void TriggerEditor::loadTriggers(Map *map)
{
	loadTriggers(map->map()->triggers().get());
}

void TriggerEditor::loadCustomTextTriggers(map::CustomTextTriggers *customTextTriggers)
{
	closeCustomTextTriggers();
	m_customTextTriggers = customTextTriggers;
	triggerActionCollection()->action("closecustomtexttriggers")->setEnabled(true);
	triggerActionCollection()->action("closeall")->setEnabled(true);

	if (triggers() == 0)
	{
		return;
	}

	if (triggerWidget()->trigger() != 0 && triggerWidget()->trigger()->isCustomText())
	{
		triggerWidget()->textEdit()->setText(triggerText(triggerWidget()->trigger()).c_str());
	}
}

void TriggerEditor::loadCustomTextTriggers(Map *map)
{
	loadCustomTextTriggers(map->map()->customTextTriggers().get());
}

void TriggerEditor::loadFromMap(Map* map)
{
	if (map->map()->findFile(map->map()->triggers()->fileName()).isValid())
	{
		loadTriggers(map);
	}
	else
	{
		QMessageBox::critical(this, tr("Error"), tr("Triggers file \"%1\" doesn't exist.").arg(map->map()->triggers()->fileName()));
	}

	if (map->map()->findFile(map->map()->customTextTriggers()->fileName()).isValid())
	{
		loadCustomTextTriggers(map);
	}
	else
	{
		QMessageBox::critical(this, tr("Error"), tr("Custom text triggers file \"%1\" doesn't exist.").arg(map->map()->customTextTriggers()->fileName()));
	}
}

void TriggerEditor::clear()
{
	if (triggers() != 0 && freeTriggers())
	{
		delete triggers();
	}

	treeWidget()->clear();
	categories().clear();
	variables().clear();
	triggerEntries().clear();
	setTriggers(0);
	m_rootItem = 0;
	triggerActionCollection()->action("savetriggers")->setEnabled(false);
	triggerActionCollection()->action("closetriggers")->setEnabled(false);

	if (customTextTriggers() == 0)
	{
		triggerActionCollection()->action("closeall")->setEnabled(false);
	}

	this->mapScriptWidget()->hide();
	this->triggerWidget()->clear();
	this->triggerWidget()->hide();
}

void TriggerEditor::openMapScript()
{
	this->triggerWidget()->hide();
	this->mapScriptWidget()->show();
}

void TriggerEditor::openTrigger(int32 index)
{
	if (boost::numeric_cast<std::size_t>(index) >= triggers()->triggers().size())
	{
		qDebug() << "Invalid trigger index " << index;
		return;
	}

	openTrigger(&triggers()->triggers()[index]);
}

void TriggerEditor::openTrigger(map::Trigger *trigger)
{
	this->mapScriptWidget()->hide();
	triggerWidget()->show();
	triggerWidget()->setEnabled(true);
	triggerWidget()->showTrigger(trigger, triggerText(trigger));
}

void TriggerEditor::loadTriggerData()
{
	if (source() == 0)
	{
		QMessageBox::critical(this, tr("Error"), tr("No source available!"));

		return;
	}

	KUrl url = KFileDialog::getOpenUrl(m_openDirectory, i18n("*|All Files\n*.txt|Warcraft III Trigger Data"), this, i18n("Open trigger data"));

	if (url.isEmpty())
	{
		return;
	}

	m_openDirectory = url.directory();

	try
	{
		this->source()->sharedData()->refreshTriggerData(this, url);
	}
	catch (const std::exception &exception)
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to read trigger data from file \"%1\".\nException: \"%2\".").arg(url.toLocalFile()).arg(exception.what()));

		return;
	}
}

void TriggerEditor::loadTriggerStrings()
{
	if (source() == 0)
	{
		QMessageBox::critical(this, tr("Error"), tr("No source available!"));

		return;
	}

	KUrl url = KFileDialog::getOpenUrl(m_openDirectory, i18n("*|All Files\n*.txt|Warcraft III Trigger Strings"), this, i18n("Open trigger strings"));

	if (url.isEmpty())
	{
		return;
	}

	m_openDirectory = url.directory();

	try
	{
		this->source()->sharedData()->refreshTriggerStrings(this, url);
	}
	catch (const std::exception &exception)
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to read trigger strings from file \"%1\".\nException: \"%2\".").arg(url.toLocalFile()).arg(exception.what()));

		return;
	}
}

void TriggerEditor::newCategory()
{
	if (triggers() != 0)
	{
		try
		{
			std::auto_ptr<map::TriggerCategory> category(new map::TriggerCategory());
			category->setName(tr("Unnamed category").toStdString());
			category->setIndex(boost::numeric_cast<int32>(triggers()->categories().size()));
			triggers()->categories().push_back(category);

			QTreeWidgetItem *item = new QTreeWidgetItem(rootItem());
			item->setText(0, tr("Unnamed category"));
			/// \todo set folder icon
			categories().append(item);

			item->setSelected(true);
			treeWidget()->scrollToItem(item);

		}
		catch (boost::bad_numeric_cast &e)
		{
			QMessageBox::critical(this, tr("Error"), tr("Invalid category index %1: \"%2\"").arg(triggers()->categories().size()).arg(e.what()));
		}
	}
}

void TriggerEditor::newTrigger()
{
	if (triggers() != 0)
	{
		QString name = newTriggerName();
		std::auto_ptr<map::Trigger> trigger(new map::Trigger());
		trigger->setName(name.toStdString());
		trigger->setEnabled(true);
		trigger->setInitiallyOn(true);

		QTreeWidgetItem *parent = treeWidget()->selectedItems().first();

		if (!categories().contains(parent))
		{
			parent = rootItem();
		}

		TriggerTreeWidgetItem *item = new TriggerTreeWidgetItem(trigger.get(), parent);
		item->setText(0, name);
		/// \todo set icon (initially on, disabled etc.)

		triggerEntries().push_back(item);
		triggers()->triggers().push_back(trigger); // push back after getting trigger

		item->setSelected(true);
		treeWidget()->scrollToItem(item);
	}
}

void TriggerEditor::newTriggerComment()
{
}

void TriggerEditor::newEvent()
{
	if (triggers() != 0)
	{
		if (triggerWidget()->isVisible())
		{
			triggerWidget()->newEvent();
		}
	}
}

void TriggerEditor::newCondition()
{
	if (triggers() != 0)
	{
		if (triggerWidget()->isVisible())
		{
			triggerWidget()->newCondition();
		}
	}
}

void TriggerEditor::newAction()
{
	if (triggers() != 0)
	{
		if (triggerWidget()->isVisible())
		{
			triggerWidget()->newAction();
		}
	}
}

void TriggerEditor::itemClicked(QTreeWidgetItem *item, int column)
{
	if (item == rootItem())
	{
		openMapScript();
		newActionCollection()->action("newtrigger")->setEnabled(false);
		newActionCollection()->action("newtriggercomment")->setEnabled(false);
		newActionCollection()->action("newevent")->setEnabled(false);
		newActionCollection()->action("newcondition")->setEnabled(false);
		newActionCollection()->action("newaction")->setEnabled(false);
	}
	else if (categories().contains(item))
	{
		qDebug() << "CATEGORY!";
		triggerWidget()->hide();
		mapScriptWidget()->hide();
		newActionCollection()->action("newtrigger")->setEnabled(true);
		newActionCollection()->action("newtriggercomment")->setEnabled(true);
		newActionCollection()->action("newevent")->setEnabled(false);
		newActionCollection()->action("newcondition")->setEnabled(false);
		newActionCollection()->action("newaction")->setEnabled(false);
	}
	else if (triggerEntries().contains(item))
	{
		qDebug() << "TRIGGER!";
		openTrigger(boost::polymorphic_cast<TriggerTreeWidgetItem*>(item)->trigger());
		newActionCollection()->action("newtrigger")->setEnabled(true);
		newActionCollection()->action("newtriggercomment")->setEnabled(true);
		newActionCollection()->action("newevent")->setEnabled(true);
		newActionCollection()->action("newcondition")->setEnabled(true);
		newActionCollection()->action("newaction")->setEnabled(true);
	}
	else
	{
		qDebug() << "Unknown item: " << item->text(0);
	}
}

void TriggerEditor::createFileActions(QMenu *menu)
{
	m_triggerActionCollection = new KActionCollection((QObject*)this);

	QAction *action = new QAction(QIcon(":/actions/opentriggers.png"), i18n("Open triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(openTriggers()));
	triggerActionCollection()->addAction("opentriggers", action);

	action = new QAction(QIcon(":/actions/opencustomtexttriggers.png"), i18n("Open custom text triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(openCustomTextTriggers()));
	triggerActionCollection()->addAction("opencustomtexttriggers", action);

	action = new QAction(KIcon(":/actions/savetriggers.png"), i18n("Save triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(saveTriggers()));
	triggerActionCollection()->addAction("savetriggers", action);

	action = new QAction(QIcon(":/actions/closetriggers.png"), i18n("Close triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(closeTriggers()));
	triggerActionCollection()->addAction("closetriggers", action);

	action = new QAction(QIcon(":/actions/closecustomtexttriggers.png"), i18n("Close custom text triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(closeCustomTextTriggers()));
	triggerActionCollection()->addAction("closecustomtexttriggers", action);

	action = new QAction(KIcon(":/actions/closeall.png"), i18n("Close all"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(closeAll()));
	triggerActionCollection()->addAction("closeall", action);

	action = new QAction(QIcon(":/actions/resettriggers.png"), i18n("Reset triggers"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(resetTriggers()));
	triggerActionCollection()->addAction("resettriggers", action);

	action = new QAction(QIcon(":/actions/rename.png"), i18n("Rename"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(renameTrigger()));
	triggerActionCollection()->addAction("rename", action);

	action = new QAction(QIcon(":/actions/loadtriggerdata.png"), i18n("Load trigger data"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(loadTriggerData()));
	triggerActionCollection()->addAction("loadtriggerdata", action);

	action = new QAction(QIcon(":/actions/loadtriggerstrings.png"), i18n("Load trigger strings"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(loadTriggerStrings()));
	triggerActionCollection()->addAction("loadtriggerstrings", action);

	triggerActionCollection()->associateWidget(menu);
}

void TriggerEditor::createEditActions(QMenu *menu)
{
	menu->addSeparator();
	QAction *action = new QAction(QIcon(":/actions/variables.png"), i18n("Variables ..."), this);
	action->setShortcut(i18n("Ctrl+B"));
	action->setIcon(this->source()->sharedData()->worldEditDataIcon("ToolBarIcon_SE_Variables", "WorldEditArt", this));

	connect(action, SIGNAL(triggered()), this, SLOT(showVariables()));
	triggerActionCollection()->addAction("variables", action);
	menu->addAction(action);

	menu->addSeparator();

	action = new QAction(QIcon(":/actions/converttotext.png"), i18n("Convert To Custom Text ..."), this);
	connect(action, SIGNAL(triggered()), this, SLOT(convertToText()));
	triggerActionCollection()->addAction("converttotext", action);
	menu->addAction(action);
}

void TriggerEditor::createMenus(QMenuBar *menuBar)
{
	m_newMenu = new QMenu(i18n("New"), menuBar);

	m_newActionCollection = new KActionCollection((QObject*)this);

	QAction *action = new QAction(QIcon(":/actions/newcategory.png"), i18n("Category"), this);
	action->setIcon(this->source()->sharedData()->worldEditDataIcon("ToolBarIcon_SE_NewCategory", "WorldEditArt", this));
	connect(action, SIGNAL(triggered()), this, SLOT(newCategory()));
	newActionCollection()->addAction("newcategory", action);

	action = new QAction(QIcon(":/actions/newtrigger.png"), i18n("Trigger"), this);
	action->setIcon(this->source()->sharedData()->worldEditDataIcon("ToolBarIcon_SE_NewTrigger", "WorldEditArt", this));
	connect(action, SIGNAL(triggered()), this, SLOT(newTrigger()));
	newActionCollection()->addAction("newtrigger", action);

	action = new QAction(QIcon(":/actions/newtriggercomment.png"), i18n("Trigger Comment"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(newTriggerComment()));
	newActionCollection()->addAction("newtriggercomment", action);

	m_newMenu->addSeparator();

	action = new QAction(QIcon(":/actions/newevent.png"), i18n("Event"), this);
	action->setIcon(this->source()->sharedData()->worldEditDataIcon("ToolBarIcon_SE_NewEvent", "WorldEditArt", this));
	connect(action, SIGNAL(triggered()), this, SLOT(newEvent()));
	newActionCollection()->addAction("newevent", action);

	action = new QAction(QIcon(":/actions/newcondition.png"), i18n("Condition"), this);
	action->setIcon(this->source()->sharedData()->worldEditDataIcon("ToolBarIcon_SE_NewCondition", "WorldEditArt", this));
	connect(action, SIGNAL(triggered()), this, SLOT(newCondition()));
	newActionCollection()->addAction("newcondition", action);

	action = new QAction(QIcon(":/actions/newaction.png"), i18n("Action"), this);
	action->setIcon(this->source()->sharedData()->worldEditDataIcon("ToolBarIcon_SE_NewAction", "WorldEditArt", this));
	connect(action, SIGNAL(triggered()), this, SLOT(newAction()));
	newActionCollection()->addAction("newaction", action);

	newActionCollection()->associateWidget(m_newMenu);

	treeWidget()->setContextMenuPolicy(Qt::ActionsContextMenu);
	treeWidget()->addAction(newActionCollection()->action("newcategory"));
	treeWidget()->addAction(newActionCollection()->action("newtrigger"));
	treeWidget()->addAction(newActionCollection()->action("newtriggercomment"));
}

void TriggerEditor::createWindowsActions(WindowsMenu *menu)
{
}

void TriggerEditor::createToolButtons(ModuleToolBar *toolBar)
{
	toolBar->addCustomAction(triggerActionCollection()->action("variables"));
	toolBar->addSeparator();
	toolBar->addCustomAction(newActionCollection()->action("newcategory"));
	toolBar->addCustomAction(newActionCollection()->action("newtrigger"));
	toolBar->addCustomAction(newActionCollection()->action("newtriggercomment"));
	toolBar->addSeparator();
	toolBar->addCustomAction(newActionCollection()->action("newevent"));
	toolBar->addCustomAction(newActionCollection()->action("newcondition"));
	toolBar->addCustomAction(newActionCollection()->action("newaction"));
}

SettingsInterface* TriggerEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void TriggerEditor::onSwitchToMap(Map *map)
{
	loadFromMap(map);
}

QString TriggerEditor::newTriggerName() const
{
	int index = 0;

	if (triggers() != 0)
	{
		foreach (map::Triggers::TriggerEntries::const_reference ref,  triggers()->triggers())
		{
			QString triggerName = ref.name().c_str();

			if (triggerName.startsWith(tr("Unnamed trigger ")))
			{
				QString suffix = triggerName.remove(0, tr("Unnamed trigger ").length());
				bool ok = false;
				int usedIndex = suffix.toInt(&ok);

				if (ok && usedIndex > index)
				{
					index = usedIndex + 1;
				}
			}
		}
	}

	return tr("Unnamed trigger %1").arg(QString::number(index)); // TODO format with 00?
}

QIcon TriggerEditor::triggerFunctionCatgoryIcon(MpqPriorityList *source, QWidget *window, const QString &code, const map::TriggerData::Functions &functions)
{
	map::TriggerData::Functions::const_iterator functionIterator = functions.find(code.toUtf8().constData());

	if (functionIterator != functions.end())
	{
		const map::TriggerData::Function *triggerDataFunction = functionIterator->second;

		if (triggerDataFunction->category() != 0)
		{
			const QString iconFilePath = MetaData::fromFilePath(triggerDataFunction->category()->iconImageFile().c_str()) + ".blp";

			return source->sharedData()->icon(iconFilePath, window);
		}
	}

	return QIcon();
}

bool TriggerEditor::triggerIsPartial(const map::Trigger &trigger)
{
	BOOST_FOREACH(map::Trigger::Functions::const_reference ref, trigger.functions())
	{
		if (!ref.isEnabled())
		{
			return true;
		}
	}

	return false;
}

QIcon TriggerEditor::triggerIcon(WarcraftIIIShared *sharedData, QWidget *window, const map::Trigger &trigger)
{
	if (trigger.isEnabled() && trigger.isInitiallyOn() && !triggerIsPartial(trigger))
	{
		return sharedData->worldEditDataIcon("SEIcon_Trigger", "WorldEditArt", window);
	}
	else if (trigger.isEnabled() && !trigger.isInitiallyOn() && !triggerIsPartial(trigger))
	{
		return sharedData->worldEditDataIcon("SEIcon_TriggerUnused", "WorldEditArt", window);
	}
	else if (!trigger.isEnabled() && !trigger.isInitiallyOn())
	{
		return sharedData->worldEditDataIcon("SEIcon_TriggerUnusedDisabled", "WorldEditArt", window);
	}
	else if (!trigger.isEnabled() && trigger.isInitiallyOn())
	{
		return sharedData->worldEditDataIcon("SEIcon_TriggerDisabled", "WorldEditArt", window);
	}
	else if (trigger.isEnabled() && trigger.isInitiallyOn() && triggerIsPartial(trigger))
	{
		return sharedData->worldEditDataIcon("SEIcon_TriggerPartial", "WorldEditArt", window);
	}
	else if (trigger.isEnabled() && !trigger.isInitiallyOn() && triggerIsPartial(trigger))
	{
		return sharedData->worldEditDataIcon("SEIcon_TriggerUnusedPartial", "WorldEditArt", window);
	}

	return QIcon();
}

QString TriggerEditor::variableInitialValueText(WarcraftIIIShared *sharedData, const map::Variable &variable)
{
	QString initialValue;

	if (variable.initialValue().empty())
	{
		initialValue = sharedData->tr("WESTRING_GVD_NOVALUE");
	}
	else
	{
		initialValue = variable.initialValue().c_str();
	}

	return initialValue;
}

QIcon TriggerEditor::icon()
{
	return QIcon(this->source()->sharedData()->worldEditDataPixmap("ToolBarIcon_Module_Script", "WorldEditArt", this));
}

#include "moc_triggereditor.cpp"

}

}
