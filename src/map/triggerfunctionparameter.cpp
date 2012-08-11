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

#include "triggerfunctionparameter.hpp"
#include "triggerdata.hpp"
#include "triggerfunction.hpp"

namespace wc3lib
{

namespace map
{

TriggerFunctionParameter::TriggerFunctionParameter() : m_type(TriggerFunctionParameter::Type::Preset)
{
}

TriggerFunctionParameter::~TriggerFunctionParameter()
{
}

std::streamsize TriggerFunctionParameter::read(InputStream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read<int32>(istream, (int32&)this->m_type, size);
	readString(istream, this->m_value, size);
	int32 functionCount = 0;
	wc3lib::read(istream, functionCount, size);

	if (functionCount > 0 && type() != Type::Function)
		std::cerr << boost::format(_("Warning: Functions in parameter which is not of type function itself - type %1%.")) % type() << std::endl;

	this->functions().reserve(functionCount);

	for (int32 i = 0; i < functionCount; ++i)
	{
		std::auto_ptr<TriggerFunction> function(new TriggerFunction());
		size += function->read(istream);
		this->functions().push_back(function);
	}

	int32 arrayIndexCount = 0;
	wc3lib::read(istream, arrayIndexCount, size);

	if (arrayIndexCount > 0 && type() != Type::Variable)
		std::cerr << boost::format(_("Warning: Array index count for non-variable parameter - type %1%.")) % type() << std::endl;

	for (int32 i = 0; i < arrayIndexCount; ++i)
	{
		std::auto_ptr<TriggerFunctionParameter> parameter(new TriggerFunctionParameter());
		size += parameter->read(istream);
		this->parameters().push_back(parameter);
	}

	return size;
}

std::streamsize TriggerFunctionParameter::write(OutputStream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write<int32>(ostream, this->type(), size);
	writeString(ostream, this->value(), size);
	wc3lib::write<int32>(ostream, this->functions().size(), size);

	BOOST_FOREACH(Functions::const_reference ref, this->functions())
		size += ref.write(ostream);

	wc3lib::write<int32>(ostream, this->parameters().size(), size);

	BOOST_FOREACH(Parameters::const_reference ref, this->parameters())
		size += ref.write(ostream);

	return size;
}

}

}
