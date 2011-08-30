/***************************************************************************
 *   Copyright (C) 2009 by Tamino Dauth                                    *
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

#ifndef WC3LIB_MAP_STRING_HPP
#define WC3LIB_MAP_STRING_HPP

#include <functional>
#include <list>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * Provides access to one single FDF or WTS file entry.
 * Stores usage data, too which is necessary or parsing/replacement (like gettext does).
 */
class String
{
	public:
		struct Usage
		{
			boost::filesystem::path filePath;
			std::size_t line;
		};

		typedef uint64_t IdType;
		typedef boost::shared_ptr<Usage> UsagePtr;
		typedef std::list<UsagePtr> Usages;

		struct IdComparator : public std::binary_function<class String*, class String*, bool>
		{
			bool operator()(const class String *first, const class String *second) const
			{
				return first->id() > second->id();
			}
		};

		static IdType id(const String &string)
		{
			stringstream sstream(string.m_idString.substr(7));
			IdType result;
			sstream >> result;

			return result;
		}

		String(const string &idString, const string &defaultString, const string &valueString) : m_idString(idString), m_defaultString(defaultString), m_valueString(valueString), m_usages()
		{
		}

		void addUsage(const boost::filesystem::path &path, std::size_t line)
		{
			UsagePtr ptr(new Usage);
			ptr->filePath = path;
			ptr->line = line;
			this->m_usages.push_back(ptr);
		}

		const string& idString() const
		{
			return this->m_idString;
		}

		const string& defaultString() const
		{
			return this->m_defaultString;
		}

		const string& valueString() const
		{
			return this->m_valueString;
		}

		IdType id() const
		{
			return String::id(*this);
		}

		const Usages& usages() const
		{
			return this->m_usages;
		}

	protected:
		string m_idString;
		string m_defaultString;
		string m_valueString;
		Usages m_usages;
};

}

}

#endif
