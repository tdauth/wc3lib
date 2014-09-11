/***************************************************************************
 *   Copyright (C) 2013 by Tamino Dauth                                    *
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

#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>

#include "triggerstrings.hpp"

namespace wc3lib
{

namespace map
{

TriggerStrings::TriggerStrings()
{
}

TriggerStrings::Entry::Entry()
{
}

void TriggerStrings::readFunction(const Txt::Entry &ref, Entries &functions)
{
	string code = ref.first;
	bool isHint = false;
	const string::size_type index = code.rfind("Hint");

	if (index != string::npos && index + strlen("Hint") == code.size())
	{
		isHint = true;
		code = code.substr(0, index);
	}

	Entries::iterator iterator = functions.find(code);

	if (iterator == functions.end())
	{
		iterator = functions.insert(code, new Entry()).first;
		std::cerr << "New entry \"" << code << "\"" << std::endl;
		iterator->second->setCode(code);
		iterator->second->setName(ref.second);
	}
	else if (isHint)
	{
		std::cerr << "Got Hint for " << code << std::endl;
		iterator->second->setHint(ref.second);
	} else
	{
		iterator->second->setLayout(ref.second);
	}
}

std::streamsize TriggerStrings::read(TriggerStrings::InputStream& istream)
{
	boost::scoped_ptr<Txt> txt(new Txt());
	std::streamsize size = txt->read(istream);

	BOOST_FOREACH(Txt::Entries::const_reference ref, txt->entries("TriggerEventStrings"))
	{
		readFunction(ref, this->events());
	}

	BOOST_FOREACH(Txt::Entries::const_reference ref, txt->entries("TriggerConditionStrings"))
	{
		readFunction(ref, this->conditions());
	}

	BOOST_FOREACH(Txt::Entries::const_reference ref, txt->entries("TriggerActionStrings"))
	{
		readFunction(ref, this->actions());
	}

	BOOST_FOREACH(Txt::Entries::const_reference ref, txt->entries("TriggerCallStrings"))
	{
		readFunction(ref, this->calls());
	}

	return size;
}

std::streamsize TriggerStrings::write(TriggerStrings::OutputStream& ostream) const
{
    return 0;
}

TriggerStrings::Entries& TriggerStrings::entries(TriggerFunction::Type type)
{
	switch (type)
	{
		case TriggerFunction::Type::Event:
		{
			return this->events();
		}

		case TriggerFunction::Type::Condition:
		{
			return this->conditions();
		}

		case TriggerFunction::Type::Action:
		{
			return this->actions();
		}

		case TriggerFunction::Type::Call:
		{
			return this->calls();
		}
	}

	throw Exception(boost::format(_("Unknown trigger function type: %1%")) % static_cast<int32>(type));
}

const TriggerStrings::Entries& TriggerStrings::entries(TriggerFunction::Type type) const
{
	switch (type)
	{
		case TriggerFunction::Type::Event:
		{
			return this->events();
		}

		case TriggerFunction::Type::Condition:
		{
			return this->conditions();
		}

		case TriggerFunction::Type::Action:
		{
			return this->actions();
		}

		case TriggerFunction::Type::Call:
		{
			return this->calls();
		}
	}

	throw Exception(boost::format(_("Unknown trigger function type: %1%")) % static_cast<int32>(type));
}

}

}
