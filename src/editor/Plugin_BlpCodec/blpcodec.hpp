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

#ifndef WC3LIB_EDITOR_BLPCODEC_HPP
#define WC3LIB_EDITOR_BLPCODEC_HPP

#include <Ogre.h>
#include <OgreImageCodec.h>

#include "../../blp.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * \todo Add MIP map support.
 */
class BlpCodec : public Ogre::ImageCodec
{
	public:
		/**
		 * Needs to be called to register BLP codec for OGRE.
		 * Usually \ref Editor handles this automatically.
		 */
		static void startup();
		/**
		 * Needs to be called to unregister BLP codec from OGRE.
		 */
		static void shutdown();

		virtual ~BlpCodec();
		Ogre::String getDataType() const override;
		/**
		 * \todo Is still not supported due to complexity of implementation (you have to support all other formats).
		 */
		virtual Ogre::DataStreamPtr encode(Ogre::MemoryDataStreamPtr &input, CodecDataPtr &pData) const override;
		/**
		 * \todo Is still not supported due to complexity of implementation (you have to support all other formats).
		 */
		virtual void encodeToFile(Ogre::MemoryDataStreamPtr &input, const Ogre::String &outFileName, CodecDataPtr &pData) const override;
		/// \todo Support alpha value and paletted image.
		virtual DecodeResult decode(const blp::Blp &blp) const;
		virtual DecodeResult decode(Ogre::DataStreamPtr &input) const override;
		virtual Ogre::String getType () const override;
		virtual bool magicNumberMatch(const char *magicNumberPtr, size_t maxbytes) const override;
		virtual Ogre::String magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const override;

	protected:
		static BlpCodec *m_self;
};

}

}

#endif
