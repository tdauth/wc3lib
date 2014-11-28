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

#ifndef WC3LIB_MDLX_MATERIAL_HPP
#define WC3LIB_MDLX_MATERIAL_HPP

#include "platform.hpp"
#include "layer.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * MDL tag "Material".
 */
class Material
{
	public:
		enum class RenderMode : long32
		{
			ConstantColor = 1,
			Unknown0 = 2,
			Unknown1 = 4,
			SortPrimitivesNearZ = 8,
			SortPrimitivesFarZ = 16,
			FullResolution = 32
		};

		/**
		 * \brief The layers of a material.
		 *
		 * Each material can have multiple layers.
		 */
		typedef std::vector<Layer> Layers;

		Material();

		/**
		 * Taken from Art Tool manual:
		 * Alpha-queued geosets can be made to draw in a specific order with relation to each other in the same model. The lower the value is, the sooner it is rendererd. Values between -20 and 20 are regularly used.
		 *
		 * @{
		 */
		void setPriorityPlane(long32 priorityPlane);
		long32 priorityPlane() const;
		/**
		 * @}
		 */
		void setRenderMode(RenderMode renderMode);
		RenderMode renderMode() const;
		void setLayers(const Layers &layers);
		const Layers& layers() const;

	protected:
		//long nbytesi;
		long32 m_priorityPlane;
		RenderMode m_renderMode;
		Layers m_layers;
};

inline constexpr bool operator&(Material::RenderMode x, Material::RenderMode y)
{
	return static_cast<bool>(static_cast<long32>(x) & static_cast<long32>(y));
}

inline void Material::setPriorityPlane(long32 priorityPlane)
{
	this->m_priorityPlane = priorityPlane;
}

inline long32 Material::priorityPlane() const
{
	return this->m_priorityPlane;
}

inline void Material::setRenderMode(Material::RenderMode renderMode)
{
	this->m_renderMode = renderMode;
}

inline Material::RenderMode Material::renderMode() const
{
	return this->m_renderMode;
}

inline void Material::setLayers(const Material::Layers &layers)
{
	this->m_layers = layers;
}

inline const Material::Layers& Material::layers() const
{
	return this->m_layers;
}

}

}

#endif
