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

#include <QtGui>
#include <QtMultimedia/QSound>

#include "splashscreen.hpp"
#include "mpqprioritylist.hpp"
#include "core.hpp"

namespace wc3lib
{

namespace editor
{

SplashScreen::SplashScreen(MpqPriorityList *source, QWidget *parent) : QSplashScreen(qobject_cast<QGuiApplication*>(QGuiApplication::instance())->primaryScreen())
{
	QSettings settings("wc3lib", "wc3editor");
	const bool firstStart = settings.value("firstStart", true).toBool();

	if (source->sharedData()->worldEditData().get() != 0)
	{
		if (source->sharedData()->worldEditData()->hasValue("WorldEditArt", "War3XLogo"))
		{
			setPixmap(source->sharedData()->worldEditDataPixmap("War3XLogo", "WorldEditArt", this));
		}
		else if (source->sharedData()->worldEditData()->hasValue("WorldEditArt", "War3Logo"))
		{
			setPixmap(source->sharedData()->worldEditDataPixmap("War3Logo", "WorldEditArt", this));
		}

		if (firstStart)
		{
			if (source->sharedData()->worldEditData()->hasValue("WorldEditSounds", "WelcomeFirstRun"))
			{
				QString file;

				if (source->download(source->sharedData()->worldEditData()->value("WorldEditSounds", "WelcomeFirstRun"), file, this))
				{
					QSound::play(file);

					source->removeTempFile(file);
				}
			}

			if (source->sharedData()->worldEditData()->hasValue("WorldEditSounds", "WelcomeFirstRunDone"))
			{
				QString file;

				if (source->download(source->sharedData()->worldEditData()->value("WorldEditSounds", "WelcomeFirstRunDone"), file, this))
				{
					QSound::play(file);

					source->removeTempFile(file);
				}
			}
		}
		else
		{
			if (source->sharedData()->worldEditData()->hasValue("WorldEditSounds", "Welcome"))
			{
				QString file;

				if (source->download(source->sharedData()->worldEditData()->value("WorldEditSounds", "Welcome"), file, this))
				{
					QSound::play(file);

					source->removeTempFile(file);
				}
			}
		}
	}

	/// \todo add licens widget with content (load)
}

SplashScreen::~SplashScreen()
{
	/*
	 * After the first start other sounds will be played.
	 */
	QSettings settings("wc3lib", "wc3editor");
	settings.setValue("firstStart", false);
}

}

}
