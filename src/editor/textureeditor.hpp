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

#ifndef WC3LIB_EDITOR_TEXTUREEDITOR_HPP
#define WC3LIB_EDITOR_TEXTUREEDITOR_HPP

#include <KUrl>
#include <KAction>
#include <KFileDialog>

#include "module.hpp"
#include "texture.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Allows you to open, view, modify and store skins/textures.
 * \todo Needs item list from object editor (skin meta data, splat meta data and ubersplat meta data).
 */
class TextureEditor : public Module
{
	Q_OBJECT

	public:
		class SaveDialog : public KFileDialog
		{
			public:
				SaveDialog(QWidget *parent);

				class KIntNumInput *qualityInput() const;
				class KIntNumInput *mipMapsInput() const;
				class QCheckBox *threadsCheckBox() const;

			private:
				class KIntNumInput *m_qualityInput;
				class KIntNumInput *m_mipMapsInput;
				class QCheckBox *m_threadsCheckBox;
		};

		typedef boost::scoped_ptr<Texture> TexturePtr;
		typedef QVector<QImage> MipMaps;

		TextureEditor(class MpqPriorityList *source, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~TextureEditor();

		QLabel* imageLabel() const;
		const TexturePtr& texture() const;
		const MipMaps& mipMaps() const;
		short mipMapIndex() const;
		bool showsAlphaChannel() const;
		bool showsTransparency() const;
		qreal factor() const;

		bool hasTexture() const;

		SaveDialog* saveDialog() const;

	public slots:
		/**
		 * \note Exception safe.
		 */
		void openFile();
		void saveFile();
		void closeFile();
		void showSettings();

		void editColorPalette();
		void makeActive();
		void makePassive();
		void makeAutocast();
		void makeInfocardNormal();
		void makeInfocardLevel();
		/**
		 * Adds little charges symbol with number \p charges to the texture.
		 */
		void setCharges(int charges);

		void showAlphaChannel();
		void showTransparency();
		void actualSize();
		void zoomToFit();
		void zoomIn();
		void zoomOut();

		void massConverter();

	protected slots:
		void showMipMap();

	protected:
		void refreshImage();

		virtual void createFileActions(class KMenu *menu);
		virtual void createEditActions(class KMenu *menu);
		virtual void createMenus(class KMenuBar *menuBar);
		virtual void createWindowsActions(class WindowsMenu *menu);
		virtual void createToolButtons(class KToolBar *toolBar);
		virtual class SettingsInterface* settings();
		virtual void onSwitchToMap(class Map *map);
		virtual QString actionName() const;

		QLabel *m_imageLabel;
		TexturePtr m_texture;
		MipMaps m_mipMaps;
		short m_mipMapIndex;
		bool m_showsAlphaChannel;
		bool m_showsTransparency;
		qreal m_factor;
		KUrl m_recentUrl;

		class KActionCollection *m_textureActionCollection;
		KAction *m_showAlphaChannelAction;
		KAction *m_showTransparencyAction;
		KMenu *m_mipMapsMenu;

		SaveDialog *m_saveDialog;
};

inline KIntNumInput* TextureEditor::SaveDialog::qualityInput() const
{
	return this->m_qualityInput;
}

inline KIntNumInput* TextureEditor::SaveDialog::mipMapsInput() const
{
	return this->m_mipMapsInput;
}

inline QCheckBox* TextureEditor::SaveDialog::threadsCheckBox() const
{
	return this->m_threadsCheckBox;
}


inline QLabel* TextureEditor::imageLabel() const
{
	return this->m_imageLabel;
}

inline const TextureEditor::TexturePtr& TextureEditor::texture() const
{
	return m_texture;
}

inline const TextureEditor::MipMaps& TextureEditor::mipMaps() const
{
	return m_mipMaps;
}

inline short TextureEditor::mipMapIndex() const
{
	return m_mipMapIndex;
}

inline bool TextureEditor::showsAlphaChannel() const
{
	return m_showsAlphaChannel;
}

inline bool TextureEditor::showsTransparency() const
{
	return m_showsTransparency;
}

inline qreal TextureEditor::factor() const
{
	return m_factor;
}

inline bool TextureEditor::hasTexture() const
{
	return texture().get() != 0;
}

inline TextureEditor::SaveDialog *TextureEditor::saveDialog() const
{
	if (this->m_saveDialog == 0)
		const_cast<TextureEditor*>(this)->m_saveDialog = new SaveDialog(const_cast<TextureEditor*>(this));

	return this->m_saveDialog;
}

inline QString TextureEditor::actionName() const
{
	return "textureeditor";
}


}

}

#endif
