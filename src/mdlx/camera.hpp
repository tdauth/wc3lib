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

#ifndef WC3LIB_MDLX_CAMERA_HPP
#define WC3LIB_MDLX_CAMERA_HPP

#include "platform.hpp"
#include "mdlxanimatedproperties.hpp"

namespace wc3lib
{

namespace mdlx
{

// not a child of class Object!
/**
 * MDL tag "Camera".
 */
class Camera
{
	public:
		static const std::size_t nameSize = 0x50;

		Camera();

		/**
		 * \return Returns name with length of \ref Camera::nameSize.
		 */
		const byte* name() const;
		const VertexData& position() const;
		float32 fieldOfView() const;
		float32 farClip() const;
		float32 nearClip() const;
		const VertexData& target() const;
		const Translations& translations() const;
		const Alphas& rotationLengths() const;
		const Translations& targetTranslations() const;

	protected:
		//long nbytesi;
		byte m_name[nameSize]; //(0x50)
		VertexData m_position;
		float32 m_fieldOfView;
		float32 m_farClip;
		float32 m_nearClip;
		VertexData m_target;
		Translations m_translations; //(KCTR)
		Alphas m_rotationLengths; //(KCRL)
		Translations m_targetTranslations; //(KTTR)
		//(BKCT) ?????????????????????????????????????????????????????????????????
};

inline const byte* Camera::name() const
{
	return this->m_name;
}

inline const VertexData& Camera::position() const
{
	return this->m_position;
}

inline float32 Camera::fieldOfView() const
{
	return this->m_fieldOfView;
}

inline float32 Camera::farClip() const
{
	return this->m_farClip;
}

inline float32 Camera::nearClip() const
{
	return this->m_nearClip;
}

inline const VertexData& Camera::target() const
{
	return this->m_target;
}

inline const Translations& Camera::translations() const
{
	return this->m_translations;
}

inline const Alphas& Camera::rotationLengths() const
{
	return this->m_rotationLengths;
}

inline const Translations& Camera::targetTranslations() const
{
	return this->m_targetTranslations;
}

}

}

#endif
