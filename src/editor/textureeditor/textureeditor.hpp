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

#include <QScopedPointer>

#include <KUrl>
#include <KAction>
#include <KFileDialog>
#include <KDialogButtonBox>

#include "../module.hpp"
#include "../texture.hpp"
#include "../colorpalettedialog.hpp"

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
		class LoadDialogWidget : public QWidget
		{
			public:
				LoadDialogWidget(QWidget *parent = 0);

				class KIntNumInput *mipMapsInput() const;
				class QCheckBox *threadsCheckBox() const;

			private:
				class KIntNumInput *m_mipMapsInput;
				class QCheckBox *m_threadsCheckBox;
		};

		class LoadDialog : public QObject
		{
			public:
				LoadDialog(QObject *parent);
				~LoadDialog();

				KFileDialog *dialog() const;
				LoadDialogWidget *widget() const;
				class KIntNumInput *mipMapsInput() const;
				class QCheckBox *threadsCheckBox() const;

			private:
				KFileDialog *m_dialog;
				LoadDialogWidget *m_widget;
		};

		class SaveDialogWidget : public QWidget
		{
			public:
				SaveDialogWidget(QWidget *parent = 0);

				class KIntNumInput *qualityInput() const;
				class KIntNumInput *mipMapsInput() const;
				class QCheckBox *threadsCheckBox() const;

			private:
				class KIntNumInput *m_qualityInput;
				class KIntNumInput *m_mipMapsInput;
				class QCheckBox *m_threadsCheckBox;
		};

		class SaveDialog : public QObject
		{
			public:
				SaveDialog(QObject *parent);
				~SaveDialog();

				KFileDialog *dialog() const;
				SaveDialogWidget *widget() const;
				class KIntNumInput *qualityInput() const;
				class KIntNumInput *mipMapsInput() const;
				class QCheckBox *threadsCheckBox() const;

			private:
				KFileDialog *m_dialog;
				SaveDialogWidget *m_widget;
		};

		class ChargesDialog : public QDialog
		{
			public:
				ChargesDialog(QWidget *parent);
				KIntNumInput* chargesInput() const;
				QCheckBox* hasChargesCheckBox() const;

			private:
				KDialogButtonBox *m_buttonBox;
				KIntNumInput *m_chargesInput;
				QCheckBox *m_hasChargesCheckBox;

		};

		typedef QScopedPointer<Texture> TexturePtr;
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

		ColorPaletteDialog* colorPaletteDialog() const;
		ChargesDialog* chargesDialog() const;
		LoadDialog* loadDialog() const;
		SaveDialog* saveDialog() const;

	public slots:
		/**
		 * \note Exception safe.
		 */
		void openFile();
		void openUrl(const KUrl &url, QMap<QString, QString> options);
		void openUrl(const KUrl &url)
		{
			openUrl(url, QMap<QString, QString>());
		}

		void saveFile();
		void closeFile();
		void showSettings();

		void editColorPalette();
		void dropColorPalette();
		void makeActive();
		void makePassive();
		void makeAutocast();
		void makeInfocardNormal();
		void makeInfocardLevel();
		void setCharges();
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
		virtual void createToolButtons(class ModuleToolBar *toolBar);
		virtual class SettingsInterface* settings();
		virtual void onSwitchToMap(class Map *map);
		virtual KAboutData moduleAboutData() const;
		virtual QString actionName() const;

		virtual void resizeEvent(QResizeEvent *event);

		QLabel *m_imageLabel;
		TexturePtr m_texture;
		MipMaps m_mipMaps;
		short m_mipMapIndex;
		bool m_showsAlphaChannel;
		bool m_showsTransparency;
		qreal m_factor;
		bool m_zoomToFit;
		KUrl m_recentUrl;

		class KActionCollection *m_textureActionCollection;
		KAction *m_showAlphaChannelAction;
		KAction *m_showTransparencyAction;
		KAction *m_zoomToFitAction;
		KMenu *m_mipMapsMenu;

		ColorPaletteDialog *m_colorPaletteDialog;
		ChargesDialog *m_chargesDialog;
		LoadDialog *m_loadDialog;
		SaveDialog *m_saveDialog;
};

inline KIntNumInput* TextureEditor::LoadDialogWidget::mipMapsInput() const
{
	return this->m_mipMapsInput;
}

inline QCheckBox* TextureEditor::LoadDialogWidget::threadsCheckBox() const
{
	return this->m_threadsCheckBox;
}

inline KFileDialog* TextureEditor::LoadDialog::dialog() const
{
	return this->m_dialog;
}

inline TextureEditor::LoadDialogWidget* TextureEditor::LoadDialog::widget() const
{
	return this->m_widget;
}

inline KIntNumInput* TextureEditor::LoadDialog::mipMapsInput() const
{
	return this->m_widget->mipMapsInput();
}

inline QCheckBox* TextureEditor::LoadDialog::threadsCheckBox() const
{
	return this->m_widget->threadsCheckBox();
}

inline KIntNumInput* TextureEditor::SaveDialogWidget::qualityInput() const
{
	return this->m_qualityInput;
}

inline KIntNumInput* TextureEditor::SaveDialogWidget::mipMapsInput() const
{
	return this->m_mipMapsInput;
}

inline QCheckBox* TextureEditor::SaveDialogWidget::threadsCheckBox() const
{
	return this->m_threadsCheckBox;
}

inline KFileDialog* TextureEditor::SaveDialog::dialog() const
{
	return this->m_dialog;
}

inline TextureEditor::SaveDialogWidget* TextureEditor::SaveDialog::widget() const
{
	return this->m_widget;
}

inline KIntNumInput* TextureEditor::SaveDialog::qualityInput() const
{
	return this->m_widget->qualityInput();
}

inline KIntNumInput* TextureEditor::SaveDialog::mipMapsInput() const
{
	return this->m_widget->mipMapsInput();
}

inline QCheckBox* TextureEditor::SaveDialog::threadsCheckBox() const
{
	return this->m_widget->threadsCheckBox();
}

inline KIntNumInput* TextureEditor::ChargesDialog::chargesInput() const
{
	return this->m_chargesInput;
}

inline QCheckBox* TextureEditor::ChargesDialog::hasChargesCheckBox() const
{
	return this->m_hasChargesCheckBox;
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
	return texture().data() != 0;
}

inline ColorPaletteDialog* TextureEditor::colorPaletteDialog() const
{
	if (this->m_colorPaletteDialog == 0)
		const_cast<TextureEditor*>(this)->m_colorPaletteDialog = new ColorPaletteDialog(const_cast<TextureEditor*>(this));

	return this->m_colorPaletteDialog;
}

inline TextureEditor::ChargesDialog* TextureEditor::chargesDialog() const
{
	if (this->m_chargesDialog == 0)
		const_cast<TextureEditor*>(this)->m_chargesDialog = new ChargesDialog(const_cast<TextureEditor*>(this));

	return this->m_chargesDialog;
}

inline TextureEditor::LoadDialog *TextureEditor::loadDialog() const
{
	if (this->m_loadDialog == 0)
		const_cast<TextureEditor*>(this)->m_loadDialog = new LoadDialog(const_cast<TextureEditor*>(this));

	return this->m_loadDialog;
}

inline TextureEditor::SaveDialog *TextureEditor::saveDialog() const
{
	if (this->m_saveDialog == 0)
		const_cast<TextureEditor*>(this)->m_saveDialog = new SaveDialog(const_cast<TextureEditor*>(this));

	return this->m_saveDialog;
}

inline KAboutData TextureEditor::moduleAboutData() const
{
	KAboutData aboutData(Module::moduleAboutData());
	aboutData.setProgramName(ki18n("Texture Editor"));

	return aboutData;
}

inline QString TextureEditor::actionName() const
{
	return "textureeditor";
}


}

}

#endif
