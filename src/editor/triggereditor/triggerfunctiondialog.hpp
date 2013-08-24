/*
 *
 */

#ifndef TRIGGERFUNCTIONDIALOG_H
#define TRIGGERFUNCTIONDIALOG_H

#include <QDialog>

#include "ui_triggerfunctiondialog.h"
#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * This dialog is used to display a simple trigger function of one trigger, usually an event, condition or action.
 * It provides a combo box for trigger categories and another one for the functions.
 * 
 * Additionally, this class supports selection of trigger calls which are a bit more compilcated since they have to be evaluated using their parameter information and
 * offer different widgets!
 */
class TriggerFunctionDialog : public QDialog, public Ui::TriggerFunctionDialog
{
	Q_OBJECT
	
	public:
		explicit TriggerFunctionDialog(map::TriggerData *triggerData, map::TriggerStrings *triggerStrings, QWidget* parent = 0, Qt::WindowFlags f = 0);
		
		/**
		 * Fills combo boxes with categories and trigger functions for \p type.
		 * Clears the current trigger function.
		 * \note This function should be used when creating new trigger functions.
		 */
		void fill(BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type);
		/**
		 * Fills combo boxes with categories and trigger functions for function type of \p function.
		 * Besides it selects \p function and shows it specified parameters.
		 * \note This function should be used when editing an existing trigger function.
		 */
		void fill(map::TriggerFunction *function);
		
		/**
		 * \return Returns the name of the function which is currently selected.
		 */
		string functionName() const;
		/**
		 * \return Returns the user defined parameters of the function which is currently selected.
		 */
		map::TriggerFunction::Parameters parameters() const;
		
		map::TriggerData* triggerData() const;
		map::TriggerStrings* triggerStrings() const;
		BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type() const;
		map::TriggerFunction* function() const;
		
		/**
		 * Each time a parameter value is selected a new dialog is displayed.
		 */
		TriggerFunctionDialog* subDialog() const;
		
	public slots:
		void changeCategory(int index);
		void changeFunction(int index);
		void editParameter(const QString &parameter);
		
	private:
		/**
		 * Fills combo boxes with categories and trigger functions for \p type.
		 */
		void fillInternal(BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type);
		
		/**
		 * Returns the corresponding string for \p triggerFunction's call in the following form:
		 * <functionname>( .. sub calls( ... ) ... )
		 */
		QString triggerFunction(const map::TriggerFunction *triggerFunction) const;
		/**
		 * Builds the corresponding string for \p parameter by encapsulating all sub calls recursively using \ref triggerFunction().
		 * If it is only a variable, JASS call or preset, it simply returns the corresponding string.
		 */
		QString triggerFunctionParameter(const map::TriggerFunctionParameter *parameter) const;
		
		template<class Functions>
		void showFunction(const QString &code, const Functions &functions, const map::TriggerStrings::Entries &entries);
		
		template<class Functions>
		void filterFunctionsByCategory(int index, const Functions &functions, const map::TriggerStrings::Entries &entries);
		
		/**
		 * \return Returns an iterator to the corresponding function of \p code from \p functions.
		 */
		template<class Functions>
		typename Functions::const_iterator triggerFunction(const string &code, const Functions &functions) const;
		
		/**
		 * \return Returns an iterator to the corresponding TriggerStrings.txt entry of \p code from \p entries.
		 */
		map::TriggerStrings::Entries::const_iterator triggerFunctionEntry(const string &code, const map::TriggerStrings::Entries &entries) const;
		
		/**
		 * \return Returns an iterator to the corresponding TriggerStrings.txt entry of \p code from entries specified by \p type.
		 */
		map::TriggerStrings::Entries::const_iterator triggerFunctionEntry(const string &code, BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type) const;
		
		map::TriggerData *m_triggerData;
		map::TriggerStrings *m_triggerStrings;
		BOOST_SCOPED_ENUM(map::TriggerFunction::Type) m_type;
		map::TriggerFunction *m_function;
		
		TriggerFunctionDialog *m_subDialog;
};

inline map::TriggerData* TriggerFunctionDialog::triggerData() const
{
	return this->m_triggerData;
}

inline map::TriggerStrings* TriggerFunctionDialog::triggerStrings() const
{
	return this->m_triggerStrings;
}

inline BOOST_SCOPED_ENUM(map::TriggerFunction::Type) TriggerFunctionDialog::type() const
{
	return this->m_type;
}

inline map::TriggerFunction* TriggerFunctionDialog::function() const
{
	return this->m_function;
}

inline TriggerFunctionDialog* TriggerFunctionDialog::subDialog() const
{
	return this->m_subDialog;
}

}

}

#endif // TRIGGERFUNCTIONDIALOG_H
