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
 * \page mdlxsection MDLX module
 * This module provides several classes to read and write Blizzard's 3D graphics formats MDX (binary) and MDL (human-readable).
 * 
 * It does not provide any display/rendering functionality. Use module \ref editor for this.
 * 
 * Class \ref wc3lib::mdlx::Mdlx provides all functionality which is needed to save or load one MDX or MDL file.
 * The class can easily be used to convert one format into the other one (application "wc3converter" provides this feature).
 * 
 * Include file \ref mdlx.hpp and use namespace \ref wc3lib::mdlx to for this module.
 * 
 * \section groups Groups
 * MDX and MDL often use groups of entries for specific sections. For example lights are stored as list of entries without specified size.
 * For each group there is a class like \ref Lights which inherits \ref GroupMdxBlock and stores a number of \ref GroupMdxBlockMember, one for each entry.
 * 
 * For accessing the members you have to use \ref Lights::members() and cast a member of type \ref GroupMdxBlockMember to \ref Light.
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
 * \ref MdlxAnimatedProperties is a template which offers all required data using a dimension parameter to specify which number of numeric values is used.
 * For example scalings and transformations usually need three coordinates whereas a rotation uses a quaternion with four values.
 * Alphas which can be animated as well only have one single value.
 * 
 * Therefore \ref MdlxAnimatedProperties is the base class of \ref MdlxScalings, \ref MdlxTranslations, \ref MdlxRotations and \ref MdlxAlphas which use different dimensions (except scalings and translations).
 * 
 * It also manages the storage of entries using \ref MdlxAnimatedProperty similar to the group member managemenent.
 * Use \ref MdlxAnimatedProperties::properties() to access all members.
 * 
 * \namespace wc3lib::mdlx
 * \brief \ref mdlxsection
 */
}

#include "mdlx/ambientcolor.hpp"
#include "mdlx/ambientcolors.hpp"
#include "mdlx/ambientintensities.hpp"
#include "mdlx/ambientintensity.hpp"
#include "mdlx/attachment.hpp"
#include "mdlx/attachments.hpp"
#include "mdlx/attachmentvisibilities.hpp"
#include "mdlx/attachmentvisibility.hpp"
#include "mdlx/bone.hpp"
#include "mdlx/bones.hpp"
#include "mdlx/bounds.hpp"
#include "mdlx/camera.hpp"
#include "mdlx/camerarotationlength.hpp"
#include "mdlx/camerarotationlengths.hpp"
#include "mdlx/cameras.hpp"
#include "mdlx/cameratargettranslation.hpp"
#include "mdlx/cameratargettranslations.hpp"
#include "mdlx/cameratranslation.hpp"
#include "mdlx/cameratranslations.hpp"
#include "mdlx/collisionshape.hpp"
#include "mdlx/collisionshapes.hpp"
#include "mdlx/emissionrate.hpp"
#include "mdlx/emissionrates.hpp"
#include "mdlx/event.hpp"
#include "mdlx/events.hpp"
#include "mdlx/eventtrack.hpp"
#include "mdlx/eventtracks.hpp"
#include "mdlx/ganimation.hpp"
#include "mdlx/geosetanimationalpha.hpp"
#include "mdlx/geosetanimationalphas.hpp"
#include "mdlx/geosetanimationcolor.hpp"
#include "mdlx/geosetanimationcolors.hpp"
#include "mdlx/geosetanimation.hpp"
#include "mdlx/geosetanimations.hpp"
#include "mdlx/geoset.hpp"
#include "mdlx/geosets.hpp"
#include "mdlx/globalsequence.hpp"
#include "mdlx/globalsequences.hpp"
#include "mdlx/groupmdxblock.hpp"
#include "mdlx/groupmdxblockmember.hpp"
#include "mdlx/groupvertex.hpp"
#include "mdlx/groupvertices.hpp"
#include "mdlx/helper.hpp"
#include "mdlx/helpers.hpp"
#include "mdlx/intensities.hpp"
#include "mdlx/intensity.hpp"
#include "mdlx/layer.hpp"
#include "mdlx/layers.hpp"
#include "mdlx/lengths.hpp"
#include "mdlx/lightambientcolor.hpp"
#include "mdlx/lightambientcolors.hpp"
#include "mdlx/lightambientvisibilities.hpp"
#include "mdlx/lightambientvisibility.hpp"
#include "mdlx/light.hpp"
#include "mdlx/lights.hpp"
#include "mdlx/materialalpha.hpp"
#include "mdlx/materialalphas.hpp"
#include "mdlx/material.hpp"
#include "mdlx/materials.hpp"
#include "mdlx/matrices.hpp"
#include "mdlx/matrixgroupcount.hpp"
#include "mdlx/matrixgroupcounts.hpp"
#include "mdlx/matrix.hpp"
#include "mdlx/mdlx.hpp"
#include "mdlx/mdlxalpha.hpp"
#include "mdlx/mdlxalphas.hpp"
#include "mdlx/mdlxrotation.hpp"
#include "mdlx/mdlxrotations.hpp"
#include "mdlx/mdlxscaling.hpp"
#include "mdlx/mdlxscalings.hpp"
#include "mdlx/mdlxtranslation.hpp"
#include "mdlx/mdlxtranslations.hpp"
#include "mdlx/mdxblock.hpp"
#include "mdlx/model.hpp"
#include "mdlx/node.hpp"
#include "mdlx/normal.hpp"
#include "mdlx/normals.hpp"
#include "mdlx/object.hpp"
#include "mdlx/particleemitter2.hpp"
#include "mdlx/particleemitter2latitude.hpp"
#include "mdlx/particleemitter2latitudes.hpp"
#include "mdlx/particleemitter2s.hpp"
#include "mdlx/particleemitter2speed.hpp"
#include "mdlx/particleemitter2speeds.hpp"
#include "mdlx/particleemitter2visibilities.hpp"
#include "mdlx/particleemitter2visibility.hpp"
#include "mdlx/particleemitter2width.hpp"
#include "mdlx/particleemitter2widths.hpp"
#include "mdlx/particleemitter.hpp"
#include "mdlx/particleemitters.hpp"
#include "mdlx/particleemittervisibilities.hpp"
#include "mdlx/particleemittervisibility.hpp"
#include "mdlx/pivotpoint.hpp"
#include "mdlx/pivotpoints.hpp"
#include "mdlx/platform.hpp"
#include "mdlx/primitivesize.hpp"
#include "mdlx/primitivesizes.hpp"
#include "mdlx/primitivetype.hpp"
#include "mdlx/primitivetypes.hpp"
#include "mdlx/primitivevertex.hpp"
#include "mdlx/primitivevertices.hpp"
#include "mdlx/ribbonemitterheightabove.hpp"
#include "mdlx/ribbonemitterheightbelow.hpp"
#include "mdlx/ribbonemitterheightsabove.hpp"
#include "mdlx/ribbonemitterheightsbelow.hpp"
#include "mdlx/ribbonemitter.hpp"
#include "mdlx/ribbonemitters.hpp"
#include "mdlx/ribbonemittervisibilities.hpp"
#include "mdlx/ribbonemittervisibility.hpp"
#include "mdlx/segmentcolor.hpp"
#include "mdlx/sequence.hpp"
#include "mdlx/sequences.hpp"
#include "mdlx/textureanimation.hpp"
#include "mdlx/textureanimationrotation.hpp"
#include "mdlx/textureanimationrotations.hpp"
#include "mdlx/textureanimationscaling.hpp"
#include "mdlx/textureanimationscalings.hpp"
#include "mdlx/textureanimations.hpp"
#include "mdlx/textureanimationtranslation.hpp"
#include "mdlx/textureanimationtranslations.hpp"
#include "mdlx/texture.hpp"
#include "mdlx/textureid.hpp"
#include "mdlx/textureids.hpp"
#include "mdlx/texturepatches.hpp"
#include "mdlx/texturepatch.hpp"
#include "mdlx/textures.hpp"
#include "mdlx/texturevertex.hpp"
#include "mdlx/texturevertices.hpp"
#include "mdlx/version.hpp"
#include "mdlx/vertex.hpp"
#include "mdlx/vertices.hpp"

#endif
