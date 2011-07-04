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

#include <string>
#include <sstream>

#include <boost/tokenizer.hpp>

#include "globalsequence.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

GlobalSequence::GlobalSequence(class GlobalSequences *globalSequences) : GroupMdxBlockMember(globalSequences, "")
{
}

std::streamsize GlobalSequence::readMdl(istream &istream) throw (class Exception)
{
	std::string line;
	std::getline(istream, line);
	boost::tokenizer<> tokenizer(line);
	boost::tokenizer<>::iterator iterator = tokenizer.begin();

	if (iterator == tokenizer.end() || (*iterator) != "Duration")
		throw Exception("Global sequence: Missing keyword \"Duration\".");

	++iterator;

	if (iterator == tokenizer.end())
		throw Exception("Global sequence: Missing value.");
	else if (*((*iterator).end() - 1) != ',')
		throw Exception("Global sequence: Missing \",\" character.");

	line = (*iterator);
	line.erase(line.length() - 1);
	std::istringstream sstream(line);
	sstream >> this->m_duration;
	
	return 0;
}

std::streamsize GlobalSequence::writeMdl(ostream &ostream) const throw (class Exception)
{
	ostream << "\tDuration " << this->duration() << ",\n";
	
	return 0;
}

std::streamsize GlobalSequence::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_duration, size);
	
	return size;
}

std::streamsize GlobalSequence::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;
	wc3lib::write(ostream, duration(), size);
	
	return size;
}

}

}
