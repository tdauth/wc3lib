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

#ifndef WC3LIB_EDITOR_TRIGGEREDITOR_TRIGGERFUNCTIONDIALOG_H
#define WC3LIB_EDITOR_TRIGGEREDITOR_TRIGGERFUNCTIONDIALOG_H

#include <QDialog>

#include "ui_triggerfunctiondialog.h"
#include "../../map.hpp"
#include "triggereditor.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * This dialog is used to display a simple trigger function of one trigger, usually an event, condition or action.
 * Furthermore it serves as sub dialog for trigger calls which are usually parameters of a normal function.
 *
 * It provides a combo box for trigger categories and another one for the functions.
 *
 * Additionally, this class supports selection of trigger calls which are a bit more compilcated since they have to be evaluated using their parameter information and
 * offer different widgets!
 */
class TriggerFunctionDialog : public QDialog, public Ui::TriggerFunctionDialog
{
	Q_OBJECT

	public:
		/**
		 * \param triggers Triggers are required for function dialogs which contain variables of a specific type.
		 */
		explicit TriggerFunctionDialog(TriggerEditor *triggerEditor, map::TriggerData *triggerData, map::TriggerStrings *triggerStrings, map::Triggers *triggers, QWidget* parent = nullptr);

		virtual ~TriggerFunctionDialog();


		/**
		 * Use this function for presets/variables/code arguments which have no custom trigger function.
		 */
		void fillCall(int argumentIndex);
		/**
		 * Use this function for calls which are arguments of another functions (for example in sub dialogs).
		 */
		void fillCall(map::TriggerFunction *function, int argumentIndex);

		/**
		 * Fills combo boxes with categories and trigger functions for \p type.
		 * Clears the current trigger function.
		 * \note This function should be used when creating new trigger functions.
		 */
		void fill(map::TriggerFunction::Type type);
		/**
		 * Fills combo boxes with categories and trigger functions for function type of \p function.
		 * Besides it selects \p function and shows it specified parameters.
		 * \note This function should be used when editing an existing trigger function.
		 */
		void fill(map::TriggerFunction *function);

		/**
		 * Fills all widgets with defaults of \p function.
		 * Works for calls as well.
		 * Use this function the first time when selecting a NEW function with now custom values.
		 */
		void fill(map::TriggerFunction::Type type, map::TriggerData::Function *function);

		/**
		 * \return Returns true if function is fully defined.
		 * \sa acceptOnlyValid()
		 */
		bool isValid() const;

		/**
		 * Applies all settings of dialog to \p parameter which means updating its parameters.
		 */
		void apply(map::TriggerFunctionParameter *parameter);
		void apply(map::TriggerFunction *function);

		/**
		 * \return Returns the name of the function which is currently selected.
		 */
		string functionName() const;
		/**
		 * \return Returns the user defined parameters of the function which is currently selected.
		 */
		map::TriggerFunction::Parameters parameters() const;

		TriggerEditor* triggerEditor() const;
		map::TriggerData* triggerData() const;
		map::TriggerStrings* triggerStrings() const;
		map::Triggers* triggers() const;
		map::TriggerFunction::Type type() const;
		map::TriggerFunction* function() const;

		/**
		 * \return Returns true if the current dialog is a sub dialog of another one used to define a function parameter.
		 */
		bool isSubDialog() const;

		/**
		 * If the current dialog is a sub dialog of another trigger function dialog this will return its parent dialog.
		 */
		TriggerFunctionDialog* parentDialog() const;

		/**
		 * Each time a parameter value is selected a new dialog is displayed which can be accessed via this function.
		 */
		TriggerFunctionDialog* subDialog() const;

	public slots:
		/**
		 * If function is not fully defined yet, dialog can only be rejected.
		 */
		void acceptOnlyValid();

		void changeCategory(int index);
		/**
		 * Changes function in combobox and creates a new one with default parameters which has to be defined by the user.
		 * \sa isValid()
		 */
		void changeFunctionToNew(int index);
		void changeFunction(int index);
		void editParameter(const QString &parameter);
		void editVariables(bool);

	private:
		/**
		 * Fills combo boxes with categories and trigger functions for \p type.
		 */
		void fillInternal(map::TriggerFunction::Type type);

		/**
		 * Shown functions in combo box depend on selected category.
		 * For events, conditions and actions there is a category called "All" which means that all functions are shown with prefixes of their corresponding trigger categories:
		 * "Game - Name"
		 *
		 *
		 */
		template<class Functions>
		void filterFunctionsByCategory(int index, const Functions &functions, const map::TriggerStrings::Entries &entries);

		/**
		 * \return Returns an iterator to the corresponding function of \p code from \p functions.
		 */
		template<class Functions>
		typename Functions::const_iterator triggerFunction(const string &code, const Functions &functions) const;

		template<class Functions>
		void showFunction(const QString &code, const Functions &functions, const map::TriggerStrings::Entries &entries);

		/**
		 * \ref Returns the corresponding type name of \p function's \p argumentIndex argument.
		 */
		string argumentType(const map::TriggerFunction *function, int32 argumentIndex) const;

		TriggerEditor *m_triggerEditor;
		map::TriggerData *m_triggerData;
		map::TriggerStrings *m_triggerStrings;
		map::Triggers *m_triggers;
		map::TriggerFunction::Type m_type;
		map::TriggerFunction *m_function;
		bool m_freeFunction;
		int m_argumentIndex;

		TriggerFunctionDialog *m_subDialog;
};

inline TriggerEditor* TriggerFunctionDialog::triggerEditor() const
{
	return this->m_triggerEditor;
}

inline map::TriggerData* TriggerFunctionDialog::triggerData() const
{
	return this->m_triggerData;
}

inline map::TriggerStrings* TriggerFunctionDialog::triggerStrings() const
{
	return this->m_triggerStrings;
}

inline map::Triggers* TriggerFunctionDialog::triggers() const
{
	return this->m_triggers;
}

inline map::TriggerFunction::Type TriggerFunctionDialog::type() const
{
	return this->m_type;
}

inline map::TriggerFunction* TriggerFunctionDialog::function() const
{
	return this->m_function;
}

inline bool TriggerFunctionDialog::isSubDialog() const
{
	return dynamic_cast<TriggerFunctionDialog*>(this->parent()) != 0;
}

inline TriggerFunctionDialog* TriggerFunctionDialog::parentDialog() const
{
	return boost::polymorphic_cast<TriggerFunctionDialog*>(this->parent());
}

inline TriggerFunctionDialog* TriggerFunctionDialog::subDialog() const
{
	if (m_subDialog == 0) {
		const_cast<TriggerFunctionDialog*>(this)->m_subDialog = new TriggerFunctionDialog(this->triggerEditor(), this->triggerData(), this->triggerStrings(), this->triggers(), const_cast<TriggerFunctionDialog*>(this));
	}

	return this->m_subDialog;
}

}

}

#endif // TRIGGERFUNCTIONDIALOG_H
