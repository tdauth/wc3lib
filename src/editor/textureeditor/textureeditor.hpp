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
#include <QCheckBox>
#include <QAction>
#include <QSpinBox>
#include <QDialog>
#include <QFileDialog>
#include <QScrollArea>

#include "../module.hpp"
#include "../texture.hpp"

namespace wc3lib
{

namespace editor
{

class MpqPriorityList;

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

				QSpinBox *mipMapsInput() const;

			private:
				QSpinBox *m_mipMapsInput;
		};

		class LoadDialog : public QWidget
		{
			public:
				LoadDialog(QWidget *parent);
				~LoadDialog();

				QFileDialog *dialog() const;
				LoadDialogWidget *widget() const;
				QSpinBox *mipMapsInput() const;

			private:
				QFileDialog *m_dialog;
				LoadDialogWidget *m_widget;
		};

		class SaveDialogWidget : public QWidget
		{
			public:
				SaveDialogWidget(QWidget *parent = 0);

				QSpinBox *qualityInput() const;
				QSpinBox *mipMapsInput() const;

			private:
				QSpinBox *m_qualityInput;
				QSpinBox *m_mipMapsInput;
		};

		class SaveDialog : public QWidget
		{
			public:
				SaveDialog(QWidget *parent);
				~SaveDialog();

				QFileDialog *dialog() const;
				SaveDialogWidget *widget() const;
				QSpinBox *qualityInput() const;
				QSpinBox *mipMapsInput() const;

			private:
				QFileDialog *m_dialog;
				SaveDialogWidget *m_widget;
		};

		class ChargesDialog : public QDialog
		{
			public:
				ChargesDialog(QWidget *parent);
				QSpinBox* chargesInput() const;
				QCheckBox* hasChargesCheckBox() const;

			private:
				QDialogButtonBox *m_buttonBox;
				QSpinBox *m_chargesInput;
				QCheckBox *m_hasChargesCheckBox;

		};

		typedef QScopedPointer<Texture> TexturePtr;
		typedef QVector<QImage> MipMaps;

		TextureEditor(MpqPriorityList *source, const QString &organization, const QString &applicationName, QWidget *parent = 0, Qt::WindowFlags f = 0);
		virtual ~TextureEditor();

		virtual bool configure() override;
		virtual void retranslateUi() override;

		QScrollArea* scrollArea() const;
		QLabel* imageLabel() const;
		const TexturePtr& texture() const;
		const MipMaps& mipMaps() const;
		short mipMapIndex() const;
		bool showsAlphaChannel() const;
		bool showsTransparency() const;
		qreal factor() const;

		bool hasTexture() const;

		ChargesDialog* chargesDialog() const;
		LoadDialog* loadDialog() const;
		SaveDialog* saveDialog() const;

	public slots:
		/**
		 * \note Exception safe.
		 */
		void openFile();
		void openUrl(const QUrl &url, QMap<QString, QString> options);
		void openUrl(const QUrl &url)
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
		void setImageActionsEnabled(bool enabled);

		virtual void createFileActions(QMenu *menu) override;
		virtual void createEditActions(QMenu *menu) override;
		virtual void createMenus(QMenuBar *menuBar) override;
		virtual void createWindowsActions(WindowsMenu *menu) override;
		virtual void createToolButtons(ModuleToolBar *toolBar) override;
		virtual SettingsInterface* settings() override;
		virtual void onSwitchToMap(Map *map) override;
		virtual QString actionName() const override;
		virtual QIcon icon() override;

		virtual void resizeEvent(QResizeEvent *event) override;

		QScrollArea *m_scrollArea;
		QLabel *m_imageLabel;
		TexturePtr m_texture;
		MipMaps m_mipMaps;
		short m_mipMapIndex;
		bool m_showsAlphaChannel;
		bool m_showsTransparency;
		qreal m_factor;
		bool m_zoomToFit;
		QUrl m_recentUrl;

		QAction *m_showAlphaChannelAction;
		QAction *m_showTransparencyAction;
		QAction *m_actualSizeAction;
		QAction *m_zoomToFitAction;
		QAction *m_zoomInAction;
		QAction *m_zoomOutAction;
		QAction *m_massConverterAction;
		QMenu *m_mipMapsMenu;

		QAction *m_editColorPaletteAction;
		QAction *m_dropColorPaletteAction;
		QAction *m_setChargesAction;

		ChargesDialog *m_chargesDialog;
		LoadDialog *m_loadDialog;
		SaveDialog *m_saveDialog;
};

inline QSpinBox* TextureEditor::LoadDialogWidget::mipMapsInput() const
{
	return this->m_mipMapsInput;
}

inline QFileDialog* TextureEditor::LoadDialog::dialog() const
{
	return this->m_dialog;
}

inline TextureEditor::LoadDialogWidget* TextureEditor::LoadDialog::widget() const
{
	return this->m_widget;
}

inline QSpinBox* TextureEditor::LoadDialog::mipMapsInput() const
{
	return this->m_widget->mipMapsInput();
}

inline QSpinBox* TextureEditor::SaveDialogWidget::qualityInput() const
{
	return this->m_qualityInput;
}

inline QSpinBox* TextureEditor::SaveDialogWidget::mipMapsInput() const
{
	return this->m_mipMapsInput;
}

inline QFileDialog* TextureEditor::SaveDialog::dialog() const
{
	return this->m_dialog;
}

inline TextureEditor::SaveDialogWidget* TextureEditor::SaveDialog::widget() const
{
	return this->m_widget;
}

inline QSpinBox* TextureEditor::SaveDialog::qualityInput() const
{
	return this->m_widget->qualityInput();
}

inline QSpinBox* TextureEditor::SaveDialog::mipMapsInput() const
{
	return this->m_widget->mipMapsInput();
}

inline QSpinBox* TextureEditor::ChargesDialog::chargesInput() const
{
	return this->m_chargesInput;
}

inline QCheckBox* TextureEditor::ChargesDialog::hasChargesCheckBox() const
{
	return this->m_hasChargesCheckBox;
}

inline QScrollArea* TextureEditor::scrollArea() const
{
	return this->m_scrollArea;
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

inline TextureEditor::ChargesDialog* TextureEditor::chargesDialog() const
{
	if (this->m_chargesDialog == 0)
	{
		const_cast<TextureEditor*>(this)->m_chargesDialog = new ChargesDialog(const_cast<TextureEditor*>(this));
	}

	return this->m_chargesDialog;
}

inline TextureEditor::LoadDialog *TextureEditor::loadDialog() const
{
	if (this->m_loadDialog == 0)
	{
		const_cast<TextureEditor*>(this)->m_loadDialog = new LoadDialog(const_cast<TextureEditor*>(this));
	}

	return this->m_loadDialog;
}

inline TextureEditor::SaveDialog *TextureEditor::saveDialog() const
{
	if (this->m_saveDialog == 0)
	{
		const_cast<TextureEditor*>(this)->m_saveDialog = new SaveDialog(const_cast<TextureEditor*>(this));
	}

	return this->m_saveDialog;
}

inline QString TextureEditor::actionName() const
{
	return "textureeditor";
}

}

}

#endif
