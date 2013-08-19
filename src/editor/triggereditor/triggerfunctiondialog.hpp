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

class TriggerFunctionDialog : public QDialog, public Ui::TriggerFunctionDialog
{
	Q_OBJECT
	
	public:
		explicit TriggerFunctionDialog(map::TriggerData *triggerData, QWidget* parent = 0, Qt::WindowFlags f = 0);
		
		void fill(BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type);
		
		map::TriggerData* triggerData() const;
		BOOST_SCOPED_ENUM(map::TriggerFunction::Type) type() const;
		
	public slots:
		void changeCategory(int index);
		void changeFunction(int index);
		
	private:
		map::TriggerData *m_triggerData;
		BOOST_SCOPED_ENUM(map::TriggerFunction::Type) m_type;
};

inline map::TriggerData* TriggerFunctionDialog::triggerData() const
{
	return this->m_triggerData;
}

inline BOOST_SCOPED_ENUM(map::TriggerFunction::Type) TriggerFunctionDialog::type() const
{
	return this->m_type;
}

}

}

#endif // TRIGGERFUNCTIONDIALOG_H
