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

#include <KActionCollection>
#include <KAction>

#include "plugin.hpp"

namespace wc3lib
{

namespace editor
{

Plugin::Plugin(const KComponentData &componentData, QObject *parent): QObject(parent)
{
	setComponentData(componentData);
}

KXMLGUIClient* Plugin::createGui(Module* module)
{
	KXMLGUIClient *client = new KXMLGUIClient(module);
	QString xmlFile;
	/*
	KActionCollection actions();
	createActions(module, xmlFile, actions);
	client->setXML(xmlFile);
	*/
	//client->plugActionList(collection);

	return client;
}

void Plugin::createActions(Module *module, QString &xmlFile, KActionCollection &actions)
{
	//this->replaceXMLFile(xmlFile);
	actions.setComponentData(componentData());
	xmlFile = this->xmlFile();


	//foreach (KAction *action, this->actionCollection()->actions())
		//actions.addAction(action->name(), action);
}

}

}
