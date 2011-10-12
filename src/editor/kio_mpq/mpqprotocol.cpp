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

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>

#include <QFile>

#include <KMimeType>
#include <KDebug>
#include <KComponentData>
#include <KGlobal>
#include <KUrl>
#include <KDebug>
#include <KLocale>
#include <kde_file.h>
#include <kio/global.h>
#include <KUser>
#include <KTemporaryFile>

#include "mpqprotocol.hpp"

namespace wc3lib
{

namespace editor
{

extern "C" int KDE_EXPORT kdemain(int argc, char **argv)
{
	kDebug(7000) << "Entering function";
	KComponentData instance("kio_mpq");

	if (argc != 4)
	{
		fprintf(stderr, "Usage: kio_mpq protocol domain-socket1 domain-socket2\n");
		exit(-1);
	}

	MpqProtocol slave(argv[2], argv[3]);
	slave.dispatchLoop();

	return 0;
}

const char *MpqProtocol::protocol= "mpq";

MpqProtocol::MpqProtocol(const QByteArray &pool, const QByteArray &app) : KIO::SlaveBase(protocol, pool, app)
{
}

void MpqProtocol::open(const KUrl &url, QIODevice::OpenMode mode)
{
	MpqArchivePtr ptr(new mpq::Mpq());

	try
	{
		ptr->open(url.toLocalFile().toUtf8().constData());
	}
	catch (Exception &exception)
	{
		error(KIO::ERR_ABORTED, i18n("%1: \"%2\"", url.toLocalFile(), exception.what().c_str()));

		return;
	}

	m_archive.swap(ptr);

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

void MpqProtocol::listDir(const KUrl &url)
{
	kDebug( 7109 ) << "MpqProtocol::listDir" << url.url();

	QString path;
	KIO::Error errorNum;

	if (!checkNewFile(url, path, errorNum, QIODevice::ReadOnly))
	{
		if (errorNum == KIO::ERR_CANNOT_OPEN_FOR_READING)
		{
			// If we cannot open, it might be a problem with the archive header (e.g. unsupported format)
			// Therefore give a more specific error message
			error(KIO::ERR_SLAVE_DEFINED,
			i18n( "Could not open the file, probably due to an unsupported file format.\n%1",
			url.prettyUrl()));

			return;
		}
		else if (errorNum != KIO::ERR_IS_DIRECTORY)
		{
			// We have any other error
			error(errorNum, url.prettyUrl());

			return;
		}

		// It's a real dir -> redirect
		KUrl redir;
		redir.setPath( url.path() );
		kDebug( 7109 ) << "Ok, redirection to" << redir.url();
		redirection( redir );
		finished();
		// And let go of the tar file - for people who want to unmount a cdrom after that
		m_archive.reset();

		return;
	}

	if (path.isEmpty())
	{
		KUrl redir(url.protocol() + QString::fromLatin1(":/"));
		kDebug( 7109 ) << "url.path()=" << url.path();
		redir.setPath(url.path() + QString::fromLatin1("/"));
		kDebug( 7109 ) << "ArchiveProtocol::listDir: redirection" << redir.url();
		redirection(redir);
		finished();

		return;
	}

	kDebug( 7109 ) << "checkNewFile done";
	BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale = mpq::MpqFile::Locale::Neutral;
	BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) platform = mpq::MpqFile::Platform::Default;
	path = MpqArchive::resolvePath(path, locale, platform);

	mpq::MpqFile *file = this->m_archive->findFile(path.toUtf8().constData(), locale, platform);

	if (file != 0)
	{
		error(KIO::ERR_IS_FILE, url.prettyUrl());

		return;
	}

	// TODO get possible paths from (listfile) file!
	/*
	const QStringList l = dir->entries();
	totalSize(l.count());

	KIO::UDSEntry entry;

	if (!l.contains("."))
	{
		createRootUDSEntry(entry);
		listEntry(entry, false);
	}

	QStringList::const_iterator it = l.begin();

	for( ; it != l.end(); ++it )
	{
		kDebug(7109) << (*it);
		const KArchiveEntry* archiveEntry = dir->entry( (*it) );

		createUDSEntry(archiveEntry, entry);

		listEntry(entry, false);
	}

	listEntry(entry, true); // ready

	finished();

	kDebug( 7109 ) << "MpqProtocol::listDir done";
	*/
}

void MpqProtocol::stat(const KUrl &url)
{
	QString path;
	KIO::UDSEntry entry;
	KIO::Error errorNum;

	if (!checkNewFile(url, path, errorNum, QIODevice::ReadOnly))
	{
		// We may be looking at a real directory - this happens
		// when pressing up after being in the root of an archive
		if (errorNum == KIO::ERR_CANNOT_OPEN_FOR_READING)
		{
			// If we cannot open, it might be a problem with the archive header (e.g. unsupported format)
			// Therefore give a more specific error message
			error(KIO::ERR_SLAVE_DEFINED,
			i18n( "Could not open the file, probably due to an unsupported file format.\n%1",
			url.prettyUrl()));

			return;
		}
		else if (errorNum != KIO::ERR_IS_DIRECTORY)
		{
			// We have any other error
			error(errorNum, url.prettyUrl());

			return;
		}
		// Real directory. Return just enough information for KRun to work
		entry.insert( KIO::UDSEntry::UDS_NAME, url.fileName());
		kDebug( 7109 ).nospace() << "ArchiveProtocol::stat returning name=" << url.fileName();

		KDE_struct_stat buff;
#ifdef Q_WS_WIN
		QString fullPath = url.path().remove(0, 1);
#else
		QString fullPath = url.path();
#endif

		if (KDE_stat( QFile::encodeName( fullPath ), &buff ) == -1 )
		{
			// Should not happen, as the file was already stated by checkNewFile
			error( KIO::ERR_COULD_NOT_STAT, url.prettyUrl() );

			return;
		}

		entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, buff.st_mode & S_IFMT);

		statEntry(entry);

		finished();

		// And let go of the tar file - for people who want to unmount a cdrom after that
		m_archive.reset();

		return;
	}

	// root entry
	if (path.isEmpty())
	{
		path = QString::fromLatin1("/");
		createRootUDSEntry(entry);
		statEntry(entry);

		finished();

		return;
	}


	mpq::MpqFile *file = resolvePath(path);

	if (file == 0)
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());

		return;
	}

	createUDSEntry(*file, entry);
	statEntry(entry);

	finished();
}

void MpqProtocol::get(const KUrl &url)
{
	// when archive file path only show list of files
	/*
	if (url.toLocalFile() == m_archive->path().string().c_str())
	{
		m_archive->directory()->entries();
	}
	*/
	//KMimeType::Ptr mt = KMimeType::findByUrl( url, buff.st_mode, true /* local URL */ );
	//emit mimeType( mt->name() );

	QString path;
	KIO::Error errorNum;

	if (!checkNewFile(url, path, errorNum, QIODevice::ReadOnly))
	{
		if (errorNum == KIO::ERR_CANNOT_OPEN_FOR_READING )
		{
			// If we cannot open, it might be a problem with the archive header (e.g. unsupported format)
			// Therefore give a more specific error message
			error(KIO::ERR_SLAVE_DEFINED,
			i18n( "Could not open the file, probably due to an unsupported file format.\n%1",
			url.prettyUrl()));

			return;
		}
		else
		{
			// We have any other error
			error(errorNum, url.prettyUrl());

			return;
		}
	}

	mpq::MpqFile *file = resolvePath(path);

	if (file == 0)
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());

		return;
	}

	if (!file->isFile())
	{
		error(KIO::ERR_IS_DIRECTORY, url.prettyUrl());

		return;
	}

	/*
	 * TODO redirection
	const KArchiveFile* archiveFileEntry = static_cast<const KArchiveFile*>(archiveEntry);

	if (!archiveEntry->symLinkTarget().isEmpty())
	{
		kDebug(7109) << "Redirection to" << archiveEntry->symLinkTarget();
		KUrl realURL(url, archiveEntry->symLinkTarget());
		kDebug(7109).nospace() << "realURL=" << realURL.url();
		redirection(realURL);
		finished();

		return;
	}
	*/

	//kDebug(7109) << "Preparing to get the archive data";

	/*
	* The easy way would be to get the data by calling archiveFileEntry->data()
	* However this has drawbacks:
	* - the complete file must be read into the memory
	* - errors are skipped, resulting in an empty file
	*/

	/*
	boost::scoped_ptr<QIODevice> io(archiveFileEntry->createDevice());

	if (!io)
	{
		error( KIO::ERR_SLAVE_DEFINED,
		i18n("The archive file could not be opened, perhaps because the format is unsupported.\n%1" ,
                url.prettyUrl()));

		return;
	}

	if (!io->open(QIODevice::ReadOnly))
	{
		error(KIO::ERR_CANNOT_OPEN_FOR_READING, url.prettyUrl());

		return;
	}
	*/

	totalSize(file->size());

	// Size of a QIODevice read. It must be large enough so that the mime type check will not fail
	const qint64 sectorSize = this->m_archive->sectorSize();

	qint64 bufferSize = sectorSize;
	QByteArray buffer;
	buffer.resize(bufferSize);

	if (buffer.isEmpty() && bufferSize > 0)
	{
		// Something went wrong
		error(KIO::ERR_OUT_OF_MEMORY, url.prettyUrl());

		return;
	}

	bool firstRead = true;

	// How much file do we still have to process?
	mpq::uint32 sectorIndex = 0;
	KIO::filesize_t processed = 0;

	// open for better streaming
	mpq::ifstream ifstream(m_archive->path(), std::ios::binary | std::ios::in);

	if (!ifstream)
	{
		error(KIO::ERR_ACCESS_DENIED, url.prettyUrl());

		return;
	}

	while (sectorIndex < file->sectors().size())
	{
		if (!firstRead)
		{
			bufferSize = sectorSize;
			buffer.resize(bufferSize);
		}

		// Avoid to use bufferSize here, in case something went wrong.
		mpq::arraystream ostream(buffer.data(), buffer.size()); // TODO check if unbuffered stream works
		qint64 read = 0;

		try
		{
			const mpq::Sector *sector = file->sectors().find(sectorIndex)->get();
			sector->seekg(ifstream);
			read = sector->writeData(ifstream, ostream);
		}
		catch (Exception &exception)
		{
			kWarning(7109) << "Read" << read << "bytes but expected" << bufferSize ;
			error(KIO::ERR_COULD_NOT_READ, i18n("%1: \"%2\".", url.prettyUrl(), exception.what().c_str()));

			return;
		}

		if (firstRead)
		{
			// We use the magic one the first data read
			// (As magic detection is about fixed positions, we can be sure that it is enough data.)
			KMimeType::Ptr mime = KMimeType::findByNameAndContent(path, buffer);
			kDebug(7109) << "Emitting mimetype" << mime->name();
			mimeType(mime->name());
			firstRead = false;
		}

		data(buffer);
		processed += read;
		processedSize(processed);
		++sectorIndex;
	}

	data(QByteArray());

	finished();
}

void MpqProtocol::put(const KUrl &url, int permissions, KIO::JobFlags flags)
{
	QString path;
	KIO::Error err;

	if (!checkNewFile(url, path, err, QIODevice::ReadWrite)) // reading for finding existing files
	{
		error(err, url.prettyUrl());

		return;
	}

	BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale = mpq::MpqFile::Locale::Neutral;
	BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) platform = mpq::MpqFile::Platform::Default;
	path = MpqArchive::resolvePath(path, locale, platform);

	if (path == "(listfile)" || path == "(signature)" || path == "(attributes)" || path.contains("(patch_metadata)"))
	{
		error(KIO::ERR_WRITE_ACCESS_DENIED, url.prettyUrl());

		return;
	}

	const mpq::MpqFile *file = this->m_archive->findFile(path.toUtf8().constData(), locale, platform);

	if (file && !(flags & KIO::Overwrite))
	{
		error(KIO::ERR_CANNOT_DELETE_ORIGINAL, url.prettyUrl());

		return;
	}

	qint64 bytes;
	KIO::filesize_t totalBytes = 0;
	QByteArray buffer;

	while (true)
	{
		dataReq();
		bytes = readData(buffer);

		if (bytes <= 0)
			break;

		totalBytes += bytes;
		processedSize(totalBytes);
	}


	try
	{
		m_archive->addFile(path.toUtf8().constData(), buffer.data(), buffer.size(), (flags & KIO::Overwrite), locale, platform);
	}
	catch (Exception &exception)
	{
		error(KIO::ERR_COULD_NOT_WRITE, i18n("%1: \"%2\".", url.prettyUrl(), exception.what().c_str()));

		return;
	}

	m_modified = QFileInfo(m_archive->path().c_str()).lastModified();

	finished();
}

void MpqProtocol::createRootUDSEntry(KIO::UDSEntry &entry)
{
	entry.clear();
	entry.insert(KIO::UDSEntry::UDS_NAME, "." );
	entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR);
	entry.insert(KIO::UDSEntry::UDS_MODIFICATION_TIME, m_modified.toTime_t());
	//entry.insert( KIO::UDSEntry::UDS_ACCESS, 07777 ); // fake 'x' permissions, this is a pseudo-directory
}

void MpqProtocol::createUDSEntry(const KArchiveEntry *archiveEntry, KIO::UDSEntry &entry)
{
	entry.clear();
	entry.insert(KIO::UDSEntry::UDS_NAME, archiveEntry->name() );
	entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, archiveEntry->permissions() & S_IFMT); // keep file type only
	entry.insert(KIO::UDSEntry::UDS_SIZE, archiveEntry->isFile() ? ((KArchiveFile *)archiveEntry)->size() : 0L);
	entry.insert(KIO::UDSEntry::UDS_MODIFICATION_TIME, archiveEntry->date());
	entry.insert(KIO::UDSEntry::UDS_LINK_DEST, archiveEntry->symLinkTarget());
}

void MpqProtocol::createUDSEntry(const mpq::MpqFile &mpqFile, KIO::UDSEntry &entry)
{
	entry.clear();
	entry.insert(KIO::UDSEntry::UDS_NAME, mpqFile.path().string().c_str());
	entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, mpqFile.isFile()); // keep file type only
	entry.insert(KIO::UDSEntry::UDS_SIZE, mpqFile.size());

	if (mpqFile.mpq()->containsAttributesFile() && (mpqFile.mpq()->attributesFile()->extendedAttributes() & mpq::Attributes::ExtendedAttributes::FileTimeStamps))
	{
		time_t time;

		if (mpqFile.fileTime(time))
			entry.insert(KIO::UDSEntry::UDS_MODIFICATION_TIME, time);
	}

	//entry.insert(KIO::UDSEntry::UDS_MODIFICATION_TIME, mpqFile.mpq()->containsAttributesFile() && (mpqFile.mpq()->attributesFile()->extendedAttributes() & mpq::Attributes::ExtendedAttributes::FileTimeStamps) ? mpqFile.fi : );
	//entry.insert(KIO::UDSEntry::UDS_LINK_DEST, archiveEntry->symLinkTarget());
}

bool MpqProtocol::checkNewFile(const KUrl &url, QString &path, KIO::Error &errorNum, QIODevice::OpenMode openMode)
{
#ifndef Q_WS_WIN
	QString fullPath = url.path();
#else
	QString fullPath = url.path().remove(0, 1);
#endif
	kDebug(7109) << "ArchiveProtocol::checkNewFile" << fullPath;


	// Are we already looking at that file ?
	if (m_archive && m_archiveName == fullPath.left(m_archiveName.length()))
	{
		// Has it changed ?
		KDE_struct_stat statbuf;

		if (KDE_stat(QFile::encodeName(m_archiveName), &statbuf) == 0)
		{
			if (m_modified.toTime_t() == statbuf.st_mtime)
			{
				path = fullPath.mid(m_archiveName.length());
				kDebug(7109) << "ArchiveProtocol::checkNewFile returning" << path;
				return true;
			}
		}
	}

	kDebug(7109) << "Need to open a new file";

	// Close previous file
	if (m_archive)
	{
		m_archive->close();
		m_archive.reset();
	}

	// Find where the tar file is in the full path
	int pos = 0;
	QString archiveFile;
	path.clear();

	int len = fullPath.length();

	if (len != 0 && fullPath[ len - 1 ] != '/')
		fullPath += '/';

	kDebug(7109) << "the full path is" << fullPath;
	KDE_struct_stat statbuf;
	statbuf.st_mode = 0; // be sure to clear the directory bit

	while ((pos=fullPath.indexOf( '/', pos+1 )) != -1)
	{
		QString tryPath = fullPath.left( pos );
		kDebug(7109) << fullPath << "trying" << tryPath;

		if (KDE_stat(QFile::encodeName(tryPath), &statbuf ) == -1)
		{
			// We are not in the file system anymore, either we have already enough data or we will never get any useful data anymore
			break;
		}

		if (!S_ISDIR(statbuf.st_mode))
		{
			archiveFile = tryPath;
			m_modified = QDateTime::fromTime_t(statbuf.st_mtime);

			path = fullPath.mid( pos + 1 );
			kDebug(7109).nospace() << "fullPath=" << fullPath << " path=" << path;
			len = path.length();

			if ( len > 1 )
			{
				if ( path[ len - 1 ] == '/' )
				path.truncate( len - 1 );
			}
			else
				path = QString::fromLatin1("/");

			kDebug(7109).nospace() << "Found. archiveFile=" << archiveFile << " path=" << path;

			break;
		}
	}

	if (archiveFile.isEmpty())
	{
		kDebug(7109) << "ArchiveProtocol::checkNewFile: not found";

		if ( S_ISDIR(statbuf.st_mode) ) // Was the last stat about a directory?
		{
			// Too bad, it is a directory, not an archive.
			kDebug(7109) << "Path is a directory, not an archive.";
			errorNum = KIO::ERR_IS_DIRECTORY;
		}
		else
			errorNum = KIO::ERR_DOES_NOT_EXIST;

		return false;
	}

	// Open new file
	if ( url.protocol() == protocol ) {
		kDebug(7109) << "Opening MPQ on" << archiveFile;
		m_archive.reset(new mpq::Mpq());
	}
	/*else if ( url.protocol() == "ar" ) {
		kDebug(7109) << "Opening KAr on " << archiveFile;
		m_archiveFile = new KAr( archiveFile );
	} else if ( url.protocol() == "zip" ) {
		kDebug(7109) << "Opening KZip on " << archiveFile;
		m_archiveFile = new KZip( archiveFile );
	}*/ else {
		kWarning(7109) << "Protocol" << url.protocol() << "not supported by this IOSlave" ;
		errorNum = KIO::ERR_UNSUPPORTED_PROTOCOL;

		return false;
	}

	if (!m_archive->open(archiveFile.toUtf8().constData()))
	{
		kDebug(7109) << "Opening" << archiveFile << "failed.";
		m_archive.reset();

		if (openMode == QIODevice::ReadOnly)
			errorNum = KIO::ERR_CANNOT_OPEN_FOR_READING;
		else if (openMode == QIODevice::WriteOnly || openMode == QIODevice::ReadWrite)
			errorNum = KIO::ERR_CANNOT_OPEN_FOR_WRITING;
		else
			errorNum = KIO::ERR_UNKNOWN;


		return false;
	}

	m_archiveName = archiveFile;

	return true;
}

mpq::MpqFile* MpqProtocol::resolvePath(QString& path)
{
	BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale = mpq::MpqFile::Locale::Neutral;
	BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) platform = mpq::MpqFile::Platform::Default;
	path = MpqArchive::resolvePath(path, locale, platform);

	return this->m_archive->findFile(path.toUtf8().constData(), locale, platform);
}


}

}
