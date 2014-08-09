/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_HPP
#define WC3LIB_MAP_HPP

namespace wc3lib
{
/**
 * \page mapsection Map module
 * Use this module to read, write and modify Warcraft III maps (*.w3m, *.w3x) and campaigns (*.w3n) and their corresponding files.
 *
 * Warcraft III maps and campaigns are usual MPQ archives containing some specific files, most of them in a binary format.
 *
 * Include file \ref map.hpp and use namespace \ref wc3lib::map for this module.
 *
 * \namespace wc3lib::map
 * \brief \ref mapsection
 */
}

#include "map/camera.hpp"
#include "map/cameras.hpp"
#include "map/campaign.hpp"
#include "map/campaigninfo.hpp"
#include "map/customobjects.hpp"
#include "map/customobjectscollection.hpp"
#include "map/customunits.hpp"
#include "map/environment.hpp"
#include "map/importedfiles.hpp"
#include "map/info.hpp"
#include "map/itemset.hpp"
#include "map/mapstrings.hpp"
#include "map/menuminimap.hpp"
#include "map/object.hpp"
#include "map/pathmap.hpp"
#include "map/platform.hpp"
#include "map/playable.hpp"
#include "map/rect.hpp"
#include "map/rects.hpp"
#include "map/shadow.hpp"
#include "map/slk.hpp"
#include "map/sound.hpp"
#include "map/sounds.hpp"
#include "map/tilepoint.hpp"
#include "map/tree.hpp"
#include "map/trees.hpp"
#include "map/trigger.hpp"
#include "map/triggercategory.hpp"
#include "map/triggerdata.hpp"
#include "map/triggerfunction.hpp"
#include "map/triggerfunctionparameter.hpp"
#include "map/triggers.hpp"
#include "map/triggerstrings.hpp"
#include "map/txt.hpp"
#include "map/variable.hpp"
#include "map/w3g.hpp"
#include "map/w3m.hpp"
#include "map/w3x.hpp"
#include "map/value.hpp"

#endif
