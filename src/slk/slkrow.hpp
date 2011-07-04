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

#ifndef WC3LIB_SLK_SLKROW_HPP
#define WC3LIB_SLK_SLKROW_HPP

#include <map>

#include "slk.hpp"
#include "../format.hpp"

namespace wc3lib
{

namespace slk
{

class SlkRow : public Format
{
	public:
		struct Predicate : std::binary_function<const SlkItemKey&, const SlkRow*, bool>
		{
			bool operator()(const SlkItemKey &key, const SlkRow *row)
			{
				return key.row() == row;
			}
		};
		
		const class SlkRows* rows() const
		{
			return m_rows;
		}
		
		const class SlkItems items() const
		{
			return this->rows()->items(this);
		}
		
		

	protected:
		class SlkRows *m_rows;
		Slk::IndexType m_row;
		map::id m_id;
};

}

}

#endif
