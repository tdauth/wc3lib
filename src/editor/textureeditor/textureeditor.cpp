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
#include <QtWidgets>

#include "textureeditor.hpp"
#include "../qblp/blpiohandler.hpp"
#include "../platform.hpp"
#include "../mpqprioritylist.hpp"

namespace wc3lib
{

namespace editor
{

TextureEditor::TextureEditor(MpqPriorityList *source, const QString &organization, const QString &applicationName, QWidget *parent)
: Module(source, organization, applicationName, parent)
, m_scrollArea(new QScrollArea(this))
, m_imageLabel(new QLabel(this))
, m_mipMapIndex(0)
, m_showsAlphaChannel(false)
, m_showsTransparency(false)
, m_factor(1.0)
, m_zoomToFit(true)
, m_showAlphaChannelAction(nullptr)
, m_showTransparencyAction(nullptr)
, m_actualSizeAction(nullptr)
, m_zoomToFitAction(nullptr)
, m_zoomInAction(nullptr)
, m_zoomOutAction(nullptr)
, m_massConverterAction(nullptr)
, m_mipMapsMenu(nullptr)
, m_editColorPaletteAction(nullptr)
, m_dropColorPaletteAction(nullptr)
, m_setChargesAction(nullptr)
, m_chargesDialog(nullptr)
, m_loadDialog(nullptr)
, m_saveDialog(nullptr)
{
	Module::setupUi();

	imageLabel()->setAlignment(Qt::AlignCenter);
	imageLabel()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	scrollArea()->setFrameShape(QFrame::NoFrame);
	scrollArea()->setAlignment(Qt::AlignCenter);
	scrollArea()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	scrollArea()->setWidget(imageLabel());
	scrollArea()->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	topLayout()->addWidget(scrollArea());

	setWindowTitle(tr("Texture Editor"));

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
		KMessageBox::error(this, tr("Service \"gvpart.desktop\" not found."));
	}
	*/

	this->setImageActionsEnabled(false);
}

TextureEditor::~TextureEditor()
{
	writeSettings();
}

bool TextureEditor::configure()
{
	readSettings();

	// Update required files if started as stand-alone module
	if (!hasEditor())
	{
		if (!this->source()->configure(this, organization(), applicationName()))
		{
			return false;
		}

		try
		{
			source()->sharedData()->refreshWorldEditorStrings(this);
		}
		catch (wc3lib::Exception &e)
		{
			QMessageBox::critical(0, tr("Error"), tr("Error when loading default files: %1").arg(e.what()));

			return false;
		}
	}

	retranslateUi();

	return true;
}

void TextureEditor::retranslateUi()
{
	Module::retranslateUi();
}

TextureEditor::LoadDialogWidget::LoadDialogWidget(QWidget *parent) : QWidget(parent), m_mipMapsInput(new QSpinBox(this))
{
	QVBoxLayout *layout = new QVBoxLayout();
	this->setLayout(layout);

	QWidget *mipMapsWidget = new QWidget();
	QHBoxLayout *mipMapsLayout = new QHBoxLayout(mipMapsWidget);
	layout->addWidget(mipMapsWidget);
	mipMapsLayout->addWidget(new QLabel(tr("MIP maps:"), mipMapsWidget));
	mipMapsInput()->setMaximum(blp::Blp::maxMipMaps);
	mipMapsInput()->setMinimum(0);
	mipMapsInput()->setValue(blp::Blp::defaultMipMaps);
	layout->addWidget(mipMapsInput());
}

TextureEditor::LoadDialog::LoadDialog(QWidget *parent) : QWidget(parent), m_widget(new LoadDialogWidget())
{
	//KMimeType::Ptr mime(KMimeType::mimeType("image/x-blp"));
	m_dialog = new QFileDialog(this, tr("Open texture"), QString(), tr("All Files (*);;|Blizzard Pictures (*.blp);;Portable Network Graphics (*.png);;JPEG Files (*.jpg);;TGA Files (*.tga)")); // TODO MIME filters do not work ("all/allfiles"). Use image filter.
	dialog()->setFileMode(QFileDialog::ExistingFile);
	dialog()->setAcceptMode(QFileDialog::AcceptOpen);
}

TextureEditor::LoadDialog::~LoadDialog()
{
	delete m_dialog; // deletes widget as well
}

TextureEditor::SaveDialogWidget::SaveDialogWidget(QWidget *parent) : QWidget(parent), m_qualityInput(new QSpinBox(this)), m_mipMapsInput(new QSpinBox(this))
{
	QVBoxLayout *layout = new QVBoxLayout();
	this->setLayout(layout);

	QWidget *qualityWidget = new QWidget();
	QHBoxLayout *qualityLayout = new QHBoxLayout(qualityWidget);
	layout->addWidget(qualityWidget);
	qualityLayout->addWidget(new QLabel(tr("Quality:"), qualityWidget));
	qualityInput()->setMaximum(100);
	qualityInput()->setMinimum(-1);
	qualityInput()->setValue(blp::Blp::defaultQuality);
	qualityLayout->addWidget(qualityInput());

	QWidget *mipMapsWidget = new QWidget();
	QHBoxLayout *mipMapsLayout = new QHBoxLayout(mipMapsWidget);
	layout->addWidget(mipMapsWidget);
	mipMapsLayout->addWidget(new QLabel(tr("MIP maps:"), mipMapsWidget));
	mipMapsInput()->setMaximum(blp::Blp::maxMipMaps);
	mipMapsInput()->setMinimum(0);
	mipMapsInput()->setValue(blp::Blp::defaultMipMaps);
	layout->addWidget(mipMapsInput());
}

TextureEditor::SaveDialog::SaveDialog(QWidget *parent) : QWidget(parent), m_widget(new SaveDialogWidget())
{
	//KMimeType::Ptr mime(KMimeType::mimeType("image/x-blp"));
	m_dialog = new QFileDialog(this, tr("Save texture"), QString(), tr("*|All Files\n*.blp|Blizzard Pictures\n*.png|Portable Network Graphics\n*.jpg|JPEG Files\n*.tga|TGA Files")); // TODO MIME filters do not work ("all/allfiles"). Use image filter.
	dialog()->setFileMode(QFileDialog::AnyFile);
	dialog()->setAcceptMode(QFileDialog::AcceptSave);
}

TextureEditor::SaveDialog::~SaveDialog()
{
	delete m_dialog; // deletes widget as well
}

TextureEditor::ChargesDialog::ChargesDialog(QWidget *parent)
: QDialog(parent)
, m_buttonBox(new QDialogButtonBox(this))
, m_chargesInput(new QSpinBox(this))
, m_hasChargesCheckBox(new QCheckBox(tr("Has Charges:"), this))
{
	this->setWindowTitle(tr("Set Charges"));
	this->setLayout(new QVBoxLayout());
	this->layout()->addWidget(chargesInput());
	this->layout()->addWidget(hasChargesCheckBox());
	this->layout()->addWidget(m_buttonBox);

	chargesInput()->setMaximum(100);
	chargesInput()->setMinimum(0);
	hasChargesCheckBox()->setChecked(true);
	m_buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(hasChargesCheckBox(), SIGNAL(toggled(bool)), chargesInput(), SLOT(setEnabled(bool)));
	connect(m_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(pressed()), this, SLOT(accept()));
	connect(m_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(pressed()), this, SLOT(reject()));
}

void TextureEditor::openFile()
{
	if (this->loadDialog()->dialog()->exec() == QDialog::Accepted)
	{
		const QUrl url = loadDialog()->dialog()->selectedUrls().front();

		if (url.isEmpty())
		{
			return;
		}

		QMap<QString, QString> options;
		options["MipMaps"] = QString::number(loadDialog()->mipMapsInput()->value());

		openUrl(url, options);
	}
}

void TextureEditor::openUrl(const QUrl &url, QMap<QString, QString> options)
{
	TexturePtr texture(new Texture(url));

	try
	{
		texture->setSource(source());
		texture->loadBlp(options);
		texture->loadQt();
	}
	catch (Exception &exception)
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to read BLP image from file \"%1\".\nException:\n\"%2\".").arg(url.toLocalFile()).arg(exception.what()));

		return;
	}

	if (texture->qt()->isNull())
	{
		QMessageBox::critical(this, tr("Error"), tr("Unable to read file \"%1\".").arg(url.toLocalFile()));

		return;
	}

	m_texture.swap(texture);
	m_mipMapIndex = 0;
	m_showsAlphaChannel = false;
	m_showsTransparency = false;
	m_factor = 1.0;
	m_zoomToFit = true;
	this->m_zoomToFitAction->setChecked(this->m_zoomToFit);

	this->setImageActionsEnabled(true);

	m_showAlphaChannelAction->setEnabled(this->texture()->qt()->hasAlphaChannel());
	m_editColorPaletteAction->setEnabled(this->texture()->qt()->format() == QImage::Format_Indexed8);
	m_dropColorPaletteAction->setEnabled(this->texture()->qt()->format() == QImage::Format_Indexed8);

	m_mipMapsMenu->clear();
	m_mipMaps.resize(this->texture()->blp()->mipMaps().size());
	std::size_t i = 0;

	BOOST_FOREACH(blp::Blp::MipMaps::const_reference mipMap, this->texture()->blp()->mipMaps())
	{
		BlpIOHandler ioHandler;
		QImage image;

		if (ioHandler.read(&image, mipMap, *this->texture()->blp()))
		{
			m_mipMaps[i] = image;
		}
		else
		{
			QMessageBox::critical(this, tr("Error"), tr("Unable to read MIP map \"%1\".").arg(i));
		}

		++i;
		QAction *action = new QAction(QIcon(":/actions/opentexture.png"), tr("MIP map %1").arg(i), this);
		action->setShortcut(tr("Ctrl+M+%1").arg(i));
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
		QMessageBox::critical(this, tr("Error"), tr("No open image file."));

		return;
	}

	if (saveDialog()->dialog()->exec() == QDialog::Accepted)
	{
		const QUrl url = saveDialog()->dialog()->selectedUrls().front();

		if (url.isEmpty())
		{
			return;
		}

		try
		{
			QMap<QString, QString> compression;
			compression["Quality"] = QString::number(saveDialog()->qualityInput()->value());
			compression["MipMaps"] = QString::number(saveDialog()->mipMapsInput()->value());
			this->texture()->save(url, QFileInfo(url.toLocalFile()).suffix().toLower(), compression);
		}
		catch (Exception &exception)
		{
			QMessageBox::critical(this, tr("Error"), tr("Unable to save image to file \"%1\".\nException:\n\"%2\".").arg(url.toEncoded().constData()).arg(exception.what()));

			return;
		}
	}
}

void TextureEditor::closeFile()
{
	this->setImageActionsEnabled(false);

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

void TextureEditor::dropColorPalette()
{
	if (texture()->hasBlp())
	{
		if (texture()->blp()->compression() == blp::Blp::Compression::Paletted)
		{
			texture()->blp()->setCompression(blp::Blp::Compression::Jpeg);
			texture()->loadQt();
			refreshImage();
			qDebug() << "Dropped color palette!";
		}
	}
	else
	{
		QMessageBox::critical(this, tr("Error"), tr("Dropping color palette is currently supported for BLP textures only."));
	}
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

void TextureEditor::setCharges()
{
	if (this->chargesDialog()->exec() == QDialog::Accepted)
	{
		if (!this->chargesDialog()->hasChargesCheckBox()->isChecked())
		{
			refreshImage();
		}
		else
		{
			setCharges(this->chargesDialog()->chargesInput()->value());
		}
	}
}

void TextureEditor::setCharges(int charges)
{
	refreshImage();
	QPixmap pixmap = m_imageLabel->pixmap(Qt::ReturnByValue);
	QPainter painter(&pixmap);
	int x = pixmap.rect().width() - 20;
	int y = pixmap.rect().height() - 20;
	painter.setPen(QColor(Qt::black));
	QRect rect(x, y, pixmap.rect().width() - x, pixmap.rect().height() - y);
	painter.fillRect(rect, Qt::SolidPattern);
	painter.setPen(QColor(Qt::white));
	painter.drawText(rect, Qt::AlignCenter, QString::number(charges));
	m_imageLabel->setPixmap(pixmap);
}

void TextureEditor::showAlphaChannel()
{
	if (!hasTexture())
	{
		return;
	}

	if (!this->m_texture->qt()->hasAlphaChannel())
	{
		QMessageBox::critical(this, tr("Error"), tr("Image doesn't have alpha channel."));

		return;
	}

	if (showsAlphaChannel())
	{
		m_showAlphaChannelAction->setText(tr("Show alpha channel"));
		m_showsAlphaChannel = false;
	}
	else
	{
		m_showAlphaChannelAction->setText(tr("Hide alpha channel"));
		m_showsAlphaChannel = true;
	}

	refreshImage();
}

void TextureEditor::showTransparency()
{
	if (!hasTexture())
	{
		return;
	}

	/// \todo According to documentation checking for mask is an expensive operation.

	if (showsTransparency())
	{
		m_showTransparencyAction->setText(tr("Show transparency"));
		m_showsTransparency = false;
	}
	else
	{
		m_showTransparencyAction->setText(tr("Hide transparency"));
		m_showsTransparency = true;
	}

	refreshImage();
}

void TextureEditor::actualSize()
{
	if (!hasTexture())
	{
		return;
	}

	this->m_factor = 1.0;
	refreshImage();
}

void TextureEditor::zoomToFit()
{
	if (!hasTexture())
	{
		return;
	}

	this->m_zoomToFit = !this->m_zoomToFit;
	this->m_zoomToFitAction->setChecked(this->m_zoomToFit);
	refreshImage();
}

void TextureEditor::zoomIn()
{
	if (!hasTexture())
	{
		return;
	}

	this->m_factor += 0.20;
	refreshImage();
}

void TextureEditor::zoomOut()
{
	if (!hasTexture())
	{
		return;
	}

	this->m_factor -= 0.20;
	refreshImage();
}

void TextureEditor::massConverter()
{
}

void TextureEditor::showMipMap()
{
	QAction *action = boost::polymorphic_cast<QAction*>(sender());
	m_mipMapIndex = this->m_mipMapsMenu->actions().indexOf(action);
	m_showsAlphaChannel = false;
	m_showsTransparency = false;
	m_factor = 1.0;
	m_zoomToFit = true;
	this->m_zoomToFitAction->setChecked(this->m_zoomToFit);
	refreshImage();
}

void TextureEditor::refreshImage()
{
	if (!hasTexture())
	{
		this->m_imageLabel->setPixmap(QPixmap());
		// set the minimum size so that the scroll area will show scroll bars for the whole image size
		this->m_imageLabel->setMinimumSize(QSize(0, 0));
		this->m_imageLabel->setMaximumSize(QSize(0, 0));

		return;
	}

	QPixmap newPixmap;
	QImage image = mipMaps()[mipMapIndex()];

	if (m_zoomToFit)
	{
		const QSize fitSize = this->scrollArea()->maximumViewportSize();


		if (!showsAlphaChannel())
		{
			newPixmap = QPixmap::fromImage(image).scaled(fitSize, Qt::KeepAspectRatio);
		}
		else
		{
			newPixmap = QPixmap::fromImage(image.createAlphaMask()).scaled(fitSize, Qt::KeepAspectRatio);
		}
	}
	else
	{
		if (!showsAlphaChannel())
		{
			newPixmap = QPixmap::fromImage(image).scaled(mipMaps()[mipMapIndex()].size() * this->factor());
		}
		else
		{
			newPixmap = QPixmap::fromImage(image.createAlphaMask()).scaled(mipMaps()[mipMapIndex()].size() * this->factor());
		}
	}

	if (showsTransparency())
	{
		newPixmap.setMask(this->m_imageLabel->pixmap(Qt::ReturnByValue).createMaskFromColor(Qt::transparent));
	}

	this->m_imageLabel->setPixmap(newPixmap);

	// fit to the viewport's size
	if (this->m_zoomToFit)
	{
		const QSize fitSize = this->scrollArea()->maximumViewportSize();

		this->m_imageLabel->setMinimumSize(fitSize);
		this->m_imageLabel->setMaximumSize(fitSize);
	}
	// set the minimum size so that the scroll area will show scroll bars for the whole image size
	else
	{
		this->m_imageLabel->setMinimumSize(newPixmap.size());
		this->m_imageLabel->setMaximumSize(newPixmap.size());
	}
}

void TextureEditor::setImageActionsEnabled(bool enabled)
{
	this->m_showAlphaChannelAction->setEnabled(enabled);
	this->m_showTransparencyAction->setEnabled(enabled);
	this->m_actualSizeAction->setEnabled(enabled);
	this->m_zoomToFitAction->setEnabled(enabled);
	this->m_zoomInAction->setEnabled(enabled);
	this->m_zoomOutAction->setEnabled(enabled);
	this->m_editColorPaletteAction->setEnabled(enabled);
	this->m_dropColorPaletteAction->setEnabled(enabled);
	this->m_setChargesAction->setEnabled(enabled);
}

void TextureEditor::createFileActions(QMenu *menu)
{
	QAction *action = 0;

	action = new QAction(QIcon(":/actions/opentexture.png"), tr("Open texture"), this);
	action->setShortcut(QKeySequence("Ctrl+O"));
	connect(action, SIGNAL(triggered()), this, SLOT(openFile()));
	menu->addAction(action);

	action = new QAction(QIcon(":/actions/savetexture.png"), tr("Save texture"), this);
	action->setShortcut(QKeySequence("Ctrl+S"));
	connect(action, SIGNAL(triggered()), this, SLOT(saveFile()));
	menu->addAction(action);

	action = new QAction(QIcon(":/actions/closetexture.png"), tr("Close texture"), this);
	action->setShortcut(QKeySequence("Ctrl+W"));
	connect(action, SIGNAL(triggered()), this, SLOT(closeFile()));
	menu->addAction(action);

	action = new QAction(QIcon(":/actions/settings.png"), tr("Settings"), this);
	//action->setShortcut(KShortcut(tr("Ctrl+O")));
	connect(action, SIGNAL(triggered()), this, SLOT(showSettings()));
	menu->addAction(action);
}

void TextureEditor::createEditActions(QMenu *menu)
{
	m_editColorPaletteAction = new QAction(QIcon(":/actions/editcolorpalette.png"), tr("Edit color palette"), this);
	connect(m_editColorPaletteAction, SIGNAL(triggered()), this, SLOT(editColorPalette()));
	menu->addAction(m_editColorPaletteAction);

	m_dropColorPaletteAction = new QAction(QIcon(":/actions/dropcolorpalette.png"), tr("Drop color palette"), this);
	connect(m_dropColorPaletteAction, SIGNAL(triggered()), this, SLOT(dropColorPalette()));
	menu->addAction(m_dropColorPaletteAction);

	m_setChargesAction = new QAction(QIcon(":/actions/setcharges.png"), tr("Set charges"), this);
	connect(m_setChargesAction, SIGNAL(triggered()), this, SLOT(setCharges()));
	menu->addAction(m_setChargesAction);
}

void TextureEditor::createMenus(QMenuBar *menuBar)
{
	QMenu *viewMenu = new QMenu(tr("View"), this);
	//this->m_viewMenu = viewMenu;
	menuBar->addMenu(viewMenu);

	m_showAlphaChannelAction = new QAction(QIcon(":/actions/showalphachannel.png"), tr("Show alpha channel"), this);
	connect(m_showAlphaChannelAction, SIGNAL(triggered()), this, SLOT(showAlphaChannel()));
	viewMenu->addAction(m_showAlphaChannelAction);

	m_showTransparencyAction = new QAction(QIcon(":/actions/showtransparency.png"), tr("Show transparency"), this);
	connect(m_showTransparencyAction, SIGNAL(triggered()), this, SLOT(showTransparency()));
	viewMenu->addAction(m_showTransparencyAction);

	m_actualSizeAction = new QAction(QIcon(":/actions/actualsize.png"), tr("Actual size"), this);
	connect(m_actualSizeAction, SIGNAL(triggered()), this, SLOT(actualSize()));
	viewMenu->addAction(m_actualSizeAction);

	m_zoomToFitAction = new QAction(QIcon(":/actions/zoomtofit.png"), tr("Zoom to fit"), this);
	m_zoomToFitAction->setCheckable(true);
	connect(m_zoomToFitAction, SIGNAL(triggered()), this, SLOT(zoomToFit()));
	viewMenu->addAction(m_zoomToFitAction);

	m_zoomInAction = new QAction(QIcon(":/actions/zoomin.png"), tr("Zoom in"), this);
	connect(m_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
	viewMenu->addAction(m_zoomInAction);

	m_zoomOutAction = new QAction(QIcon(":/actions/zoomout.png"), tr("Zoom out"), this);
	connect(m_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
	viewMenu->addAction(m_zoomOutAction);

	QMenu *toolsMenu = new QMenu(tr("Tools"), this);
	//this->m_toolsMenu = toolsMenu;
	menuBar->addMenu(toolsMenu);

	// TODO add check buttons to mass converter widget which allow you to a) convert and b) generate info cards etc. and c) to remove old files.
	m_massConverterAction = new QAction(QIcon(":/actions/massconverter.png"), tr("Mass converter"), this);
	connect(m_massConverterAction, SIGNAL(triggered()), this, SLOT(massConverter()));
	toolsMenu->addAction(m_massConverterAction);

	this->m_mipMapsMenu = new QMenu(tr("MIP maps"), this);
	menuBar->addMenu(m_mipMapsMenu);
}

void TextureEditor::createWindowsActions(WindowsMenu *menu)
{
}

void TextureEditor::createToolButtons(ModuleToolBar *toolBar)
{
}

SettingsInterface* TextureEditor::settings()
{
	/// @todo FIXME
	return 0;
}

void TextureEditor::onSwitchToMap(Map *map)
{
}

void TextureEditor::resizeEvent(QResizeEvent *event)
{
	if (m_zoomToFit)
	{
		refreshImage();
	}

	Module::resizeEvent(event);
}

QIcon TextureEditor::icon()
{
	return QIcon(this->source()->sharedData()->worldEditDataPixmap("ToolBarIcon_Module_Terrain", "WorldEditArt", this));
}

#include "moc_textureeditor.cpp"

}

}
