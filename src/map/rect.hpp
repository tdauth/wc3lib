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

#ifndef WC3LIB_MAP_RECT_HPP
#define WC3LIB_MAP_RECT_HPP

#include "../color.hpp"

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

class Rect : public Format
{
	public:
		Rect(class Rects *rects);

		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		float32 left() const;
		float32 right() const;
		float32 bottom() const;
		float32 top() const;
		const string& name() const;
		int32 index() const;
		id weatherEffectId() const;
		const string& soundName() const;
		const struct Rgb& color() const;

	protected:
		class Rects *m_rects;
		float32 m_left;
		float32 m_right;
		float32 m_bottom;
		float32 m_top;
		string m_name;
		int32 m_index;
		id m_weatherEffectId;
		string m_soundName; // class Sound *m _sound
		Rgb m_color; // no alpha!

};

inline float32 Rect::left() const
{
	return this->m_left;
}

inline float32 Rect::right() const
{
	return this->m_right;
}

inline float32 Rect::bottom() const
{
	return this->m_bottom;
}

inline float32 Rect::top() const
{
	return this->m_top;
}

inline const string& Rect::name() const
{
	return this->m_name;
}

inline int32 Rect::index() const
{
	return this->m_index;
}

inline id Rect::weatherEffectId() const
{
	return this->m_weatherEffectId;
}

inline const string& Rect::soundName() const
{
	return this->m_soundName;
}

inline const Rgb& Rect::color() const
{
	return this->m_color;
}

}

}

#endif
