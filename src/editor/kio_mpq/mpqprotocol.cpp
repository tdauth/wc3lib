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

MpqProtocol::MpqProtocol(const QByteArray &pool, const QByteArray &app) : m_file(0), m_seekPos(0), KIO::SlaveBase(protocol, pool, app)
{
	kDebug(7000) << "Created MPQ slave";
}

bool MpqProtocol::parseUrl(const KUrl &url, QString &fileName, QByteArray &archivePath)
{
	kDebug(7000) << "MPQ: Parsing url " << url.prettyUrl();
	
	QString path = url.path();

	bool appended = false;

	if (path.at(path.size() - 1) != KDIR_SEPARATOR)
	{
		path.append(KDIR_SEPARATOR);
		appended = true;
	}

	int pos = 0;
	int nextPos = 0;

	while ((nextPos = path.indexOf(KDIR_SEPARATOR, pos + 1)) != -1)
	{

		if (!QFileInfo(QFile::encodeName(path.left(nextPos))).exists())
		{
			break;
		}

		pos = nextPos;
	}

	if (pos == 0)
	{
		return false;
	}

	if (appended)
	{
		path.chop(1);
	}

	fileName = QFile::encodeName(path.left(pos));

	if (!QFileInfo(fileName).isFile())
	{
		kDebug(7000) << "Is not a file";
		
		return false;
	}

	toArchivePath(archivePath, path.mid(pos+1, -1));
	
	kDebug(7000) << "Got it with archive path " << archivePath.constData() << "\nand file name " << fileName;

	return true;

}

void MpqProtocol::toArchivePath(QByteArray &to, const QString &from)
{
#if KDIR_SEPARATOR == '\\'
	to = from.toUtf8();
#else
	to = from.toUtf8().replace(KDIR_SEPARATOR, '\\');
#endif // KDIR_SEPARATOR == '\\'

}

bool MpqProtocol::openArchive(const QString &archive, QString &error)
{
	kDebug(7000) << "opening archive " << archive;
	
	// TEST
	if (!m_archive.isNull())
	{
		kDebug(7000) << "With old path " << this->m_archive->path().c_str();
	}
	// TEST END
	
	// if file paths are equal file must already be open, so don't open again
	if (this->m_archive.isNull() || this->m_archive->path().string() != archive.toUtf8().constData())
	{
		kDebug(7000) << "New opening!";
		
		MpqArchivePtr ptr(new mpq::Mpq());
		
		try
		{
			ptr->open(archive.toUtf8().constData());
		}
		catch (Exception &exception)
		{
			error = exception.what().c_str();
			
			kDebug(7000) << "Error: " << error;
			
			return false;
		}
		
		if (ptr->containsAttributesFile())
		{
			try
			{
				ptr->attributesFile()->readData(); // attributes data is required for file time stamps
			}
			catch (Exception &exception)
			{
				error = exception.what().c_str();
				
				kDebug(7000) << "Error: " << error;
				
				return false;
			}
		}

		m_archive.swap(ptr); // exception safety
	}
	
	return true;
}


void MpqProtocol::open(const KUrl &url, QIODevice::OpenMode mode)
{
	QString fileName;
	QByteArray archivePath;
	
	if (!parseUrl(url, fileName, archivePath))
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
		
		return;
	}
	
	QString errorText;
	
	if (!openArchive(fileName, errorText))
	{
		error(KIO::ERR_ABORTED, i18n("%1: \"%2\"", fileName, errorText));

		return;
	}
	
	mpq::MpqFile *file = m_archive->findFile(archivePath.constData()); // TODO locale and platform
	
	kDebug(7000) << "Opening: " << archivePath.constData();
	
	if (file == 0)
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
		
		return;
	}
	
	this->m_file = file;

	if (mode == QIODevice::ReadOnly || mode == QIODevice::ReadWrite)
	{
		if (QString::fromUtf8(archivePath).endsWith(".mpq", Qt::CaseInsensitive))
		{
			mimeType("application/x-mpq");
		}
		// detect MIME type by name and content, therefore read the whole file
		else
		{
			stringstream stream;
			
			try
			{
				std::streamsize size = this->m_file->writeData(stream);
				QByteArray data = stream.str().c_str();
				KMimeType::Ptr fileMimeType = KMimeType::findByNameAndContent(url.fileName(), data);
				mimeType(fileMimeType->name());
				
			}
			catch (Exception &e)
			{
				kDebug(7000) << "Unable to read file and detect MIME type: " << url.prettyUrl() << ": " << e.what().c_str();
			}
		}
	}

	
	kDebug(7000) << "Successfully open";
	KIO::filesize_t size = 0;
	
	try
	{
		size = boost::numeric_cast<KIO::filesize_t>(this->m_file->compressedSize());
	}
	catch (Exception &e)
	{
		error(KIO::ERR_ABORTED, i18n("%1: %2", url.prettyUrl(), e.what().c_str()));
		
		return;
	}
	
	totalSize(size);
	position(0);
	opened();
}

void MpqProtocol::close()
{
	this->m_file = 0;
	this->m_seekPos = 0;
}

void MpqProtocol::read(KIO::filesize_t size)
{
	stringstream sstream;
	
	try // TODO faster way without writing ALL data
	{
		m_file->writeData(sstream); // write data first to decompress everything
	}
	catch (Exception &e)
	{
		error(KIO::ERR_COULD_NOT_READ, i18n("%1: %2", m_file->path().c_str(), e.what().c_str()));
		
		return;
	}
	
	sstream.seekg(m_seekPos);
	byte content[size];
	sstream.read(content, size);
	
	data(QByteArray::fromRawData(content, size));
}

void MpqProtocol::seek(KIO::filesize_t offset)
{
	this->m_seekPos += offset;
}

void MpqProtocol::mkdir(const KUrl& url, int permissions)
{
	// MPQ archives does not support directory structure
	// Only simulate creating directory
	
	finished();
}

void MpqProtocol::listDir(const KUrl &url)
{
	kDebug(7000) << "MpqProtocol::listDir" << url.url();

	QString fileName;
	QByteArray archivePath;
	
	if (!parseUrl(url, fileName, archivePath))
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
		
		return;
	}
	
	if (!archivePath.isEmpty() && archivePath.at(archivePath.size() - 1) != '\\')
	{
		archivePath.append('\\'); // interpret as directory
	}

	QString errorText;
	
	if (!openArchive(fileName, errorText))
	{
		// use slave defined for custom text other than error code!
		error(KIO::ERR_SLAVE_DEFINED, i18n("%1: \"%2\"", url.prettyUrl(), errorText));

		return;
	}
	
	// TODO get all files contained in directory using (listfile)
	mpq::Listfile *listfile = m_archive->listfileFile();
	
	if (listfile == 0)
	{
		// use slave defined for custom text other than error code!
		error(KIO::ERR_SLAVE_DEFINED, i18n("%1: Missing (listfile).", url.prettyUrl()));

		return;
	}
	
	mpq::Listfile::Entries entries = listfile->dirEntries(archivePath.constData(), false);
	const bool hasAttributes = m_archive->containsAttributesFile();
	bool hasFileTime = false;
	
	if (!hasAttributes)
	{
		warning(i18n("%1: Missing \"(attributes)\" file.", url.prettyUrl()));
	}
	else
	{
		hasFileTime = m_archive->attributesFile()->extendedAttributes() & mpq::Attributes::ExtendedAttributes::FileTimeStamps;
		
		if (!hasFileTime)
		{
			warning(i18n("%1: Missing \"(attributes)\" file time stamps.", url.prettyUrl()));
		}
	}
	
	if (archivePath.isEmpty()) // root directory
	{
		// list files which are not listed in "(listfile)"
		
		if (m_archive->containsListfileFile()) // should always be the case
		{
			entries.push_back(m_archive->listfileFile()->fileName());
		}
		
		if (m_archive->containsAttributesFile())
		{
			entries.push_back(m_archive->attributesFile()->fileName());
		}
		
		if (m_archive->containsSignatureFile())
		{
			entries.push_back(m_archive->signatureFile()->fileName());
		}
	}
	
	
	BOOST_FOREACH (mpq::Listfile::Entries::reference ref, entries)
	{
		boost::iterator_range<string::iterator> r = boost::find_last(ref, "\\");
		
		kDebug(7000) << "Entry \"" << ref.c_str() << "\"";
		
		if (!r.empty() && r.begin() == --ref.end()) // is directory, ends with back slash
		{
			kDebug(7000) << "Is dir";
			
			ref.erase(ref.size() - 1);
			r = boost::find_last(ref, "\\");
			
			if (!r.empty()) // cut full path
			{
				ref.erase(ref.begin(), r.end());
			}
			
			kDebug(7000) << "New path \"" << ref.c_str() << "\"";
			
			KIO::UDSEntry entry;
			entry.insert(KIO::UDSEntry::UDS_NAME, QFile::decodeName(ref.c_str()));
			entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR);
			entry.insert(KIO::UDSEntry::UDS_ACCESS, (S_IRWXU | S_IRWXG | S_IRWXO));
			listEntry(entry, false);
		}
		else
		{
			kDebug(7000) << "Is file";
			QString fileName;
			
			if (!r.empty()) // cut full path
			{
				fileName = QFile::decodeName(string(++r.begin(), ref.end()).c_str());
			}
			else
			{
				fileName = QFile::decodeName(ref.c_str());
			}
			
			kDebug(7000) << "New path \"" << fileName << "\"";
			
			mpq::MpqFile *file = m_archive->findFile(ref); // TODO locale and platform
			
			if (file != 0)
			{
				//quint64 fileTime = 0;
				time_t fileTime = 0;
				bool validFileTime = false;
				
				// file time is extended attribute!
				if (hasAttributes && hasFileTime)
				{
					try
					{
						const bool cast = file->fileTime().toTime(fileTime);
						
						if (!cast)
						{
							warning(i18n("%1: Invalid file time for \"%2\": high - %3, low - %4", url.prettyUrl(), ref.c_str(), file->fileTime().highDateTime, file->fileTime().lowDateTime));
						}
						else
						{
							validFileTime = true;
						}
					}
					catch (mpq::Attributes::Exception &e)
					{
						warning(i18n("%1: File time is not stored in \"(attributes)\" file: %2", url.prettyUrl(), e.what().c_str()));
					}
				}
				
				KIO::UDSEntry entry;
				entry.insert(KIO::UDSEntry::UDS_NAME, fileName);
				entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFREG);
				entry.insert(KIO::UDSEntry::UDS_SIZE, file->compressedSize());
				
				
				if (validFileTime)
				{
					entry.insert(KIO::UDSEntry::UDS_MODIFICATION_TIME, fileTime);
				}
				
				entry.insert(KIO::UDSEntry::UDS_ACCESS, (S_IRWXU | S_IRWXG | S_IRWXO));
				
				KMimeType::Ptr ptr = KMimeType::findByPath(QFile::decodeName(ref.c_str())); // TODO find by content?

				if (!ptr.isNull())
				{
					entry.insert(KIO::UDSEntry::UDS_MIME_TYPE, ptr->name());
				}
				
				listEntry(entry, false);
			}
			else // invalid entry
			{
				warning(i18n("%1: Invalid directory entry \"%2\"", url.prettyUrl(), ref.c_str()));
			}
		}
	}
	
	listEntry(KIO::UDSEntry(), true); // ready
	finished();
}

void MpqProtocol::stat(const KUrl &url)
{
	kDebug(7000) << "MpqProtocol::stat" << url.url();

	QString fileName;
	QByteArray archivePath;
	
	if (!parseUrl(url, fileName, archivePath))
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
		
		return;
	}
	
	if (!archivePath.isEmpty() && archivePath.at(archivePath.size() - 1) != '\\')
	{
		archivePath.append('\\'); // interpret as directory
	}
	
	QString errorText;
	
	if (!openArchive(fileName, errorText))
	{
		error(KIO::ERR_ABORTED, i18n("%1: \"%2\"", url.prettyUrl(), errorText));

		return;
	}
	
	const bool hasAttributes = m_archive->containsAttributesFile();
	bool hasFileTime = false;
	
	if (!hasAttributes)
	{
		warning(i18n("%1: Missing \"(attributes)\" file.", url.prettyUrl()));
	}
	else
	{
		hasFileTime = m_archive->attributesFile()->extendedAttributes() & mpq::Attributes::ExtendedAttributes::FileTimeStamps;
		
		if (!hasFileTime)
		{
			warning(i18n("%1: Missing \"(attributes)\" file time stamps.", url.prettyUrl()));
		}
	}
	
	mpq::MpqFile *file = m_archive->findFile(archivePath.constData()); // TODO locale and platform
			
	if (file != 0) // TODO includes dirs?
	{
		//quint64 fileTime = 0;
		time_t fileTime = 0;
		bool validFileTime = false;
		
		// file time is extended attribute!
		if (hasAttributes && hasFileTime)
		{
			try
			{
				const bool cast = file->fileTime().toTime(fileTime);
				
				if (!cast)
				{
					warning(i18n("%1: Invalid file time: high - %2, low - %3", url.prettyUrl(), file->fileTime().highDateTime, file->fileTime().lowDateTime));
				}
				else
				{
					validFileTime = true;
				}
			}
			catch (mpq::Attributes::Exception &e)
			{
				warning(i18n("%1: File time is not stored in \"(attributes)\" file: %2", url.prettyUrl(), e.what().c_str()));
			}
		}
		
		KIO::UDSEntry entry;
		
		entry.insert(KIO::UDSEntry::UDS_NAME, url.path());
		entry.insert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFREG);
		entry.insert(KIO::UDSEntry::UDS_SIZE, file->compressedSize());
		
		if (validFileTime)
		{
			entry.insert(KIO::UDSEntry::UDS_MODIFICATION_TIME, fileTime);
		}
		
		entry.insert(KIO::UDSEntry::UDS_ACCESS, (S_IRWXU | S_IRWXG | S_IRWXO));
		
		KMimeType::Ptr ptr = KMimeType::findByPath(url.path()); // TODO find by content?

		if (!ptr.isNull())
		{
			entry.insert(KIO::UDSEntry::UDS_MIME_TYPE, ptr->name());
		}
		
		statEntry(entry);
	}
	else // invalid entry or dir?
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
		
		return;
	}
	
	finished();
}

void MpqProtocol::get(const KUrl &url)
{
	QString fileName;
	QByteArray archivePath;
	
	kDebug(7000) << "Getting file " << url.prettyUrl();
	
	if (!parseUrl(url, fileName, archivePath))
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
		
		return;
	}
	
	QString errorText;
	
	if (!openArchive(fileName, errorText))
	{
		error(KIO::ERR_ABORTED, i18n("%1: \"%2\"", url.prettyUrl(), errorText));

		return;
	}

	mpq::MpqFile *file = m_archive->findFile(archivePath.constData()); // TODO locale and platform

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
	uint32 sectorIndex = 0;
	KIO::filesize_t processed = 0;

	// open for better streaming
	ifstream ifstream(m_archive->path(), std::ios::binary | std::ios::in);

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
		arraystream ostream(buffer.data(), buffer.size()); // TODO check if unbuffered stream works
		qint64 read = 0;

		try
		{
			file->sectors()[sectorIndex].seekg(ifstream);
			read = file->sectors()[sectorIndex].writeData(ifstream, ostream);
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
			KMimeType::Ptr mime = KMimeType::findByNameAndContent(archivePath, buffer);
			kDebug(7000) << "Emitting mimetype" << mime->name();
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
	
	QString fileName;
	QByteArray archivePath;
	
	if (!parseUrl(url, fileName, archivePath))
	{
		error(KIO::ERR_DOES_NOT_EXIST, url.prettyUrl());
		
		return;
	}
	
	QString errorText;
	
	if (!openArchive(fileName, errorText))
	{
		error(KIO::ERR_ABORTED, i18n("%1: \"%2\"", fileName, errorText));

		return;
	}

	BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale = mpq::MpqFile::Locale::Neutral; // TODO get from URL
	BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) platform = mpq::MpqFile::Platform::Default;
	
	// TODO read URL into temporary local file and add file to MPQ archive!
	
	warning(i18n("Not supported yet!"));
	
	finished();

	/*
	if (archivePath == "(listfile)" || archivePath == "(signature)" || archivePath == "(attributes)" || archivePath.contains("(patch_metadata)"))
	{
		error(KIO::ERR_WRITE_ACCESS_DENIED, url.prettyUrl());

		return;
	}
	
	Q

	const mpq::MpqFile *file = this->m_archive->findFile(archivePath.constData(), locale, platform);

	if (file != 0 && !(flags & KIO::Overwrite))
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
		m_archive->addFile(archivePath.constData(), buffer.data(), buffer.size(), (flags & KIO::Overwrite), locale, platform);
	}
	catch (Exception &exception)
	{
		error(KIO::ERR_COULD_NOT_WRITE, i18n("%1: \"%2\".", url.prettyUrl(), exception.what().c_str()));

		return;
	}

	m_modified = QFileInfo(m_archive->path().c_str()).lastModified();

	finished();
	*/
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
