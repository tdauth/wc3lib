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

#include <QtCore>
#include <QtGui>

#include "blpiohandler.hpp"
#include "../../platform.hpp"
#include "../platform.hpp"

namespace wc3lib
{

namespace editor
{

BlpIOHandler::BlpIOHandler() : QImageIOHandler()
{
}

BlpIOHandler::~BlpIOHandler()
{
}

bool BlpIOHandler::canRead() const
{
	blp::dword identifier = 0;

	return (this->device() != 0
		&& this->device()->isReadable()
		&& this->device()->peek(reinterpret_cast<char*>(&identifier), sizeof(identifier)) == sizeof(identifier)
		&& blp::Blp::hasFormat(reinterpret_cast<blp::byte*>(&identifier), sizeof(identifier)));
}

bool BlpIOHandler::read(QImage *image)
{
	// read buffer into input stream
	const QByteArray all = this->device()->readAll();

	iarraystream istream(all.constData(), all.size()); // copies buffer data!
	//istream.rdbuf()->pubsetbuf(buffer, all.size()); TODO setting buffer directly does not work

	QScopedPointer<blp::Blp> blpImage(new blp::Blp());

	try
	{
		// Read only the first MIP map, QImage does not support MIP mapping.
		blpImage->read(istream, 1);
	}
	catch (class Exception &exception)
	{
		qDebug() << "BLP Input Exception: " << exception.what();

		return false;
	}

	if (!read(image, *blpImage))
	{
		return false;
	}

	return true;
}

bool BlpIOHandler::write(const QImage &image)
{
	QScopedPointer<blp::Blp> blpImage(new blp::Blp());

	if (!write(image, blpImage.data()))
	{
		return false;
	}

	/*
	 * We must use a temporary file or a buffered output stream since writing BLPs needs to seek forward
	 * which is not supported by a string stream.
	 *
	 * TODO a binary buffered stream which directly connects to the device this->device() would be much more efficient.
	 */
	QTemporaryFile file;

	if (!file.open())
	{
		return false;
	}

	qDebug() << "Temporary file: " << file.fileName();
	fstream out(file.fileName().toUtf8().constData(), std::ios::out | std::ios::binary);

	if (!out)
	{
		return false;
	}

	try
	{
		blpImage->write(out);
	}
	catch (class Exception &exception)
	{
		qDebug() << "BLP Output Exception: " << exception.what();

		return false;
	}

	/*
	 * Flush stream.
	 */
	out.close();

	/*
	 * Write all data into the output device.
	 * TODO slow - see above
	 */
	const QByteArray data = file.readAll();
	this->device()->write(data, data.size());

	return true;
}

bool BlpIOHandler::read(QImage *image, const blp::Blp::MipMap &mipMap, const blp::Blp &blpImage)
{
	QImage::Format format;
	/*
	 * Only if all alpha values are taken from the color palette as well it can be paletted in Qt.
	 * TODO find another solution for custom alpha values.
	 */
	const bool paletted = blpImage.compression() == blp::Blp::Compression::Paletted && blpImage.pictureType() == blp::Blp::PictureType::PalettedWithoutAlpha;

	if (paletted)
	{
		format = QImage::Format_Indexed8;
	}
	else if (blpImage.compression() == blp::Blp::Compression::Jpeg || blpImage.compression() == blp::Blp::Compression::Paletted)
	{
		format = QImage::Format_ARGB32;
	}
	/*
	 * Unsupported compression.
	 */
	else
	{
		return false;
	}

	QImage result(boost::numeric_cast<int>(mipMap.width()), boost::numeric_cast<int>(mipMap.height()), format);

	if (paletted)
	{
		result.setColorCount(blp::Blp::compressedPaletteSize);

		for (int index = 0; index < result.colorCount(); ++index)
		{
			result.setColor(index, colorToArgb(blpImage.palette()[index]));
		}
	}

	for (blp::dword width = 0; width < mipMap.width(); ++width)
	{
		for (blp::dword height = 0; height < mipMap.height(); ++height)
		{
			if (blpImage.compression() == blp::Blp::Compression::Paletted)
			{
				/*
				 * Only if it does not use a custom alpha value per pixel a palette can be used.
				 */
				if (blpImage.pictureType() == blp::Blp::PictureType::PalettedWithoutAlpha)
				{
					result.setPixel(width, height, mipMap.paletteIndexAt(width, height));
				}
				/*
				 * If each pixel has its custom alpha value we cannot use a paletted compression
				 */
				else if (blpImage.pictureType() == blp::Blp::PictureType::PalettedWithAlpha1 || blpImage.pictureType() == blp::Blp::PictureType::PalettedWithAlpha2)
				{
					// use the custom alpha value from the pixel and not the one from the palette
					result.setPixel(width, height, colorToArgb(mipMap.colorAt(width, height).paletteColor(blpImage.palette().get())));
				}
			}
			else if (blpImage.compression() == blp::Blp::Compression::Jpeg)
			{
				result.setPixel(width, height, colorToArgb(mipMap.colorAt(width, height).rgba()));
			}

		}
	}

	*image = result;

	return true;
}

bool BlpIOHandler::read(QImage *image, const blp::Blp &blpImage)
{
	if (blpImage.mipMaps().empty()) // no MIP maps
	{
		return false;
	}

	read(image, blpImage.mipMaps().front(), blpImage);

	return true;
}

bool BlpIOHandler::write(const QImage &image, blp::Blp *blpImage)
{
	blpImage->setFormat(blp::Blp::Format::Blp1);

	// TODO there is no way to detect if the image is paletted but with custom alpha values?!
	if (image.format() == QImage::Format_Indexed8)
	{
		qDebug() << "Is paletted";
		blpImage->setCompression(blp::Blp::Compression::Paletted);
	}
	else
	{
		blpImage->setCompression(blp::Blp::Compression::Jpeg);
	}

	if (blpImage->compression() == blp::Blp::Compression::Paletted)
	{
		if (image.format() == QImage::Format_Indexed8)
		{
			for (int i = 0; i < image.colorTable().size(); ++i)
			{
				blpImage->palette()[i] = image.colorTable()[i];
			}
		}
		/// \todo Otherwise we would need to generate our color table - very slow and  limited to 256 different colors.
		else
		{
			return false;
		}
	}

	if (image.hasAlphaChannel())
	{
		blpImage->setFlags(blp::Blp::Flags::Alpha);
	}
	else
	{
		blpImage->setFlags(blp::Blp::Flags::NoAlpha);
	}

	blpImage->setWidth(image.width());
	blpImage->setHeight(image.height());
	// TODO when to use with alpha??
	if (blpImage->compression() == blp::Blp::Compression::Paletted)
	{
		blpImage->setPictureType(blp::Blp::PictureType::PalettedWithoutAlpha);
	}

	// generate all necessary MIP maps
	// palette is filled automatically by Blp::write
	if (blpImage->generateRequiredMipMaps() == 0)
	{
		return false;
	}

	for (std::size_t i = 0; i < blpImage->mipMaps().size(); ++i)
	{
		blp::Blp::MipMap &mipMap = blpImage->mipMaps()[i];
		const QImage scaledImage = image.scaled(mipMap.width(), mipMap.height());

		if (scaledImage.width() != boost::numeric_cast<int>(mipMap.width()) || scaledImage.height() != boost::numeric_cast<int>(mipMap.height()))
		{
			return false;
		}

		for (blp::dword width = 0; width < mipMap.width(); ++width)
		{
			for (blp::dword height = 0; height < mipMap.height(); ++height)
			{
				if (blpImage->compression() == blp::Blp::Compression::Paletted)
				{
					// TODO support custom alpha value!
					const int index = scaledImage.pixelIndex(width, height); // index has to be set because paletted compression can also be used
					mipMap.setColorIndex(width, height, boost::numeric_cast<blp::byte>(index));
				}
				else
				{
					// set color
					const QRgb rgb = scaledImage.pixel(width, height);
					const blp::color argb = argbToColor(rgb);
					mipMap.setColorRgba(width, height, argb);
				}
			}
		}
	}

	return true;
}

blp::BlpHeader BlpIOHandler::header() const
{
	blp::BlpHeader header;
	device()->peek(reinterpret_cast<char*>(&header), sizeof(header));

	return header;
}

}

}
