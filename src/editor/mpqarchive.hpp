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

#ifndef WC3LIB_EDITOR_MPQARCHIVE_HPP
#define WC3LIB_EDITOR_MPQARCHIVE_HPP

#include <KArchive>

#include "../mpq.hpp"
#include "../core.hpp"

namespace wc3lib
{

namespace editor
{

/// \todo Add extended attributes support (time etc.).
class MpqArchive : public KArchive
{
	public:
		typedef boost::scoped_ptr<mpq::Mpq> MpqPtr;
		typedef boost::shared_ptr<mpq::MpqFile> MpqFilePtr;
		
		virtual ~MpqArchive();
		
		virtual bool writeData(const char *data, qint64 size);
		virtual bool writeFile(const QString &name, const QString &user, const QString &group, const char *data, qint64 size, mode_t perm=0100644, time_t atime=UnknownTime, time_t mtime=UnknownTime, time_t ctime=UnknownTime);
		
		const MpqPtr& mpq() const;
		const MpqFilePtr& mpqFile() const;
		
	protected:
		friend class MpqProtocol;
		
		MpqArchive(const QString &fileName);
		MpqArchive(QIODevice *dev);
		virtual bool closeArchive();
		virtual bool doFinishWriting (qint64 size);
		virtual bool doPrepareWriting (const QString &name, const QString &user, const QString &group, qint64 size, mode_t perm, time_t atime, time_t mtime, time_t ctime);
		virtual bool doWriteDir(const QString &name, const QString &user, const QString &group, mode_t perm, time_t atime, time_t mtime, time_t ctime);
		virtual bool doWriteSymLink (const QString &name, const QString &target, const QString &user, const QString &group, mode_t perm, time_t atime, time_t mtime, time_t ctime);
		virtual bool openArchive(QIODevice::OpenMode mode);
		virtual void virtual_hook(int id, void *data);
		
		MpqPtr m_mpq;
		MpqFilePtr m_mpqFile;
};

inline const MpqArchive::MpqPtr& MpqArchive::mpq() const
{
	return m_mpq;
}

inline const MpqArchive::MpqFilePtr& MpqArchive::mpqFile() const
{
	return m_mpqFile;
}

}

}

#endif
