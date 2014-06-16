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

#ifndef WC3LIB_JASS_ASTVISITORS_HPP
#define WC3LIB_JASS_ASTVISITORS_HPP

/**
 * \file
 * Defines visitor classes for variant types of the JASS Abstract Syntax Tree.
 */

#include <boost/variant/static_visitor.hpp>

#include "../core.hpp"
#include "ast.hpp"

namespace wc3lib
{

namespace jass
{

/**
 * Extracts the string identifier from a JASS AST node.
 */
class identifier_visitor : public boost::static_visitor<string>
{
	public:
		string operator()(const string &id) const
		{
			return id;
		}

		/**
		 * \ref jass_type_reference
		 *
		 * @{
		 */
		string operator()(const jass_type *type) const
		{
			return type->identifier;
		}
		/**
		 * @}
		 */

		/**
		 * \ref jass_var_reference
		 *
		 * @{
		 */
		string operator()(const jass_var_declaration *var) const
		{
			return var->identifier;
		}
		/**
		 * @}
		 */

		/**
		 * \ref jass_function_reference
		 */
		string operator()(const jass_function_declaration *declaration) const
		{
			return declaration->identifier;
		}
};

}

}

#endif
