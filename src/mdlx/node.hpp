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

#include "mdlxanimatedproperties.hpp"

namespace wc3lib
{

namespace mdlx
{

/**
 * \note This class does not represent an MDX block!
 *
 * Node types can have parent relationships and inherit rotation, scaling and translation of their parent if flags are set (\ref Node::Type::DontInheritTranslation, \ref Node::Type::DontInheritRotation, \ref Node::Type::DontInheritScaling).
 * Consider that \ref Node::translations(), \ref Node::rotations() and \ref Node::scalings() won't return 0
 * if the node does inherit any of them. They do only have empty lists.
 * Use \ref Node::hasParent() to check if there is any parent inheritance relationship.
 *
 * \note MDLX format brings its own dynamic type system by saving the type of each node in its structure. Use \ref Node::type() to get an node's type.
 * \note Inclusive byte count is only related to the note structure itself (excluding any further chunk data).
 */
class Node
{
	public:
		enum class Type : long32
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
			UnshadedOrEmitterUsesMdl = 32768, /// \note Is ignored by \ref MdlGenerator
			SortPrimitivesFarZOrEmitterUsesTga = 65536, /// \note Is ignored by \ref MdlGenerator
			LineEmitter = 131072,
			Unfogged = 262144,
			ModelSpace = 524288,
			XYQuad = 1048576
		};

		static const std::size_t nameSize = 0x50;

		Node();
		virtual ~Node();

		void setName(const string &name);
		void setName(const byte name[nameSize]);
		/**
		 * \return Returns name with constant length of \ref nameSize.
		 */
		const byte* name() const;
		void setObjectId(long32 objectId);
		long32 objectId() const;
		void setParentId(long32 parentId);
		long32 parentId() const;
		bool hasParent() const;
		/**
		 * Type additions will be hold.
		 */
		void setType(Type type);
		Type type() const;
		void setTranslations(const Translations &translations);
		const Translations& translations() const;
		void setRotations(const Rotations &rotations);
		const Rotations& rotations() const;
		void setScalings(const Scalings &scalings);
		const Scalings& scalings() const;

		bool inheritsTranslation() const;
		bool inheritsRotation() const;
		bool inheritsScaling() const;

	protected:
		byte m_name[nameSize];
		long32 m_objectId;
		long32 m_parentId;
		Type m_type;
		Translations m_translations;
		Rotations m_rotations;
		Scalings m_scalings;
};

inline constexpr Node::Type operator|(Node::Type x, Node::Type y)
{
	return static_cast<Node::Type>(static_cast<long32>(x) | static_cast<long32>(y));
}

inline constexpr bool operator&(Node::Type x, Node::Type y)
{
	return static_cast<bool>(static_cast<long32>(x) & static_cast<long32>(y));
}

inline void Node::setName(const string &name)
{
	if (name.size() + 1 > Node::nameSize)
	{
		throw std::runtime_error("");
	}

	memset(this->m_name, 0, nameSize); // init name with 0 byte
	memcpy(this->m_name, name.c_str(), name.size() + 1); // copy with 0 terminating character
}

inline void Node::setName(const byte name[Node::nameSize])
{
	memcpy(this->m_name, name, sizeof(byte) * Node::nameSize);
}

inline const byte* Node::name() const
{
	return this->m_name;
}

inline void Node::setObjectId(long32 objectId)
{
	this->m_objectId = objectId;
}

inline long32 Node::objectId() const
{
	return this->m_objectId;
}

inline void Node::setParentId(long32 parentId)
{
	this->m_parentId = parentId;
}

inline long32 Node::parentId() const
{
	return this->m_parentId;
}

inline bool Node::hasParent() const
{
	return this->m_parentId != noneId;
}

inline void Node::setType(Node::Type type)
{
	this->m_type = type;
}

inline Node::Type Node::type() const
{
	return this->m_type;
}

inline void Node::setTranslations(const Translations &translations)
{
	this->m_translations = translations;
}

inline const Translations& Node::translations() const
{
	return this->m_translations;
}

inline void Node::setRotations(const Rotations &rotations)
{
	this->m_rotations = rotations;
}

inline const Rotations& Node::rotations() const
{
	return this->m_rotations;
}

inline void Node::setScalings(const Scalings &scalings)
{
	this->m_scalings = scalings;
}

inline const Scalings& Node::scalings() const
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
