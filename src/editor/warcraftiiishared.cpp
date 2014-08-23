/***************************************************************************
 *   Copyright (C) 2014 by Tamino Dauth                                    *
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

#include <boost/format.hpp>

#include <QtGui>

#include "warcraftiiishared.hpp"
#include "../exception.hpp"
#include "mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

WarcraftIIIShared::WarcraftIIIShared(MpqPriorityList* source) : m_source(source)
{
}

void WarcraftIIIShared::refreshDefaultFiles(QWidget* window)
{
	this->refreshWorldEditorStrings(window);
	this->refreshTriggerStrings(window);
	this->refreshTriggerData(window);
}

Texture* WarcraftIIIShared::teamColorTexture(TeamColor teamColor) const throw (class Exception)
{
	TeamColorTextures::iterator iterator = this->m_teamColorTextures.find(teamColor);

	if (iterator == this->m_teamColorTextures.end())
	{
		std::auto_ptr<Texture> texture(new Texture(teamColorUrl(teamColor)));
		texture->setSource(this->source());
		Texture *result = texture.get(); // improve performance on first allocation by returning it directly without calling "find"
		this->m_teamColorTextures.insert(teamColor, texture);

		return result;
	}

	return this->m_teamColorTextures.find(teamColor)->second;
}

Texture* WarcraftIIIShared::teamGlowTexture(TeamColor teamGlow) const throw (class Exception)
{
	TeamColorTextures::iterator iterator = this->m_teamGlowTextures.find(teamGlow);

	if (iterator == this->m_teamGlowTextures.end())
	{
		std::auto_ptr<Texture> texture(new Texture(teamGlowUrl(teamGlow)));
		texture->setSource(this->source());
		Texture *result = texture.get(); // improve performance on first allocation by returning it directly without calling "find"
		this->m_teamGlowTextures.insert(teamGlow, texture);

		return result;
	}

	return this->m_teamGlowTextures.find(teamGlow)->second;
}

void WarcraftIIIShared::refreshWorldEditorStrings(QWidget *window, const KUrl &url) throw (Exception)
{
	QString target;

	if (!this->source()->download(url, target, window))
	{
		throw Exception(boost::format(_("Unable to download file \"%1%\".")) % url.toLocalFile().toStdString());
	}

	qDebug() << "World Editor strings target: " << target;
	WorldEditorStringsPtr ptr(new map::Txt());
	ifstream ifstream(target.toStdString(), std::ios::binary | std::ios::in);

	if (!ifstream)
	{
		throw Exception(boost::format(_("Unable to read from file \"%1%\".")) % target.toStdString());
	}

	ptr->read(ifstream);
	m_worldEditorStrings.swap(ptr); // exception safe
}

QString WarcraftIIIShared::tr(const QString &key, const QString &group, const QString &defaultValue) const
{
	// TODO if group is empty, search all sections
	if (this->worldEditorStrings().get() != 0) {
		qDebug() << "Trying it in WorldEditorStrings.txt";

		try {
			const map::Txt::Pairs &pairs = this->worldEditorStrings()->entries(group.toStdString());

			// TODO linear search
			foreach (map::Txt::Pairs::const_reference ref, pairs) {
				if (ref.first == key.toStdString()) {
					return ref.second.c_str();
				}
			}
		} catch (Exception &e) {
		}
	}

	QStringList files;
	// all files which contain strings
	files
	<< "UI/WorldEditStrings.txt"
	<< "UI/WorldEditGameStrings.txt"
	<< "UI/TriggerStrings.txt"
	<< "UI/TipStrings.txt"
	<< "UI/CampaignStrings.txt"
	;

	if (!defaultValue.isEmpty())
	{
		return defaultValue;
	}

	return group + "[" + key + "]";
}

void WarcraftIIIShared::refreshTriggerData(QWidget *window, const KUrl &url) throw (Exception)
{
	QString target;

	if (!this->source()->download(url, target, window))
	{
		throw Exception(_("Unable to download file \"UI/TriggerData.txt\"."));
	}

	qDebug() << "Trigger data target: " << target;
	TriggerDataPtr ptr(new map::TriggerData());
	ifstream ifstream(target.toStdString(), std::ios::binary | std::ios::in);

	if (!ifstream)
	{
		throw Exception(boost::format(_("Unable to read from file \"%1%\".")) % target.toStdString());
	}

	ptr->read(ifstream);
	m_triggerData.swap(ptr); // exception safe
}

void WarcraftIIIShared::refreshTriggerStrings(QWidget *window, const KUrl &url) throw (Exception)
{
	QString target;

	if (!this->source()->download(url, target, window))
	{
		throw Exception(_("Unable to download file \"UI/TriggerStrings.txt\"."));
	}

	qDebug() << "Trigger strings target: " << target;
	TriggerStringsPtr ptr(new map::TriggerStrings());
	ifstream ifstream(target.toStdString(), std::ios::binary | std::ios::in);

	if (!ifstream)
	{
		throw Exception(boost::format(_("Unable to read from file \"%1%\".")) % target.toStdString());
	}

	ptr->read(ifstream);
	m_triggerStrings.swap(ptr); // exception safe
}

}

}