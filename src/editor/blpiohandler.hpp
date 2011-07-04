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

#ifndef WC3LIB_EDITOR_BLPIOHANDLER_HPP
#define WC3LIB_EDITOR_BLPIOHANDLER_HPP

#include <QtGui/QImageIOHandler>

#include "../blp/blp.hpp"

namespace wc3lib
{

namespace editor
{

/**
* Provides access to Blizzard's image format.
* You do not necessarily have to use a device to read from. There are also member functions which do only use BLP objects.
* @see BlpIOPlugin
*/
class BlpIOHandler : public QImageIOHandler
{
	public:
		BlpIOHandler();
		virtual ~BlpIOHandler();
		virtual bool canRead() const;
		/**
		* Assigns data from device to \p image.
		*/
		virtual bool read(QImage *image);
		virtual bool supportsOption(ImageOption option) const;
		/**
		* Writes data from \p image to device.
		*/
		virtual bool write(const QImage &image);
		
		/**
		 * Assigns data from \p blp to \p image.
		 */
		virtual bool read(QImage *image, const blp::Blp &blpImage);
		/**
		 * Writes data from \p image to \p blpImage.
		 */
		virtual bool write(const QImage &image, blp::Blp *blpImage);
};

}

}

#endif
