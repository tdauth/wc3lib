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

#include <vector>

#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

#include "../config.h"
#ifdef MPQ
#include "../mpq.hpp"
#else
#warning No MPQ support in map module since MPQ module is not included.
#endif
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
#include "triggerdata.hpp"
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

/**
 * \brief Class for reading Warcraft III: Reign of Chaos maps (usually with *.w3m extension).
 *
 * To open a map and to read from a file format you have to do the following:
 * \code
 * W3m map;
 * map.open("mymap.w3m");
   // reads map strings
 * map.readFileFormat(map.strings().get());
 * \endcode
 * This requires MPQ support to be enabled in wc3lib.
 *
 * \note When opening a map it doesn't read any file format automatically. To read all file formats at once you can use \ref readAllFileFormats().
 * \sa W3x
 * \todo Maybe allocate on request as well?
 */
class W3m :
#ifdef MPQ
public mpq::Archive,
#endif
public Playable
{
	public:
		static const std::size_t signatureSize = 256;

		typedef boost::scoped_ptr<Environment> EnvironmentPtr;
		typedef boost::scoped_ptr<Shadow> ShadowPtr;
		typedef boost::scoped_ptr<Pathmap> PathmapPtr;
		typedef boost::scoped_ptr<Trees> TreesPtr;
		typedef boost::scoped_ptr<Info> InfoPtr;
		typedef boost::scoped_ptr<MapStrings> MapStringsPtr;
		typedef boost::scoped_ptr<Minimap> MinimapPtr;
		typedef boost::scoped_ptr<MenuMinimap> MenuMinimapPtr;
		typedef boost::scoped_ptr<CustomUnits> CustomUnitsPtr;
		typedef boost::scoped_ptr<Triggers> TriggersPtr;
		typedef boost::scoped_ptr<Cameras> CamerasPtr;
		typedef boost::scoped_ptr<Rects> RectsPtr;
		typedef boost::scoped_ptr<Sounds> SoundsPtr;
		typedef boost::scoped_ptr<CustomTextTriggers> CustomTextTriggersPtr;
		typedef boost::scoped_ptr<ImportedFiles> ImportedFilesPtr;
		typedef boost::scoped_array<byte> Signature;

		W3m();
		virtual ~W3m();

#ifdef MPQ
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
		virtual std::streamsize readFileFormat(FileFormat *format);
		/**
		 * \brief Convenience member function to read all files of the map at once.
		 *
		 * \param triggerData Is required for reading the triggers. If this value is 0, triggers will be ignored.
		 */
		virtual std::streamsize readAllFileFormats(const map::TriggerData *triggerData);
		virtual std::streamsize read(InputStream &istream);
		/**
		 * Triggers have to be read separately since they need corresponding trigger data.
		 * \throw Exception Throws an exception if there is no triggers file or if there occured any error while reading it.
		 */
		virtual std::streamsize readTriggers(const TriggerData &triggerData);
#endif

		/**
		 * Creates an MPQ archive with map header and all required files.
		 */
		virtual std::streamsize write(OutputStream &ostream) const override;

		virtual int32 fileId() const;
		virtual int32 latestFileVersion() const;

		const string& name() const;
		MapFlags flags() const;
		int32 maxPlayers() const;
		const EnvironmentPtr& environment() const;
		const ShadowPtr& shadow() const;
		const PathmapPtr& pathmap() const;
		const TreesPtr& trees() const;
		CustomUnitsPtr& customUnits();
		const CustomUnitsPtr& customUnits() const;
		const InfoPtr& info() const;
		const MapStringsPtr& strings() const;
		const MinimapPtr& minimap() const;
		const MenuMinimapPtr& menuMinimap() const;
		const TriggersPtr& triggers() const;
		const CamerasPtr& cameras() const;
		const RectsPtr& rects() const;
		const SoundsPtr& sounds() const;
		const CustomTextTriggersPtr& customTextTriggers() const;
		const ImportedFilesPtr& importedFiles() const;

		bool hasSignature() const;
		/**
		 * If \ref hasSignature() is true it returns an array of \ref signatureSize bytes containing the digital signature of the map.
		 */
		const Signature& signature() const;

	protected:
		std::streamsize readHeader(InputStream &istream);
		std::streamsize readSignature(InputStream &istream);
		std::streamsize writeHeader(OutputStream &ostream) const;

		string m_name;
		MapFlags m_flags;
		int32 m_maxPlayers;

		EnvironmentPtr m_environment;
		ShadowPtr m_shadow;
		PathmapPtr m_pathmap;
		TreesPtr m_trees;
		CustomUnitsPtr m_customUnits;
		InfoPtr m_info;
		MapStringsPtr m_strings;
		MinimapPtr m_minimap;
		MenuMinimapPtr m_menuMinimap;
		TriggersPtr m_triggers;
		CamerasPtr m_cameras;
		RectsPtr m_rects;
		SoundsPtr m_sounds;
		CustomTextTriggersPtr m_customTextTriggers;
		ImportedFilesPtr m_importedFiles;

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

inline const string& W3m::name() const
{
	return m_name;
}

inline MapFlags W3m::flags() const
{
	return m_flags;
}

inline int32 W3m::maxPlayers() const
{
	return m_maxPlayers;
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

inline W3m::CustomUnitsPtr& W3m::customUnits()
{
	return m_customUnits;
}

inline const W3m::CustomUnitsPtr& W3m::customUnits() const
{
	return m_customUnits;
}

inline const W3m::TriggersPtr& W3m::triggers() const
{
	return m_triggers;
}

inline const W3m::CamerasPtr& W3m::cameras() const
{
	return this->m_cameras;
}

inline const W3m::RectsPtr& W3m::rects() const
{
	return this->m_rects;
}

inline const W3m::SoundsPtr& W3m::sounds() const
{
	return this->m_sounds;
}

inline const W3m::InfoPtr& W3m::info() const
{
	return m_info;
}

inline const W3m::MapStringsPtr& W3m::strings() const
{
	return this->m_strings;
}

inline const W3m::MinimapPtr& W3m::minimap() const
{
	return this->m_minimap;
}

inline const W3m::MenuMinimapPtr& W3m::menuMinimap() const
{
	return this->m_menuMinimap;
}

inline const W3m::CustomTextTriggersPtr& W3m::customTextTriggers() const
{
	return m_customTextTriggers;
}

inline const W3m::ImportedFilesPtr& W3m::importedFiles() const
{
	return this->m_importedFiles;
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
