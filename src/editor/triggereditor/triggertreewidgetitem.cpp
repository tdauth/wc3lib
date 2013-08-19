/*
 *
 */

#include "triggertreewidgetitem.hpp"

namespace wc3lib
{

namespace editor
{

TriggerTreeWidgetItem::TriggerTreeWidgetItem(map::Trigger *trigger, QTreeWidgetItem *parent, const QStringList& strings, int type) : m_trigger(trigger), QTreeWidgetItem(parent, strings, type)
{
}

}

}
