/***************************************************************************
 *   Copyright (C) 2010 by Tamino Dauth                                    *
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

#ifndef WC3LIB_EDITOR_RESOURCE_HPP
#define WC3LIB_EDITOR_RESOURCE_HPP

#include <QFileInfo>
#include <QDir>
#include <QUrl>

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;

/**
 * \brief Abstract base class for all kind of resources.
 * Provides virtual load, reload and save member functions and dynamic type information.
 * Besides each resource has its corresponding file URL and data source which makes all resource instance completly independent from each other.
 * Resources depend on \ref MpqPriorityList instances, called sources, when being loaded or saved. Therefore one single source instance can be assigned to each resource.
 */
class Resource
{
	public:
		/**
		 * The resource's type affects its treatment when added to an \ref MpqPriorityList instance for example.
		 * Campaigns and map's are added as additional source entries with their mpq:/ URL.
		 * Model's directories are added as additional source entries for corresponding textures with relative paths.
		 * \sa type(), sourceUrl()
		 */
		enum class Type
		{
			Texture,
			Model,
			Sound,
			Music,
			Map,
			Campaign,
			MetaData
		};

		Resource(const QUrl &url, Type type);
		virtual ~Resource();
		/**
		 * Assigns the corresponding source.
		 * Removes the resource from its old source and adds it to \p source.
		 * It the can be accessed via \p MpqPriorityList::resources().
		 *
		 * \sa source()
		 */
		void setSource(MpqPriorityList *source);
		MpqPriorityList* source() const;
		/**
		 * Each resource should have its unique URL which indicates where it is loaded from.
		 * Relative URLs will be checked from all sources paths.
		 * \sa MpqPriorityList::download()
		 * \sa MpqPriorityList::upload()
		 */
		const QUrl& url() const;
		Type type() const;

		virtual void load() = 0;
		virtual void reload() = 0;
		virtual void save(const QUrl &url) const = 0;

	protected:
		MpqPriorityList *m_source;
		QUrl m_url;
		Type m_type;
};

inline MpqPriorityList* Resource::source() const
{
	return m_source;
}

inline const QUrl& Resource::url() const
{
	return m_url;
}

inline Resource::Type Resource::type() const
{
	return m_type;
}

}

}

#endif
