/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
 *   tamino@cdauth.de                                                      *
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

#ifndef WC3LIB_EDITOR_FILEMETAINFO_HPP
#define WC3LIB_EDITOR_FILEMETAINFO_HPP

#include <kfilemetainfo.h>

namespace wc3lib
{

namespace editor
{

/**
* Custom file meta info class for integrated file system browser MPQ support.
*/
class FileMetaInfo : public KFileMetaInfo
{
	public:
		FileMetaInfo();
		const KFileMetaInfoItem& item(const QString &key) const;
		KFileMetaInfoItem& item(const QString &key);
		const QHash<QString, KFileMetaInfoItem>& items() const;
};

}

}

#endif
