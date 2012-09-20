/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
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

#include <boost/scoped_ptr.hpp>

#include "blpcodec.hpp"
#include "../../i18n.hpp"
#include "../../platform.h"

namespace wc3lib
{

namespace editor
{

BlpCodec* BlpCodec::m_self = 0;

void BlpCodec::startup()
{
	if (m_self == 0)
	{
		//Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, _("BLP codec registering"));

		m_self = OGRE_NEW BlpCodec();
		Ogre::Codec::registerCodec(m_self);
	}
}

void BlpCodec::shutdown()
{
	if(m_self != 0)
	{
		Ogre::Codec::unRegisterCodec(m_self);
		OGRE_DELETE m_self;
		m_self = 0;
	}

}

BlpCodec::~BlpCodec()
{
}

Ogre::String BlpCodec::getDataType() const
{
}

Ogre::DataStreamPtr BlpCodec::code(Ogre::MemoryDataStreamPtr &input, CodecDataPtr &pData) const
{
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, _("BLP encoding not supported"), "BlpCodec::code");
}

void BlpCodec::codeToFile(Ogre::MemoryDataStreamPtr &input, const Ogre::String &outFileName, CodecDataPtr &pData) const
{
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, _("BLP encoding not supported"), "BlpCodec::codeToFile");
}

BlpCodec::DecodeResult BlpCodec::decode(const blp::Blp &blp) const
{
	BlpCodec::ImageData* imgData = new BlpCodec::ImageData();
	imgData->format = Ogre::PF_A8R8G8B8;
	imgData->height = boost::numeric_cast<std::size_t>(blp.mipMaps().front().height());
	imgData->width = boost::numeric_cast<std::size_t>(blp.mipMaps().front().width());
	imgData->num_mipmaps = 0;
	// TODO support MIP maps in image (how are they stored?)
	//imgData->num_mipmaps = blp.mipMaps().size();
	imgData->depth = 1;
	imgData->size = Ogre::PixelUtil::getMemorySize(imgData->width, imgData->height, imgData->depth, imgData->format); //is equal to sizeof(blp::color) * imgData->height * imgData->width;

	Ogre::MemoryDataStreamPtr pixelData(OGRE_NEW Ogre::MemoryDataStream(imgData->size));
	unsigned char *imageData = pixelData->getPtr();
	const blp::dword mipMapHeight = blp.mipMaps().front().height();
	const blp::dword mipMapWidth = blp.mipMaps().front().width();
	std::size_t i = 0;

	for (blp::dword height = 0; height < mipMapHeight; ++height)
	{
		for (blp::dword width = 0; width < mipMapWidth; ++width)
		{
			const blp::color argb = blp.mipMaps().front().colorAt(width, height).argb();
			imageData[i] = blp::alpha(argb);
			++i;
			imageData[i] = blp::red(argb);
			++i;
			imageData[i] = blp::green(argb);
			++i;
			imageData[i] = blp::blue(argb);
			++i;
		}
	}

	DecodeResult ret;
	ret.first = pixelData;
	ret.second = CodecDataPtr(imgData);

	return ret;
}

BlpCodec::DecodeResult BlpCodec::decode(Ogre::DataStreamPtr &input) const
{
	boost::scoped_ptr<blp::Blp> blp(new blp::Blp());
	boost::scoped_array<blp::char8> buffer(new blp::char8[input->size()]);
	input->read(buffer.get(), input->size());
	iarraystream istream(buffer.get(), input->size());
	blp->read(istream, 1, blp::Blp::defaultThreads); /// \todo Add MIP map support and do not only read the first MIP map!

	return decode(*blp);
}

Ogre::String BlpCodec::getType () const
{
	return "blp";
}

bool BlpCodec::magicNumberMatch(const char *magicNumberPtr, size_t maxbytes) const
{
	if (maxbytes != 4)
		return false;

	return blp::Blp::hasFormat((blp::byte*)magicNumberPtr, maxbytes);
}

Ogre::String BlpCodec::magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const
{
	if (magicNumberMatch(magicNumberPtr, maxbytes))
		return "blp";

	return Ogre::StringUtil::BLANK;
}

}

}
