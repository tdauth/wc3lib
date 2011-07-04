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

#include <KMimeType>

#include "mpqprotocol.hpp"

namespace wc3lib
{

namespace editor
{

MpqProtocol::MpqProtocol(const QByteArray &pool, const QByteArray &app) : KIO::SlaveBase("mpq", pool, app)
{
}

void MpqProtocol::open(const KUrl &url, QIODevice::OpenMode mode)
{
	/*
	MpqArchivePtr ptr(new MpqArchive(url.toLocalFile()));
	
	if (!ptr->open(mode))
	{
		error(KIO::ERR_ABORTED, url.toLocalFile());
		return;
	}
	
	m_archive.swap(ptr);
	*/
	 // Determine the mimetype of the file to be retrieved, and emit it.
	// This is mandatory in all slaves (for KRun/BrowserRun to work).
	// If we're not opening the file ReadOnly or ReadWrite, don't attempt to
	// read the file and send the mimetype.
	/*
	if (mode & QIODevice::ReadOnly)
	{
		KMimeType::Ptr mt = KMimeType::findByUrl(url, 0, true /* local URL *//* );
		emit mimeType(mt->name());
	}
	
	totalSize(m_archive->mpq()->size());
	position(0);
	
	emit opened();
	*/
}

void MpqProtocol::get(const KUrl &url)
{
	//KMimeType::Ptr mt = KMimeType::findByUrl( url, buff.st_mode, true /* local URL */ );
    //emit mimeType( mt->name() );
}

}

}
