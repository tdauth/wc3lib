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

#include <QScopedPointer>
#include <QDateTime>

#include <kdemacros.h>
#include <KIO/SlaveBase>

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
 * \todo Use KIO::SlaveBase member functions to provide meta data (extended attributes).
 */
class KDE_EXPORT MpqProtocol : public KIO::SlaveBase
{
	public:
		static const char *protocol;

		// FIXME Don't use class MpqArchive until KArchive becomes extensible for custom Entry types etc.
		//typedef boost::scoped_ptr<MpqArchive> MpqArchivePtr;
		typedef QScopedPointer<mpq::Mpq> MpqArchivePtr;


		MpqProtocol(const QByteArray &pool, const QByteArray &app);

		/**
		 * Taken from "SMPQ".
		 *
		 * \param fileName The file name of the MPQ archive.
		 * \param
		 */
		bool parseUrl(const KUrl &url, QString &fileName, QByteArray &archivePath);
		/**
		 * Taken from "SMPQ".
		 *
		 * It replaces \p to by a Windows file path using '\' as separator since
		 * (listfile) entries usually have the Windows format.
		 *
		 * This function should be used on file paths refering to files in an MPQ archive.
		 */
		void toArchivePath(QByteArray &to, const QString &from);
		/**
		 * Opens archive using file path \p archive.
		 * \param error Stores the error message if function returns true. Otherwise value won't be changed.
		 * \return Returns true if archive has been open successfully or is already open. Otherwise if any error occurs it returns false.
		 */
		bool openArchive(const QString &archive, QString &error);

		virtual void listDir(const KUrl &url);
		virtual void stat(const KUrl &url);

		virtual void open(const KUrl &url, QIODevice::OpenMode mode);
		/**
		 * Closes the opened file of the MPQ archive.
		 */
		virtual void close();

		virtual void read(KIO::filesize_t size);
		virtual void seek(KIO::filesize_t offset);
		virtual void mkdir(const KUrl& url, int permissions);

		virtual void get(const KUrl &url);
		virtual void put(const KUrl &url, int permissions, KIO::JobFlags flags);

	private:
		void createRootUDSEntry(KIO::UDSEntry &entry);
		void createUDSEntry(const mpq::MpqFile &mpqFile, KIO::UDSEntry &entry);
		/**
		* \brief find, check and open the archive file
		* \param url The URL of the archive
		* \param path Path where the archive really is (returned value)
		* \param errNum KIO error number (undefined if the function returns true)
		* \return true if file was found, false if there was an error
		*/
		bool checkNewFile(const KUrl &url, QString &path, KIO::Error &errorNum, QIODevice::OpenMode openMode);

		MpqArchivePtr m_archive;
		mpq::MpqFile *m_file;
		std::streampos m_seekPos;
		QString m_archiveName;
		QDateTime m_modified;
};

}

}

#endif
