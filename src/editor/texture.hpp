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

#ifndef WC3LIB_EDITOR_TEXTURE_HPP
#define WC3LIB_EDITOR_TEXTURE_HPP

#include <QImage>

#include <OgreImage.h>

#include "resource.hpp"
#include "../core.hpp"
#include "../blp.hpp"

namespace wc3lib
{

namespace editor
{

class Texture : public Resource
{
	public:
		typedef boost::scoped_ptr<blp::Blp> BlpPtr;
		typedef boost::scoped_ptr<QImage> QtPtr;
		typedef boost::scoped_ptr<Ogre::Image> OgrePtr;
		
		Texture(const KUrl &url);
		virtual ~Texture();
		
		/**
		 * \exception Exception Exception safe!
		 */
		virtual void loadBlp() throw (Exception);
		/**
		 * \exception Exception Exception safe!
		 */
		virtual void loadQt() throw (Exception);
		/**
		 * \exception Exception Exception safe!
		 */
		virtual void loadOgre() throw (Exception);
		/**
		 * \exception Exception Exception safe!
		 */
		virtual void loadAll() throw (Exception);
		
		/**
		 * Saves texture at \p url with format \p format and compression options \p compression.
		 * \param format If this value is empty format is detected by file's extension. If no valid format could be detected first usable is used in order BLP, Qt and OGRE.
		 * \param compression Defines compression options as key-value pairs separated by : char. Example: Quality=10:OtherKey=Value.
		 * Supported compression keys are:
		 * <ul>
		 * <li>"Quality" - -1-100 where -1 means undefined/default quality and 100 means best. Supported by all Qt supported image formats and BLP JFIF compresion.</li>
		 * <li>"MipMaps" - 0-16 where 0 means undefined/default number of MIP maps and 16 is maximum. Supported by BLP format only.</li>
		 * </ul>
		 * \todo Option quality only works for non-BLP formats.
		 * \exception Exception Exception safe!
		 */
		virtual void save(const KUrl &url, const QString &format = "blp", const QString &compression = "") throw (Exception);

		bool hasBlp() const;
		bool hasQt() const;
		bool hasOgre() const;
		bool hasAll() const;
		
		const BlpPtr& blp() const;
		const QtPtr& qt() const;
		const OgrePtr& ogre() const;
		
	private:
		BlpPtr m_blp;
		QtPtr m_qt;
		OgrePtr m_ogre;
		
};

inline bool Texture::hasBlp() const
{
	return blp().get() != 0;
}

inline bool Texture::hasQt() const
{
	return qt().get() != 0;
}

inline bool Texture::hasOgre() const
{
	return ogre().get() != 0;
}

inline bool Texture::hasAll() const
{
	return hasBlp() && hasQt() && hasOgre();
}

inline const Texture::BlpPtr& Texture::blp() const
{
	return m_blp;
}

inline const Texture::QtPtr& Texture::qt() const
{
	return m_qt;
}

inline const Texture::OgrePtr& Texture::ogre() const
{
	return m_ogre;
}

}

}

#endif
