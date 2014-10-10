/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_NEWMAPDIALOG_HPP
#define WC3LIB_EDITOR_NEWMAPDIALOG_HPP

#include <QDialog>

#include <kdemacros.h>

#include "ui_newmapdialog.h"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;
class Tileset;
class Tile;

/**
 * Dialog which is displayed when creating a new map in editor.
 * Allows you to select size, main tileset, filling tile, initial cliff and water level and if there is a randomized height level of map.
 */
class KDE_EXPORT NewMapDialog : public QDialog, protected Ui::NewMapDialog
{
	public:
		NewMapDialog(MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);

		void setSize(const QSize &size);
		QSize size() const;
		void setMainTileset(Tileset *tileset);
		Tileset* tileset() const;
		void setFillingTile(Tile *tile);
		Tile* fillingTile() const;
		void setInitialCliffLevel(int16 cliffLevel);
		int16 initialCliffLevel() const;
		void setInitialWaterLevel(int16 waterLevel);
		int16 initialWaterLevel() const;
		void setRandomizedHeightLevel(bool randomizedHeightLevel);
		bool hasRandomizedHeightLevel() const;
};

}

}

#endif
