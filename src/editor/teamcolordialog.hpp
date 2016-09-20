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

#include "ui_teamcolordialog.h"
#include "platform.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Widget with a \ref KColorComboBox which is limited to all available team colors in Warcraft III.
 * \sa TeamColor
 */
class TeamColorDialog : public QDialog, protected Ui::TeamColorDialog
{
	public:
		TeamColorDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

		/**
		 * Sets the selected team color of the dialog.
		 * \param teamColor The team color which is selected.
		 */
		void setTeamColor(TeamColor teamColor);

		/**
		 * \return Returns the selected team color of the dialog.
		 */
		TeamColor teamColor() const;
};

inline void TeamColorDialog::setTeamColor(TeamColor teamColor)
{
	this->m_colorComboBox->setCurrentIndex((int)teamColor);
}

inline TeamColor TeamColorDialog::teamColor() const
{
	return editor::TeamColor(this->m_colorComboBox->currentIndex());
}

}

}

#endif // TEAMCOLORDIALOG_HPP
