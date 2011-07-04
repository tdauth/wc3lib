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

class String
{
	public:
		typedef uint64_t IdType;

		struct UsageData
		{
			boost::filesystem::path &filePath;
			std::size_t line;
		};

		struct IdComparator : public std::binary_function<class String*, class String*, bool>
		{
			bool operator()(const class String *first, const class String *second) const
			{
				return first->id() > second->id();
			};
		};

		static IdType id(const class String &string)
		{
			stringstream sstream(string.m_idString.substr(7));
			IdType result;
			sstream >> result;

			return result;
		}

		String(const string &idString, const string &defaultString, const string &valueString) : m_idString(idString), m_defaultString(defaultString), m_valueString(valueString), m_usageData(std::list<struct UsageData*>())
		{
		}

		~String()
		{
			BOOST_FOREACH(struct UsageData *usageData, this->m_usageData)
				delete usageData;
		}

		void addUsage(const boost::filesystem::path &path, std::size_t line)
		{
			struct UsageData *usageData = new UsageData;
			usageData->filePath = path;
			usageData->line = line;
			this->m_usageData.push_back(usageData);
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
		};
		IdType id() const
		{
			return String::id(*this);
		};
		const std::list<struct UsageData*>& usageData() const
		{
			return this->m_usageData;
		}

	protected:
		string m_idString;
		string m_defaultString;
		string m_valueString;
		std::list<struct UsageData*> m_usageData;
};

}

}

#endif
