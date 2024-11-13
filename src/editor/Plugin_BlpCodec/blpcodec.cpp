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
		Ogre::Codec::unregisterCodec(m_self);
		OGRE_DELETE m_self;
		m_self = 0;
	}

}

BlpCodec::~BlpCodec()
{
}

Ogre::String BlpCodec::getDataType() const
{
	return Ogre::String();
}

Ogre::DataStreamPtr BlpCodec::encode(const Ogre::Any &input) const
{
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, _("BLP encoding not supported"), "BlpCodec::code");
}

void BlpCodec::encodeToFile(const Ogre::Any &input, const Ogre::String &outFileName) const
{
	OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, _("BLP encoding not supported"), "BlpCodec::codeToFile");
}

// TODO decode directly to paletted OGRE image if possible!
void BlpCodec::decode(const blp::Blp &blp, const Ogre::Any &output) const
{
	BlpCodec::ImageData* imgData = new BlpCodec::ImageData();
	imgData->format = Ogre::PF_R8G8B8A8;
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
			blp::color rgba = 0;

			if (blp.compression() == blp::Blp::Compression::Paletted)
			{
				if (blp.pictureType() == blp::Blp::PictureType::PalettedWithoutAlpha)
				{
					rgba = blp.palette()[blp.mipMaps().front().paletteIndexAt(width, height)];
				}
				else
				{
					rgba = blp.mipMaps().front().colorAt(width, height).paletteColor(blp.palette().get());
				}
			}
			else if (blp.compression() == blp::Blp::Compression::Jpeg)
			{
				rgba = blp.mipMaps().front().colorAt(width, height).rgba();
			}

			// NOTE little endian order - reverse order
			imageData[i] = blp::red(rgba);
			++i;
			imageData[i] = blp::green(rgba);
			++i;
			imageData[i] = blp::blue(rgba);
			++i;
			imageData[i] = blp::alpha(rgba);
			++i;
		}
	}

	DecodeResult ret = output.get<DecodeResult>();
	ret.first = pixelData;
	ret.second = CodecDataPtr(imgData);

	//return ret;
}

void BlpCodec::decode(const Ogre::DataStreamPtr &input, const Ogre::Any &output) const
{
	boost::scoped_ptr<blp::Blp> blp(new blp::Blp());
	boost::scoped_array<blp::char8> buffer(new blp::char8[input->size()]);
	input->read(buffer.get(), input->size());
	iarraystream istream(buffer.get(), input->size());
	blp->read(istream, 1); /// \todo Add MIP map support and do not only read the first MIP map!

	return decode(*blp, output);
}

Ogre::String BlpCodec::getType () const
{
	return "blp";
}

Ogre::String BlpCodec::magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const
{
	if (maxbytes == 4 && blp::Blp::hasFormat((blp::byte*)magicNumberPtr, maxbytes)) {
		return "blp";
	}

	return Ogre::StringUtil::BLANK;
}

}

}
