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
	wc3lib::read(istream, this->m_name, bytes);
	wc3lib::read(istream, this->m_position, bytes);
	wc3lib::read(istream, this->m_fieldOfView, bytes);
	wc3lib::read(istream, this->m_farClip, bytes);
	wc3lib::read(istream, this->m_nearClip, bytes);
	wc3lib::read(istream, this->m_target, bytes);
	bytes += this->m_translations->readMdx(istream);
	bytes += this->m_rotationLengths->readMdx(istream);
	bytes += this->m_targetTranslations->readMdx(istream);
	//(BKCT) ?????????????????????????????????????????????????????????????????

	if (bytes != nbytesi)
		throw Exception(boost::format(_("Camera: File byte count is not equal to real byte count.\nFile byte count: %1%.\nReal byte count: %2%.\n")) % nbytesi % bytes);

	return bytes;
}

std::streamsize Camera::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);
	
	std::streamsize bytes = 0;
	wc3lib::write(ostream, this->m_name, bytes);
	wc3lib::write(ostream, this->m_position, bytes);
	wc3lib::write(ostream, this->m_fieldOfView, bytes);
	wc3lib::write(ostream, this->m_farClip, bytes);
	wc3lib::write(ostream, this->m_nearClip, bytes);
	wc3lib::write(ostream, this->m_target, bytes);
	bytes += this->m_translations->writeMdx(ostream);
	bytes += this->m_rotationLengths->writeMdx(ostream);
	bytes += this->m_targetTranslations->writeMdx(ostream);
	//(BKCT) ?????????????????????????????????????????????????????????????????
	
	long32 nbytesi = bytes;
	writeByteCount(ostream, nbytesi, position, bytes, true);

	return bytes;
}

}

}
