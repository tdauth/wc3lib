/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include <QtCore>

#include "mpqarchive.hpp"

namespace wc3lib
{

namespace editor
{

const KArchiveEntry* MpqArchiveRootDirectory::entry(const QString &name) const
{
	/*
	QString realName = QDir::cleanPath(name);
	int pos = realName.indexOf( QLatin1Char('/') );

	if ( pos == 0 ) // ouch absolute path (see also KArchive::findOrCreate)
	{
		if (realName.length()>1)
		{
			realName = realName.mid( 1 ); // remove leading slash
			pos = realName.indexOf( QLatin1Char('/') ); // look again
		}
		else // "/"
			return this;
	}



	// trailing slash ? -> remove
	if ( pos != -1 && pos == realName.length()-1 )
	{
		realName = realName.left( pos );
		pos = realName.indexOf( QLatin1Char('/') ); // look again
	}

	if ( pos != -1 )
	{
		const QString left = realName.left(pos);
		const QString right = realName.mid(pos + 1);

		//kDebug() << "left=" << left << "right=" << right;

		const KArchiveEntry* e = d->entries.value( left );

		if ( !e || !e->isDirectory() )
			return 0;

		return static_cast<const KArchiveDirectory*>(e)->entry( right );
	}

	return d->entries.value( realName );
	*/
}


MpqArchive::~MpqArchive()
{
}

bool MpqArchive::writeData(const char *data, qint64 size)
{
	if (m_mpqFile == 0 || m_mpqFile->size() < size)
		return false;

	try
	{
		iarraystream stream(data, size);
		m_mpqFile->readData(stream);
	}
	catch (Exception &exception)
	{
		return false;
	}

	return true;
}

bool MpqArchive::writeFile(const QString &name, const QString &user, const QString &group, const char *data, qint64 size, mode_t perm, time_t atime, time_t mtime, time_t ctime)
{
	iarraystream stream(data, size);
	BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale;
	BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) platform;
	QString path(resolvePath(name, locale, platform));
	// TODO Change!
	//mpq::MpqFile *file = m_mpq->addFile(path.toUtf8().constData(), locale, platform, &stream);

	//if (file == 0)
		//return false;

	return true;
}

MpqArchive::MpqArchive(const QString &fileName) : m_mpq(new mpq::Mpq()), m_mpqFile(0), KArchive(fileName)
{
}

MpqArchive::MpqArchive(QIODevice *dev) : m_mpq(new mpq::Mpq()), m_mpqFile(0), KArchive(dev)
{
}

bool MpqArchive::closeArchive()
{
	try
	{
		this->m_mpq->close();
	}
	catch (const Exception &e)
	{
		return false;
	}

	return true;
}

KArchiveDirectory* MpqArchive::rootDir()
{
	return KArchive::rootDir();
	/*
	if (!d->rootDir)
	{
		d->rootDir = new MpqArchiveRootDirectory(this, QLatin1String("/"), (int)(0777 + S_IFDIR), 0, "", "", QString());
	}

	return d->rootDir;
	*/
}

bool MpqArchive::doFinishWriting(qint64 size)
{
	return true;
}

bool MpqArchive::doPrepareWriting(const QString &name, const QString &user, const QString &group, qint64 size, mode_t perm, time_t atime, time_t mtime, time_t ctime)
{
	BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale;
	BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) platform;
	QString path(resolvePath(name, locale, platform));
	mpq::MpqFile *file = this->m_mpq->findFile(path.toUtf8().constData(), locale, platform);

	if (file == 0 || file->size() < size)
		return false;

	m_mpqFile = file;

	return true;
}

bool MpqArchive::doWriteDir(const QString &name, const QString &user, const QString &group, mode_t perm, time_t atime, time_t mtime, time_t ctime)
{
	return false;
}

bool MpqArchive::doWriteSymLink(const QString &name, const QString &target, const QString &user, const QString &group, mode_t perm, time_t atime, time_t mtime, time_t ctime)
{
	iarraystream stream(target.toUtf8().constData(), target.toUtf8().size());
	BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale;
	BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) platform;
	QString path(resolvePath(name, locale, platform));
	// TODO FIXME
	//mpq::MpqFile *file = m_mpq->addFile(path.toUtf8().constData(), locale, platform, &stream);

	//if (file == 0)
		//return false;

	return true;
}

bool MpqArchive::openArchive(QIODevice::OpenMode mode)
{
	try
	{
		this->m_mpq->open(fileName().toUtf8().constData());
	}
	catch (Exception &exception)
	{
		return false;
	}

	return true;
}

void MpqArchive::virtual_hook(int id, void* data)
{
	KArchive::virtual_hook(id, data);
}

}

}
