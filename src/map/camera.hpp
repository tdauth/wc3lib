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

#ifndef WC3LIB_MAP_CAMERA_HPP
#define WC3LIB_MAP_CAMERA_HPP

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

class Camera : public Format
{
	public:
		Camera();

		void setTargetX(float32 targetX);
		float32 targetX() const;
		void setTargetY(float32 targetY);
		float32 targetY() const;
		void setZOffset(float32 zOffset);
		float32 zOffset() const;
		void setRotation(float32 rotation);
		float32 rotation() const;
		void setAngleOfAttack(float32 angleOfAttack);
		float32 angleOfAttack() const;
		void setDistance(float32 distance);
		float32 distance() const;
		void setRoll(float32 roll);
		float32 roll() const;
		void setFieldOfView(float32 fieldOfView);
		float32 fieldOfView() const;
		void setFarZ(float32 farZ);
		float32 farZ() const;
		void setUnknown(float32 unknown);
		float32 unknown() const;
		void setName(const string &name);
		const string& name() const;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

	protected:
		float32 m_targetX;
		float32 m_targetY;
		float32 m_zOffset;
		float32 m_rotation;
		float32 m_angleOfAttack;
		float32 m_distance;
		float32 m_roll;
		float32 m_fieldOfView;
		float32 m_farZ;
		float32 m_unknown; // 100
		string m_name;
};

inline void Camera::setTargetX(float32 targetX)
{
	this->m_targetX = targetX;
}

inline float32 Camera::targetX() const
{
	return this->m_targetX;
}

inline void Camera::setTargetY(float32 targetY)
{
	this->m_targetY = targetY;
}

inline float32 Camera::targetY() const
{
	return this->m_targetY;
}

inline void Camera::setZOffset(float32 zOffset)
{
	this->m_zOffset = zOffset;
}

inline float32 Camera::zOffset() const
{
	return this->m_zOffset;
}

inline void Camera::setRotation(float32 rotation)
{
	this->m_rotation = rotation;
}

inline float32 Camera::rotation() const
{
	return this->m_rotation;
}

inline void Camera::setAngleOfAttack(float32 angleOfAttack)
{
	this->m_angleOfAttack = angleOfAttack;
}

inline float32 Camera::angleOfAttack() const
{
	return this->m_angleOfAttack;
}

inline void Camera::setDistance(float32 distance)
{
	this->m_distance = distance;
}

inline float32 Camera::distance() const
{
	return this->m_distance;
}

inline void Camera::setRoll(float32 roll)
{
	this->m_roll = roll;
}

inline float32 Camera::roll() const
{
	return this->m_roll;
}

inline void Camera::setFieldOfView(float32 fieldOfView)
{
	this->m_fieldOfView = fieldOfView;
}

inline float32 Camera::fieldOfView() const
{
	return this->m_fieldOfView;
}

inline void Camera::setFarZ(float32 farZ)
{
	this->m_farZ = farZ;
}

inline float32 Camera::farZ() const
{
	return this->m_farZ;
}

inline void Camera::setUnknown(float32 unknown)
{
	this->m_unknown = unknown;
}

inline float32 Camera::unknown() const
{
	return this->m_unknown;
}

inline void Camera::setName(const string &name)
{
	this->m_name = name;
}

inline const string& Camera::name() const
{
	return this->m_name;
}

}

}

#endif
