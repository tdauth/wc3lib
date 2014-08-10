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

#ifndef WC3LIB_EDITOR_MODELEDITORVIEW_HPP
#define WC3LIB_EDITOR_MODELEDITORVIEW_HPP

#include "../modelview.hpp"
#include "../root.hpp"

namespace wc3lib
{

namespace editor
{

/**
 * Customized version of \ref ModelView which implements "hit test" and other things required by \ref ModelEditor.
 */
class ModelEditorView : public ModelView
{
	public:
		ModelEditorView(Root *root, class ModelEditor *modelEditor, Qt::WFlags f = 0, Ogre::SceneType ogreSceneType = Ogre::ST_EXTERIOR_CLOSE, const Ogre::NameValuePairList *ogreParameters = 0);
		virtual ~ModelEditorView();

		class ModelEditor* modelEditor() const;
		void setHitTest(bool hitTest);
		bool hitTest() const;

	protected:
		/// Implements "hit test" based selection. \sa ModelEditor::hitTest.
		virtual void mousePressEvent(QMouseEvent *event);

		class ModelEditor *m_modelEditor;
		bool m_hitTest;
};

inline class ModelEditor* ModelEditorView::modelEditor() const
{
	return m_modelEditor;
}

inline void ModelEditorView::setHitTest(bool hitTest)
{
	this->m_hitTest = hitTest;
}

inline bool ModelEditorView::hitTest() const
{
	return this->m_hitTest;
}

}

}

#endif
