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

#include <QtGui>

#include <KFileDialog>
#include <KMessageBox>
#include <KLocale>
#include <KToolBar>
#include <KMenu>
#include <KAction>
#include <KActionCollection>
#include <KMenuBar>
#include <KStandardAction>
#include <KService>

#include "textureeditor.hpp"
#include "qblp/blpiohandler.hpp"
#include "platform.hpp"

namespace wc3lib
{

namespace editor
{

TextureEditor::TextureEditor(class MpqPriorityList *source, QWidget *parent, Qt::WindowFlags f) : Module(source, parent, f), m_imageLabel(new QLabel(this)), m_texture(), m_showsAlphaChannel(false), m_showsTransparency(false), m_factor(1.0)
{
	Module::setupUi();
	topLayout()->addWidget(imageLabel());

	/*
	KService::Ptr service = KService::serviceByDesktopPath("gvpart.desktop");

	if (service)
	{
		m_part = service->createInstance<KParts::ReadWritePart>(this);

		if (m_part)
		{
			// tell the KParts::MainWindow that this is indeed
			// the main widget
			gridLayout_2->addWidget(m_part->widget());

			//setupGUI(ToolBar | Keys | StatusBar | Save);

			// and integrate the part's GUI with the shell's
			//createGUI(m_part);
		}
		else
			qDebug() << "Was not able to create gwenview part!";
	}
	else
	{
		// if we couldn't find our Part, we exit since the Shell by
		// itself can't do anything useful
		KMessageBox::error(this, i18n("Service \"gvpart.desktop\" not found."));
	}
	*/

	foreach (QAction *action, m_textureActionCollection->actions())
		action->setEnabled(false);
}

TextureEditor::~TextureEditor()
{
}

void TextureEditor::openFile()
{
	/*
	QPluginLoader loader("libqblp.so");
	loader.load();

	if (!loader.isLoaded())
	{
		KMessageBox::error(this, i18n("Unable to load BLP plugin: \"%1\".", loader.errorString()));

		return;
	}
	*/

	KMimeType::Ptr mime(KMimeType::mimeType("image/x-blp"));
	KUrl url;

	// if MIME type exists it will be considered in image open dialog
	if (!mime.isNull())
		url = KFileDialog::getImageOpenUrl(this->m_recentUrl, this, i18n("Open texture"));
	else
		url = KFileDialog::getOpenUrl(this->m_recentUrl, i18n("*|All Files"), this, i18n("Open texture"));

	if (url.isEmpty())
		return;

	//BlpIOHandler handler;
	//handler.setDevice(&file);
	TexturePtr texture(new Texture(url));

	/*if (!handler.canRead())
	{
		KMessageBox::error(this, i18n("Unable to detect any BLP format in file \"%1\".", url.toLocalFile()));

		return;
	}
	*/

	try
	{
		texture->setSource(source());
		texture->loadAll();
	}
	catch (Exception &exception)
	{
		KMessageBox::error(this, i18n("Unable to read BLP image from file \"%1\".\nException:\n\"%2\".", url.toLocalFile(), exception.what().c_str()));

		return;
	}

	qDebug() << "Size is " << texture->qt()->width() << " | " << texture->qt()->height();

	if (texture->qt()->isNull())
	{
		KMessageBox::error(this, i18n("Unable to read file \"%1\".", url.toLocalFile()));

		return;
	}

	m_texture.swap(texture);
	m_mipMapIndex = 0;
	m_showsAlphaChannel = false;
	m_showsTransparency = false;
	m_factor = 1.0;
	qDebug() << "Set pixmap to label ";
	qDebug() << "Label size is " << this->m_imageLabel->width() << " | " << this->m_imageLabel->height();

	foreach (QAction *action, m_textureActionCollection->actions())
		action->setEnabled(true);

	m_showAlphaChannelAction->setEnabled(this->texture()->qt()->hasAlphaChannel());
	this->m_textureActionCollection->action("editcolorpalette")->setEnabled(this->texture()->qt()->format() == QImage::Format_Indexed8);

	m_mipMapsMenu->clear();
	m_mipMaps.resize(this->texture()->blp()->mipMaps().size());
	std::size_t i = 0;

	BOOST_FOREACH(blp::Blp::MipMaps::const_reference mipMap, this->texture()->blp()->mipMaps())
	{
		BlpIOHandler ioHandler;
		QImage image;

		if (ioHandler.read(&image, *mipMap, *this->texture()->blp()))
			m_mipMaps[i] = image;
		else
			KMessageBox::error(this, i18n("Unable to read MIP map \"%1\".", i));

		++i;
		KAction *action = new KAction(KIcon(":/actions/opentexture.png"), i18n("MIP map %1", i), this);
		action->setShortcut(KShortcut(i18n("Ctrl+M+%1", i)));
		connect(action, SIGNAL(triggered()), this, SLOT(showMipMap()));
		m_mipMapsMenu->addAction(action);
	}

	/// \todo Image format (e. g. Format_Indexed8 is not stored) which leads to another format when saving image somewhere.
	refreshImage();
}

void TextureEditor::saveFile()
{
	if (!hasTexture() || this->texture()->qt()->isNull())
	{
		KMessageBox::error(this, i18n("No open image file."));

		return;
	}

	KUrl url = KFileDialog::getSaveUrl(this->m_recentUrl, i18n("*|All Files\n*.blp|Blizzard Pictures\n*.png|Portable Network Graphics\n*.jpg|JPEG Files"), this, i18n("Save texture")); // TODO MIME filters do not work ("all/allfiles").

	if (url.isEmpty())
		return;

	try
	{
		this->texture()->save(url, QFileInfo(url.toLocalFile()).suffix().toLower());
	}
	catch (Exception &exception)
	{
		KMessageBox::error(this, i18n("Unable to save image to file \"%1\".\nException:\n\"%2\".", url.toEncoded().constData(), exception.what().c_str()));

		return;
	}
}

void TextureEditor::closeFile()
{
	foreach (QAction *action, m_textureActionCollection->actions())
		action->setEnabled(false);

	m_mipMapsMenu->clear();

	m_texture.reset();
	refreshImage();
}

void TextureEditor::showSettings()
{
}

void TextureEditor::editColorPalette()
{
}

void TextureEditor::makeActive()
{
}

void TextureEditor::makePassive()
{
}

void TextureEditor::makeAutocast()
{
}

void TextureEditor::makeInfocardNormal()
{
}

void TextureEditor::makeInfocardLevel()
{
}

void TextureEditor::setCharges(int charges)
{
}

void TextureEditor::showAlphaChannel()
{
	if (!hasTexture())
		return;

	if (!this->m_texture->qt()->hasAlphaChannel())
	{
		KMessageBox::error(this, i18n("Image doesn't have alpha channel."));

		return;
	}

	if (showsAlphaChannel())
	{
		m_showAlphaChannelAction->setText(i18n("Show alpha channel"));
		m_showsAlphaChannel = false;
	}
	else
	{
		m_showAlphaChannelAction->setText(i18n("Hide alpha channel"));
		m_showsAlphaChannel = true;
	}

	refreshImage();
}

void TextureEditor::showTransparency()
{
	if (!hasTexture())
		return;

	/// \todo According to documentation checking for mask is an expensive operation.

	if (showsTransparency())
	{
		m_showTransparencyAction->setText(i18n("Show transparency"));
		m_showsTransparency = false;
	}
	else
	{
		m_showTransparencyAction->setText(i18n("Hide transparency"));
		m_showsTransparency = true;
	}

	refreshImage();
}

void TextureEditor::actualSize()
{
	if (!hasTexture())
		return;

	this->m_factor = 1.0;
	refreshImage();
}

void TextureEditor::zoomToFit()
{
	if (!hasTexture())
		return;

	refreshImage();
}

void TextureEditor::zoomIn()
{
	if (!hasTexture())
		return;

	this->m_factor += 0.20;
	refreshImage();
}

void TextureEditor::zoomOut()
{
	if (!hasTexture())
		return;

	this->m_factor -= 0.20;
	refreshImage();
}

void TextureEditor::massConverter()
{
}

void TextureEditor::showMipMap()
{
	KAction *action = boost::polymorphic_cast<KAction*>(sender());
	m_mipMapIndex = this->m_mipMapsMenu->actions().indexOf(action);
	m_showsAlphaChannel = false;
	m_showsTransparency = false;
	m_factor = 1.0;
	refreshImage();
}

void TextureEditor::refreshImage()
{
	if (!hasTexture())
	{
		this->m_imageLabel->setPixmap(QPixmap());

		return;
	}

	QPixmap newPixmap;

	if (!showsAlphaChannel())
		newPixmap = QPixmap::fromImage(mipMaps()[mipMapIndex()]).scaled(mipMaps()[mipMapIndex()].size() * this->factor());
	else
		newPixmap = QPixmap::fromImage(mipMaps()[mipMapIndex()].createAlphaMask()).scaled(mipMaps()[mipMapIndex()].size() * this->factor());

	if (showsTransparency())
		newPixmap.setMask(this->m_imageLabel->pixmap()->createMaskFromColor(Qt::transparent));

	this->m_imageLabel->setPixmap(newPixmap);
	this->m_imageLabel->resize(this->m_imageLabel->pixmap()->size());
}

void TextureEditor::createFileActions(class KMenu *menu)
{
	m_textureActionCollection = new KActionCollection((QObject*)this);

	class KAction *action;

	action = new KAction(KIcon(":/actions/opentexture.png"), i18n("Open texture"), this);
	action->setShortcut(KShortcut(i18n("Ctrl+O")));
	connect(action, SIGNAL(triggered()), this, SLOT(openFile()));
	menu->addAction(action);

	action = new KAction(KIcon(":/actions/savetexture.png"), i18n("Save texture"), this);
	action->setShortcut(KShortcut(i18n("Ctrl+S")));
	connect(action, SIGNAL(triggered()), this, SLOT(saveFile()));
	menu->addAction(action);
	m_textureActionCollection->addAction("savetexture", action);

	action = new KAction(KIcon(":/actions/closetexture.png"), i18n("Close texture"), this);
	action->setShortcut(KShortcut(i18n("Ctrl+W")));
	connect(action, SIGNAL(triggered()), this, SLOT(closeFile()));
	menu->addAction(action);
	m_textureActionCollection->addAction("closetexture", action);

	action = new KAction(KIcon(":/actions/settings.png"), i18n("Settings"), this);
	//action->setShortcut(KShortcut(i18n("Ctrl+O")));
	connect(action, SIGNAL(triggered()), this, SLOT(showSettings()));
	menu->addAction(action);
}

void TextureEditor::createEditActions(class KMenu *menu)
{
	KAction *action = new KAction(KIcon(":/actions/editcolorpalette.png"), i18n("Edit color palette"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(editColorPalette()));
	menu->addAction(action);
	m_textureActionCollection->addAction("editcolorpalette", action);
}

void TextureEditor::createMenus(class KMenuBar *menuBar)
{
	KMenu *viewMenu = new KMenu(i18n("View"), this);
	//this->m_viewMenu = viewMenu;
	menuBar->addMenu(viewMenu);

	m_showAlphaChannelAction = new KAction(KIcon(":/actions/showalphachannel.png"), i18n("Show alpha channel"), this);
	connect(m_showAlphaChannelAction, SIGNAL(triggered()), this, SLOT(showAlphaChannel()));
	viewMenu->addAction(m_showAlphaChannelAction);
	m_textureActionCollection->addAction("showalphachannel", m_showAlphaChannelAction);

	m_showTransparencyAction = new KAction(KIcon(":/actions/showtransparency.png"), i18n("Show transparency"), this);
	connect(m_showTransparencyAction, SIGNAL(triggered()), this, SLOT(showTransparency()));
	viewMenu->addAction(m_showTransparencyAction);
	m_textureActionCollection->addAction("showtransparency", m_showTransparencyAction);

	KAction *action = KStandardAction::actualSize(this, SLOT(actualSize()), this);
	viewMenu->addAction(action);
	m_textureActionCollection->addAction("actualsize", action);

	action = new KAction(KIcon(":/actions/zoomtofit.png"), i18n("Zoom to fit"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(zoomToFit()));
	viewMenu->addAction(action);
	m_textureActionCollection->addAction("zoomtofit", action);

	action = KStandardAction::zoomIn(this, SLOT(zoomIn()), this);
	viewMenu->addAction(action);
	m_textureActionCollection->addAction("zoomin", action);

	action = KStandardAction::zoomOut(this, SLOT(zoomOut()), this);
	viewMenu->addAction(action);
	m_textureActionCollection->addAction("zoomout", action);

	KMenu *toolsMenu = new KMenu(i18n("Tools"), this);
	//this->m_toolsMenu = toolsMenu;
	menuBar->addMenu(toolsMenu);

	// TODO add check buttons to mass converter widget which allow you to a) convert and b) generate info cards etc. and c) to remove old files.
	action = new KAction(KIcon(":/actions/massconverter.png"), i18n("Mass converter"), this);
	connect(action, SIGNAL(triggered()), this, SLOT(massConverter()));
	toolsMenu->addAction(action);

	this->m_mipMapsMenu = new KMenu(i18n("MIP maps"), this);
	menuBar->addMenu(m_mipMapsMenu);
}

void TextureEditor::createWindowsActions(class WindowsMenu *menu)
{
}

void TextureEditor::createToolButtons(class KToolBar *toolBar)
{
}

class SettingsInterface* TextureEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void TextureEditor::onSwitchToMap(Map *map)
{
}

#include "moc_textureeditor.cpp"

}

}
