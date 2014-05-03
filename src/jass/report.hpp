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

#ifndef WC3LIB_JASS_REPORT_HPP
#define WC3LIB_JASS_REPORT_HPP

#include <string>

namespace wc3lib
{

namespace jass
{

struct jass_ast_node;

/**
 * Basic reports of the AST contain the corresponding node and the corresponding message.
 * They are used by the \ref Analyzer to report errors and warnings.
 */
class Report {
	public:
		Report(const jass_ast_node *node, const std::string &message);

		virtual std::string output() const;

	private:
		const jass_ast_node *m_node;
		std::string m_message;
};

}

}

#endif // REPORT_H
