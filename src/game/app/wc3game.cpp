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

#include <QApplication>

#include "../../game.hpp"

using namespace wc3lib::game;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	const QString organization = "wc3lib";

	wc3lib::editor::Root root;

	if (root.configure())
	{
		Game game(&root, organization, "wc3game");
		/*
		 * Pops up the dialog for source directories if necessary and loads default shared files.
		 */
		if (editor.configure(0))
		{
			SplashScreen splash(&editor);
			splash.show();

			splash.close();
			//TerrainEditor *terrainEditor = new TerrainEditor(editor, editor);
			//editor->addModule(terrainEditor);
			//terrainEditor->show();
			/*
			/// @todo Allow parsing multiple files as arguments.
			/// FIXME Crashes application when canceling file dialog.
			KCmdLineArgs *args = KCmdLineArgs::parsedArgs("+[file]");

			for (int i = 0; i < args->count(); ++i)
				editor.openMap(args->url(i));
			*/

			return app.exec();
		}
	}

	return 1;
}
