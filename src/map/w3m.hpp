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

#ifndef WC3LIB_MAP_W3M_HPP
#define WC3LIB_MAP_W3M_HPP

#include "../mpq.hpp"
#include "playable.hpp"
#include "environment.hpp"
#include "shadow.hpp"
#include "pathmap.hpp"
#include "trees.hpp"
#include "info.hpp"
#include "mapstrings.hpp"
#include "minimap.hpp"
#include "menuminimap.hpp"
#include "customunits.hpp"
#include "triggers.hpp"
#include "cameras.hpp"
#include "rects.hpp"
#include "sounds.hpp"
#include "customtexttriggers.hpp"
#include "importedfiles.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace map
{

class W3m : public mpq::Mpq, public Playable
{
	public:
		static const std::size_t signatureSize = 256;

		typedef boost::scoped_ptr<Environment> EnvironmentPtr;
		typedef boost::scoped_ptr<Shadow> ShadowPtr;
		typedef boost::scoped_ptr<Pathmap> PathmapPtr;
		typedef boost::scoped_ptr<Trees> TreesPtr;
		typedef boost::scoped_ptr<Info> InfoPtr;
		typedef boost::scoped_ptr<MapStrings> StringsPtr;
		typedef boost::scoped_ptr<Minimap> MinimapPtr;
		typedef boost::scoped_ptr<MenuMinimap> MenuMinimapPtr;
		typedef boost::scoped_ptr<CustomUnits> CustomUnitsPtr;
		typedef boost::scoped_ptr<Triggers> TriggersPtr;
		typedef boost::scoped_ptr<Cameras> CamerasPtr;
		typedef boost::scoped_ptr<Rects> RectsPtr;
		typedef boost::scoped_ptr<Sounds> SoundsPtr;
		typedef boost::scoped_ptr<CustomTextTriggers> CustomTextTriggersPtr;
		typedef boost::scoped_ptr<ImportedFiles> ImportedFilesPtr;
		typedef std::vector<FileFormat*> FileFormats;
		typedef boost::scoped_array<char8> Signature;

		W3m();
		virtual ~W3m();

		/**
		 * \param istream has to contain the map's header + the map's MPQ archive.
		 * Here's a list of all possible file names:
		 * <ul>
		 * <li>war3map.w3e</li>
		 * <li>war3map.w3i</li>
		 * <li>war3map.wtg</li>
		 * <li>war3map.wct</li>
		 * <li>war3map.wts</li>
		 * <li>war3map.j</li>
		 * <li>war3map.shd</li>
		 * <li>war3mapMap.blp</li>
		 * <li>war3mapMap.b00</li>
		 * <li>war3mapMap.tga</li>
		 * <li>war3mapPreview.tga</li>
		 * <li>war3map.mmp</li>
		 * <li>war3mapPath.tga</li>
		 * <li>war3map.wpm</li>
		 * <li>war3map.doo</li>
		 * <li>war3mapUnits.doo</li>
		 * <li>war3map.w3r</li>
		 * <li>war3map.w3c</li>
		 * <li>war3map.w3s</li>
		 * <li>war3map.w3u</li>
		 * <li>war3map.w3t</li>
		 * <li>war3map.w3a</li>
		 * <li>war3map.w3b</li>
		 * <li>war3map.w3d</li>
		 * <li>war3map.w3q</li>
		 * <li>war3mapMisc.txt</li>
		 * <li>war3mapSkin.txt</li>
		 * <li>war3mapExtra.txt</li>
		 * <li>war3map.imp</li>
		 * <li>war3mapImported\*.*</li>
		 * </ul>
		 * \note You can use classes's virtual member functions called "fileName" to get the corresponding file name of the class's format.
		 */
		virtual std::streamsize read(InputStream &istream, const mpq::Listfile::Entries &listfileEntries) throw (class Exception);
		virtual std::streamsize read(InputStream& istream) throw (class Exception)
		{
			return read(istream, mpq::Listfile::Entries());
		}
		/**
		 * Creates an MPQ archive with map header and all required files.
		 */
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);

		virtual int32 fileId() const;
		virtual int32 latestFileVersion() const;

		int32 width() const;
		int32 height() const;

		const EnvironmentPtr& environment() const;
		const ShadowPtr& shadow() const;
		const PathmapPtr& pathmap() const;
		const TreesPtr& trees() const;
		const CustomUnitsPtr& customUnits() const;
		const InfoPtr& info() const;
		const StringsPtr& strings() const;
		const MinimapPtr& minimap() const;
		const MenuMinimapPtr& menuMinimap() const;
		const TriggersPtr& triggers() const;
		const CamerasPtr& cameras() const;
		const RectsPtr& rects() const;
		const SoundsPtr& sounds() const;
		const CustomTextTriggersPtr& customTextTriggers() const;
		const ImportedFilesPtr& importedFiles() const;

		const FileFormats& fileFormats() const;
		FileFormats& fileFormats();

		bool hasSignature() const;
		/**
		 * If \ref hasSignature() is true it returns an array of \ref signatureSize bytes containing the digital signature of the map.
		 */
		const Signature& signature() const;

	protected:
		std::streamsize readHeader(InputStream &istream) throw (class Exception);
		std::streamsize readSignature(InputStream &istream) throw (class Exception);

		string m_name;
		BOOST_SCOPED_ENUM(MapFlags) m_flags;
		int32 m_maxPlayers;

		EnvironmentPtr m_environment;
		ShadowPtr m_shadow;
		PathmapPtr m_pathmap;
		TreesPtr m_trees;
		CustomUnitsPtr m_customUnits;
		InfoPtr m_info;
		StringsPtr m_strings;
		MinimapPtr m_minimap;
		MenuMinimapPtr m_menuMinimap;
		TriggersPtr m_triggers;
		CamerasPtr m_cameras;
		RectsPtr m_rects;
		SoundsPtr m_sounds;
		CustomTextTriggersPtr m_customTextTriggers;
		ImportedFilesPtr m_importedFiles;
		FileFormats m_fileFormats;
/*
w3x
		class ArtificialIntelligence *m_artificialIntelligence;
		class Misc *m_misc;
		class Skin *m_skin;
		class Extra *m_extra;
*/

		Signature m_signature; // size of \ref signatureSize

};

inline int32 W3m::fileId() const
{
	return *reinterpret_cast<const int32*>("HM3W");
}

inline int32 W3m::latestFileVersion() const
{
	return 0;
}

inline int32 W3m::width() const
{
	return this->m_environment->mapWidth();
}

inline int32 W3m::height() const
{
	return this->m_environment->mapHeight();
}

inline const W3m::EnvironmentPtr& W3m::environment() const
{
	return m_environment;
}

inline const W3m::ShadowPtr& W3m::shadow() const
{
	return m_shadow;
}

inline const W3m::PathmapPtr& W3m::pathmap() const
{
	return m_pathmap;
}

inline const W3m::TreesPtr& W3m::trees() const
{
	return m_trees;
}

inline const W3m::CustomUnitsPtr& W3m::customUnits() const
{
	return m_customUnits;
}

inline const W3m::TriggersPtr& W3m::triggers() const
{
	return m_triggers;
}

inline const W3m::InfoPtr& W3m::info() const
{
	return m_info;
}

inline const W3m::CustomTextTriggersPtr& W3m::customTextTriggers() const
{
	return m_customTextTriggers;
}

inline const W3m::FileFormats& W3m::fileFormats() const
{
	return this->m_fileFormats;
}

inline W3m::FileFormats& W3m::fileFormats()
{
	return this->m_fileFormats;
}

inline bool W3m::hasSignature() const
{
	return signature().get() != 0;
}

inline const W3m::Signature& W3m::signature() const
{
	return m_signature;
}


}

}

#endif
