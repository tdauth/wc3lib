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

#include <boost/format.hpp>
#include <boost/foreach.hpp>

#include "cameras.hpp"
#include "camera.hpp"
#include "../utilities.hpp"
#include "../i18n.hpp"

namespace wc3lib
{

namespace map
{

std::streamsize Cameras::read(InputStream &istream)
{
	std::streamsize size = 0;
	wc3lib::read(istream, this->m_version, size);

	if (this->m_version != latestFileVersion())
	{
		std::cerr << boost::format(_("Cameras: Unknown version \"%1%\", expected \"%2%\".")) % this->version() % latestFileVersion() << std::endl;
	}

	int32 number;
	wc3lib::read(istream, number, size);
	this->cameras().reserve(number);

	for (int32 i = 0; i < number; ++i)
	{
		std::unique_ptr<Camera> cam(new Camera());
		size += cam->read(istream);
		this->cameras().push_back(std::move(cam));
	}

	return size;
}

std::streamsize Cameras::write(std::ostream &ostream) const
{
	if (this->version() != latestFileVersion())
	{
		std::cerr << boost::format(_("Cameras: Unknown version \"%1%\", expected \"%2%\".")) % this->version() % latestFileVersion() << std::endl;
	}

	std::streamsize size = 0;
	wc3lib::write(ostream, this->version(), size);
	wc3lib::write<int32>(ostream, this->cameras().size(), size);

	BOOST_FOREACH(CameraContainer::const_reference camera, this->cameras())
	{
		size += camera.write(ostream);
	}

	return size;
}

}

}
