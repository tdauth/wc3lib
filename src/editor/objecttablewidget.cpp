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

ObjectTableWidget::ObjectTableWidget(QWidget *parent, const QLinkedList<map::MetaData*> &metaData)
{
}

void ObjectTableWidget::update(const QLinkedList<map::Data*> &data)
{
}

void ObjectTableWidget::editItem(QTableWidgetItem *item)
{
	foreach (ObjectTableWidgetPair *pair, m_pairs)
	{
		if (pair->valueItem() == item)
		{
			switch (pair->metaData()->type())
			{
				case map::Value::Integer:
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
				
				case map::Value::Real:
					break;
					
				case map::Value::Unreal:
					break;
					
				case map::Value::String:
					break;
					
				case map::Value::Boolean:
					break;
					
				case map::Value::Character:
					break;
					
				case map::Value::UnitList:
					break;
					
				case map::Value::ItemList:
					break;
					
				case map::Value::RegenerationType:
					break;
					
				case map::Value::AttackType:
					break;
					
				case map::Value::WeaponType:
					break;
					
				case map::Value::TargetType:
					break;
					
				case map::Value::MoveType:
					break;
					
				case map::Value::DefenseType:
					break;
					
				case map::Value::PathingTexture:
					break;
					
				case map::Value::UpgradeList:
					break;
					
				case map::Value::StringList:
					break;
					
				case map::Value::AbilityList:
					break;
					
				case map::Value::HeroAbilityList:
					break;
					
				case map::Value::MissileArt:
					break;
					
				case map::Value::AttributeType:
					break;
					
				case map::Value::AttackBits:
					break;
			}
			
			break;
		}
	}
}

}

}
