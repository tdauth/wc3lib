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

#ifndef WC3LIB_EDITOR_PLUGIN_HPP
#define WC3LIB_EDITOR_PLUGIN_HPP

#include <QObject>

#include <KAboutData>
#include <KXMLGUIClient>
#include <KComponentData>

#include "platform.hpp"
#include "module.hpp"

namespace wc3lib
{

namespace editor
{

class PluginInterface
{
	public:
		virtual bool load() = 0;
		virtual bool unload() = 0;
};

/**
 * Plugins should be \ref KParts compatible.
 * Each plugin should have corresponding .desktop file with a
 * description. The .desktop file template looks like:
 * @code
 * [Desktop Entry]
 * Type=Service
 * Exec=blubb
 * Name=
 * GenericName=
 * Comment=
 * Icon=
 * ServiceTypes=wc3editor/Plugin
 * X-KDE-Library=
 * X-KDE-PluginInfo-Name=
 * X-KDE-PluginInfo-Author=
 * X-KDE-PluginInfo-Version=
 * X-KDE-PluginInfo-License=
 * X-KDE-PluginInfo-Category=
 * x-wc3editor-Version=
 * X-wc3editor-Dependencies=
 * X-wc3editor-Mode=
 * @endcode
 * <b>Description of parameters in .desktop file:</b>
 * - <i>Name</i> is a translatable name of a plugin, it is used in the plugin
 *   selection list (required);
 * - <i>GenericName</i> is a translatable generic name of a plugin, it should
 *   describe in general what the plugin can do  (required);
 * - <i>Comment</i> is a short description about the plugin (optional);
 * - <i>Icon</i> is a plugin icon (preferred);
 *   <i>X-KDE-library</i>this is the name of the .so file to load for this plugin (required);
 * - <i>X-KDE-PluginInfo-Name</i> is a non-translatable user-readable plugin
 *   identifier used in KTrader queries (required);
 * - <i>X-KDE-PluginInfo-Author</i> is a non-translateable name of the plugin
 *   author (optional);
 * - <i>X-KDE-PluginInfo-Version</i> is version number of the plugin (optional);
 * - <i>X-KDE-PluginInfo-License</i> is a license (optional). can be: GPL,
 * LGPL, BSD, Artistic, QPL or Custom. If this property is not set, license is
 * considered as unknown;
 * - <i>X-wc3editor-Version</i> is the wc3editor plugin API version this plugin
 *   works with (required);
 * - <i>X-wc3editor-Mode</i> is either GUI or NoGUI to indicate whether a plugin can run
 *   with the GUI components loaded or not (required);
 */
class Plugin : public QObject, public PluginInterface, public KXMLGUIClient
{
	public:
		Plugin(const KComponentData &componentData, QObject *parent = 0);

		BOOST_SCOPED_ENUM_START(Type)
		{
		};
		BOOST_SCOPED_ENUM_END

		/**
		 * Can create a new KXMLGUIClient, and set it up correctly with all the plugins per-module GUI actions.
		 *
		 * The caller owns the created object, including all contained actions. The object is destroyed as soon as
		 * the module is closed.
		 *
		 * The default implementation calls the convenience function \ref createActionsForModule and uses it to fill a custom KXMLGUIClient.
		 *
		 * Only override this version if you need more specific features of KXMLGUIClient, or other special per-module handling.
		 *
		 * \param module The module the actions are created for
		 */
		virtual KXMLGUIClient* createGui(Module *module);

		/**
		 * This function allows allows setting up plugin actions conveniently. Unless \ref createGui was overridden,
		 * this is called for each new module, and the plugin should add its actions to \p actions, and write its KXMLGui xml file
		 * into \p xmlFile.
		 *
		 * \param module The module the actions are created for
		 * \param xmlFile Change this to the XML file that needed for merging the actions into the GUI
		 * \param actions Add your actions here. A new set of actions has to be created for each module.
		 */
		virtual void createActions(Module *module, QString &xmlFile, KActionCollection &actions);

		/**
		 * This function is necessary because there is no proper way to signal errors from plugin constructor.
		 * \returns True if the plugin has encountered an error, false otherwise.
		 */
		virtual bool hasError() const = 0;

		/**
		 * Description of the last encountered error, of an empty string if none.
		 */
		virtual QString errorDescription() const = 0;
};

}

}

Q_DECLARE_INTERFACE(wc3lib::editor::PluginInterface, "wc3lib.PluginInterface/1.0")

#endif // PLUGIN_HPP
