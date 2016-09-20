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

#ifndef WC3LIB_EDITOR_KIO_MPQ_HPP
#define WC3LIB_EDITOR_KIO_MPQ_HPP

#include <QScopedPointer>
#include <QDateTime>

#include <KIO/SlaveBase>

#include "../../core.hpp"
#include "../../mpq.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief KDE I/O slave for MPQ archives.
 *
 * Implementation for Blizzard's MPQ format which should be usable in KDE applications as normal archive protocol (I/O slave) called "mpq".
 * "kio_mpq" is installed as usual MPQ plugin and therefore should be usable in all KDE-based applications.
 */
class MpqSlave : public KIO::SlaveBase
{
	public:
		static const char *protocol;

		// FIXME Don't use class MpqArchive until KArchive becomes extensible for custom Entry types etc.
		//typedef boost::scoped_ptr<MpqArchive> MpqArchivePtr;
		typedef QScopedPointer<mpq::Archive> MpqArchivePtr;


		MpqSlave(const QByteArray &pool, const QByteArray &app);
		virtual ~MpqSlave();

		virtual void listDir(const QUrl &url) override;
		virtual void stat(const QUrl &url) override;

		virtual void open(const QUrl &url, QIODevice::OpenMode mode) override;
		/**
		 * Closes the opened file of the MPQ archive.
		 */
		virtual void close() override;

		virtual void read(KIO::filesize_t size) override;
		virtual void seek(KIO::filesize_t offset) override;
		virtual void mkdir(const QUrl& url, int permissions) override;

		virtual void get(const QUrl &url) override;
		virtual void put(const QUrl &url, int permissions, KIO::JobFlags flags) override;

	private:
		/**
		 * Taken from "SMPQ".
		 *
		 * \param fileName The file name of the MPQ archive.
		 * \param
		 */
		bool parseUrl(const QUrl &url, QString &fileName, QByteArray &archivePath);
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

		MpqArchivePtr m_archive;
		bool m_hasAttributes;
		int32 m_attributesVersion;
		mpq::Attributes::ExtendedAttributes m_extendedAttributes;
		mpq::Attributes::Crc32s m_crcs;
		mpq::Attributes::Md5s m_md5s;
		mpq::Attributes::FileTimes m_fileTimes;
		mpq::File m_file;
		std::streampos m_seekPos;
		QString m_archiveName;
		QDateTime m_modified;
};

}

}

#endif
