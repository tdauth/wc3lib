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

#ifndef WC3LIB_EDITOR_METADATA_HPP
#define WC3LIB_EDITOR_METADATA_HPP

#include <unordered_map>

#include <boost/variant.hpp>

#include <QHash>

#include <kdemacros.h>

#include "resource.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \brief Base class for all possible meta data formats (units, abilities, upgrades etc.).
 *
 * This class can either store a SYLK file or a .txt file and provides a unified interface
 * to retrieve any value by its keys.
 *
 * Retrieving values by keys is much faster than searching for them with linear complexity.
 *
 * SLK or .txt files in Warcraft III usually provide a row and column item or a section and entry name (for .txt files)
 * from which the required value can be identified.
 * This class allows fast access to both kind of values through hashing.
 * Use \ref value() to get a value by both keys.
 *
 * With \ref hasValue() it can be checked if a value does actually exist.
 *
 * Uses \ref map::Slk to load a SYLK file which contains all meta data.
 * In addition to \ref map::Slk it stores columns and rows by keys (their first cell values) through hashing.
 * This allows fast access of columns or rows by keys which is usually required for meta data.
 *
 * Uses \ref map::Txt to load a .txt file which contain additional object data such as strings.
 *
 * Both kind of files use the UTF-8 encoding by default.
 *
 * You can use \ref hasSlk() or \ref hasTxt() to check which kind of file is actually behind the interface.
 *
 * \note Theoretically it is possible to store several values under the same keys using multiple equally named rows, columns, sections or entries but this class assumes that each identifier is picked uniquely. An exception might be entries in sections since there can be at least one entry per section so if multiple sections have the same entry it still would be unique in that section which is enough to use it as a hash value.
 *
 * \todo Remove unnecessary wrapper methods which are duplicated from \ref map::Slk.
 */
class KDE_EXPORT MetaData : public Resource
{
	public:
		/**
		 * A source can be a SLK file or a TXT file.
		 */
		typedef boost::variant<map::Slk, map::Txt> Source;
		/**
		 * Values of the first cells are used to get the corresponding number of row or column.
		 */
		typedef QHash<QString, map::Slk::Table::size_type> SlkKeys;

		/**
		 * Use the section names as keys for the sections.
		 */
		typedef QHash<QString, map::Txt::Section*> TxtSectionKeys;

		/**
		 * The key to one TXT entry is its section and its value key.
		 */
		typedef QPair<map::Txt::Section*, QString> TxtEntryKey;
		/**
		 * Use the key of the key value pair as key for the entry.
		 */
		typedef QHash<TxtEntryKey, map::Txt::Entry*> TxtEntryKeys;

		MetaData(const KUrl &url);

		virtual void clear() throw ();

		virtual void load() throw (Exception);
		virtual void reload() throw (Exception);
		virtual void save(const KUrl &url) const throw (Exception)
		{
			throw Exception(_("Saving meta data is not supported yet."));
		}

		/**
		 * \return Returns true if there is currently an SLK file stored.
		 *
		 * \sa hasTxt()
		 */
		bool hasSlk() const;
		/**
		 * \return Returns true if there is currently a TXT file stored.
		 *
		 * \sa hasSlk()
		 */
		bool hasTxt() const;

		map::Slk& slk();
		const map::Slk& slk() const;
		map::Txt& txt();
		const map::Txt& txt() const;
		const SlkKeys& columnKeys() const;
		const SlkKeys& rowKeys() const;
		const TxtSectionKeys& sectionKeys() const;
		const TxtEntryKeys& entryKeys() const;
		map::Slk::ConstView row(const QString &key) const;
		map::Slk::ConstView column(const QString &key) const;

		/**
		 * Can only be used for a stored TXT and returns the corresponding section of \p key
		 * which is the section's name.
		 */
		map::Txt::Section* section(const QString &key) const;
		map::Txt::Entry* entry(const QString &sectionKey, const QString &key) const;

		QString value(int row, const QString &columnKey) const;
		QString value(const QString &rowKey, int column) const;
		/**
		 * Searches for value \p rowKey in the first column and for value \p columnKey in the
		 * first row.
		 * If both values could be found it returns the corresponding cell value.
		 *
		 * This allows you fast retrieving of values by two keys which is usually
		 * what you need to do to get SLK or TXT data for Warcraft III.
		 *
		 * \note For TXT files the \p rowKey is the section name and the \p columnKey is the entry name of an entry in that section.
		 *
		 * \param rowKey The key of the row or section. Quotes will be added automatically for SLK files.
		 * \param columnKey The key of the column or the entry. Quotes will be added automatically for SLK files.
		 *
		 * \return Returns the keys matching value. If no value is found it returns an empty string. For SLK and TXT files quotes are removed automatically.
		 *
		 * \note It removes starting and ending quotes automatically.
		 *
		 * \sa hasValue()
		 */
		QString value(const QString &rowKey, const QString &columnKey) const;

		/**
		 * \sa value()
		 */
		bool hasValue(const QString &rowKey, const QString &columnKey) const;

		/**
		 * \return Returns true if the underlying file entries are empty.
		 */
		bool isEmpty() const;

		/**
		 * Cuts " characters at start and end if available and returns resulting string.
		 */
		static QString fromSlkString(const QString &value);
		static QString toSlkString(const QString &value);

		static QString cutQuotes(const QString &value);

		/**
		 * Converts \p value to a native file path.
		 */
		static QString fromFilePath(const QString &value);

	protected:
		Source m_source;
		SlkKeys m_columnKeys;
		SlkKeys m_rowKeys;
		TxtSectionKeys m_sectionKeys;
		TxtEntryKeys m_entryKeys;
};

inline bool MetaData::hasSlk() const
{
	return this->m_source.which() == 0;
}

inline bool MetaData::hasTxt() const
{
	return this->m_source.which() == 1;
}

inline map::Slk& MetaData::slk()
{
	return boost::get<map::Slk&>(this->m_source);
}

inline const map::Slk& MetaData::slk() const
{
	return boost::get<const map::Slk&>(this->m_source);
}

inline map::Txt& MetaData::txt()
{
	return boost::get<map::Txt&>(this->m_source);
}

inline const map::Txt& MetaData::txt() const
{
	return boost::get<const map::Txt&>(this->m_source);
}

inline const MetaData::SlkKeys& MetaData::columnKeys() const
{
	return this->m_columnKeys;
}

inline const MetaData::SlkKeys& MetaData::rowKeys() const
{
	return this->m_rowKeys;
}

inline const MetaData::TxtSectionKeys& MetaData::sectionKeys() const
{
	return this->m_sectionKeys;
}

inline const MetaData::TxtEntryKeys& MetaData::entryKeys() const
{
	return this->m_entryKeys;
}

inline map::Slk::ConstView MetaData::row(const QString &key) const
{
	if (!hasSlk())
	{
		throw Exception();
	}

	SlkKeys::const_iterator iterator = this->rowKeys().find(toSlkString(key));

	if (iterator == this->rowKeys().end())
	{
		throw Exception(boost::format(_("Missing row %1%.")) % toSlkString(key).toUtf8().constData());
	}

	return this->slk().row(iterator.value());
}

inline map::Slk::ConstView MetaData::column(const QString &key) const
{
	if (!hasSlk())
	{
		throw Exception();
	}

	SlkKeys::const_iterator iterator = this->columnKeys().find(toSlkString(key));

	if (iterator == this->columnKeys().end())
	{
		throw Exception(boost::format(_("Missing column %1%.")) % toSlkString(key).toUtf8().constData());
	}

	return this->slk().column(iterator.value());
}

inline map::Txt::Section* MetaData::section(const QString &key) const
{
	TxtSectionKeys::const_iterator iterator = this->sectionKeys().find(key);

	if (iterator == this->sectionKeys().end())
	{
		return 0;
	}

	return iterator.value();
}

inline map::Txt::Entry* MetaData::entry(const QString &sectionKey, const QString& key) const
{
	map::Txt::Section *section = this->section(sectionKey);

	if (section == 0)
	{
		return 0;
	}

	TxtEntryKeys::const_iterator iterator = this->entryKeys().find(TxtEntryKey(section, key));

	if (iterator == this->entryKeys().end())
	{
		return 0;
	}

	return iterator.value();
}

inline QString MetaData::value(int row, const QString &columnKey) const
{
	if (!hasSlk())
	{
		throw Exception();
	}

	SlkKeys::const_iterator iterator = this->columnKeys().find(toSlkString(columnKey));

	if (iterator == this->columnKeys().end())
	{
		throw Exception(boost::format(_("Missing column %1%.")) % toSlkString(columnKey).toUtf8().constData());
	}

	return fromSlkString(QString::fromUtf8(this->slk().table()[iterator.value()][row].c_str()));
}

inline QString MetaData::value(const QString &rowKey, int column) const
{
	if (!hasSlk())
	{
		throw Exception();
	}

	SlkKeys::const_iterator iterator = this->rowKeys().find(toSlkString(rowKey));

	if (iterator == this->rowKeys().end())
	{
		throw Exception(boost::format(_("Missing row %1%.")) % toSlkString(rowKey).toUtf8().constData());
	}

	return fromSlkString(QString::fromUtf8(this->slk().table()[column][iterator.value()].c_str()));
}

inline bool MetaData::isEmpty() const
{
	if (hasSlk())
	{
		return this->slk().table().empty();
	}

	return this->txt().sections().empty();
}

inline QString MetaData::fromSlkString(const QString& value)
{
	QString result = value;

	if (result.startsWith('"'))
	{
		result = result.mid(1);
	}

	if (result.endsWith('"'))
	{
		result = result.mid(0, result.size() - 1);
	}

	return result;
}

inline QString MetaData::toSlkString(const QString& value)
{
	QString result = value;

	if (!result.startsWith('"'))
	{
		result.prepend('"');
	}

	if (!result.endsWith('"'))
	{
		result.append('"');
	}

	return result;
}

inline QString MetaData::fromFilePath(const QString& value)
{
	QString result = value;
	result.replace('\\', '/');

	return result;
}

}

}

#endif
