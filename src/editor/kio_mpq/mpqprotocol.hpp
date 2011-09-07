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
#include "../../core.hpp"
#include "../../mpq.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Implementation for Blizzard's MPQ format which should be usable in KDE applications as normal archive protocol (I/O slave) called "mpq".
 * "kio_mpq" is installed as usual MPQ plugin and therefore should be usable in all KDE-based applications.
 * \note Based on code of class \ref ArchiveProtocol which is part of kdebase-runtime libraries.
 * \todo Finish and make installable as KDE plugin (such like qblp).
 */
class MpqProtocol : public KIO::SlaveBase
{
	public:
		static const char *protocol;

		typedef boost::scoped_ptr<MpqArchive> MpqArchivePtr;

		MpqProtocol(const QByteArray &pool, const QByteArray &app);

		virtual void listDir( const KUrl & url );
		virtual void stat( const KUrl & url );

		virtual void open(const KUrl &url, QIODevice::OpenMode mode);
		virtual void get(const KUrl &url);
		virtual void put(const KUrl &url, int permissions, KIO::JobFlags flags);

	private:
		void createRootUDSEntry(KIO::UDSEntry &entry);
		void createUDSEntry(const KArchiveEntry *archiveEntry, KIO::UDSEntry &entry);
		/**
		* \brief find, check and open the archive file
		* \param url The URL of the archive
		* \param path Path where the archive really is (returned value)
		* \param errNum KIO error number (undefined if the function returns true)
		* \return true if file was found, false if there was an error
		*/
		bool checkNewFile(const KUrl &url, QString &path, KIO::Error &errorNum, QIODevice::OpenMode openMode);

		MpqArchivePtr m_archive;
		QString m_archiveName;
		QDateTime m_modified;
};

}

}

#endif
