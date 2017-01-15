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

#include "../../editor.hpp"

#ifdef USE_OGREBLP
#include "../modeleditor/modeleditor.hpp"
#endif
#include "../objecteditor/objecteditor.hpp"
#include "../textureeditor/textureeditor.hpp"
#include "../triggereditor/triggereditor.hpp"
#include "../mpqeditor/mpqeditor.hpp"

using namespace wc3lib::editor;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	const QString organization = "wc3lib";

#if defined(USE_OGREBLP)
	Root root;

	if (root.configure())
	{
		Editor editor(&root, organization, "wc3editor");
#else
		Editor editor(organization, "wc3editor");
#endif
		/*
		 * Pops up the dialog for source directories if necessary and loads default shared files.
		 */
		if (editor.configure(0))
		{
			SplashScreen splash(&editor);
			splash.show();

#if defined(MDLX) && defined(USE_OGREBLP)
			splash.showMessage(QObject::tr("Loading Model Editor ..."), Qt::AlignCenter | Qt::AlignBottom, Qt::white);
			ModelEditor *modelEditor = new ModelEditor(&root, &editor);

			if (modelEditor->configure())
			{
				editor.addModule(modelEditor);
			}
#endif

			splash.showMessage(QObject::tr("Loading Object Editor ..."), Qt::AlignCenter | Qt::AlignBottom, Qt::white);
			ObjectEditor *objectEditor = new ObjectEditor(&editor, organization, "wc3object");

			if (objectEditor->configure())
			{
				editor.addModule(objectEditor);
			}

			splash.showMessage(QObject::tr("Loading Texture Editor ..."), Qt::AlignCenter | Qt::AlignBottom, Qt::white);
			TextureEditor *textureEditor = new TextureEditor(&editor, organization, "wc3texture");

			if (textureEditor->configure())
			{
				editor.addModule(textureEditor);
			}

			splash.showMessage(QObject::tr("Loading Trigger Editor ..."), Qt::AlignCenter | Qt::AlignBottom, Qt::white);
			TriggerEditor *triggerEditor = new TriggerEditor(&editor, organization, "wc3trigger");

			if (triggerEditor->configure())
			{
				editor.addModule(triggerEditor);
			}

			splash.showMessage(QObject::tr("Loading MPQ Editor ..."), Qt::AlignCenter | Qt::AlignBottom, Qt::white);
			MpqEditor *mpqEditor = new MpqEditor(&editor, organization, "wc3mpq");

			if (mpqEditor->configure())
			{
				editor.addModule(mpqEditor);
				mpqEditor->show();
			}

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
#if defined(USE_OGREBLP)
	}
#endif

	return 1;
}
