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

#include <QLocale>
#include <QDir>

#include <KArchive>

#include "../../mpq.hpp"
#include "../../core.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Implements custom URL resolution to support multiple platforms and locales.
 * \todo Implement!!!
 */
class MpqArchiveRootDirectory : public KArchiveDirectory
{
	public:
		virtual const KArchiveEntry* entry(const QString& name) const;
};

/**
 * Some file attributes can be specified via URL:
 * <ul>
 * <li>locale</li>
 * <li>platform</li>
 * </ul>
 * So the usual file URL has the following format: "<archive URL>/<locale id>/<platform id>/<file path>".
 * If platform or locale id is not specified via URL default values will be used (\ref mpq::MpqFile::Platform::Default and \ref mpq::MpqFile::Locale::Neutral).
 * Use static functions \ref MpqArchive::locale() and \ref MpqArchive::platform() to get corresponding string ids.
 * Function \ref MpqArchive::resolvePath() can defined locale and platform ids and additionally remove them from path string by returning the resolved path.
 * \todo File attributes via URL should be supported (see above).
 * \todo Add extended attributes support (time etc.).
 */
class MpqArchive : public KArchive
{
	public:
		typedef boost::scoped_ptr<mpq::Mpq> MpqPtr;
		typedef boost::shared_ptr<mpq::MpqFile> MpqFilePtr;

		static QString locale(const BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) &locale) throw (Exception);
		static BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) locale(const QString &locale) throw (Exception);
		static QString platform(const BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) &platform) throw (Exception);
		static BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) platform(const QString &platform) throw (Exception);
		/**
		 * If you have a path like "/de/def/mydir/test.txt" it will result in "mydir/test.txt" and set \p locale to \ref mpq::MpqFile::Locale::German and \p platform to \ref mpq::MpqFile::Platform::Default.
		 * Initial / root character is always removed since archives expect relative paths.
		 * \note Order of locale and platform directory ids doesn't matter!
		 * \note If there is no locale or platform directory id specified they'll get default values (\ref mpq::MpqFile::Platform::Default and \ref mpq::MpqFile::Locale::Neutral).
		 */
		static QString resolvePath(const QString &path, BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) &locale, BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) &platform);

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
		/**
		 * Overwritten to create directory of type \ref MpqArchiveRootDirectory.
		 * \todo We cannot overwrite this behaviour until member \ref d is private.
		 */
		virtual KArchiveDirectory* rootDir();
		virtual bool doFinishWriting (qint64 size);
		virtual bool doPrepareWriting (const QString &name, const QString &user, const QString &group, qint64 size, mode_t perm, time_t atime, time_t mtime, time_t ctime);
		virtual bool doWriteDir(const QString &name, const QString &user, const QString &group, mode_t perm, time_t atime, time_t mtime, time_t ctime);
		virtual bool doWriteSymLink (const QString &name, const QString &target, const QString &user, const QString &group, mode_t perm, time_t atime, time_t mtime, time_t ctime);
		virtual bool openArchive(QIODevice::OpenMode mode);
		virtual void virtual_hook(int id, void *data);

		MpqPtr m_mpq;
		MpqFilePtr m_mpqFile;
};

inline QString MpqArchive::locale(const BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) &locale) throw (Exception)
{
	switch (locale)
	{
		case mpq::MpqFile::Locale::Neutral:
			return "ne";

		// all supported languages for MPQ archives
		case mpq::MpqFile::Locale::Chinese:
		case mpq::MpqFile::Locale::Czech:
		case mpq::MpqFile::Locale::Dutch:
		case mpq::MpqFile::Locale::English:
		case mpq::MpqFile::Locale::EnglishUK:
		case mpq::MpqFile::Locale::French:
		case mpq::MpqFile::Locale::German:
		case mpq::MpqFile::Locale::Italian:
		case mpq::MpqFile::Locale::Japanese:
		case mpq::MpqFile::Locale::Korean:
		case mpq::MpqFile::Locale::Polish:
		case mpq::MpqFile::Locale::Portuguese:
		case mpq::MpqFile::Locale::Russsian:
		case mpq::MpqFile::Locale::Spanish:
			return QLocale::languageToString(QLocale::Language(locale));
	}

	throw Exception();
}

inline BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) MpqArchive::locale(const QString &locale) throw (Exception)
{
	if (locale == "ne")
		return mpq::MpqFile::Locale::Neutral;

	throw Exception();
}

inline QString MpqArchive::platform(const BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) &platform) throw (Exception)
{
	switch (platform)
	{
		case mpq::MpqFile::Platform::Default:
			return "def";
	}

	throw Exception();
}

inline BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) MpqArchive::platform(const QString &platform) throw (Exception)
{
	if (platform == "def")
		return mpq::MpqFile::Platform::Default;

	throw Exception();
}

inline QString MpqArchive::resolvePath(const QString &path, BOOST_SCOPED_ENUM(mpq::MpqFile::Locale) &locale, BOOST_SCOPED_ENUM(mpq::MpqFile::Platform) &platform)
{
	QString result(QDir::cleanPath(path));
	int pos = result.indexOf(QLatin1Char('/'));
	// initial default values
	locale = mpq::MpqFile::Locale::Neutral;
	platform = mpq::MpqFile::Platform::Default;
	bool localeDefined = false;
	bool platformDefined = false;

	if (pos == 0) // absolute path
	{
		if (result.length() > 1)
		{
			result = result.mid(1); // remove leading slash
			pos = result.indexOf(QLatin1Char('/')); // look again
		}
		else // "/"
			return result;
	}

	short i = 0;

	// check first two directories
	while (pos != -1 && i < 2)
	{
		QString idString = result.left(pos);
		bool set = false;

		if (!localeDefined)
		{
			try
			{
				locale = MpqArchive::locale(idString);
				localeDefined = true;
				set = true;
			}
			catch (Exception &exception)
			{
			}
		}

		if (!platformDefined && !set)
		{
			try
			{
				platform = MpqArchive::platform(idString);
				platformDefined = true;
				set = true;
			}
			catch (Exception &exception)
			{
			}
		}

		// no valid entry was found
		if (!set)
			break;

		result = result.mid(pos + 1); // remove valid locale or platform entry
		pos = result.indexOf(QLatin1Char('/'));
		++i;
	}

	return result;
}

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
