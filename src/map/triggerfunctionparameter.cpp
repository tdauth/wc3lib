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

#include "triggerfunctionparameter.hpp"
#include "triggerdata.hpp"
#include "triggerfunction.hpp"

namespace wc3lib
{

namespace map
{

TriggerFunctionParameter::TriggerFunctionParameter() : m_type(TriggerFunctionParameter::Type::Preset), m_function(0)
{
}

TriggerFunctionParameter::~TriggerFunctionParameter()
{
	if (function() != 0)
		delete function();
}

std::streamsize TriggerFunctionParameter::read(InputStream &istream, const TriggerData &triggerData) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read<int32>(istream, (int32&)this->m_type, size);
	readString(istream, this->m_value, size);
	int32 value;
	wc3lib::read(istream, value, size);

	if (value == 1 && type() != Type::Function)
		std::cerr << _("Warning: Type is not function.") << std::endl;
	else if (value > 1)
		throw Exception(_("Value should not be greater than 1."));

	if (value == 1)
	{
		m_function = new TriggerFunction();
		size += m_function->read(istream, triggerData);
	}

	wc3lib::read(istream, value, size);

	if (value == 1 && type() != Type::Variable)
		std::cerr << _("Warning: Type is not variable.") << std::endl;
	else if (value > 1)
		throw Exception(_("Value should not be greater than 1."));

	if (value == 1)
	{
		m_parameter.reset(new TriggerFunctionParameter());
		size += m_parameter->read(istream, triggerData);
	}

	return size;
}

std::streamsize TriggerFunctionParameter::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, this->type(), size);
	writeString(ostream, this->value(), size);

	if (function() != 0)
	{
		wc3lib::write<int32>(ostream, 1, size);
		size += function()->write(ostream);
	}
	else
		wc3lib::write<int32>(ostream, 0, size);

	if (parameter().get() != 0)
	{
		wc3lib::write<int32>(ostream, 1, size);
		size += parameter()->write(ostream);
	}
	else
		wc3lib::write<int32>(ostream, 0, size);

	return size;
}

}

}
