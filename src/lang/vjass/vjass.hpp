/***************************************************************************
 *   Copyright (C) 2008, 2009 by Tamino Dauth                              *
 *   tamino@cdauth.de                                                      *
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

#ifndef WC3LIB_LANG_VJASS_VJASS_HPP
#define WC3LIB_LANG_VJASS_VJASS_HPP

#include "../language.hpp"

namespace wc3lib
{
	
namespace lang
{
	
namespace vjass
{

class Vjass : public Language
{
	public:
		Vjass();
		virtual const std::string& name() const;
		virtual bool compatibleTo(const class Language &language) const;

		void setForceMethodEvaluate(bool forceMethodEvaluate);		
		bool forceMethodEvaluate() const;
		void setNoImplicitThis(bool noImplicitThis);
		bool noImplicitThis() const;
		
		class Object::List& externalCalls();
		class Object::List& functionInterfaces();
		class Object::List& hooks();
		
	protected:
		bool m_forceMethodEvaluate;
		bool m_noImplicitThis;
};

inline void Vjass::setForceMethodEvaluate(bool forceMethodEvaluate)
{
	this->m_forceMethodEvaluate = forceMethodEvaluate;
}

inline bool Vjass::forceMethodEvaluate() const
{
	return this->m_forceMethodEvaluate;
}

inline void Vjass::setNoImplicitThis(bool noImplicitThis)
{
	this->m_noImplicitThis = noImplicitThis;
}

inline bool Vjass::noImplicitThis() const
{
	return this->m_noImplicitThis;
}

}

}

}

#endif
