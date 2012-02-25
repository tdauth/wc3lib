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

#include <boost/format.hpp>

#include "geosetanimation.hpp"
#include "geosetanimationalphas.hpp"
#include "geosetanimationcolors.hpp"
#include "../internationalisation.hpp"
#include "../utilities.hpp"

namespace wc3lib
{

namespace mdlx
{

GeosetAnimation::GeosetAnimation(class GeosetAnimations *geosetAnimations) : GroupMdxBlockMember(geosetAnimations, "GeosetAnim"), m_staticAlpha(0.0), m_colorAnimation(GeosetAnimation::ColorAnimation::None), m_colorRed(0.0), m_colorGreen(0.0), m_colorBlue(0.0), m_geosetId(0), m_alphas(new GeosetAnimationAlphas(this)),  m_colors(new GeosetAnimationColors(this))
{
}

GeosetAnimation::~GeosetAnimation()
{
	delete this->m_alphas;
	delete this->m_colors;
}

std::streamsize GeosetAnimation::readMdl(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;//GroupMdxBlockMember::readMdl(istream);

	if (size == 0)
		return 0;

	/// \todo Finish!
	return size;
}

std::streamsize GeosetAnimation::writeMdl(ostream &ostream) const throw (class Exception)
{
	std::streamsize size = 0;

	if (colorAnimation() == ColorAnimation::DropShadow || colorAnimation() == ColorAnimation::Both)
		writeMdlProperty(ostream, "DropShadow", size);

	if (this->alphas()->properties().empty())
		writeMdlStaticValueProperty(ostream, "Alpha", staticAlpha(), size);
	else
		size += this->alphas()->writeMdl(ostream);

	if (colorAnimation() == ColorAnimation::Color || colorAnimation() == ColorAnimation::Both)
	{
		if (this->colors()->properties().empty())
		{
			writeMdlStaticVectorProperty(ostream, "Color", VertexData(colorBlue(), colorGreen(), colorRed()), size);
		}
		else
			size += this->colors()->writeMdl(ostream);
	}

	writeMdlValueProperty(ostream, "GeosetId", geosetId(), size);

	return size;
}

std::streamsize GeosetAnimation::readMdx(istream &istream) throw (class Exception)
{
	long32 nbytesi;
	std::streamsize size = 0;
	wc3lib::read(istream, nbytesi, size);
	wc3lib::read(istream, this->m_staticAlpha, size);
	long32 colorAnimation;
	wc3lib::read(istream, colorAnimation, size);
	this->m_colorAnimation = static_cast<BOOST_SCOPED_ENUM(ColorAnimation)>(colorAnimation);
	wc3lib::read(istream, this->m_colorRed, size);
	wc3lib::read(istream, this->m_colorGreen, size);
	wc3lib::read(istream, this->m_colorBlue, size);
	wc3lib::read(istream, this->m_geosetId, size);

	std::cout << "Static alpha is " << this->m_staticAlpha << std::endl;

	if (this->staticAlpha() == 1.0)
		size += this->m_alphas->readMdx(istream);

	size += this->m_colors->readMdx(istream); /// @todo Seems to be optional, file Krieger.mdx doesn't have this block.
	std::cout << "After colors" << std::endl;

	if (nbytesi != size)
		throw Exception(boost::str(boost::format(_("Geoset animation: File byte count isn't equal to real byte count:\nFile byte count %1%.\nReal byte count %2%.\n")) % nbytesi % size));

	return size;
}

std::streamsize GeosetAnimation::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	std::streamsize size = 0;
	wc3lib::write(ostream, this->staticAlpha(), size);
	wc3lib::write(ostream, this->colorAnimation(), size);
	wc3lib::write(ostream, this->colorRed(), size);
	wc3lib::write(ostream, this->colorGreen(), size);
	wc3lib::write(ostream, this->colorBlue(), size);
	wc3lib::write(ostream, this->geosetId(), size);

	if (this->staticAlpha() == 1.0)
		size += this->m_alphas->writeMdx(ostream);

	size += this->m_colors->writeMdx(ostream);

	long32 nbytesi = size;
	writeByteCount(ostream, nbytesi, position, size, true);

	return size;
}

}

}
