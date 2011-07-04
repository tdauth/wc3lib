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

#include "layer.hpp"
#include "materialalphas.hpp"
#include "textureids.hpp"

namespace wc3lib
{

namespace mdlx
{

Layer::Layer(class Layers *layers) : m_alphas(new MaterialAlphas(this)), m_textureIds(new TextureIds(this)), GroupMdxBlockMember(layers, "Layer")
{
}

Layer::~Layer()
{
	delete this->m_alphas;
	delete this->m_textureIds;
}

std::streamsize Layer::readMdl(istream &istream) throw (class Exception)
{
	return 0;
}

std::streamsize Layer::writeMdl(ostream &ostream) const throw (class Exception)
{
	return 0;
}

std::streamsize Layer::readMdx(istream &istream) throw (class Exception)
{
	std::streamsize size = 0;
	long32 nbytesi;
	wc3lib::read(istream, nbytesi, size);
	wc3lib::read<long32>(istream, *reinterpret_cast<long32*>(&this->m_filterMode), size);
	wc3lib::read<long32>(istream, *reinterpret_cast<long32*>(&this->m_shading), size);
	wc3lib::read(istream, this->m_textureId, size);
	wc3lib::read(istream, this->m_tvertexAnimationId, size);
	wc3lib::read(istream, this->m_coordinatesId, size);
	wc3lib::read(istream, this->m_alpha, size);
	size += this->m_alphas->readMdx(istream);
	size += this->m_textureIds->readMdx(istream);

	return size;
}

std::streamsize Layer::writeMdx(ostream &ostream) const throw (class Exception)
{
	std::streampos position;
	skipByteCount<long32>(ostream, position);
	
	std::streamsize size = 0;
	wc3lib::write(ostream, static_cast<long32>(this->filterMode()), size);
	wc3lib::write(ostream, static_cast<long32>(this->shading()), size);
	wc3lib::write(ostream, this->textureId(), size);
	wc3lib::write(ostream, this->tvertexAnimationId(), size);
	wc3lib::write(ostream, this->coordinatesId(), size);
	wc3lib::write(ostream, this->alpha(), size);
	size += this->alphas()->writeMdx(ostream);
	size += this->textureIds()->writeMdx(ostream);

	long32 nbytesi = size;
	writeByteCount(ostream, nbytesi, position, size, true);

	return size;
}

}

}
