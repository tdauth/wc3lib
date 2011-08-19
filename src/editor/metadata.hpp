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

#ifndef WC3LIB_EDITOR_METADATA_HPP
#define WC3LIB_EDITOR_METADATA_HPP

#include "resource.hpp"
#include "../map.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Base class for all possible meta data formats (units, abilities, upgrades etc.).
 * Simply reimplement virtual member functions \ref createMetaDataEntry() and \ref filleMetaDataEntry() to create and fill your custom meta data.
 * \todo Add more abstract class which is the base of water data, sound data etc. as well.
 * Resource URL should refer a SYLK file.
 */
class MetaData : public Resource
{
	public:
		typedef QVector<QVariant> Row;
		typedef QVector<Row> Rows;
		
		typedef boost::shared_ptr<map::MetaData> MetaDataPtr;
		typedef std::vector<MetaDataPtr> MetaDataEntries;
		
		MetaData(class MpqPriorityList *source, const KUrl &url);
		
		virtual void clear() throw ();
		
		/**
		 * Provides simple SYLK file parser functionality.
		 */
		virtual void load() throw (Exception);
		virtual void reload() throw (Exception);
		virtual void save(const KUrl &url) const throw (Exception)
		{
			throw Exception(_("Saving meta data is not supported yet."));
		}
	
		class MpqPriorityList* source() const;
		const MetaDataEntries& metaDataEntries() const;
		
	protected:
		virtual map::MetaData* createMetaDataEntry();
		virtual void filleMetaDataEntry(MetaDataPtr &entry, const Row &row);
		
		class MpqPriorityList *m_source;
		MetaDataEntries m_metaDataEntries;
};

inline class MpqPriorityList* MetaData::source() const
{
	return m_source;
}

inline const MetaData::MetaDataEntries& MetaData::metaDataEntries() const
{
	return m_metaDataEntries;
}

}

}

#endif
