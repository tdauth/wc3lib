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

#include "triggerfunction.hpp"
#include "triggerdata.hpp"

namespace wc3lib
{

namespace map
{

TriggerFunction::TriggerFunction() : m_type(TriggerFunction::Type::Event), m_name(), m_isEnabled(false)
{
}

namespace
{

int32 functionCount(const TriggerData::Functions &functions, const string &identifier)
{
	BOOST_FOREACH(TriggerData::Functions::left_const_reference value, functions.left)
	{
		if (value.second->code() == identifier)
			return value.second->types().size();
	}

	throw Exception(boost::format(_("Warning: Didn't find function \"%1%\".")) % identifier);

	return 0;
}

}

std::streamsize TriggerFunction::read(InputStream &istream, const TriggerData &triggerData) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read<int32>(istream, (int32&)this->m_type, size);
	readString(istream, this->m_name, size);
	wc3lib::read<int32>(istream, (int32&)this->m_isEnabled, size);
	int32 parameters = 0;

	switch (type())
	{
		case Type::Event:
			parameters = functionCount(triggerData.events(), name());

			break;

		case Type::Condition:
			parameters = functionCount(triggerData.conditions(), name());

			break;

		case Type::Action:
			parameters = functionCount(triggerData.actions(), name());

			break;
	}

	this->parameters().resize(parameters);

	for (int32 i = 0; i < parameters; ++i)
	{
		ParameterPtr ptr(new TriggerFunctionParameter());
		size += ptr->read(istream, triggerData);
		this->parameters()[i].swap(ptr);
	}

	return size;
}

std::streamsize TriggerFunction::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, this->type(), size);
	writeString(ostream, this->m_name, size);
	wc3lib::write<int32>(ostream, this->isEnabled(), size);

	BOOST_FOREACH(Parameters::const_reference value, this->parameters())
		size += value->write(ostream);

	return size;
}

}

}
