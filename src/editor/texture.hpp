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
#include <QScopedPointer>

#include <kdemacros.h>

#include <OgreImage.h>
#include <OgreTexture.h>

#include "resource.hpp"
#include "../core.hpp"
#include "../blp.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Provides access to one single texture in four different ways. Firstly there is an instance of \ref blp::Blp if it's a BLP image which provides you direct access to MIP map color data/color palette etc., secondly there is access to an object of \ref QImage which can be used to display your texture in your GUI and finally there is access to an object of \ref Ogre::Image which can be used to display your texture in your 3D scene and its corresponding \ref Ogre::Texture instance.
 * You do not have to store all four objects or any of them! Everything is optional!
 * Please consider that this class tries to get the most performant way to load all those objects by checking if it already has one of them and if so, using the object's buffer instead of loading it from the corresponding file again which usually is much slower than reading from memory.
 * Besides when saving the texture via \ref Texture::save() it tries to get the most performant way, as well. For example, if you save your texture as BLP image and there is a \ref blp::Blp instance already in your texture (\ref Texture::hasBlp()) it won't save the texture via its Qt or OGRE object since it's much faster to write the BLP object on disk.
 */
class KDE_EXPORT Texture : public Resource
{
	public:
		typedef QScopedPointer<blp::Blp> BlpPtr;
		typedef QScopedPointer<QImage> QtPtr;
		typedef QScopedPointer<Ogre::Image> OgrePtr;
		typedef Ogre::TexturePtr OgreTexturePtr;

		Texture(const KUrl &url);
		virtual ~Texture();

		void clearBlp();
		void clearQt();
		void clearOgre();
		void clearOgreTexture();

		/**
		 * Frees all allocated texture related data.
		 * \sa reload()
		 */
		virtual void clear() throw ();

		/**
		 * \exception Exception Exception safe!
		 */
		virtual void loadBlp(const QMap<QString, QString> &options);
		virtual void loadBlp()
		{
			loadBlp(QMap<QString, QString>());
		}

		/**
		 * \exception Exception Exception safe!
		 */
		virtual void loadQt();
		/**
		 * \exception Exception Exception safe!
		 */
		virtual void loadOgre();
		/**
		 * \exception Exception Exception safe!
		 */
		virtual void loadOgreTexture();
		/**
		 * \exception Exception Exception safe!
		 */
		virtual void loadAll();
		virtual void load() { loadAll(); }
		/**
		 * Calls \ref clear() and frees everything. Afterwards it loads all images which has been allocated formerly.
		 */
		virtual void reload();

		/**
		 * Saves texture at \p url with format \p format and compression options \p compression.
		 * \param format If this value is empty format is detected by file's extension. If no valid format could be detected first usable is used in order BLP, Qt and OGRE.
		 * \param compression Defines compression options as key-value pairs.
		 * Supported compression keys are:
		 * <ul>
		 * <li>"Quality" - -1-100 where -1 means undefined/default quality and 100 means best. Supported by all Qt supported image formats and BLP JFIF compresion.</li>
		 * <li>"MipMaps" - 0-16 where 0 means undefined/default number of MIP maps and 16 is maximum. Supported by BLP format only.</li>
		 * <li>"SharedHeader" - true/1/TRUE means that a shared header is written for BLP files (default).</li>
		 * </ul>
		 * \todo Option quality only works for non-BLP formats.
		 * \exception Exception Exception safe!
		 */
		virtual void save(const KUrl &url, const QString &format, const QMap<QString, QString> &compression) const;
		virtual void save(const KUrl &url) const
		{
			save(url, "", QMap<QString, QString>());
		}

		bool hasBlp() const;
		bool hasQt() const;
		bool hasOgre() const;
		bool hasOgreTexture() const;
		bool hasAll() const;

		const BlpPtr& blp() const;
		const QtPtr& qt() const;
		const OgrePtr& ogre() const;
		const OgreTexturePtr& ogreTexture() const;

	private:
		BlpPtr m_blp;
		QtPtr m_qt;
		OgrePtr m_ogre;
		OgreTexturePtr m_ogreTexture;
};

inline bool Texture::hasBlp() const
{
	return blp().data() != 0;
}

inline bool Texture::hasQt() const
{
	return qt().data() != 0;
}

inline bool Texture::hasOgre() const
{
	return ogre().data() != 0;
}

inline bool Texture::hasOgreTexture() const
{
	return !ogreTexture().isNull();
}

inline bool Texture::hasAll() const
{
	return hasBlp() && hasQt() && hasOgre() && hasOgreTexture();
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

inline const Texture::OgreTexturePtr& Texture::ogreTexture() const
{
	return m_ogreTexture;
}


}

}

#endif
