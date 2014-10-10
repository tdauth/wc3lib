/***************************************************************************
 *   Copyright (C) 2012 by Tamino Dauth                                    *
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

#ifndef WC3LIB_COLORPALETTEDIALOG_HPP
#define WC3LIB_COLORPALETTEDIALOG_HPP

#include <QImage>

#include <kdemacros.h>
#include <KDialog>
#include <KColorCells>

namespace wc3lib
{

namespace editor
{

class KDE_EXPORT ColorPaletteDialog : public KDialog
{
	Q_OBJECT

	public:
		explicit ColorPaletteDialog(QWidget *parent = 0);

		bool applyFromImage(const QImage &image);
		bool applyToImage(QImage &image);

		KColorCells* colorCells() const;

	private:
		KColorCells *m_colorCells;

};

inline bool ColorPaletteDialog::applyFromImage(const QImage &image)
{
	this->colorCells()->setColumnCount(16);
	this->colorCells()->setRowCount(image.colorCount() / this->colorCells()->columnCount());

	int i = 0;

	foreach (const QRgb &color, image.colorTable())
	{
		this->colorCells()->setColor(i, QColor(color));
		++i;
	}

	return true;
}

inline bool ColorPaletteDialog::applyToImage(QImage &image)
{
	if (image.colorCount() != this->colorCells()->colorCount())
	{
		return false;
	}

	QVector<QRgb> colorTable = image.colorTable();

	for (int i = 0; i < colorTable.size(); i++)
	{
		colorTable[i] = colorCells()->color(i).rgb();
	}

	image.setColorTable(colorTable);

	return true;
}

inline class KColorCells* ColorPaletteDialog::colorCells() const
{
	return this->m_colorCells;
}

}

}

#endif // COLORPALETTEDIALOG_HPP
