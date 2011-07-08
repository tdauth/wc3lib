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

#include "teamcolordialog.hpp"

namespace wc3lib
{

namespace editor
{

TeamColorDialog::TeamColorDialog(QWidget *parent, Qt::WFlags flags) : QDialog(parent, flags)
{
	setupUi(this);
	
	QList<QColor> colors;
	
	for (BOOST_SCOPED_ENUM(TeamColor) teamColor = TeamColor::Red; teamColor < TeamColor::MaxTeamColors; teamColor = (BOOST_SCOPED_ENUM(TeamColor))((int)teamColor + 1))
		colors << editor::teamColor(teamColor);
	
	this->m_colorComboBox->clear();
	this->m_colorComboBox->setColors(colors);
	//connect(this->m_colorComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateColor(int)));
}

}

}

