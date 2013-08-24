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
#include <set>

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/variant/variant.hpp>

#include "platform.hpp"
#include "txt.hpp"

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
		/**
		 * Simple vector type for splitting up comma separated values in TriggerData.txt file.
		 */
		typedef std::vector<string> SplitVector;
		
		TriggerData();
		
		class Category : public Format
		{
			public:
				Category();
				
				void setName(const string &name);
				// Key: Arbitrary category identifier
				const string& name() const;
				void setDisplayText(const string &displayText);
				// Value 0: Display text
				const string& displayText() const;
				void setIconImageFile(const string &iconImageFile);
				// Value 1: Icon image file
				const string& iconImageFile() const;
				void setDisplayName(bool displayName);
				// Value 2: Optional flag (defaults to 0) indicating to disable display of category name
				bool displayName() const;

				virtual std::streamsize read(InputStream &istream) throw (Exception);
				virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

			private:
				string m_name;
				string m_displayText;
				string m_iconImageFile;
				bool m_displayName;
		};

		class Type : public Format
		{
			public:
				Type();
				
				void setName(const string &name);
				// Key: type name
				const string& name() const;
				void setCanBeGlobal(bool canBeGlobal);
				// Value 0: flag (0 or 1) indicating if this type can be a global variable
				bool canBeGlobal() const;
				void setCanBeCompared(bool canBeCompared);
				// Value 1: flag (0 or 1) indicating if this type can be used with comparison operators
				bool canBeCompared() const;
				void setDisplayText(const string &displayText);
				// Value 2: string to display in the editor
				const string& displayText() const;
				void setBaseType(Type *type);
				// Value 3: base type, used only for custom types
				Type* baseType() const;
				void setDefaultValue(const string &defaultValue);
				// If a type does not have an entry here, it will be set to null if it is a handle
				const string& defaultValue() const;

				virtual std::streamsize read(InputStream &istream) throw (Exception);
				virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

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
				void setName(const string &name);
				const string& name() const;
				// Value 0: variable type
				void setType(Type *type);
				Type* type() const;
				// Value 1: code text (used in script)
				// Note: If the code text is a literal string, surround it with backward single quotes (`),
				// and they will be converted to double quotes in the script.
				void setCode(const string &code);
				const string& code() const;
				// Value 2: display text
				void setDisplayText(const string &displayText);
				const string& displayText() const;

				virtual std::streamsize read(InputStream &istream) throw (Exception);
				virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

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
				/**
				 * There exist special types which do not match any defined type:
				 * "nothing", "AnyGlobal", "Null"
				 * Those have to be stored as string.
				 */
				typedef boost::variant<Type*, string> ArgumentType;
				typedef std::vector<ArgumentType> ArgumentTypes;
				typedef boost::variant<string, int32, Parameter*> Value;
				typedef std::vector<Value> Values;

				// Key: script event function
				void setCode(const string &code);
				const string& code() const;
				// Values: argument types
				ArgumentTypes& types();
				const ArgumentTypes& types() const;
				// events: Note that the first argument is always a `trigger`, and is excluded here

				// optional values
				void setCategory(Category *category);
				Category* category() const;
				Values& defaults();
				const Values& defaults() const;
				Values& limits();
				const Values& limits() const;
				
				virtual std::streamsize read(InputStream& istream) throw (Exception);
				virtual std::streamsize write(OutputStream& ostream) const throw (Exception);

				virtual void fillTypes(TriggerData *triggerData, const SplitVector &values);

			private:
				string m_code;
				ArgumentTypes m_types;
				Category *m_category;
				Values m_defaults;
				Values m_limits;
		};
		
		class FunctionArgumentVisitor : public boost::static_visitor<string>
		{
			public:
				string operator()(string v) const
				{
					return v;
				}
				
				string operator()(TriggerData::Type *v) const
				{
					return v->name();
				}
		};

		// Defines function calls which may be used as parameter values
		// Key: Function name
		// Value 0: flag (0 or 1) indicating if the call can be used in events
		// Value 1: return type
		// Value 2+: argument types
		//
		// Note: Operators are specially handled by the editor
		class Call : public Function
		{
			public:
				bool canBeUsedInEvents() const;
				const ArgumentType& returnType() const;
				
				virtual std::streamsize read(InputStream& istream) throw (Exception);
				virtual std::streamsize write(OutputStream& ostream) const throw (Exception);

				// TODO C++11 override
				virtual void fillTypes(TriggerData *triggerData, const SplitVector &values);

			private:
				bool m_canBeUsedInEvents;
		};

		class DefaultTrigger : public Format
		{
			public:
				typedef std::vector<Function*> Functions;

				void setName(const string &name);
				const string& name() const;
				void setComment(const string &comment);
				const string& comment() const;
				void setTriggerCategory(const string &triggerCategory);
				/**
				 * \sa TriggerData::defaultTriggerCategories()
				 */
				const string& triggerCategory() const;
				Functions& events();
				const Functions& events() const;
				Functions& conditions();
				const Functions& conditions() const;
				Functions& actions();
				const Functions& actions() const;

				virtual std::streamsize read(InputStream &istream) throw (Exception);
				virtual std::streamsize write(OutputStream &ostream) const throw (Exception);

			private:
				string m_name;
				string m_comment;
				string m_triggerCategory;
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
		typedef boost::ptr_vector<DefaultTrigger> DefaultTriggers;

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
		
		typedef std::set<string> SpecialTypes;
		
		/**
		 * There is some special types like "nothing" and "Null" which should be interpreted as literals instead of
		 * searching for the corresponding type.
		 * Otherwise, it will at least print a warning.
		 */
		const SpecialTypes& specialTypes() const;

	private:
		template<class FunctionType>
		void readFunction(const Txt::Pair &ref, boost::ptr_map<string, FunctionType> &functions);
		
		string::size_type firstNonNumericChar(const string &value) const;
		
		Categories m_categories;
		Types m_types;
		Parameters m_parameters;
		Functions m_events;
		Functions m_conditions;
		Functions m_actions;
		Calls m_calls;
		DefaultTriggerCategories m_defaultTriggerCategories;
		DefaultTriggers m_defaultTriggers;
		
		SpecialTypes m_specialTypes;
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

inline void TriggerData::Category::setName(const string &name)
{
	this->m_name = name;
}

inline const string& TriggerData::Category::name() const
{
	return m_name;
}

inline void TriggerData::Category::setDisplayText(const string &displayText)
{
	this->m_displayText = displayText;
}

inline const string& TriggerData::Category::displayText() const
{
	return m_displayText;
}

inline void TriggerData::Category::setIconImageFile(const string &iconImageFile)
{
	this->m_iconImageFile = iconImageFile;
}

inline const string& TriggerData::Category::iconImageFile() const
{
	return m_iconImageFile;
}

inline void TriggerData::Category::setDisplayName(bool displayName)
{
	this->m_displayName = displayName;
}

inline bool TriggerData::Category::displayName() const
{
	return m_displayName;
}

inline void TriggerData::Type::setName(const string &name)
{
	this->m_name = name;
}

inline void TriggerData::Type::setBaseType(TriggerData::Type *type)
{
	this->m_baseType = type;
}

inline void TriggerData::Type::setCanBeCompared(bool canBeCompared)
{
	this->m_canBeCompared = canBeCompared;
}

inline void TriggerData::Type::setDefaultValue(const string &defaultValue)
{
	this->m_defaultValue = defaultValue;
}

inline void TriggerData::Type::setDisplayText(const string& displayText)
{
	this->m_displayText = displayText;
}

inline void TriggerData::Type::setCanBeGlobal(bool canBeGlobal)
{
	this->m_canBeGlobal = canBeGlobal;
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

inline void TriggerData::Parameter::setName(const string& name)
{
	this->m_name = name;
}

inline const string& TriggerData::Parameter::name() const
{
	return m_name;
}

inline void TriggerData::Parameter::setType(TriggerData::Type* type)
{
	this->m_type = type;
}

inline TriggerData::Type* TriggerData::Parameter::type() const
{
	return m_type;
}

inline void TriggerData::Parameter::setCode(const string& code)
{
	this->m_code = code;
}

inline const string& TriggerData::Parameter::code() const
{
	return m_code;
}

inline void TriggerData::Parameter::setDisplayText(const string& displayText)
{
	this->m_displayText = displayText;
}

inline const string& TriggerData::Parameter::displayText() const
{
	return m_displayText;
}

inline void TriggerData::Function::setCode(const string &code)
{
	this->m_code = code;
}

inline const string& TriggerData::Function::code() const
{
	return m_code;
}

inline TriggerData::Function::ArgumentTypes& TriggerData::Function::types()
{
	return m_types;
}

inline const TriggerData::Function::ArgumentTypes& TriggerData::Function::types() const
{
	return m_types;
}

inline void TriggerData::Function::setCategory(TriggerData::Category *category)
{
	this->m_category = category;
}

inline TriggerData::Category* TriggerData::Function::category() const
{
	return m_category;
}

inline TriggerData::Function::Values& TriggerData::Function::defaults()
{
	return this->m_defaults;
}

inline const TriggerData::Function::Values& TriggerData::Function::defaults() const
{
	return m_defaults;
}

inline TriggerData::Function::Values& TriggerData::Function::limits()
{
	return this->m_limits;
}

inline const TriggerData::Function::Values& TriggerData::Function::limits() const
{
	return m_limits;
}

inline bool TriggerData::Call::canBeUsedInEvents() const
{
	return m_canBeUsedInEvents;
}


inline const TriggerData::Function::ArgumentType& TriggerData::Call::returnType() const
{
	return this->types()[0];
}

inline void TriggerData::DefaultTrigger::setName(const string& name)
{
	this->m_name = name;
}

inline const string& TriggerData::DefaultTrigger::name() const
{
	return m_name;
}

inline void TriggerData::DefaultTrigger::setComment(const string& comment)
{
	this->m_comment = comment;
}

inline const string& TriggerData::DefaultTrigger::comment() const
{
	return m_comment;
}

inline void TriggerData::DefaultTrigger::setTriggerCategory(const string &triggerCategory)
{
	this->m_triggerCategory = triggerCategory;
}

inline const string& TriggerData::DefaultTrigger::triggerCategory() const
{
	return m_triggerCategory;
}

inline TriggerData::DefaultTrigger::Functions& TriggerData::DefaultTrigger::events()
{
	return this->m_events;
}

inline const TriggerData::DefaultTrigger::Functions& TriggerData::DefaultTrigger::events() const
{
	return m_events;
}

inline TriggerData::DefaultTrigger::Functions& TriggerData::DefaultTrigger::conditions()
{
	return this->m_conditions;
}

inline const TriggerData::DefaultTrigger::Functions& TriggerData::DefaultTrigger::conditions() const
{
	return m_conditions;
}

inline TriggerData::DefaultTrigger::Functions& TriggerData::DefaultTrigger::actions()
{
	return this->m_actions;
}

inline const TriggerData::DefaultTrigger::Functions& TriggerData::DefaultTrigger::actions() const
{
	return m_actions;
}

inline const TriggerData::SpecialTypes& TriggerData::specialTypes() const
{
	return this->m_specialTypes;
}

}

}

#endif
