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

#ifndef WC3LIB_MDLX_BOUNDS_HPP
#define WC3LIB_MDLX_BOUNDS_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * Should be inherited by classes \ref Model, \ref Sequence, \ref Geoset and \ref Ganimation.
 */
class Bounds : public Format
{
	public:
		Bounds();
		virtual ~Bounds();

		void setBoundsRadius(float32 boundsRadius);
		float32 boundsRadius() const;
		void setMinimumExtent(const VertexData &vertexData);
		const VertexData& minimumExtent() const;
		void setMaximumExtent(const VertexData &vertexData);
		const VertexData& maximumExtent() const;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

	protected:
		float32 m_boundsRadius;
		VertexData m_minimumExtent;
		VertexData m_maximumExtent;
};

inline void Bounds::setBoundsRadius(float32 boundsRadius)
{
	this->m_boundsRadius = boundsRadius;
}

inline float32 Bounds::boundsRadius() const
{
	return this->m_boundsRadius;
}

inline void Bounds::setMinimumExtent(const VertexData &vertexData)
{
	this->m_minimumExtent = vertexData;
}

inline const VertexData& Bounds::minimumExtent() const
{
	return this->m_minimumExtent;
}

inline void Bounds::setMaximumExtent(const VertexData &vertexData)
{
	this->m_maximumExtent = vertexData;
}

inline const VertexData& Bounds::maximumExtent() const
{
	return this->m_maximumExtent;
}

}

}

#endif
