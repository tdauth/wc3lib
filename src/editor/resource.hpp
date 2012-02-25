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

#include <boost/detail/scoped_enum_emulation.hpp>

#include <KUrl>

#include "../core.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Abstract base class for all kind of resources.
 * Provides virtual load, reload and save member functions and dynamic type information.
 * Besides each resource has its corresponding file URL and data source which makes all resource instance completly independent from each other.
 */
class Resource
{
	public:
		BOOST_SCOPED_ENUM_START(Type)
		{
			Texture,
			Model,
			Sound,
			Music,
			Map,
			Campaign,
			MetaData
		};
		BOOST_SCOPED_ENUM_END

		Resource(const KUrl &url, BOOST_SCOPED_ENUM(Type) type);
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
		BOOST_SCOPED_ENUM(Type) type() const;

		virtual void load() throw (Exception) = 0;
		virtual void reload() throw (Exception) = 0;
		virtual void save(const KUrl &url) const throw (Exception) = 0;

	protected:
		class MpqPriorityList *m_source;
		KUrl m_url;
		BOOST_SCOPED_ENUM(Type) m_type;
};

inline class MpqPriorityList* Resource::source() const
{
	return m_source;
}

inline const KUrl& Resource::url() const
{
	return m_url;
}

inline BOOST_SCOPED_ENUM(Resource::Type) Resource::type() const
{
	return m_type;
}

}

}

#endif
