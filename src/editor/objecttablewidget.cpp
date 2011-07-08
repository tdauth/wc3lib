/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include "objecttablewidget.hpp"
#include "objecttablewidgetpair.hpp"
#include "objectintegerdialog.hpp"

namespace wc3lib
{

namespace editor
{

ObjectTableWidget::ObjectTableWidget(QWidget *parent, const MetaData *metaData)
{
	int i = 0;
	
	foreach (MetaData::MetaDataEntries::const_reference entry, metaData->metaDataEntries())
	{
		ObjectTableWidgetPair *pair = new ObjectTableWidgetPair(this, i, entry.get());
		++i;
	}
}

ObjectTableWidget::Pairs ObjectTableWidget::pairs(BOOST_SCOPED_ENUM(map::Section) section)
{
	Pairs result;
	
	foreach (ObjectTableWidgetPair *pair, pairs())
	{
		if (pair->metaData()->section() == section)
			result << pair;
	}
	
	return result;
}

void ObjectTableWidget::editItem(QTableWidgetItem *item)
{
	foreach (ObjectTableWidgetPair *pair, m_pairs)
	{
		if (pair->valueItem() == item)
		{
			switch (pair->metaData()->type())
			{
				case map::Value::Type::Integer:
				{
					ObjectIntegerDialog *dialog = new ObjectIntegerDialog(this);
					dialog->setCaption(pair->metaData()->displayName().c_str());
					dialog->intSpinBox()->setMaximum(pair->metaData()->maxValue().toInteger());
					dialog->intSpinBox()->setMinimum(pair->metaData()->minValue().toInteger());
					dialog->intSpinBox()->setValue(pair->data()->left.find(pair->metaData())->second.toInteger());
					
					if (dialog->exec() == QDialog::Accepted)
						pair->setCustomValue(map::Value(boost::numeric_cast<map::int32>(dialog->intSpinBox()->value())));
					
					break;
				}
				
				case map::Value::Type::Real:
					break;
					
				case map::Value::Type::Unreal:
					break;
					
				case map::Value::Type::String:
					break;
					
				case map::Value::Type::Boolean:
					break;
					
				case map::Value::Type::Character:
					break;
					
				case map::Value::Type::UnitList:
					break;
					
				case map::Value::Type::ItemList:
					break;
					
				case map::Value::Type::RegenerationType:
					break;
					
				case map::Value::Type::AttackType:
					break;
					
				case map::Value::Type::WeaponType:
					break;
					
				case map::Value::Type::TargetType:
					break;
					
				case map::Value::Type::MoveType:
					break;
					
				case map::Value::Type::DefenseType:
					break;
					
				case map::Value::Type::PathingTexture:
					break;
					
				case map::Value::Type::UpgradeList:
					break;
					
				case map::Value::Type::StringList:
					break;
					
				case map::Value::Type::AbilityList:
					break;
					
				case map::Value::Type::HeroAbilityList:
					break;
					
				case map::Value::Type::MissileArt:
					break;
					
				case map::Value::Type::AttributeType:
					break;
					
				case map::Value::Type::AttackBits:
					break;
			}
			
			break;
		}
	}
}

}

}
