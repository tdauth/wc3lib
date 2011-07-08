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

#ifndef WC3LIB_EDITOR_TEAMCOLORDIALOG_HPP
#define WC3LIB_EDITOR_TEAMCOLORDIALOG_HPP

#include <QDialog>

#include "ui/ui_teamcolordialog.h"
#include "platform.hpp"

namespace wc3lib
{

namespace editor
{

class TeamColorDialog : public QDialog, protected Ui::TeamColorDialog
{
	public:
		TeamColorDialog(QWidget *parent = 0, Qt::WFlags flags = 0);
		
		void setTeamColor(BOOST_SCOPED_ENUM(TeamColor) teamColor);
		BOOST_SCOPED_ENUM(TeamColor) teamColor() const;
};

inline void TeamColorDialog::setTeamColor(BOOST_SCOPED_ENUM(TeamColor) teamColor)
{
	this->m_colorComboBox->setCurrentIndex((int)teamColor);
}

inline BOOST_SCOPED_ENUM(TeamColor) TeamColorDialog::teamColor() const
{
	return editor::teamColor(this->m_colorComboBox->color());
}

}

}

#endif // TEAMCOLORDIALOG_HPP
