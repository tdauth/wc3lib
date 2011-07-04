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

#include "mpqarchive.hpp"

namespace wc3lib
{

namespace editor
{

MpqArchive::~MpqArchive()
{
}

bool MpqArchive::writeData(const char *data, qint64 size)
{
	if (m_mpqFile.get() == 0 || m_mpqFile->size() < size)
		return false;
	
	try
	{
		mpq::stringstream sstream;
		sstream.rdbuf()->sputn(data, size);
		m_mpqFile->readData(sstream);
	}
	catch (Exception &exception)
	{
		return false;
	}
	
	return true;
}

bool MpqArchive::writeFile(const QString &name, const QString &user, const QString &group, const char *data, qint64 size, mode_t perm, time_t atime, time_t mtime, time_t ctime)
{
	mpq::stringstream sstream;
	sstream.rdbuf()->sputn(data, size);
	mpq::MpqFile *file = m_mpq->addFile(name.toUtf8().constData(), mpq::MpqFile::Locale::Neutral, mpq::MpqFile::Platform::Default, &sstream);
	
	if (file == 0)
		return false;
	
	return true;
}

MpqArchive::MpqArchive(const QString &fileName) : m_mpq(new mpq::Mpq()), KArchive(fileName)
{
}

MpqArchive::MpqArchive(QIODevice *dev) : m_mpq(new mpq::Mpq()), KArchive(dev)
{
}

bool MpqArchive::closeArchive()
{
	this->m_mpq->close();
}

bool MpqArchive::doFinishWriting(qint64 size)
{
	return true;
}

bool MpqArchive::doPrepareWriting(const QString &name, const QString &user, const QString &group, qint64 size, mode_t perm, time_t atime, time_t mtime, time_t ctime)
{
	mpq::MpqFile *file = this->m_mpq->findFile(name.toUtf8().constData());
	
	if (file == 0 || file->size() < size)
		return false;
	
	m_mpqFile.reset(file);
	
	return true;
}

bool MpqArchive::doWriteDir(const QString &name, const QString &user, const QString &group, mode_t perm, time_t atime, time_t mtime, time_t ctime)
{
	return false;
}

bool MpqArchive::doWriteSymLink(const QString &name, const QString &target, const QString &user, const QString &group, mode_t perm, time_t atime, time_t mtime, time_t ctime)
{
	mpq::stringstream sstream;
	sstream.write(target.toUtf8().constData(), target.toUtf8().size());
	mpq::MpqFile *file = m_mpq->addFile(name.toUtf8().constData(), mpq::MpqFile::Locale::Neutral, mpq::MpqFile::Platform::Default, &sstream);
	
	if (file == 0)
		return false;
	
	return true;
}

bool MpqArchive::openArchive(QIODevice::OpenMode mode)
{
	this->m_mpq->open(fileName().toUtf8().constData());
}

void MpqArchive::virtual_hook(int id, void* data)
{
	KArchive::virtual_hook(id, data);
}

}

}
