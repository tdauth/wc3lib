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

#ifndef WC3LIB_EDITOR_SPLASHSCREEN_HPP
#define WC3LIB_EDITOR_SPLASHSCREEN_HPP

#include <QSplashScreen>

namespace wc3lib
{

namespace editor
{

/**
 * Default World Editor splash screen which shows license when World Editor is being started for the first time.
 * Otherwise it shows compact information and logo only.
 * Besides it should play sound "Uther's Return".
 * \todo Add class SplashScreenX for splash screen of Frozen Throne's World Editor.
 * \sa SplashScreenX
 */
class SplashScreen : public QSplashScreen
{
	public:
		SplashScreen(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f = 0);
};

}

}

#endif
