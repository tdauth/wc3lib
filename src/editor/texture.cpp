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

#include <QFileInfo>

#include <KTemporaryFile>
#include <KIO/AccessManager>
#include <KIO/NetAccess>

#include <Ogre.h>

#include "texture.hpp"
#include "blpiohandler.hpp"
#include "blpcodec.hpp"

namespace wc3lib
{

namespace editor
{

Texture::Texture(const KUrl &url) : Resource(url, Type::Texture)
{
}

Texture::~Texture()
{
}

void Texture::loadBlp() throw (Exception)
{
	if (hasBlp())
		return;
	
	if (!hasQt()) /// TODO cannot convert from OGRE to BLP.
	{
		QString tmpFileName;
		
		if (!KIO::NetAccess::download(url(), tmpFileName, 0))
			throw Exception(boost::format(_("Unable to download file from URL \"%1%\".")) % url().toLocalFile().toUtf8().constData());
		
		boost::filesystem::basic_ifstream<blp::byte> ifstream(tmpFileName.toUtf8().constData(), std::ios::binary | std::ios::in);
		
		if (!ifstream)
			throw Exception(boost::format(_("Unable to open temporary file \"%1%\".")) % tmpFileName.toUtf8().constData());
		
		BlpPtr blpImage(new blp::Blp());
		
		blpImage->read(ifstream);
		
		m_blp.swap(blpImage); // exception safe (won't change image if ->read throws exception
	}
	// if we have already an image it seems to be faster to read from it instead of the original file
	else
	{
		BlpIOHandler ioHandler;
		BlpPtr blpImage(new blp::Blp());
		
		if (!ioHandler.write(*qt().get(), blpImage.get()))
			throw Exception(_("Unable to convert Qt image into BLP."));
		
		blpImage.swap(m_blp); // exception safe (won't change image if handler has some error
	}
}

void Texture::loadQt() throw (Exception)
{
	if (hasQt())
		return;
	
	// if we have already an image it seems to be faster to read from it instead of the original file
	if (hasBlp())
	{
		BlpIOHandler ioHandler;
		QtPtr qtImage(new QImage());
		
		if (!ioHandler.read(qtImage.get(), *m_blp.get()))
			throw Exception(_("Unable to convert BLP image into Qt."));
		
		m_qt.swap(qtImage); // exception safe (won't change image if handler has some error
	}
	/// TODO cannot convert from OGRE to Qt.
	/*
	 * else if (hasOgre())
	 */
	else
	{
		QString tmpFileName;
		
		if (!KIO::NetAccess::download(url(), tmpFileName, 0))
			throw Exception(boost::format(_("Unable to download file from URL \"%1%\".")) % url().toLocalFile().toUtf8().constData());
		
		QtPtr qtImage(new QImage());
		
		if (!qtImage->load(tmpFileName))
			throw Exception(boost::format(_("Unable to load Qt image from temporary file \"%1%\".")) % tmpFileName.toUtf8().constData());
		
		m_qt.swap(qtImage); // exception safe (won't change image if ->read throws exception
	}
}

void Texture::loadOgre() throw (Exception)
{
	if (hasOgre())
		return;
	
	// if we have already an image it seems to be faster to read from it instead of the original file
	if (hasBlp())
	{
		OgrePtr ogreImage(new Ogre::Image());
		BlpCodec codec;
		Ogre::DataStreamPtr ptr(codec.decode(*m_blp.get()).first.get());
		ogreImage->load(ptr);
		
		m_ogre.swap(ogreImage); // exception safe (won't change image if handler has some error
	}
	// if we have already an image it seems to be faster to read from it instead of the original file
	else if (hasQt())
	{
	}
	// read from file
	else
	{
		QString tmpFileName;
		
		if (!KIO::NetAccess::download(url(), tmpFileName, 0))
			throw Exception(boost::format(_("Unable to download file from URL \"%1%\".")) % url().toLocalFile().toUtf8().constData());
		
		OgrePtr ogreImage(new Ogre::Image());
		ogreImage->load(tmpFileName.toUtf8().constData(), "");
		// TODO check correct loading state
		
		m_ogre.swap(ogreImage); // exception safe (won't change image if ->read throws exception
	}
}

void Texture::loadAll() throw (Exception)
{
	loadBlp();
	loadQt();
	loadOgre();
}

namespace
{

inline QString compressionOption(const QStringList &list, const QString key)
{
	const int index = list.indexOf("Quality");
	
	if (index == -1)
		return "";
	
	const int charIndex = list[index].indexOf('=');
	
	if (charIndex == -1 || charIndex == list[index].length() - 1)
		return "";
	
	return list[index].mid(charIndex + 1);
}

}

void Texture::save(const KUrl &url, const QString &format, const QString &compression) throw (Exception)
{
	KTemporaryFile tmpFile;
	
	if (!tmpFile.open())
		throw Exception(boost::format(_("Temporary file \"%1%\" cannot be opened.")) % tmpFile.fileName().toUtf8().constData());
	
	// get all compression options
	const QStringList compressionOptions = compression.split(":");
	int quality = -1;
	QString qualityString = compressionOption(compressionOptions, "Quality");
	std::size_t mipMaps = 0;
	QString mipMapsString = compressionOption(compressionOptions, "MipMaps");

	/// \todo qualityString.isNumeric.
	if (qualityString.toInt() >= 0 && qualityString.toInt() <= 100)
		quality = qualityString.toInt();
	
	/// \todo mipMapsString.isNumeric.
	if (mipMapsString.toInt() >= 1 && qualityString.toInt() <= blp::Blp::maxMipMaps)
		mipMaps = mipMapsString.toInt();
	
	if (format == "blp" && hasBlp())
	{
		std::basic_stringstream<blp::byte> sstream(std::ios::binary | std::ios::out);
		blp()->write(sstream, quality, mipMaps);
		const std::streampos position = endPosition(sstream);
		boost::scoped_array<blp::byte> buffer(new blp::byte[position + (std::streampos)1]);
		sstream.read(buffer.get(), position + (std::streampos)1);
		tmpFile.write(buffer.get(), position + (std::streampos)1);
	}
	else if (hasQt())
	{
		qt()->save(&tmpFile, format.toUtf8().constData(), quality);
	}
	/// TODO we cannot convert OGRE images (write into stream etc.)
	else
	{
		throw Exception(boost::format(_("Temporary file \"%1%\" cannot be converted by using an OGRE image: Not implemented yet!")) % tmpFile.fileName().toUtf8().constData());
	}
	
	/// TODO Indeed temporary files should be autoremoved.
	BOOST_SCOPE_EXIT((&tmpFile))
	{
		tmpFile.remove();
	} BOOST_SCOPE_EXIT_END
	
	if  (!KIO::NetAccess::upload(tmpFile.fileName(), url, 0))
		throw Exception(boost::format(_("Unable to upload temporary file \"%1%\" to URL \"%2%\"")) % tmpFile.fileName().toUtf8().constData() % url.toEncoded().constData());
}

}

}
