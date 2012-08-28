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

#ifndef WC3LIB_MAP_TRIGGERDATA_HPP
#define WC3LIB_MAP_TRIGGERDATA_HPP

#include <vector>

#include <boost/ptr_container/ptr_map.hpp>

#include "platform.hpp"

namespace wc3lib
{

namespace map
{

/**
 * Provides access to "UI/TriggerData.txt" file.
 * This class is required when implementing a trigger editor to get all possible categories, events, conditions, actions, types and calls.
 * It provides performant search functionality by storing entries in maps indicated by their name.
 * Currently, this is necessary for classes like \ref Variable, \ref TriggerFunction and \ref TriggerFunctionParameter which rely on trigger data entries for proper reading.
 * \todo Finish, using Boost Spirit to parse!
 */
class TriggerData : public FileFormat
{
	public:
		class Category : public Format
		{
			public:
				// Key: Arbitrary category identifier
				const string& name() const;
				// Value 0: Display text
				const string& displayText() const;
				// Value 1: Icon image file
				const string& iconImageFile() const;
				// Value 2: Optional flag (defaults to 0) indicating to disable display of category name
				bool displayName() const;

			private:
				string m_name;
				string m_displayText;
				string m_iconImageFile;
				bool m_displayName;
		};

		class Type : public Format
		{
			public:
				// Key: type name
				const string& name() const;
				// Value 0: flag (0 or 1) indicating if this type can be a global variable
				bool canBeGlobal() const;
				// Value 1: flag (0 or 1) indicating if this type can be used with comparison operators
				bool canBeCompared() const;
				// Value 2: string to display in the editor
				const string& displayText() const;
				// Value 3: base type, used only for custom types
				Type* baseType() const;
				// If a type does not have an entry here, it will be set to null if it is a handle
				const string& defaultValue() const;

			private:
				string m_name;
				bool m_canBeGlobal;
				bool m_canBeCompared;
				string m_displayText;
				Type *m_baseType;
				string m_defaultValue;
		};

		class Parameter : public Format
		{
			public:
				// Key: arbitrary text
				const string& name() const;
				// Value 0: variable type
				Type* type() const;
				// Value 1: code text (used in script)
				// Note: If the code text is a literal string, surround it with backward single quotes (`),
				// and they will be converted to double quotes in the script.
				const string& code() const;
				// Value 2: display text
				const string& displayText() const;

			private:
				string m_name;
				Type *m_type;
				string m_code;
				string m_displayText;
		};

		// Class for events, conditions and actions
		// TODO support defaults, limits and category
		class Function : public Format
		{
			public:
				typedef std::vector<Type*> Types;
				typedef std::vector<string> Strings;

				// Key: script event function
				const string& code() const;
				// Values: argument types
				const Types& types() const;
				// events: Note that the first argument is always a `trigger`, and is excluded here

				// optional values
				Category* category() const;
				const Strings& defaults() const;
				const Strings& limits() const;

			private:
				string m_code;
				Types m_types;
				Category *m_category;
				Strings m_defaults;
				Strings m_limits;
		};

		// Defines function calls which may be used as parameter values
		// Key: Function name
		// Value 0: flag (0 or 1) indicating if the call can be used in events
		// Value 1: return type
		// Value 2+: argument types
		//
		// Note: Operators are specially handled by the editor
		class Call : public Format
		{
			public:
				typedef std::vector<Type*> Types;

				const string& code() const;
				bool canBeUsedInEvents() const;
				Type* returnType() const;
				const Types& types() const;

			private:
				string m_code;
				bool m_canBeUsedInEvents;
				Type *m_returnType;
				Types m_types;
		};

		class DefaultTrigger : public Format
		{
			public:
				typedef std::vector<Function*> Functions;

				const string& name() const;
				const string& comment() const;
				/**
				 * \sa TriggerData::defaultTriggerCategories()
				 */
				int32 triggerCategory() const;
				const Functions& events() const;
				const Functions& conditions() const;
				const Functions& actions() const;

			private:
				string m_name;
				string m_comment;
				int32 m_triggerCategory;
				Functions m_events;
				Functions m_conditions;
				Functions m_actions;

		};

		typedef boost::ptr_map<string, Category> Categories;
		typedef boost::ptr_map<string, Type> Types;
		typedef boost::ptr_map<string, Parameter> Parameters;
		typedef boost::ptr_map<string, Function> Functions;
		typedef boost::ptr_map<string, Call> Calls;
		typedef std::vector<string> DefaultTriggerCategories;
		typedef boost::ptr_map<string, DefaultTrigger> DefaultTriggers;

		virtual std::streamsize read(InputStream &istream) throw (Exception);
		virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

		virtual const byte* fileName() const;
		virtual const byte* fileTextId() const;
		virtual uint32 latestFileVersion() const;

		const Categories& categories() const;
		const Types& types() const;
		const Parameters& parameters() const;
		const Functions& events() const;
		const Functions& conditions() const;
		const Functions& actions() const;
		const Calls& calls() const;
		const DefaultTriggerCategories& defaultTriggerCategories() const;
		const DefaultTriggers& defaultTriggers() const;
		Categories& categories();
		Types& types();
		Parameters& parameters();
		Functions& events();
		Functions& conditions();
		Functions& actions();
		Calls& calls();
		DefaultTriggerCategories& defaultTriggerCategories();
		DefaultTriggers& defaultTriggers();

	private:
		Categories m_categories;
		Types m_types;
		Parameters m_parameters;
		Functions m_events;
		Functions m_conditions;
		Functions m_actions;
		Calls m_calls;
		DefaultTriggerCategories m_defaultTriggerCategories;
		DefaultTriggers m_defaultTriggers;
};

inline const byte* TriggerData::fileName() const
{
	return "TriggerData.txt";
}

inline const byte* TriggerData::fileTextId() const
{
	return "";
}

inline uint32 TriggerData::latestFileVersion() const
{
	return 0;
}

inline const string& TriggerData::Category::name() const
{
	return m_name;
}

inline const TriggerData::Categories& TriggerData::categories() const
{
	return m_categories;
}

inline const TriggerData::Types& TriggerData::types() const
{
	return m_types;
}

inline const TriggerData::Parameters& TriggerData::parameters() const
{
	return m_parameters;
}

inline const TriggerData::Functions& TriggerData::events() const
{
	return m_events;
}

inline const TriggerData::Functions& TriggerData::conditions() const
{
	return m_conditions;
}

inline const TriggerData::Functions& TriggerData::actions() const
{
	return m_actions;
}

inline const TriggerData::Calls& TriggerData::calls() const
{
	return m_calls;
}

inline const TriggerData::DefaultTriggerCategories& TriggerData::defaultTriggerCategories() const
{
	return m_defaultTriggerCategories;
}

inline const TriggerData::DefaultTriggers& TriggerData::defaultTriggers() const
{
	return m_defaultTriggers;
}

inline TriggerData::Categories& TriggerData::categories()
{
	return m_categories;
}

inline TriggerData::Types& TriggerData::types()
{
	return m_types;
}

inline TriggerData::Parameters& TriggerData::parameters()
{
	return m_parameters;
}

inline TriggerData::Functions& TriggerData::events()
{
	return m_events;
}

inline TriggerData::Functions& TriggerData::conditions()
{
	return m_conditions;
}

inline TriggerData::Functions& TriggerData::actions()
{
	return m_actions;
}

inline TriggerData::Calls& TriggerData::calls()
{
	return m_calls;
}

inline TriggerData::DefaultTriggerCategories& TriggerData::defaultTriggerCategories()
{
	return m_defaultTriggerCategories;
}

inline TriggerData::DefaultTriggers& TriggerData::defaultTriggers()
{
	return m_defaultTriggers;
}

inline const string& TriggerData::Category::displayText() const
{
	return m_displayText;
}

inline const string& TriggerData::Category::iconImageFile() const
{
	return m_iconImageFile;
}

inline bool TriggerData::Category::displayName() const
{
	return m_displayName;
}

inline const string& TriggerData::Type::name() const
{
	return m_name;
}

inline bool TriggerData::Type::canBeGlobal() const
{
	return m_canBeGlobal;
}

inline bool TriggerData::Type::canBeCompared() const
{
	return m_canBeCompared;
}

inline const string& TriggerData::Type::displayText() const
{
	return m_displayText;
}

inline TriggerData::Type* TriggerData::Type::baseType() const
{
	return m_baseType;
}

inline const string& TriggerData::Type::defaultValue() const
{
	return m_defaultValue;
}

inline const string& TriggerData::Parameter::name() const
{
	return m_name;
}

inline TriggerData::Type* TriggerData::Parameter::type() const
{
	return m_type;
}

inline const string& TriggerData::Parameter::code() const
{
	return m_code;
}

inline const string& TriggerData::Parameter::displayText() const
{
	return m_displayText;
}

inline const string& TriggerData::Function::code() const
{
	return m_code;
}

inline const TriggerData::Function::Types& TriggerData::Function::types() const
{
	return m_types;
}

inline TriggerData::Category* TriggerData::Function::category() const
{
	return m_category;
}

inline const TriggerData::Function::Strings& TriggerData::Function::defaults() const
{
	return m_defaults;
}

inline const TriggerData::Function::Strings& TriggerData::Function::limits() const
{
	return m_limits;
}

inline const string& TriggerData::Call::code() const
{
	return m_code;
}

inline bool TriggerData::Call::canBeUsedInEvents() const
{
	return m_canBeUsedInEvents;
}


inline TriggerData::Type* TriggerData::Call::returnType() const
{
	return m_returnType;
}


inline const TriggerData::Call::Types& TriggerData::Call::types() const
{
	return m_types;
}

inline const string& TriggerData::DefaultTrigger::name() const
{
	return m_name;
}

inline const string& TriggerData::DefaultTrigger::comment() const
{
	return m_comment;
}

inline int32 TriggerData::DefaultTrigger::triggerCategory() const
{
	return m_triggerCategory;
}

inline const TriggerData::DefaultTrigger::Functions& TriggerData::DefaultTrigger::events() const
{
	return m_events;
}

inline const TriggerData::DefaultTrigger::Functions& TriggerData::DefaultTrigger::conditions() const
{
	return m_conditions;
}

inline const TriggerData::DefaultTrigger::Functions& TriggerData::DefaultTrigger::actions() const
{
	return m_actions;
}



}

}

#endif
