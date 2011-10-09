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

#ifndef WC3LIB_EDITOR_SOURCESDIALOG_HPP
#define WC3LIB_EDITOR_SOURCESDIALOG_HPP

#include <KDialog>

#include "ui/ui_sourcesdialog.h"

namespace wc3lib
{

namespace editor
{

/**
 * This class provides a GUI which shows a list of selected sources based on class \ref MpqPriorityList.
 * It uses a simple KDE edit list box with a URL completion model.
 * All changes on its list do directly affect its corresponding source (\ref source()).
 * \note Since MpqPriorityList doesn't provide any signals you'll have to call update when any changes on its source were made.
 */
class SourcesDialog : public KDialog, protected Ui::SourcesDialog
{
	Q_OBJECT

	public slots:
		/**
		 * Updates the list of all source based on stored sources of corresponding source object (\ref source()).
		 */
		void update();

	public:
		SourcesDialog(class MpqPriorityList *source, QWidget *parent = 0, Qt::WFlags flags = 0);

		class MpqPriorityList* source() const;

	protected slots:
		void added(const QString &text);

	private:
		class MpqPriorityList *m_source;
};

inline MpqPriorityList* SourcesDialog::source() const
{
	return m_source;
}

}

}

#endif
