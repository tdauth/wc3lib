/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#ifndef WC3LIB_JASS_HPP
#define WC3LIB_JASS_HPP

namespace wc3lib
{
/**
 * \page jasssection JASS module
 * The JASS module aims to provide a full compiler environment for the scripting language JASS which is used in custom maps to provide a custom game logic.
 * This module offers types for an AST (Abstract Syntax Tree) as well as the class \ref Grammar for generating such a tree from user input.
 * Class \ref Analyser can be used for full parsing/syntax validation by checking for static semantic rules like type checks.
 * 
 * Include file \ref jass.hpp and use namespace \ref wc3lib::jass for this module.
 * 
 * \namespace wc3lib::jass
 * \brief \ref jasssection
 */
}

#include "jass/analyzer.hpp"
#include "jass/ast.hpp"
#include "jass/error.hpp"
#include "jass/grammar.hpp"
#include "jass/report.hpp"
#include "jass/warning.hpp"

#endif
