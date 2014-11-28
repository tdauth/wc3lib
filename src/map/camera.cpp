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

#include "camera.hpp"

namespace wc3lib
{

namespace map
{

Camera::Camera()
: m_targetX(0.0)
, m_targetY(0.0)
, m_zOffset(0.0)
, m_rotation(0.0)
, m_angleOfAttack(0.0)
, m_distance(0.0)
, m_roll(0.0)
, m_fieldOfView(0.0)
, m_farZ(0.0)
, m_unknown(0.0)
{
}

std::streamsize Camera::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_targetX, size);
	wc3lib::read(istream, this->m_targetY, size);
	wc3lib::read(istream, this->m_zOffset, size);
	wc3lib::read(istream, this->m_rotation, size);
	wc3lib::read(istream, this->m_angleOfAttack, size);
	wc3lib::read(istream, this->m_distance, size);
	wc3lib::read(istream, this->m_roll, size);
	wc3lib::read(istream, this->m_fieldOfView, size);
	wc3lib::read(istream, this->m_farZ, size);
	wc3lib::read(istream, this->m_unknown, size);
	readString(istream, this->m_name, size);

	return size;
}

std::streamsize Camera::write(OutputStream &ostream) const
{
	std::streamsize size = 0;
	wc3lib::write(ostream, this->m_targetX, size);
	wc3lib::write(ostream, this->m_targetY, size);
	wc3lib::write(ostream, this->m_zOffset, size);
	wc3lib::write(ostream, this->m_rotation, size);
	wc3lib::write(ostream, this->m_angleOfAttack, size);
	wc3lib::write(ostream, this->m_distance, size);
	wc3lib::write(ostream, this->m_roll, size);
	wc3lib::write(ostream, this->m_fieldOfView, size);
	wc3lib::write(ostream, this->m_farZ, size);
	wc3lib::write(ostream, this->m_unknown, size);
	writeString(ostream, this->m_name, size);

	return size;
}

}

}
