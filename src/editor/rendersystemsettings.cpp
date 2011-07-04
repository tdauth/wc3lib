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

#include "rendersystemsettings.hpp"

namespace wc3lib
{

namespace editor
{

RenderSystemSettings::RenderSystemSettings(Ogre::RenderSystem *renderSystem) : m_renderSystem(renderSystem)
{
}

void RenderSystemSettings::read(const KConfigGroup &group)
{
	/// @todo video mode (resolution) -> config options map
	// renderSystem->getConfigOptions()["Video Mode"].currentValue
	// vsync
	// TODO These settings are stored and only activated when RenderSystem::initialise or RenderSystem::reinitialise are called.

	/*
	setAmbientLight (float r, float g, float b)=0
 	Sets the colour & strength of the ambient (global directionless) light in the world.
virtual void 	setShadingType (ShadeOptions so)=0
 	Sets the type of light shading required (default = Gouraud).
virtual void 	setLightingEnabled (bool enabled)=0
 	Sets whether or not dynamic lighting is enabled.
void 	setWBufferEnabled (bool enabled)
 	Sets whether or not W-buffers are enabled if they are available for this renderer.
 	*/
	//virtual void 	setStencilCheckEnabled (bool enabled)=0
 	//Turns stencil buffer checking on or off.
 	//virtual void 	setNormaliseNormals (bool normalise)=0
 	//Sets whether or not normals are to be automatically normalised.

	//virtual void 	setInvertVertexWinding (bool invert)
 	//Sets whether or not vertex windings set should be inverted; this can be important for rendering reflections.
	//virtual void 	setCurrentPassIterationCount (const size_t count)
 	//set the current multi pass count value.
}

void RenderSystemSettings::write(KConfigGroup &group) const
{
}

QString RenderSystemSettings::groupName() const
{
	return "RenderSystem";
}

}

}
