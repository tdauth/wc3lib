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

#ifndef WC3LIB_MDLX_NODE
#define WC3LIB_MDLX_NODE

#include "mdlxproperty.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * No MDX block!
 * Node types can have parent relationships and inherit rotation, scaling and translation of their parent if flags are set (\ref Node::Type::inheritsTranslation, \ref Node::Type::inheritsRotation, \ref Node::Type::inheritsScaling).
 * Consider that \ref Node::translations(), \ref Node::rotations() and \ref Node::scalings() won't return 0
 * if the node does inherit any of them. They do only have empty lists.
 * Use Node::hasParent to check if there is any parent inheritance relationship.
 * All objects are stored in a map of the Mdlx instance indicated with their id for easier and faster access (\ref Mdlx::node()).
 * \note MDLX format brings its own dynamic type system by saving the type of each node in its structure. Use Node::type to get an node's type.
* \note Inclusive byte count is only related to the note structure itself (excluding any further chunk data).
*/
class Node : public MdlxProperty
{
	public:
		BOOST_SCOPED_ENUM_START(Type) // : long32
		{
			Helper = 0,
			DontInheritTranslation = 1,
			DontInheritRotation = 2,
			DontInheritScaling = 4,
			Billboarded = 8,
			BillboardedLockX = 16,
			BillboardedLockY = 32,
			BillboardedLockZ = 64,
			CameraAnchored = 128,
			Bone = 256,
			Light = 512,
			EventObject = 1024,
			Attachment = 2048,
			ParticleEmitter = 4096,
			CollisionShape = 8192,
			RibbonEmitter = 16384,
			UnshadedOrEmitterUsesMdl = 32768,
			SortPrimitivesFarZOrEmitterUsesTga = 65536,
			LineEmitter = 131072,
			Unfogged = 262144,
			ModelSpace = 524288,
			XYQuad = 1048576
		};
		BOOST_SCOPED_ENUM_END
		
		static const std::size_t nameSize = 0x50;

		Node(class Mdlx *mdlx);
		virtual ~Node();

		void setName(const ascii name[nameSize]);
		/**
		 * \return Returns name with constant length of \ref nameSize.
		 */
		const ascii* name() const;
		long32 id() const;
		long32 parentId() const;
		bool hasParent() const;
		/**
		 * Type additions will be hold.
		 */
		void setType(BOOST_SCOPED_ENUM(Type) type);
		BOOST_SCOPED_ENUM(Type) type() const;
		void setTranslations(class MdlxTranslations *translations);
		class MdlxTranslations* translations() const;
		void setRotations(class MdlxRotations *rotations);
		class MdlxRotations* rotations() const;
		void setScalings(class MdlxScalings *scalings);
		class MdlxScalings* scalings() const;

		bool inheritsTranslation() const;
		bool inheritsRotation() const;
		bool inheritsScaling() const;

		virtual std::streamsize readMdl(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdl(ostream &ostream) const throw (class Exception);
		virtual std::streamsize readMdx(istream &istream) throw (class Exception);
		virtual std::streamsize writeMdx(ostream &ostream) const throw (class Exception);

		ostream& print(ostream &ostream) const;

	protected:
		class Mdlx *m_mdlx;
		ascii m_name[nameSize];
		long32 m_id;
		long32 m_parentId;
		BOOST_SCOPED_ENUM(Type) m_type;
		class MdlxTranslations *m_translations;
		class MdlxRotations *m_rotations;
		class MdlxScalings *m_scalings;
};


inline void Node::setName(const ascii name[Node::nameSize])
{
	memcpy(this->m_name, name, sizeof(name));
}


inline const ascii* Node::name() const
{
	return this->m_name;
}

inline long32 Node::id() const
{
	return this->m_id;
}

inline long32 Node::parentId() const
{
	return this->m_parentId;
}

inline bool Node::hasParent() const
{
	return this->m_parentId != 0xFFFFFFFF;
}

inline void Node::setType(BOOST_SCOPED_ENUM(Node::Type) type)
{
	this->m_type = type;
}

inline BOOST_SCOPED_ENUM(Node::Type) Node::type() const
{
	return this->m_type;
}

inline void Node::setTranslations(class MdlxTranslations *translations)
{
	this->m_translations = translations;
}

inline class MdlxTranslations* Node::translations() const
{
	return this->m_translations;
}

inline void Node::setRotations(class MdlxRotations *rotations)
{
	this->m_rotations = rotations;
}

inline class MdlxRotations* Node::rotations() const
{
	return this->m_rotations;
}

inline void Node::setScalings(class MdlxScalings *scalings)
{
	this->m_scalings = scalings;
}

inline class MdlxScalings* Node::scalings() const
{
	return this->m_scalings;
}


inline bool Node::inheritsTranslation() const
{
	return !(this->type() & Node::Type::DontInheritTranslation);
}

inline bool Node::inheritsRotation() const
{
	return !(this->type() & Node::Type::DontInheritRotation);
}

inline bool Node::inheritsScaling() const
{
	return !(this->type() & Node::Type::DontInheritScaling);
}

}

}

#endif
