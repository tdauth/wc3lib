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

#include <boost/foreach.hpp>
#include <boost/variant.hpp>

#include "triggerfunction.hpp"
#include "trigger.hpp"
#include "triggers.hpp"

namespace wc3lib
{

namespace map
{

TriggerFunction::TriggerFunction() : m_type(TriggerFunction::Type::Event), m_name(), m_isEnabled(false)
{
}

std::streamsize TriggerFunction::read(InputStream &istream, const TriggerData &triggerData)
{
	std::streamsize size = 0;
	wc3lib::read<int32>(istream, (int32&)this->m_type, size);
	readString(istream, this->m_name, size);

	TriggerData::Functions::const_iterator iterator;

	switch (this->type())
	{
		case TriggerFunction::Type::Event:
		{
			iterator = triggerData.events().find(this->name());

			if (iterator == triggerData.events().end())
			{
				throw Exception(boost::format(_("Function \"%1%\" not found in events of TriggerData.txt.")) % this->name());
			}

			break;
		}

		case TriggerFunction::Type::Condition:
		{
			iterator = triggerData.conditions().find(this->name());

			if (iterator == triggerData.conditions().end())
			{
				throw Exception(boost::format(_("Function \"%1%\" not found in conditions of TriggerData.txt.")) % this->name());
			}

			break;
		}

		case TriggerFunction::Type::Action:
		{
			iterator = triggerData.actions().find(this->name());

			if (iterator == triggerData.actions().end())
			{
				throw Exception(boost::format(_("Function \"%1%\" not found in actions of TriggerData.txt.")) % this->name());
			}

			break;
		}

		case TriggerFunction::Type::Call:
		{
			iterator = triggerData.calls().find(this->name());

			if (iterator == triggerData.calls().end())
			{
				throw Exception(boost::format(_("Function \"%1%\" not found in actions of TriggerData.txt.")) % this->name());
			}

			break;
		}

		default:
		{
			throw Exception(boost::format(_("Function type %1% is invalid.")) % static_cast<int32>(this->type()));
		}
	}

	// trigger calls have return type + parameters!
	const int32 count = this->type() == TriggerFunction::Type::Call ? iterator->second->types().size() - 1 :  iterator->second->types().size();
	wc3lib::read<int32>(istream, (int32&)this->m_isEnabled, size);

	if (count == 1)
	{
		const string firstParameter = boost::apply_visitor(TriggerData::FunctionArgumentVisitor(), iterator->second->types()[(this->type() == TriggerFunction::Type::Call ? 1 : 0)]);

		// cancel if first parameter is nothing
		if (firstParameter == "nothing")
		{
			return size;
		}
	}

	this->parameters().reserve(count);

	for (int32 i = 0; i < count; ++i)
	{
		std::unique_ptr<TriggerFunctionParameter> ptr(new TriggerFunctionParameter());
		size += ptr->read(istream, triggerData);
		this->parameters().push_back(std::move(ptr));
	}

	return size;
}

std::streamsize TriggerFunction::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, static_cast<int32>(this->type()), size);
	writeString(ostream, this->m_name, size);
	wc3lib::write<int32>(ostream, this->isEnabled(), size);

	BOOST_FOREACH(Parameters::const_reference value, this->parameters())
	{
		size += value->write(ostream);
	}

	return size;
}

}

}
