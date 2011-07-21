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

		/// Do not add to list \p source since sometimes it is discarded because of errors after loading it.
		Resource(class MpqPriorityList *source, const KUrl &url, BOOST_SCOPED_ENUM(Type) type) : m_source(source), m_url(url), m_type(type) { };
		class MpqPriorityList* source() const { return m_source; }
		void setSource(class MpqPriorityList *source)
		{
			if (this->source() == source)
				return;
			
			m_source = source;
			reload();
		}
		const KUrl& url() const { return m_url; };
		BOOST_SCOPED_ENUM(Type) type() const { return m_type; };
		
		virtual void load() throw (Exception) = 0;
		virtual void reload() throw (Exception) = 0;
		virtual void save(const KUrl &url) const throw (Exception) = 0;

	protected:
		template<class T>
		friend void boost::checked_delete(T*); // for destruction by shared ptr
		
		virtual ~Resource() { };
		
		class MpqPriorityList *m_source;
		KUrl m_url;
		BOOST_SCOPED_ENUM(Type) m_type;
};

}

}

#endif
