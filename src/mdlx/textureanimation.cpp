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

#include "textureanimation.hpp"
#include "textureanimationtranslations.hpp"
#include "textureanimationrotations.hpp"
#include "textureanimationscalings.hpp"
#include "../utilities.hpp"
#include "../i18n.hpp"

namespace wc3lib
{

namespace mdlx
{

TextureAnimation::TextureAnimation(class TextureAnimations *textureAnimations) : GroupMdxBlockMember(textureAnimations, "TVertexAnim"), m_translations(new TextureAnimationTranslations(this)), m_rotations(new TextureAnimationRotations(this)), m_scalings(new TextureAnimationScalings(this))
{
}

TextureAnimation::~TextureAnimation()
{
	delete this->m_translations;
	delete this->m_rotations;
	delete this->m_scalings;
}

std::streamsize TextureAnimation::readMdl(istream &istream)
{
	return 0;
}

std::streamsize TextureAnimation::writeMdl(ostream &ostream) const
{
	std::streamsize size = 0;
	writeMdlBlock(ostream, size, "TVertexAnim");

	if (!this->translations()->properties().empty())
		size += this->translations()->writeMdl(ostream);

	if (!this->rotations()->properties().empty())
		size += this->rotations()->writeMdl(ostream);

	if (!this->scalings()->properties().empty())
		size += this->scalings()->writeMdl(ostream);

	writeMdlBlockConclusion(ostream, size);

	return size;
}


std::streamsize TextureAnimation::readMdx(istream &istream)
{
	std::streamsize size = 0;
	long32 nbytesi = 0;
	wc3lib::read(istream, nbytesi, size);
	size += this->translations()->readMdx(istream);
	size += this->rotations()->readMdx(istream);
	size += this->scalings()->readMdx(istream);

	checkBytesIncluding(size, nbytesi);

	return size;
}

std::streamsize TextureAnimation::writeMdx(ostream &ostream) const
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);

	std::streamsize size = this->translations()->writeMdx(ostream);
	size += this->rotations()->writeMdx(ostream);
	size += this->scalings()->writeMdx(ostream);

	long32 nbytesi = size;
	writeByteCount(ostream, nbytesi, position, size, true);

	return size;
}

}

}
