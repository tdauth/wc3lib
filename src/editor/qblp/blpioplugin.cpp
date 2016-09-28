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

#include "blpioplugin.hpp"
#include "blpiohandler.hpp"
#include "../../blp/blp.hpp"

namespace wc3lib
{

namespace editor
{

BlpIOPlugin::BlpIOPlugin(QObject *parent) : QImageIOPlugin(parent)
{
}

BlpIOPlugin::~BlpIOPlugin()
{
}

BlpIOPlugin::Capabilities BlpIOPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
	if (format.toLower() == "blp")
	{
		return QImageIOPlugin::CanRead | QImageIOPlugin::CanWrite;
	}

	if (!(format.isEmpty() && device->isOpen()))
	{
		return 0;
	}

	Capabilities cap;
	blp::dword identifier;

	if (device->isReadable() && device->peek(reinterpret_cast<char*>(&identifier), sizeof(identifier)) == sizeof(identifier) && blp::Blp::hasFormat(reinterpret_cast<blp::byte*>(&identifier), sizeof(identifier)))
	{
		cap |= CanRead;
	}

	if (device->isWritable())
	{
		cap |= CanWrite;
	}

	return cap;
}

QImageIOHandler* BlpIOPlugin::create(QIODevice *device, const QByteArray &format) const
{
	BlpIOHandler *result = new BlpIOHandler();
	result->setDevice(device);
	result->setFormat(format);

	return result;
}

}

}

#include "moc_blpioplugin.cpp"

