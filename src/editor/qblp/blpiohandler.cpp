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

	return (this->device() != 0 && this->device()->isReadable() && this->device()->peek(reinterpret_cast<char*>(&identifier), sizeof(identifier)) == sizeof(identifier) && blp::Blp::hasFormat(reinterpret_cast<blp::byte*>(&identifier), sizeof(identifier)));
}

bool BlpIOHandler::read(QImage *image)
{
	// TEST
	/*
	QFile *file = dynamic_cast<QFile*>(this->device());
	qDebug() << "File error: " << file->error();
	qDebug() << "File name: " << file->fileName();
	qDebug() << "Device error string 1: " << this->device()->errorString();
	qDebug() << "Is open: " << this->device()->isOpen();
	qDebug() << "Is readable: " << this->device()->isReadable();
	qDebug() << "At end: " << this->device()->atEnd();
	qDebug() << "Bytes available: " << this->device()->bytesAvailable();
	*/
	// TEST END
	
	// read buffer into input stream
	const QByteArray all = this->device()->readAll();
	
	//qDebug() << "All size: " << all.size();
	//qDebug() << "Device error string 2: " << this->device()->errorString();
	
	//char *buffer = all.data(); // NOTE Store buffer for non const treatment!
	iarraystream istream(all.constData(), all.size()); // copies buffer data!
	//istream.rdbuf()->pubsetbuf(buffer, all.size()); TODO setting buffer directly does not work
	
	// TEST
	/*
	try
	{
		wc3lib::checkStream(istream);
	}
	catch (Exception &e)
	{
		qDebug() << e.what().c_str();
	}
	*/
	// TEST END
	
	QScopedPointer<blp::Blp> blpImage(new blp::Blp());

	try
	{
		blpImage->read(istream, 1, blp::Blp::defaultThreads); // only read the first MIP map, QImage does not support MIP mapping
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

bool BlpIOHandler::supportsOption(ImageOption option) const
{
	switch (option)
	{
		case SubType:
			return true;

		case Size:
			return true;

		case Quality:
			return this->option(SubType).toUInt() == blp::Blp::Compression::Jpeg;
	}

	return false;
}

QVariant BlpIOHandler::option(ImageOption option) const
{
	switch (option)
	{
		case SubType:
		{
			if (!canRead())
				return QVariant();

			return QVariant(header().compression);
		}

		case Size:
		{
			if (!canRead())
				return QVariant();

			blp::BlpHeader header(this->header());

			return QSize(header.width, header.height);
		}

		case Quality:
		{
			if (!canRead())
				return QVariant();

			blp::dword compression(this->option(SubType).toUInt());

			if (compression == blp::Blp::Compression::Jpeg)
			{
				/// \todo Read JPEG header of first MIP map by using jpeg lib and get compression.
				return 0;
			}
		}
	}

	return QVariant();
}

bool BlpIOHandler::write(const QImage &image)
{
	QScopedPointer<blp::Blp> blpImage(new blp::Blp());

	if (!write(image, blpImage.data()))
		return false;

	ostringstream ostream;

	try
	{
		blpImage->write(ostream, option(Quality).toInt(), blp::Blp::defaultMipMaps, blp::Blp::defaultThreads);
	}
	catch (class Exception &exception)
	{
		qDebug() << "BLP Output Exception: " << exception.what().c_str();

		return false;
	}

	const std::string data = ostream.str();
	// NOTE using arraysink as output device is useless since it doesn't use any buffer and we don't know the exact size
	//std::streamsize bufferSize = ostream.rdbuf()->in_avail();
	//boost::scoped_array<char> buffer(new char[bufferSize]);
	//ostream.rdbuf()->sgetn(buffer.get(), bufferSize);
	
	this->device()->write(data.c_str(), data.size());

	return true;
}

bool BlpIOHandler::read(QImage *image, const blp::Blp::MipMap &mipMap, const blp::Blp &blpImage)
{
	QImage::Format format;

	if (blpImage.compression() == blp::Blp::Compression::Paletted)
		format = QImage::Format_Indexed8;
	else if (blpImage.flags() & blp::Blp::Flags::Alpha)
		format = QImage::Format_ARGB32;
	else
		format = QImage::Format_RGB32;

	QImage result(boost::numeric_cast<int>(mipMap.width()), boost::numeric_cast<int>(mipMap.height()), format);

	if (blpImage.compression() != blp::Blp::Compression::Paletted)
	{
		for (blp::dword width = 0; width < mipMap.width(); ++width)
		{
			for (blp::dword height = 0; height < mipMap.height(); ++height)
				result.setPixel(width, height,  colorToRgba(mipMap.colorAt(width, height).argb())); // numeric_cast has already been done!
		}
	}
	else
	{
		result.setColorCount(blp::Blp::compressedPaletteSize);

		for (int index = 0; index < result.colorCount(); ++index)
			result.setColor(index, colorToRgba(blpImage.palette()[index]));

		for (blp::dword width = 0; width < mipMap.width(); ++width)
		{
			for (blp::dword height = 0; height < mipMap.height(); ++height)
				result.setPixel(width, height, mipMap.colorAt(width, height).paletteIndex()); // numeric_cast has already been done!
		}
	}

	*image = result;

	return true;
}

bool BlpIOHandler::read(QImage *image, const blp::Blp &blpImage)
{
	if (blpImage.mipMaps().empty()) // no MIP maps
		return false;

	read(image, blpImage.mipMaps().front(), blpImage);

	return true;
}

bool BlpIOHandler::write(const QImage &image, blp::Blp *blpImage)
{
	if (!option(SubType).isNull())
	{
		blpImage->setCompression(BOOST_SCOPED_ENUM(blp::Blp::Compression)(option(SubType).toUInt()));
		qDebug() << "SubType has been set to " << BOOST_SCOPED_ENUM(blp::Blp::Compression)(option(SubType).toUInt());
	}
	else if (image.format() == QImage::Format_Indexed8)
	{
		qDebug() << "Is paletted";
		blpImage->setCompression(blp::Blp::Compression::Paletted);
	}
	else
		blpImage->setCompression(blp::Blp::Compression::Jpeg);

	if (blpImage->compression() == blp::Blp::Compression::Paletted)
	{
		if (image.format() == QImage::Format_Indexed8)
		{
			for (int i = 0; i < image.colorTable().size(); ++i)
				blpImage->palette()[i] = image.colorTable()[i];
		}
		/// \todo Otherwise we would need to generate our color table - very slow and  limited to 256 different colors.
		else
			return false;
	}

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
	if (blpImage->generateMipMaps(1) != 1)
		return false;

	blp::Blp::MipMap &mipMap = blpImage->mipMaps()[0];

	for (int width = 0; width < image.width(); ++width)
	{
		for (int height = 0; height < image.height(); ++height)
		{
			if (blpImage->compression() == blp::Blp::Compression::Paletted)
			{
				const int index = image.pixelIndex(width, height); // index has to be set because paletted compression can also be used
				mipMap.setColorIndex(width, height, index);
			}
			else
			{
				// set color
				const QRgb rgb = image.pixel(width, height);
				const blp::color argb = rgbaToColor(rgb);
				mipMap.setColor(width, height, argb);
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
