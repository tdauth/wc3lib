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
 * to retrieve any value by keys.
 *
 * Uses \ref map::Slk to load a SYLK file which contains all meta data.
 * In addition to \ref map::Slk it stores columns and rows by keys (their first cell values) through hashing.
 * This allows fast access of columns or rows by keys which is usually required for meta data.
 *
 * Uses \ref map::Txt to load a .txt file which contain additional object data such as strings.
 *
 * Both kind of files use the UTF-8 encoding by default.
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
		typedef QHash<QString, int> SlkKeys;

		/**
		 * Use the section names as keys for the sections.
		 */
		typedef QHash<QString, map::Txt::Section*> TxtKeys;

		MetaData(const KUrl &url);

		virtual void clear() throw ();

		virtual void load() throw (Exception);
		virtual void reload() throw (Exception);
		virtual void save(const KUrl &url) const throw (Exception)
		{
			throw Exception(_("Saving meta data is not supported yet."));
		}

		bool hasSlk() const;
		bool hasTxt() const;

		map::Slk& slk();
		const map::Slk& slk() const;
		map::Txt& txt();
		const map::Txt& txt() const;
		const SlkKeys& columnKeys() const;
		const SlkKeys& rowKeys() const;
		const TxtKeys& sectionKeys() const;
		map::Slk::ConstView row(const QString &key) const;
		map::Slk::ConstView column(const QString &key) const;

		QString value(int row, const QString &columnKey) const;
		QString value(const QString &rowKey, int column) const;
		QString value(const QString &rowKey, const QString &columnKey) const;

		bool hasValue(const QString &rowKey, const QString &columnKey) const;

		bool isEmpty() const;

	protected:
		Source m_source;
		SlkKeys m_columnKeys;
		SlkKeys m_rowKeys;
		TxtKeys m_sectionKeys;
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

inline const MetaData::TxtKeys& MetaData::sectionKeys() const
{
	return this->m_sectionKeys;
}

inline map::Slk::ConstView MetaData::row(const QString &key) const
{
	if (!hasSlk())
	{
		throw Exception();
	}

	SlkKeys::const_iterator iterator = this->rowKeys().find(key);

	if (iterator == this->rowKeys().end())
	{
		throw Exception(boost::format(_("Row %1% does not exist.")) % key.toUtf8().constData());
	}

	return this->slk().row(iterator.value());
}

inline map::Slk::ConstView MetaData::column(const QString &key) const
{
	if (!hasSlk())
	{
		throw Exception();
	}

	SlkKeys::const_iterator iterator = this->columnKeys().find(key);

	if (iterator == this->columnKeys().end())
	{
		throw Exception();
	}

	return this->slk().column(iterator.value());
}

inline QString MetaData::value(int row, const QString &columnKey) const
{
	if (!hasSlk())
	{
		throw Exception();
	}

	SlkKeys::const_iterator iterator = this->columnKeys().find(columnKey);

	if (iterator == this->columnKeys().end())
	{
		throw Exception();
	}

	return QString::fromUtf8(this->slk().table()[iterator.value()][row].c_str());
}

inline QString MetaData::value(const QString &rowKey, int column) const
{
	if (!hasSlk())
	{
		throw Exception();
	}

	SlkKeys::const_iterator iterator = this->rowKeys().find(rowKey);

	if (iterator == this->rowKeys().end())
	{
		throw Exception();
	}

	return QString::fromUtf8(this->slk().table()[column][iterator.value()].c_str());
}

inline QString MetaData::value(const QString &rowKey, const QString &columnKey) const
{
	if (hasSlk())
	{
		SlkKeys::const_iterator columnIterator = this->columnKeys().find(columnKey);

		if (columnIterator == this->columnKeys().end())
		{
			throw Exception();
		}

		SlkKeys::const_iterator rowIterator = this->rowKeys().find(rowKey);

		if (rowIterator == this->rowKeys().end())
		{
			throw Exception();
		}

		const int column = columnIterator.value();
		const int row = rowIterator.value();
		qDebug() << "Value " << column << "|" << row;
		const string &var = this->slk().table()[column][row];

		qDebug() << "Value var " << var.c_str();

		if (column < this->slk().columns() && row < this->slk().rows())
		{
			return QString::fromUtf8(this->slk().table()[column][row].c_str());
		}
		else
		{
			throw Exception();
		}
	}

	// TXT
	TxtKeys::const_iterator rowIterator = this->sectionKeys().find(rowKey);

	if (rowIterator == this->sectionKeys().end())
	{
		throw Exception();
	}

	const map::Txt::Section &section = *rowIterator.value();

	foreach (map::Txt::Entries::const_reference entry, section.entries)
	{
		if (entry.first == columnKey.toUtf8().constData())
		{
			return QString::fromUtf8(entry.second.c_str());
		}
	}

	throw Exception();

}

inline bool MetaData::hasValue(const QString &rowKey, const QString &columnKey) const
{
	SlkKeys::const_iterator columnIterator = this->columnKeys().find(columnKey);

	if (columnIterator == this->columnKeys().end())
	{
		return false;
	}

	SlkKeys::const_iterator rowIterator = this->rowKeys().find(rowKey);

	if (rowIterator == this->rowKeys().end())
	{
		return false;
	}

	return true;
}

inline bool MetaData::isEmpty() const
{
	if (hasSlk())
	{
		return this->slk().table().empty();
	}

	return this->txt().sections().empty();
}

}

}

#endif
