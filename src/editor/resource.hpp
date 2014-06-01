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

#include <KUrl>

#include "../core.hpp"
#include "kio_mpq/mpqprotocol.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Abstract base class for all kind of resources.
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

		Resource(const KUrl &url, Type type);
		virtual ~Resource();
		/**
		 * Assigns the corresponding source.
		 * \param load If this value is true resource will be loaded/reloaded automatically.
		 * \note This adds the resources to the resources of \p source which can be accessed via \ref Source::resources().
		 * \note Additionally, this calls either \ref load() or \ref reload() depending on there has already set a source before. This only happens if \p load is true!
		 */
		void setSource(class MpqPriorityList *source, bool load = false) throw (Exception);
		class MpqPriorityList* source() const;
		/**
		 * Each resource should have its unique URL which indicates where it is loaded from.
		 * Relative URLs will be checked from all sources paths.
		 * \sa MpqPriorityList::download()
		 * \sa MpqPriorityList::upload()
		 */
		const KUrl& url() const;
		Type type() const;
		/**
		 * Formatted URL used as source.
		 * \note This should ALWAYS return the same URL for one resource it is added and removed as source whenever the resource is added or removed to or from a \ref MpqPriorityList instance.
		 */
		KUrl sourceUrl() const;

		virtual void load() throw (Exception) = 0;
		virtual void reload() throw (Exception) = 0;
		virtual void save(const KUrl &url) const throw (Exception) = 0;

	protected:
		class MpqPriorityList *m_source;
		KUrl m_url;
		Type m_type;
};

inline class MpqPriorityList* Resource::source() const
{
	return m_source;
}

inline const KUrl& Resource::url() const
{
	return m_url;
}

inline Resource::Type Resource::type() const
{
	return m_type;
}

inline KUrl Resource::sourceUrl() const
{
	KUrl result = this->url();

	// add additional archive source
	if (this->type() == Resource::Type::Map || this->type() == Resource::Type::Campaign)
	{
		if (result.protocol() != "mpq")
		{
			result.setProtocol("mpq");
		}
	}
	// add file's directory as source for relative paths as textures
	else if (this->type() == Resource::Type::Model)
	{
		result.setPath(QFileInfo(result.toLocalFile()).dir().absolutePath()); // TODO remove file info?
	}

	return result;
}

}

}

#endif
