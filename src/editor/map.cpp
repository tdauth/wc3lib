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

#include <KTemporaryFile>
#include <KMessageBox>

#include "map.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

Map::Map(const KUrl &url) : Resource(url, Type::Map)
{
}

void Map::load() throw (Exception)
{
	QString target;

	if (!this->source()->download(url(), target, 0))
		throw Exception();

	ifstream istream(target.toUtf8().constData(), std::ios::in | std::ios::binary);

	MapPtr map(new map::W3m());
	map->open(target.toUtf8().constData());

	// triggers have to be loaded separately when trigger data file ("UI/TriggerData.txt") is available
	mpq::Mpq::FilePtrConst file = map->findFile(map->triggers()->fileName());

	if (file.get() != 0)
	{
		if (source()->triggerData().get() == 0)
			throw Exception("Trigger data file \"UI/TriggerData.txt\" is not available.");

		// TODO data has to be refreshed somewhere in GUI
		//source()->refreshTriggerData(); // if trigger data is not available we cannot load trigger data
		stringstream stream;
		file->writeData(stream);
		map->triggers()->read(stream, *source()->triggerData().get());
	}

	this->map().swap(map); // exception safe
}

void Map::reload() throw (Exception)
{
	load();
}

void Map::save(const KUrl &url) const throw (Exception)
{
	KTemporaryFile tmpFile;

	if (!tmpFile.open())
		throw Exception(boost::format(_("Temporary file \"%1%\" cannot be opened.")) % tmpFile.fileName().toUtf8().constData());

	ofstream ostream(tmpFile.fileName().toUtf8().constData(), std::ios::out | std::ios::binary);

	map()->write(ostream);

	if  (!this->source()->upload(tmpFile.fileName(), url, 0))
		throw Exception(boost::format(_("Unable to upload temporary file \"%1%\" to URL \"%2%\"")) % tmpFile.fileName().toUtf8().constData() % url.toEncoded().constData());
}

}

}
