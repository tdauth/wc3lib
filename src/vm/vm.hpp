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

#ifndef VM_VM_HPP
#define VM_VM_HPP

#include <istream>
#include <map>

#include "../exception.hpp"

namespace wc3lib
{

namespace vm
{

class Vm
{
    public:
	/**
	* Reads Jass code and interprets it.
	*/
	void read(std::istream &istream) throw (class Exception);

	/**
	* Runs interpreted Jass code.
	*/
	void run() throw (class Exception);

    protected:
	std::map<std::string, class Type*> m_types;
	std::map<std::string, class Function*> m_functions;
	std::map<std::string, class Variable*> m_variables;
	class Function *m_currentFunction;
};

}

}

#endif
