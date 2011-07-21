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

#include <sstream>
#include <utility>

#include <boost/foreach.hpp>

#include <QtCore>
#include <QtGui>

#include "blpiohandler.hpp"

namespace wc3lib
{

namespace editor
{

namespace
{

/**
* \return Returns BLP ARGB color.
*/
inline blp::color rgbaToColor(QRgb rgba)
{
	return (blp::color)(qAlpha(rgba)) << 24 | (rgba & 0x00FFFFFF);
}

/**
* \return Returns Qt RGBA color.
*/
inline QRgb colorToRgba(blp::color c)
{
	return qRgba(blp::red(c), blp::green(c), blp::blue(c), blp::alpha(c));
}

}

BlpIOHandler::BlpIOHandler() : QImageIOHandler()
{
}

BlpIOHandler::~BlpIOHandler()
{
}

bool BlpIOHandler::canRead() const
{
	blp::dword identifier;

	if (this->device() != 0 && this->device()->isReadable() && this->device()->peek(reinterpret_cast<char*>(&identifier), sizeof(identifier)) == sizeof(identifier) && blp::Blp::hasFormat(reinterpret_cast<blp::byte*>(&identifier), sizeof(identifier)))
		return true;

	return false;
}

bool BlpIOHandler::read(QImage *image)
{
	// read buffer into input stream
	QByteArray all = this->device()->readAll();
	std::basic_istringstream<blp::byte> istream;
	istream.rdbuf()->pubsetbuf(reinterpret_cast<blp::byte*>(all.data()), all.size());
	boost::scoped_ptr<blp::Blp> blpImage(new blp::Blp());

	try
	{
		blpImage->read(istream);
	}
	catch (class Exception &exception)
	{
		qDebug() << "BLP Input Exception: " << exception.what().c_str();

		return false;
	}

	if (!read(image, *blpImage))
		return false;

	return true;
}

/// @todo Where should we get the options from?!
bool BlpIOHandler::supportsOption(ImageOption option) const
{
	switch (option)
	{
		case QImageIOHandler::Quality:
			return true;
	}

	return false;
}

bool BlpIOHandler::write(const QImage &image)
{
	boost::scoped_ptr<blp::Blp> blpImage(new blp::Blp());

	if (!write(image, blpImage.get()))
		return false;
	
	std::basic_ostringstream<blp::byte> ostream;

	try
	{
		blpImage->write(ostream, option(Quality).toInt(), 0);
	}
	catch (class Exception &exception)
	{
		qDebug() << "BLP Output Exception: " << exception.what().c_str();

		return false;
	}

	std::streamsize bufferSize = ostream.rdbuf()->in_avail();
	char buffer[bufferSize];
	ostream.rdbuf()->sgetn(reinterpret_cast<blp::byte*>(buffer), bufferSize);
	this->device()->write(buffer, bufferSize);

	/// \todo Recognize image IO handler options!

	return true;
}

bool BlpIOHandler::read(QImage *image, const blp::Blp &blpImage)
{
	if (blpImage.mipMaps().empty()) // no mip maps
		return false;

	// write blp data into image
	//if (this->m_blp->flags() == blp::Blp::Alpha)
		//image->
	const blp::Blp::MipMap *mipMap = blpImage.mipMaps()[0].get(); // first mip map has original size

	if (blpImage.flags() & blp::Blp::Flags::Alpha)
		qDebug() << "Image has alpha channel";

	QImage::Format format;

	if (blpImage.compression() == blp::Blp::Compression::Paletted)
		format = QImage::Format_Indexed8;
	else if (blpImage.flags() & blp::Blp::Flags::Alpha)
		format = QImage::Format_ARGB32;
	else
		format = QImage::Format_RGB32;

	qDebug() << "Image has size (" << mipMap->width() << "|" << mipMap->height() << ").";
	image = new QImage(boost::numeric_cast<int>(mipMap->width()), boost::numeric_cast<int>(mipMap->height()), format);
	qDebug() << "Before colors";
	qDebug() << "Color map size " << mipMap->colors().size();
	qDebug() << "After colors.";

	if (blpImage.compression() != blp::Blp::Compression::Paletted)
	{
		foreach (blp::Blp::MipMap::MapEntryType mapEntry, mipMap->colors())
		{
			const blp::Blp::MipMap::Coordinates &coordinates = mapEntry.first;
			const blp::Blp::MipMap::Color &color = mapEntry.second;
			const QRgb pixelColor = colorToRgba(color.argb());
			image->setPixel(coordinates.first, coordinates.second, pixelColor);
		}
	}
	else
	{
		image->setColorCount(blp::Blp::compressedPaletteSize);
		
		for (int index = 0; index < image->colorCount(); ++index)
			image->setColor(index, colorToRgba(blpImage.palette()[index]));
		
		foreach (blp::Blp::MipMap::MapEntryType mapEntry, mipMap->colors())
		{
			const blp::Blp::MipMap::Coordinates &coordinates = mapEntry.first;
			const blp::Blp::MipMap::Color &color = mapEntry.second;
			image->setPixel(coordinates.first, coordinates.second, color.paletteIndex());
		}
	}
	
	return true;
}

bool BlpIOHandler::write(const QImage &image, blp::Blp *blpImage)
{
	if (image.format() == QImage::Format_Indexed8)
	{
		qDebug() << "Is paletted";
		blpImage->setCompression(blp::Blp::Compression::Paletted);
		
		for (int i = 0; i < image.colorTable().size(); ++i)
			blpImage->palette()[i] = image.colorTable()[i];
	}
	else
		blpImage->setCompression(blp::Blp::Compression::Jpeg);

	if (image.hasAlphaChannel())
		blpImage->setFlags(blp::Blp::Flags::Alpha);
	else
		blpImage->setFlags(blp::Blp::Flags::NoAlpha);

	blpImage->setWidth(image.width());
	blpImage->setHeight(image.height());
	blpImage->setPictureType(0);
	/*
	if (image.hasAlphaChannel())
		this->m_blp->setPictureType(3);
	else
		this->m_blp->setPictureType(5);
	*/

	// create mip map
	// palette is filled automatically by Blp::write
	blpImage->generateMipMaps(1);
	blp::Blp::MipMap *mipMap = blpImage->mipMaps()[0].get();

	for (int width = 0; width < image.size().width(); ++width)
	{
		for (int height = 0; height < image.size().height(); ++height)
		{
			// set color
			const QRgb rgb = image.pixel(width, height);
			int index = 0;

			if (blpImage->compression() == blp::Blp::Compression::Paletted)
				index = image.pixelIndex(width, height); // index has to be set because paletted compression can also be used

			const blp::color argb = rgbaToColor(rgb);
			mipMap->setColor(width, height, argb, qAlpha(rgb), index);
		}
	}

	blpImage->generateMipMaps(); // generate other MIP maps after setting up initial MIP map (image)
	
	return true;
}

}

}
