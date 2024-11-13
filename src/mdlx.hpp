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

#ifndef WC3LIB_MDLX_HPP
#define WC3LIB_MDLX_HPP

namespace wc3lib
{
/**
 * \page mdl MDL
 * MDL is the human readable model file format for Warcraft III.
 * It has a rather simple structure:
 * <code>
 * TODO add example
 * </code>
 *
 * \ref mdlx::MdlGrammar provides parsing support.
 * \ref mdlx::MdlGenerator provides generating support.
 *
 * \page mdx MDX
 * The MDX format is Warcraft III's binary model file format.
 *
 * \page mdlxsection MDLX module
 * MDLX refers both formats for Warcraft III's model files:
 * <ul>
 * <li>MDL - the human readable version</li>
 * <li>MDX - the compressed binary version</li>
 * </ul>
 *
 * For further information see \ref mdl and \ref mdx.
 *
 * This module provides several classes to read and write Blizzard's 3D graphics formats MDX (binary) and MDL (human-readable).
 * It does not provide any display/rendering functionality. Use module \ref editor for this.
 *
 * Class \ref wc3lib::mdlx::Mdlx provides all functionality which is needed to save or load one MDX or MDL file.
 * The class can easily be used to convert one format into the other one (application "wc3converter" provides this feature).
 *
 * Include file \ref mdlx.hpp and use namespace \ref wc3lib::mdlx to for this module.
 *
 * \section groups Groups
 * MDX and MDL often use groups of entries for specific sections. For example lights are stored as list of entries without specified size.
 * For each group there is a class like \ref mdlx::Lights which inherits \ref mdlx::GroupMdxBlock and stores a number of \ref mdlx::GroupMdxBlockMember, one for each entry.
 *
 * For accessing the members you have to use \ref mdlx::Lights::members() and cast a member of type \ref mdlx::GroupMdxBlockMember to \ref mdlx::Light.
 *
 * The implementation therefore relies on polymorphism:
 * \code
 * Light *light2 = boost::polymorphic_cast<Light*>(&lights->members().front());
 * \endcode
 *
 * \section animated Animated Properties
 * MDX and MDL support properties which can be animated using a specified time of a sequence.
 * Those properties are scalings, transformations or rotations.
 *
 * \ref mdlx::MdlxAnimatedProperties is a template which offers all required data using a dimension parameter to specify which number of numeric values is used.
 * For example scalings and transformations usually need three coordinates whereas a rotation uses a quaternion with four values.
 * Alphas which can be animated as well only have one single value.
 *
 * Therefore \ref mdlx::MdlxAnimatedProperties is the base class of \ref mdlx::MdlxScalings, \ref mdlx::MdlxTranslations, \ref mdlx::MdlxRotations and \ref mdlx::MdlxAlphas which use different dimensions (except scalings and translations).
 *
 * It also manages the storage of entries using \ref mdlx::MdlxAnimatedProperty similar to the group member managemenent.
 * Use \ref mdlx::MdlxAnimatedProperties::properties() to access all members.
 *
 * \namespace wc3lib::mdlx
 * \brief \ref mdlxsection
 */
}
#include "mdlx/attachment.hpp"
#include "mdlx/bone.hpp"
#include "mdlx/bounds.hpp"
#include "mdlx/camera.hpp"
#include "mdlx/collisionshape.hpp"
#include "mdlx/event.hpp"
#include "mdlx/geoset.hpp"
#include "mdlx/geosetanimation.hpp"
#include "mdlx/interpolator.hpp"
#include "mdlx/layer.hpp"
#include "mdlx/light.hpp"
#include "mdlx/material.hpp"
#include "mdlx/mdlgenerator.hpp"
#include "mdlx/mdlgrammar.hpp"
#include "mdlx/mdlgrammarclient.hpp"
#include "mdlx/mdlx.hpp"
#include "mdlx/mdlxanimatedproperties.hpp"
#include "mdlx/mdlxanimatedproperty.hpp"
#include "mdlx/model.hpp"
#include "mdlx/node.hpp"
#include "mdlx/object.hpp"
#include "mdlx/particleemitter.hpp"
#include "mdlx/particleemitter2.hpp"
#include "mdlx/platform.hpp"
#include "mdlx/ribbonemitter.hpp"
#include "mdlx/sequence.hpp"
#include "mdlx/texture.hpp"
#include "mdlx/textureanimation.hpp"

#endif
