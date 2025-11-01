/***************************************************************************
 *   Copyright (C) 2016 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_RESOURCESDIALOG_HPP
#define WC3LIB_EDITOR_RESOURCESDIALOG_HPP

#include <QDialog>

#include "ui_resourceswidget.h"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;

/**
 * \brief Lists all loaded resources from a single \ref MpqPriorityList.
 *
 * This dialog helps the user to show which resources are currently loaded.
 * It lists the URLs of all resources from a \ref MpqPriorityList instance.
 */
class ResourcesDialog : public QDialog, protected Ui::ResourcesWidget
{
	Q_OBJECT

	public:
		explicit ResourcesDialog(QWidget *parent = nullptr);

		/**
		 * Updates the GUI by clearing it and listing all resources from \p sources.
		 */
		void setSources(MpqPriorityList *sources);
		MpqPriorityList* sources() const;
        
    protected:
        void showEvent(QShowEvent *e) override;
        
	private:
		MpqPriorityList *m_sources;
};

inline MpqPriorityList* ResourcesDialog::sources() const
{
	return this->m_sources;
}

}

}

#endif
