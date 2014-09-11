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

#ifndef WC3LIB_MAP_CAMERAS_HPP
#define WC3LIB_MAP_CAMERAS_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include "platform.hpp"
#include "camera.hpp"

namespace wc3lib
{

namespace map
{

/**
 * "war3map.w3c" file of maps contains camera information.
 */
class Cameras : public FileFormat
{
	public:
		typedef boost::ptr_vector<Camera> CameraContainer;

		virtual std::streamsize read(InputStream &istream) override;
		virtual std::streamsize write(OutputStream &ostream) const override;

		virtual const byte* fileTextId() const override;
		virtual const byte* fileName() const override;
		virtual uint32 latestFileVersion() const override;

		CameraContainer& cameras();
		const CameraContainer& cameras() const;

	protected:
		CameraContainer m_cameras;
};

inline const byte* Cameras::fileTextId() const
{
	return "";
}

inline const byte* Cameras::fileName() const
{
	return "war3map.w3c";
}

inline uint32 Cameras::latestFileVersion() const
{
	return 0;
}

inline Cameras::CameraContainer& Cameras::cameras()
{
	return this->m_cameras;
}

inline const Cameras::CameraContainer& Cameras::cameras() const
{
	return this->m_cameras;
}

}

}

#endif
