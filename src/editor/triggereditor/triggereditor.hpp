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

#ifndef WC3LIB_EDITOR_TRIGGEREDITOR_TRIGGEREDITOR_HPP
#define WC3LIB_EDITOR_TRIGGEREDITOR_TRIGGEREDITOR_HPP

#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include <QHash>
#include <QTreeWidget>
#include <QDebug>

#include <KUrl>

#include "../module.hpp"
#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief The trigger editor is one of the most important modules of the World Editor when creating non-melee maps since it offers you the possibility to react on game events and therefore to define a completely new behaviour of the game.
 * 
 * \note Bear in mind that all code of custom text triggers is stored in a separate file named "war3map.wtc" (\ref map::CustomTextTriggers) whereas all trigger data is stored in "war3map.wtg" (\ref map::Triggers). Therefore you have to load both files to make custom text triggers storable. When using editor mode (\ref hasEditor()) both files will be loaded automatically. Alternatively you could use \ref loadFromMap().
 *
 * \sa VariablesDialog
 * \sa TriggerFunctionDialog
 */
class TriggerEditor : public Module
{
	Q_OBJECT

	public:
		typedef QVector<QTreeWidgetItem*> TreeItems;
		/// Stores indices of tree items with keys of REAL category indices.
		typedef QHash<int32, int> CategoryIndices;

		/**
		 * Cutting quotes from trigger string entries.
		 */
		static string cutQuotes(const string &value);
		
		TriggerEditor(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);

		/**
		 * \return Returns empty string if there is no custom text triggers or no valid text entry.
		 * \note \p trigger has to be part of \ref triggers().
		 */
		string triggerText(map::Trigger *trigger) const;

		map::Triggers* triggers() const;
		map::CustomTextTriggers* customTextTriggers() const;
		QTreeWidget* treeWidget() const;
		QTreeWidgetItem* rootItem() const;
		class MapScriptWidget* mapScriptWidget() const;
		class TriggerWidget* triggerWidget() const;
		class VariablesDialog* variablesDialog() const;
		class KActionCollection* triggerActionCollection() const;
		class KActionCollection* newActionCollection() const;
		
		/**
		 * \return Returns an iterator to the corresponding TriggerStrings.txt entry of \p code from entries specified by \p type.
		 */
		static map::TriggerStrings::Entries::const_iterator triggerFunctionEntry(const map::TriggerStrings *triggerStrings, const string &code, BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type);
		
		/**
		 * Returns the minimum integer limit value of \p limit.
		 * \p hasLimit If this value is not 0 it will store if there's stored a valid int limit. Otherwise no limit is given and should never be set in any numeric input.
		 */
		static int triggerFunctionLimitIntMinimum(const map::TriggerData::Call::Limit &limit, bool *hasLimit = 0);
		static int triggerFunctionLimitIntMaximum(const map::TriggerData::Call::Limit &limit, bool *hasLimit = 0);
		
		static double triggerFunctionLimitDoubleMinimum(const map::TriggerData::Call::Limit &limit, bool *hasLimit = 0);
		static double triggerFunctionLimitDoubleMaximum(const map::TriggerData::Call::Limit &limit, bool *hasLimit = 0);
		
		/**
		 * Returns the corresponding string for \p triggerFunction's call in the following form:
		 * <functionname>( .. sub calls( ... ) ... )
		 */
		static QString triggerFunction(const map::TriggerData *triggerData, const map::TriggerStrings *triggerStrings, const map::TriggerFunction *triggerFunction);
		/**
		 * Builds the corresponding string for \p parameter by encapsulating all sub calls recursively using \ref triggerFunction().
		 * If it is only a variable, JASS call or preset, it simply returns the corresponding string.
		 */
		static QString triggerFunctionParameter(const map::TriggerData *triggerData, const map::TriggerStrings *triggerStrings, const map::TriggerFunctionParameter *parameter);
		
		template<class Functions>
		static QString triggerFunctionCategoryText(const map::TriggerData* triggerData, const Functions &functions, const QString& code);
		
		/**
		 * Returns formatted text of function code \p code and function \p function using its "UI/TriggerData.txt" entry and filling all ~values with parameters.
		 * This text is usually shown in a \ref TriggerFunctionDialog when editing a function and its parameters.
		 * 
		 * For all parameters which are function calls itself it returns the corresponding string in the following form if \p function is not 0:
		 * (<sub call level 1>(<sub calls level 2>))
		 * 
		 * If \p function is 0 it will return formatted text for a newly created function.
		 * \param withLinks If this value is true it will create selectable links for all parameters.
		 * \param withHint If this value is true, the function's hint will be appended after a line break.
		 * \param withCategory If this value is true, the function's trigger category will be prepended if it is displayed in trigger editor.
		 */
		template<class Functions>
		static QString triggerFunctionText(const map::TriggerData *triggerData, const map::TriggerStrings *triggerStrings, const QString &code, map::TriggerFunction *function, const Functions &functions, const map::TriggerStrings::Entries &entries, bool withLinks = false, bool withHint = false, bool withCategory = false);
		
		
		/**
		 * Fills \p function with default values of parameters or empty values.
		 * \note Expects \p function to be set to its correct type and code value.
		 * \throws std::runtime_error Throws an expection if function is not found in \p triggerData.
		 */
		static void fillNewTriggerFunctionParameters(const map::TriggerData *triggerData, map::TriggerFunction *function);

	public slots:
		/**
		 * Opens file dialog for loading triggers file (*.wtg).
		 * \sa triggersFilter()
		 */
		void openTriggers();
		
		void openTriggersUrl(const KUrl &url);
		/**
		 * Opens file dialog for loading custom text triggers file (*.wtc).
		 * \sa customTextTriggersFilter()
		 */
		void openCustomTextTriggers();
		
		void saveTriggers();
		
		/**
		 * Similar to \ref clear().
		 */
		void closeTriggers();
		void closeCustomTextTriggers();
		/**
		 * Closes open triggers and custom text triggers.
		 * \sa closeTriggers()
		 * \sa closeCustomTextTriggers()
		 */
		void closeAll();
		void resetTriggers();
		void renameTrigger();
		void showVariables();
		void convertToText();

		void loadTriggers(map::Triggers *triggers);
		void loadTriggers(Map *map);
		void loadCustomTextTriggers(map::CustomTextTriggers *customTextTriggers);
		void loadCustomTextTriggers(Map *map);
		
		/**
		 * Loads triggers and custom text triggers from map \p map.
		 * \sa loadTriggers()
		 * \sa loadCustomTextTriggers()
		 */
		void loadFromMap(Map *map);
		/**
		 * \note Clears only triggers not custom text triggers!
		 */
		void clear();
		/**
		 * Displays the custom map script instead of any trigger, category etc..
		 */
		void openMapScript();
		/**
		 * Displays trigger of \p index.
		 */
		void openTrigger(int32 index);
		void openTrigger(map::Trigger *trigger);
		
		/**
		 * Loads corresponding trigger data ("UI/TriggerData.txt") which contains definitions of trigger types, categories, functions etc. and is required to resolve all trigger functions.
		 * \note Usually, data from the corresponding \ref MpqPriorityList::source() would be taken (\ref MpqPriorityList::triggerData()). This function is useful if the corresponding source has no trigger data file.
		 */
		void loadTriggerData();
		/**
		 * Loads corresponding trigger strings ("UI/TriggerStrings.txt") which contains string definitions and layouts for all trigger functions defined by trigger data.
		 * This data is required for proper display of names and parameter selection.
		 * \note Usually, data from the corresponding \ref MpqPriorityList::source() would be taken (\ref MpqPriorityList::triggerStrings()). This function is useful if the corresponding source has no trigger strings file.
		 */
		void loadTriggerStrings();
		
		
		void newCategory();
		void newTrigger();
		void newTriggerComment();
		void newEvent();
		void newCondition();
		void newAction();

	protected slots:
		void itemClicked(class QTreeWidgetItem *item, int column);

	protected:
		virtual void createFileActions(class KMenu *menu);
		virtual void createEditActions(class KMenu *menu);
		virtual void createMenus(class KMenuBar *menuBar);
		virtual void createWindowsActions(class WindowsMenu *menu);
		virtual void createToolButtons(class ModuleToolBar *toolBar);
		virtual class SettingsInterface* settings();
		virtual KAboutData moduleAboutData() const;
		virtual void onSwitchToMap(Map *map);
		virtual QString actionName() const;

		void setTriggers(map::Triggers *triggers);
		void setCustomTextTriggers(map::CustomTextTriggers *customTextTriggers);
		void setFreeTriggers(bool freeTriggers);
		void setFreeCustomTextTriggers(bool freeCustomTextTriggers);
		/**
		 * \return Returns true if triggers have been allocated for trigger editor only and do not belong to any object. In this case they will be deleted automatically when closed.
		 */
		bool freeTriggers() const;
		bool freeCustomTextTriggers() const;
		TreeItems& categories();
		TreeItems& variables();
		TreeItems& triggerEntries();

	private:
		QString newTriggerName() const;
		
		map::Triggers *m_triggers;
		map::CustomTextTriggers *m_customTextTriggers;
		bool m_freeTriggers;
		bool m_freeCustomTextTriggers;
		CategoryIndices m_categoryIndices; /// Stores indices of tree items with keys of REAL category indices.
		TreeItems m_categories;
		TreeItems m_variables;
		TreeItems m_triggerEntries;
		
		KMenu *m_newMenu;

		QTreeWidget *m_treeWidget;
		QTreeWidgetItem *m_rootItem;
		class MapScriptWidget *m_mapScriptWidget;
		class TriggerWidget *m_triggerWidget;
		class VariablesDialog *m_variablesDialog;

		KActionCollection *m_triggerActionCollection;
		KActionCollection *m_newActionCollection;
};

inline string TriggerEditor::triggerText(map::Trigger *trigger) const
{
	if (customTextTriggers() == 0)
		return "";

	bool found = false;
	int32 index = 0;

	BOOST_FOREACH(map::Triggers::TriggerEntries::const_reference trig, triggers()->triggers())
	{
		if (&trig == trigger)
		{
			found = true;

			break;
		}

		++index;
	}


	// no corresponding number which should never occur actually
	if (!found)
		return "";

	int32 i = 0;

	BOOST_FOREACH(map::CustomTextTriggers::TriggerTexts::const_reference triggerText, customTextTriggers()->triggerTexts())
	{
		if (i == index)
			return triggerText;

		++i;
	}

	// no corresponding custom trigger text which should never occur actually
	return "";
}

inline map::Triggers* TriggerEditor::triggers() const
{
	return m_triggers;
}

inline map::CustomTextTriggers* TriggerEditor::customTextTriggers() const
{
	return m_customTextTriggers;
}

inline QTreeWidget* TriggerEditor::treeWidget() const
{
	return this->m_treeWidget;
}

inline QTreeWidgetItem* TriggerEditor::rootItem() const
{
	return this->m_rootItem;
}

inline class MapScriptWidget* TriggerEditor::mapScriptWidget() const
{
	return this->m_mapScriptWidget;
}

inline class TriggerWidget* TriggerEditor::triggerWidget() const
{
	return this->m_triggerWidget;
}

inline class VariablesDialog* TriggerEditor::variablesDialog() const
{
	return m_variablesDialog;
}

inline class KActionCollection* TriggerEditor::triggerActionCollection() const
{
	return m_triggerActionCollection;
}

inline KActionCollection* TriggerEditor::newActionCollection() const
{
	return m_newActionCollection;
}


inline KAboutData TriggerEditor::moduleAboutData() const
{
	KAboutData aboutData(Module::moduleAboutData());
	aboutData.setProgramName(ki18n("Trigger Editor"));

	return aboutData;
}

inline QString TriggerEditor::actionName() const
{
	return "triggereditor";
}

inline void TriggerEditor::setTriggers(map::Triggers *triggers)
{
	m_triggers = triggers;
}

inline void TriggerEditor::setCustomTextTriggers(map::CustomTextTriggers *customTextTriggers)
{
	m_customTextTriggers = customTextTriggers;
}

inline void TriggerEditor::setFreeTriggers(bool freeTriggers)
{
	m_freeTriggers = freeTriggers;
}

inline void TriggerEditor::setFreeCustomTextTriggers(bool freeCustomTextTriggers)
{
	m_freeCustomTextTriggers = freeCustomTextTriggers;
}

inline bool TriggerEditor::freeTriggers() const
{
	return m_freeTriggers;
}

inline bool TriggerEditor::freeCustomTextTriggers() const
{
	return m_freeCustomTextTriggers;
}

inline TriggerEditor::TreeItems& TriggerEditor::categories()
{
	return m_categories;
}

inline TriggerEditor::TreeItems& TriggerEditor::variables()
{
	return m_variables;
}

inline TriggerEditor::TreeItems& TriggerEditor::triggerEntries()
{
	return m_triggerEntries;
}

template<class Functions>
QString TriggerEditor::triggerFunctionCategoryText(const map::TriggerData* triggerData, const Functions &functions, const QString& code)
{
	map::TriggerData::Functions::const_iterator functionIterator = functions.find(code.toUtf8().constData());
		
	if (functionIterator != functions.end()) {
		return QString(functionIterator->second->category()->displayText().c_str());
	}
	
	return QString();

}


template<class Functions>
QString TriggerEditor::triggerFunctionText(const map::TriggerData *triggerData, const map::TriggerStrings *triggerStrings, const QString &code, map::TriggerFunction *function, const Functions &functions, const map::TriggerStrings::Entries &entries, bool withLinks, bool withHint, bool withCategory)
{
	map::TriggerStrings::Entries::const_iterator iterator = entries.find(code.toUtf8().constData());
	QString text;
	
	if (iterator != entries.end()) {
		typedef std::vector<string> SplitVector;
		SplitVector vector; // #2: Search for tokens
		boost::algorithm::split(vector, iterator->second->layout(), boost::is_any_of(","), boost::token_compress_on);
		
		typename Functions::const_iterator functionIterator = functions.find(code.toUtf8().constData());
		
		if (functionIterator != functions.end()) {
			const map::TriggerData::Function *triggerDataFunction = functionIterator->second;
			
			if (withCategory && triggerDataFunction->category() != 0 && triggerDataFunction->category()->displayName()) {
				text = (triggerDataFunction->category()->displayText() + " - ").c_str();
			}
			
			std::size_t i = 0;
			
			BOOST_FOREACH (SplitVector::reference ref, vector) {
				if (ref[0] == '~') {
					// encapsulated calls!
					if (function != 0) {
						if (function->parameters().size() > i) {
							string parameter = triggerFunctionParameter(triggerData, triggerStrings, &function->parameters().at(i)).toUtf8().constData();
							
							qDebug() << "Parameter " << i << ": \"" << parameter.c_str() << "\"";
							
							if (parameter.empty()) { // empty parameter for newly created function
								parameter = ref.substr(1); // cut ~
							}
							
							if (withLinks) {
								ref = boost::str(boost::format("<a href=\"%1%\">%2%</a>") % i % parameter);
							} else {
								ref = parameter;
							}
						} else {
							qDebug() << "Invalid parameter index " << i;
						}
					// new function with default or name of ~Value
					} else {
						bool gotDefault = false;
						
						// show default if exists
						if (triggerDataFunction->defaults().size() > i) {
							string parameter = boost::apply_visitor(map::TriggerData::FunctionValueVisitor(), triggerDataFunction->defaults()[i]);
							
							// "_" means no default value
							if (parameter != "_") {
								if (!parameter.empty()) {
									
									// defaults can be presets or calls
									if (QChar(parameter[0]).isLetter()) {
										map::TriggerData::Parameters::const_iterator presetIterator = triggerData->parameters().find(parameter);
										
										if (presetIterator != triggerData->parameters().end()) {
											parameter = presetIterator->second->displayText();
											
											
										// could be a call
										} else {
											map::TriggerStrings::Entries::const_iterator callIterator = triggerStrings->calls().find(parameter);
											
											if (callIterator != triggerStrings->calls().end()) {
												parameter = "(" + cutQuotes(callIterator->second->name()) + ")";
											}
										}
									// strings are enclosed by single quotes
									} else if (parameter[0] == '\"') {
										parameter = cutQuotes(parameter);
									}
								}
								
								if (withLinks) {
									ref = boost::str(boost::format("<a href=\"%1%\">%2%</a>") % i % parameter);
								} else {
									ref = parameter;
								}
								
								gotDefault = true;
							}
						}
						
						// if it's been "_" or there is no default defined use the type's function
						if (!gotDefault) {
							const string parameter = ref.substr(1); // cut ~
							
							if (withLinks) {
								ref = boost::str(boost::format("<a href=\"%1%\">%2%</a>") % i % parameter);
							} else {
								ref = parameter;
							}
						}
					}
					
					++i;
				} else {
					ref = TriggerEditor::cutQuotes(ref);
				}
			}
			
			BOOST_FOREACH (SplitVector::reference ref, vector) {
				text += ref.c_str();
			}
		} else {
			qDebug() << "Did not find function " << code << " in trigger data";
		}
		
		if (withHint && !iterator->second->hint().empty()) {
			text += "<br>";
			//text + tr("Hint:<br>");
			text += cutQuotes(iterator->second->hint()).c_str();
		} else {
			qDebug() << "Hint is empty for " << code;
		}
	} else {
		qDebug() << "Did not find function " << code << " in trigger strings";
	}
	
	return text;
}

}

}

#endif
