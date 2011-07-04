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

#include "environment.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace mpq
{

class Mpq;

}

namespace blp
{

class Blp;

}

namespace map
{

class W3m : public FileFormat
{
	public:
		W3m();
		virtual ~W3m();

		virtual std::streamsize read(class mpq::Mpq *mpq) throw (class Exception);
		/**
		 * \param istream has to contain the map MPQ archive.
		 */
		virtual std::streamsize read(InputStream &istream) throw (class Exception);
		/**
		 * \param headerStream Each map is a file with an MPQ archive and a header before. This stream should contain the map's header data.
		 * \param paths List which should contain all necessary file paths. Files will be deteced automatically by names.
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
		 * \note You can use classes's static members called "fileName" to get the corresponding file name of the class's format.
		 */
		virtual std::streamsize read(InputStream &headerStream, const std::list<boost::filesystem::path> &paths) throw (class Exception);
		/**
		 * Creates an MPQ archive with map header and all required files.
		 */
		virtual std::streamsize write(OutputStream &ostream) const throw (class Exception);
		
		virtual int32 fileId() const;
		virtual int32 latestFileVersion() const;

		int32 width() const;
		int32 height() const;

	protected:
		std::streamsize readHeader(InputStream &istream) throw (class Exception);
		std::streamsize readSignature(InputStream &istream) throw (class Exception);
		bool findPath(const std::list<boost::filesystem::path> &paths, boost::filesystem::path &path, const string &fileName);

		string m_name;
		BOOST_SCOPED_ENUM(MapFlags) m_flags;
		int32 m_maxPlayers;

		class Environment *m_environment;
		class Shadow *m_shadow;
		class PathMap *m_pathMap;
		class Trees *m_trees;
		class Units *m_units;
		class Info *m_info;
		class Strings *m_strings;
		class blp::Blp *m_minimap;
		class MenuMinimap *m_menuMinimap;
		class CustomUnits *m_customUnits;
		class Triggers *m_triggers;
		class Cameras *m_cameras;
		class Rects *m_rects;
		class Sounds *m_sounds;
		class CustomTextTriggers *m_customTextTriggers;
		class ImportedFiles *m_importedFiles;
/*
w3x
		class ArtificialIntelligence *m_artificialIntelligence;
		class Misc *m_misc;
		class Skin *m_skin;
		class Extra *m_extra;
*/

		bool m_hasSignature;
		char8 m_authentification[256];

};

inline int32 W3m::fileId() const
{
	return (int32)"HM3W";
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

}

}

#endif
