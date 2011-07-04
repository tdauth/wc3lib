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

#ifndef WC3LIB_EDITOR_MPQPROTOCOL_HPP
#define WC3LIB_EDITOR_MPQPROTOCOL_HPP

#include <KIO/SlaveBase>

#include "mpqarchive.hpp"
#include "../core.hpp"
#include "../mpq.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Implementation for Blizzard's MPQ format which should be usable in KDE applications as normal archive protocol (I/O slave) called "mpq".
 * "kio_mpq" is installed as usual MPQ plugin and therefore should be usable in all KDE-based applications.
 * \todo Finish and make installable as KDE plugin (such like qblp).
 */
class MpqProtocol : public KIO::SlaveBase
{
	public:
		typedef boost::scoped_ptr<MpqArchive> MpqArchivePtr;
		
		MpqProtocol(const QByteArray &pool, const QByteArray &app);
		
		virtual void open(const KUrl &url, QIODevice::OpenMode mode);
		virtual void get(const KUrl &url);
		
	private:
		MpqArchivePtr m_archive;
};

}

}

#endif
