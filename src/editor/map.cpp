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

#include <QTemporaryFile>

#include "map.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

Map::Map(const QUrl &url) : Resource(url, Type::Map)
{
}

void Map::load()
{
	QString target;

	if (!this->source()->download(url(), target, 0))
	{
		throw Exception(boost::format(_("Error on downloading %1%.")) % url().toEncoded().constData());
	}

	// TODO should not be locked by default
	QFileInfo info(target);

	if (!info.isWritable())
	{
		throw Exception(boost::format(_("File must be writable since MPQ archives are locked by default.")));
	}

	ifstream istream(target.toStdString(), std::ios::in | std::ios::binary);
	MapPtr map(new map::W3m());
	map->open(target.toStdString());
	// NOTE Do not remove the temporary file since MPQ archives rely on local files instead of reading everything into the heap.

	if (map->triggers().get() != 0)
	{
		// triggers have to be loaded separately when trigger data file ("UI/TriggerData.txt") is available
		mpq::File file = map->findFile(map->triggers()->fileName());

		if (file.isValid())
		{
			if (source()->sharedData()->triggerData().get() == 0)
			{
				throw Exception("Trigger data file \"UI/TriggerData.txt\" is not available.");
			}

			// TODO data has to be refreshed somewhere in GUI
			//source()->refreshTriggerData(); // if trigger data is not available we cannot load trigger data
			stringstream stream;
			file.decompress(stream);
			map->triggers()->read(stream, *source()->sharedData()->triggerData());
		}
		else
		{
			throw Exception(boost::format(_("File \"%1%\" not found although triggers exist in map.")) % map->triggers()->fileName());
		}
	}

	this->map().swap(map); // exception safe
}

void Map::reload()
{
	load();
}

void Map::save(const QUrl &url) const
{
	QTemporaryFile tmpFile;

	if (!tmpFile.open())
	{
		throw Exception(boost::format(_("Temporary file \"%1%\" cannot be opened.")) % tmpFile.fileName().toStdString());
	}

	ofstream ostream(tmpFile.fileName().toStdString(), std::ios::out | std::ios::binary);

	map()->write(ostream);

	if  (!this->source()->upload(tmpFile.fileName(), url, 0))
	{
		throw Exception(boost::format(_("Unable to upload temporary file \"%1%\" to URL \"%2%\"")) % tmpFile.fileName().toStdString() % url.toEncoded().constData());
	}
}

}

}
