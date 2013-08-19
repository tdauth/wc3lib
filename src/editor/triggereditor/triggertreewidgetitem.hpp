/*
 *
 */

#ifndef WC3LIB_EDITOR_TRIGGERTREEWIDGETITEM_H
#define WC3LIB_EDITOR_TRIGGERTREEWIDGETITEM_H

#include <QTreeWidget>

#include "../../map.hpp"

namespace wc3lib
{

namespace editor
{

class TriggerTreeWidgetItem : public QTreeWidgetItem
{
	public:
		TriggerTreeWidgetItem(map::Trigger *trigger, QTreeWidgetItem *parent, const QStringList& strings = QStringList(), int type = Type);
		
		map::Trigger* trigger() const;

	private:
		map::Trigger *m_trigger;;
};

inline map::Trigger* TriggerTreeWidgetItem::trigger() const
{
	return this->m_trigger;
}

}

}

#endif // TRIGGERTREEWIDGETITEM_H
