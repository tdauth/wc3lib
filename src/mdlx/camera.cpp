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

#include "camera.hpp"
#include "cameratranslations.hpp"
#include "camerarotationlengths.hpp"
#include "cameratargettranslations.hpp"

namespace wc3lib
{

namespace mdlx
{

Camera::Camera(class Cameras *cameras) : GroupMdxBlockMember(cameras, "Camera"), m_translations(new CameraTranslations(this)), m_rotationLengths(new CameraRotationLengths(this)), m_targetTranslations(new CameraTargetTranslations(this))
{
}

Camera::~Camera()
{
	delete this->m_translations;
	delete this->m_rotationLengths;
	delete this->m_targetTranslations;
}

std::streamsize Camera::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Camera::writeMdl(ostream &ostream) const throw (class Exception)
{
	return 0;
}

std::streamsize Camera::readMdx(istream &istream) throw (class Exception)
{
	long32 nbytesi;
	std::streamsize bytes = 0;
	wc3lib::read(istream, nbytesi, bytes);
	wc3lib::read(istream, this->m_name, bytes, nameSize);
	bytes += this->m_position.read(istream);
	wc3lib::read(istream, this->m_fieldOfView, bytes);
	wc3lib::read(istream, this->m_farClip, bytes);
	wc3lib::read(istream, this->m_nearClip, bytes);
	bytes += m_target.read(istream);
	bytes += this->m_translations->readMdx(istream);
	bytes += this->m_rotationLengths->readMdx(istream);
	bytes += this->m_targetTranslations->readMdx(istream);
	//(BKCT) ?????????????????????????????????????????????????????????????????

	checkBytesIncluding(bytes, nbytesi);

	return bytes;
}

std::streamsize Camera::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	std::streamsize bytes = 0;
	wc3lib::write(ostream, this->name(), bytes, nameSize);
	bytes += this->position().write(ostream);
	wc3lib::write(ostream, this->fieldOfView(), bytes);
	wc3lib::write(ostream, this->farClip(), bytes);
	wc3lib::write(ostream, this->nearClip(), bytes);
	bytes += target().write(ostream);
	bytes += this->translations()->writeMdx(ostream);
	bytes += this->rotationLengths()->writeMdx(ostream);
	bytes += this->targetTranslations()->writeMdx(ostream);
	//(BKCT) ?????????????????????????????????????????????????????????????????

	long32 nbytesi = bytes;
	writeByteCount(ostream, nbytesi, position, bytes, true);

	return bytes;
}

}

}
