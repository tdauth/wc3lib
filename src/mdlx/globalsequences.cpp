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

#include <boost/foreach.hpp>

#include "globalsequences.hpp"
#include "globalsequence.hpp"
#include "../internationalisation.hpp"

namespace wc3lib
{

namespace mdlx
{

GlobalSequences::GlobalSequences(class Mdlx *mdlx) : GroupMdxBlock("GLBS", "GlobalSequences", false), m_mdlx(mdlx)
{
}

std::streamsize GlobalSequences::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize GlobalSequences::writeMdl(ostream &ostream) const throw (class Exception)
{
	ostream << "GlobalSequences " << this->globalSequences().size() << " {\n";

	BOOST_FOREACH(const class GlobalSequence *globalSequence, globalSequences())
		globalSequence->writeMdl(ostream);

	ostream << "}\n";
	
	return 0;
}

class GroupMdxBlockMember* GlobalSequences::createNewMember()
{
	return new GlobalSequence(this);
}

}

}
