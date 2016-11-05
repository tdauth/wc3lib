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

#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "triggerdata.hpp"
#include "txt.hpp"

namespace wc3lib
{

namespace map
{

TriggerData::TriggerData()
{
	m_specialTypes.insert("nothing");
	m_specialTypes.insert("Null");
}

// Value 2: Optional flag (defaults to 0) indicating to disable display of category name
TriggerData::Category::Category() : m_displayName(true)
{
}

std::streamsize TriggerData::Category::read(InputStream &istream)
{
	return 0;
}

std::streamsize TriggerData::Category::write(OutputStream &ostream) const
{
	return 0;
}

TriggerData::Type::Type() : m_baseType(0)
{
}

std::streamsize TriggerData::Type::read(InputStream &istream)
{
	return 0;
}

std::streamsize TriggerData::Type::write(OutputStream &ostream) const
{
	return 0;
}

TriggerData::Parameter::Parameter() : m_type(0)
{
}

std::streamsize TriggerData::Parameter::read(InputStream &istream)
{
	return 0;
}

std::streamsize TriggerData::Parameter::write(OutputStream &ostream) const
{
	return 0;
}

TriggerData::Function::Function() : m_category(0)
{
}

std::streamsize TriggerData::Function::read(InputStream &istream)
{
	return 0;
}

std::streamsize TriggerData::Function::write(OutputStream &ostream) const
{
	return 0;
}

TriggerData::Call::Call() : m_canBeUsedInEvents(false)
{
}

std::streamsize TriggerData::Call::read(InputStream &istream)
{
	return 0;
}

std::streamsize TriggerData::Call::write(OutputStream &ostream) const
{
	return 0;
}

std::streamsize TriggerData::DefaultTrigger::read(InputStream &istream)
{
	return 0;
}

std::streamsize TriggerData::DefaultTrigger::write(OutputStream &ostream) const
{
	return 0;
}

void TriggerData::Function::fillTypes(TriggerData *triggerData, const SplitVector &values)
{
	for (std::size_t i = 0; i < values.size(); ++i)
	{
		if (triggerData->specialTypes().find(values[i]) != triggerData->specialTypes().end())
		{
			this->types().push_back(values[i]);
		}
		else
		{
			TriggerData::Types::iterator iterator = triggerData->types().find(values[i]);

			if (iterator != triggerData->types().end())
			{
				this->types().push_back(iterator->second);
			}
			else
			{
				this->types().push_back(values[i]);
				std::cerr << boost::format(_("Warning: Unknown type \"%1%\" at index %2% for trigger function \"%3%\".")) % values[i] % i % this->code() << std::endl;
			}
		}
	}
}

void TriggerData::Call::fillTypes(TriggerData *triggerData, const SplitVector &values)
{
	if (values.size() >= 1)
	{
		this->m_canBeUsedInEvents = boost::lexical_cast<bool>(values[0]);

		for (std::size_t i = 1; i < values.size(); ++i)
		{
			if (triggerData->specialTypes().find(values[i]) != triggerData->specialTypes().end())
			{
				this->types().push_back(values[i]);
			}
			else
			{
				TriggerData::Types::iterator iterator = triggerData->types().find(values[i]);

				if (iterator != triggerData->types().end())
				{
					this->types().push_back(iterator->second);
				}
				else
				{
					this->types().push_back(values[i]);
					std::cerr << boost::format(_("Warning: Unknown type \"%1%\" at index %2% for trigger function \"%3%\".")) % values[i] % i % this->code() << std::endl;
				}
			}
		}
	}
	else
	{
		std::cerr << boost::format(_("Missing \"canBeUsedInEvents\" value for trigger call \"%1%\".")) % this->code() << std::endl;
	}
}

template<class FunctionType>
void TriggerData::readFunction(const Txt::Entry &ref, boost::ptr_map<string, FunctionType> &functions)
{
	FunctionType *function = 0;
	string code = ref.first;

	if (!boost::starts_with(code, "_"))
	{
		std::auto_ptr<FunctionType> functionPtr(new FunctionType());
		function = functionPtr.get();
		function->setCode(code);
		functions.insert(code, functionPtr);
	}
	else
	{
		code = code.substr(1);

		string::size_type index = code.find("_");

		if (index != string::npos)
		{
			code = code.substr(0, index);
		}
		else
		{
			std::cerr << boost::format(_("Missing suffix for trigger function entry \"%1%\".")) % ref.first << std::endl;
		}

		typename boost::ptr_map<string, FunctionType>::iterator iterator = functions.find(code);

		if (iterator == functions.end())
		{
			std::cerr << boost::format(_("Missing base trigger function \"%1%\" for trigger function entry \"%2%\".")) % code % ref.first << std::endl;

			return;
		}

		function = iterator->second;
	}

	SplitVector values;
	boost::algorithm::split(values, ref.second, boost::is_any_of(","), boost::algorithm::token_compress_on);

	string::size_type index = ref.first.find_last_of("_");

	if (index == string::npos)
	{
		function->fillTypes(this, values); // since trigger calls have other values than events, conditions and actions, we call the virtual implementation!
	}
	else if (ref.first.substr(index + 1) == "Defaults")
	{
		if (!function->defaults().empty())
		{
			function->defaults().clear();
			std::cerr << boost::format(_("Redefinition of defaults for trigger function \"%1%\".")) % code << std::endl;
		}

		for (std::size_t i = 0; i < values.size(); ++i)
		{
			Parameters::iterator iterator = this->parameters().find(values[i]);

			if (iterator != this->parameters().end())
			{
				function->defaults().push_back(iterator->second);
			}
			else
			{
				try
				{ // float
					function->defaults().push_back(boost::lexical_cast<float32>(values[i]));
				}
				catch (boost::bad_lexical_cast &e)
				{ // int
					try
					{
						function->defaults().push_back(boost::lexical_cast<int32>(values[i]));
					}
					catch (boost::bad_lexical_cast &e)
					{ // string
						function->defaults().push_back(values[i]);
					}
				}
			}
		}
	}
	else if (ref.first.substr(index + 1) == "Category")
	{
		if (values.size() >= 1)
		{
			Categories::iterator iterator = this->categories().find(values[0]);

			if (iterator != this->categories().end())
			{
				function->setCategory(iterator->second);
			}
			else
			{
				std::cerr << boost::format(_("Invalid category \"%1%\" for trigger function \"%2\".")) % values[0] % code << std::endl;
			}
		}
		else
		{
			std::cerr << boost::format(_("Empty category for trigger function \"%1%\".")) % code << std::endl;
		}
	}
	else if (ref.first.substr(index + 1) == "Limits")
	{
		if (!function->limits().empty())
		{
			function->limits().clear();
			std::cerr << boost::format(_("Redefinition of limits for trigger function \"%1%\".")) % code << std::endl;
		}

		for (std::size_t i = 0; i < values.size(); ++i)
		{
			Function::Limit limit;
			bool minimumIsInt = false;
			bool minimumIsString = false;
			bool minimumIsDouble = false;

			// get minimum
			/*
			 * NOTE parameters should only occur for defaults not for limits
			Parameters::iterator iterator = this->parameters().find(values[i]);

			if (iterator != this->parameters().end()) {
				limit.first = iterator->second;
			} else {
			*/
				try
				{ // float
					limit.first = boost::lexical_cast<float32>(values[i]);

					minimumIsDouble = true;
				}
				catch (boost::bad_lexical_cast &e)
				{ // int
					try
					{
						limit.first = boost::lexical_cast<int32>(values[i]);

						minimumIsInt = true;

					}
					catch (boost::bad_lexical_cast &e)
					{ // string
						limit.first = values[i];

						minimumIsString = true;
					}
				}
			//}

			++i; // get maximum

			if (i < values.size())
			{

				/*
				 * NOTE parameters should only occur for defaults not for limits
				Parameters::iterator iterator = this->parameters().find(values[i]);

				if (iterator != this->parameters().end()) {
					limit.second = iterator->second;
				} else {
				*/
					try
					{ // float
						limit.second = boost::lexical_cast<float32>(values[i]);

						minimumIsDouble = true;
					}
					catch (boost::bad_lexical_cast &e)
					{ // int
						try
						{
							limit.second = boost::lexical_cast<int32>(values[i]);

							minimumIsInt = true;

						}
						catch (boost::bad_lexical_cast &e)
						{ // string
							limit.second = values[i];

							minimumIsString = true;
						}
					}
				//}
				// TODO handle _ values?

			}
			else
			{
				std::cerr << boost::format(_("Limits of function \"%1%\" miss maximum value for limit %2%")) % code % function->limits().size() << std::endl;
			}

			function->limits().push_back(limit);
		}
	}
}

void TriggerData::writeFunction(const Function *function, Txt::Section &section) const
{
	Txt::Entry entry;
	entry.setKey(function->code());

	const std::size_t argSize = function->types().size();

	stringstream sstream;

	// TODO needs extra treatment for calls? canBeUsedInEvents?
	for (std::size_t i = 0; i < argSize; ++i)
	{
		if (i > 0)
		{
			sstream << ",";
		}

		FunctionArgumentVisitor visitor;
		sstream << boost::apply_visitor<FunctionArgumentVisitor>(visitor, function->types()[i]);
	}

	const string value = sstream.str();
	entry.setValue(value);

	section.entries.push_back(entry);

	Txt::Entry defaultsEntry;
	defaultsEntry.setKey(string("_") + function->code() + "_Defaults");

	/*
	 * Defaults can have an empty value if no defaults are defined.
	 */
	if (!function->defaults().empty())
	{
		for (std::size_t i = 0; i < argSize; ++i)
		{
			if (i > 0)
			{
				sstream << ",";
			}

			FunctionValueVisitor visitor;
			const string value = i >= function->defaults().size() ? "_" : boost::apply_visitor<FunctionValueVisitor>(visitor, function->defaults()[i]);
			sstream << value;
		}
	}

	section.entries.push_back(defaultsEntry);

	/*
	 * Limits are optional.
	 */
	if (!function->limits().empty())
	{
		Txt::Entry limitsEntry;
		limitsEntry.setKey(string("_") + function->code() + "_Limits");

		for (std::size_t i = 0; i < argSize; ++i)
		{
			if (i > 0)
			{
				sstream << ",";
			}

			FunctionValueVisitor visitor;
			const string value0 = i >= function->limits().size() ? "_" : boost::apply_visitor<FunctionValueVisitor>(visitor, function->limits()[i].first);
			const string value1 = i >= function->limits().size() ? "_" : boost::apply_visitor<FunctionValueVisitor>(visitor, function->limits()[i].second);
			sstream << value0 << "," << value1;
		}

		section.entries.push_back(limitsEntry);
	}

	Txt::Entry categoryEntry;
	categoryEntry.setKey(string("_") + function->code() + "_Category");

	if (function->category() != nullptr)
	{
		categoryEntry.setValue(function->category()->name());
	}

	section.entries.push_back(categoryEntry);
}

std::size_t TriggerData::firstNonNumericChar(const string& value) const
{
	string::size_type index = string::npos;

	for (string::size_type i = 0; i < value.size(); ++i)
	{
		if (value[i] > '9' || value[i] < '0')
		{ // not numeric
			index = i;

			break;
		}
	}

	return index;
}

std::streamsize TriggerData::read(InputStream &istream)
{
	boost::scoped_ptr<Txt> txt(new Txt());
	std::streamsize size = txt->read(istream);

	/*
	 * Fill map with all section entries for fast access of section entries.
	 */
	for (std::size_t i = 0; i < txt->sections().size(); ++i)
	{
		m_sectionEntries.insert(std::make_pair(txt->sections()[i].name, &txt->sections()[i].entries));
	}

	BOOST_FOREACH(Txt::Entries::const_reference ref, sectionEntries("TriggerCategories"))
	{
		std::auto_ptr<Category> category(new Category());

		string name = ref.first;
		category->setName(name);

		SplitVector values;
		boost::algorithm::split(values, ref.second, boost::is_any_of(","), boost::algorithm::token_compress_on);

		if (values.size() >= 1)
		{
			category->setDisplayText(values[0]);
		}

		if (values.size() >= 2)
		{
			category->setIconImageFile(values[1]);
		}

		if (values.size() >= 3)
		{
			category->setDisplayName(!boost::lexical_cast<bool>(values[2]));
		}

		this->categories().insert(name, category);
	}

	typedef std::map<string, string> BaseTypes;
	BaseTypes baseTypes;

	BOOST_FOREACH(Txt::Entries::const_reference ref, sectionEntries("TriggerTypes"))
	{
		std::auto_ptr<Type> type(new Type());

		// Key: type name
		string name = ref.first;
		type->setName(name);

		SplitVector values;
		boost::algorithm::split(values, ref.second, boost::is_any_of(","), boost::algorithm::token_compress_on);

		// Value 0: flag (0 or 1) indicating if this type can be a global variable
		if (values.size() >= 1)
		{
			type->setCanBeGlobal(boost::lexical_cast<bool>(values[0]));
		}

		// Value 1: flag (0 or 1) indicating if this type can be used with comparison operators
		if (values.size() >= 2)
		{
			type->setCanBeCompared(boost::lexical_cast<bool>(values[1]));
		}

		// Value 2: string to display in the editor
		if (values.size() >= 3)
		{
			type->setDisplayText(values[2]);
		}

		// Value 3: base type, used only for custom types
		if (values.size() >= 4)
		{
			baseTypes[name] = values[3];
		} /*else {
			std::cerr << boost::format(_("Missing base type for type \"%1%\".")) % name << std::endl;
		}*/

		/*
		 * Detected The Frozen Throne trigger data file.
		 * Use TriggerDataEx to load this.
		 */
		if (values.size() > 4)
		{
			throw Exception(boost::format(_("Detected The Frozen Throne type in a Reign of Chaos TriggerData.txt file for type \"%1%\".")) % name);
		}

		this->types().insert(name, type);
	}

	// set trigger types bases
	BOOST_FOREACH(BaseTypes::reference ref, baseTypes)
	{
		const string type = ref.first;
		const string baseType = ref.second;

		Types::iterator baseTypeIterator = this->types().find(baseType);

		if (baseTypeIterator != this->types().end())
		{
			Types::iterator typeIterator = this->types().find(type);

			if (typeIterator != this->types().end())
			{
				typeIterator->second->setBaseType(baseTypeIterator->second);
			}
			else
			{
				std::cerr << boost::format(_("Missing type \"%1%\" for base type \"%2%\".")) % type % baseType << std::endl;
			}
		}
		else
		{
			std::cerr << boost::format(_("Missing base type \"%1%\" for type \"%2%\".")) % baseType % type << std::endl;
		}
	}

	// set trigger type defaults which are stored in a separated category
	BOOST_FOREACH(Txt::Entries::const_reference ref, sectionEntries("TriggerTypeDefaults"))
	{
		Types::iterator defaultIterator = this->types().find(ref.first);

		if (defaultIterator != this->types().end())
		{
			defaultIterator->second->setDefaultValue(ref.second);
		}
		else
		{
			std::cerr << boost::format(_("Missing default type \"%1%\".")) % ref.first << std::endl;
		}
	}

	BOOST_FOREACH(Txt::Entries::const_reference ref, sectionEntries("TriggerParams"))
	{
		std::auto_ptr<Parameter> parameter(new Parameter());
		string name = ref.first;

		parameter->setName(name);

		SplitVector values;
		boost::algorithm::split(values, ref.second, boost::is_any_of(","), boost::algorithm::token_compress_on);

		if (values.size() >= 1)
		{
			Types::iterator iterator = this->types().find(values[0]);

			if (iterator != this->types().end())
			{
				parameter->setType(iterator->second);
			}
		}

		if (values.size() >= 2)
		{
			parameter->setCode(values[1]);
		}

		if (values.size() >= 3)
		{
			parameter->setDisplayText(values[2]);
		}

		this->parameters().insert(name, parameter);
	}

	BOOST_FOREACH(Txt::Entries::const_reference ref, sectionEntries("TriggerEvents"))
	{
		readFunction<Function>(ref, this->events());
	}

	BOOST_FOREACH(Txt::Entries::const_reference ref, sectionEntries("TriggerConditions"))
	{
		readFunction<Function>(ref, this->conditions());
	}

	BOOST_FOREACH(Txt::Entries::const_reference ref, sectionEntries("TriggerActions"))
	{
		readFunction<Function>(ref, this->actions());
	}

	BOOST_FOREACH(Txt::Entries::const_reference ref, sectionEntries("TriggerCalls"))
	{
		readFunction<Call>(ref, this->calls());
	}

	std::size_t numCategories = 0;
	std::size_t actualCategories = 0;

	BOOST_FOREACH(Txt::Entries::const_reference ref, sectionEntries("DefaultTriggerCategories"))
	{
		if (ref.first == "NumCategories")
		{
			numCategories = boost::lexical_cast<std::size_t>(ref.second);
			this->defaultTriggerCategories().resize(numCategories);
		}
		else if (boost::starts_with(ref.first, "Category"))
		{
			std::size_t index = boost::lexical_cast<std::size_t>(ref.first.substr(strlen("Category"))) - 1;

			if (index < this->defaultTriggerCategories().size())
			{
				this->defaultTriggerCategories()[index] = ref.second;
			}
			else
			{
				std::cerr << boost::format(_("Invalid index %1% for default trigger category \"%2%\".")) % index % ref.second << std::endl;
			}

			++actualCategories;
		}
	}

	if (numCategories != actualCategories)
	{
		std::cerr << boost::format(_("Expected default trigger categories %1% are not equal to actual ones %2%.")) % numCategories % actualCategories << std::endl;
	}

	std::size_t numTriggers = 0;
	std::size_t actualTriggers = 0;

	BOOST_FOREACH(Txt::Entries::const_reference ref, sectionEntries("DefaultTriggers"))
	{
		if (ref.first == "NumTriggers")
		{
			this->defaultTriggers().resize(boost::lexical_cast<std::size_t>(ref.second));
		}
		else if (boost::starts_with(ref.first, "Trigger"))
		{
			string substr = ref.first.substr(strlen("Trigger"));
			string::size_type substrIndex = this->firstNonNumericChar(substr);

			if (substrIndex == string::npos)
			{
				std::cerr << boost::format(_("Default trigger entry \"%1%\" is missing type.")) % ref.first << std::endl;

				continue;
			}

			const std::size_t index = boost::lexical_cast<std::size_t>(substr.substr(0, substrIndex)) - 1;

			if (index < this->defaultTriggers().size())
			{
				DefaultTrigger *trigger = 0;

				if (this->defaultTriggers().is_null(index))
				{ // first creation
					std::auto_ptr<DefaultTrigger> triggerPtr(new DefaultTrigger());
					trigger = triggerPtr.get();
					this->defaultTriggers().replace(index, triggerPtr);
					++actualTriggers;
				}
				else
				{
					trigger = &this->defaultTriggers()[index];
				}

				const string type = substr.substr(substrIndex);

				if (type == "Name")
				{
					trigger->setName(ref.second);
				}
				else if (type == "Comment")
				{
					trigger->setComment(ref.second);
				}
				else if (type == "Category")
				{
					/*
					 * Starts counting at 1 so subtract 1.
					 */
					const std::size_t category = boost::lexical_cast<std::size_t>(ref.second) - 1;

					if (category < this->defaultTriggerCategories().size())
					{
						trigger->setTriggerCategory(this->defaultTriggerCategories()[category]);
					}
					else
					{
						throw Exception(boost::format(_("Default trigger category %1% does not exist but is used by default trigger %2%.")) % (category + 1) % index);
					}
				}
				else if (type == "Events")
				{
					const std::size_t eventsCount = boost::lexical_cast<std::size_t>(ref.second);
					trigger->events().resize(eventsCount);
				}
				else if (type == "Conditions")
				{
					const std::size_t conditionsCount = boost::lexical_cast<std::size_t>(ref.second);
					trigger->conditions().resize(conditionsCount);
				}
				else if (type == "Actions")
				{
					const std::size_t actionsCount = boost::lexical_cast<std::size_t>(ref.second);
					trigger->actions().resize(actionsCount);
				}
				else if (boost::starts_with(type, "Event"))
				{
					const std::size_t eventIndex = boost::lexical_cast<std::size_t>(type.substr(strlen("Event"))) - 1;

					Functions::iterator iterator = this->events().find(ref.second);

					if (iterator != this->events().end())
					{
						if (eventIndex < trigger->events().size())
						{
							trigger->events()[eventIndex] = iterator->second;
						}
						else
						{
							std::cerr << boost::format(_("Invalid event index %1% for default trigger entry %2%.")) % eventIndex % index << std::endl;
						}
					}
					else
					{
						std::cerr << boost::format(_("Default trigger entry %1% has invalid event value \"%2%\".")) % index % ref.second << std::endl;
					}
				}
				else if (boost::starts_with(type, "Condition"))
				{
					const std::size_t conditionIndex = boost::lexical_cast<std::size_t>(type.substr(strlen("Condition"))) - 1;

					Functions::iterator iterator = this->conditions().find(ref.second);

					if (iterator != this->conditions().end())
					{
						if (conditionIndex < trigger->conditions().size())
						{
							trigger->conditions()[conditionIndex] = iterator->second;
						}
						else
						{
							std::cerr << boost::format(_("Invalid condition index %1% for default trigger entry %2%.")) % conditionIndex % index << std::endl;
						}
					}
					else
					{
						std::cerr << boost::format(_("Default trigger entry %1% has invalid condition value \"%2%\".")) % index % ref.second << std::endl;
					}
				}
				else if (boost::starts_with(type, "Action"))
				{
					const std::size_t actionIndex = boost::lexical_cast<std::size_t>(type.substr(strlen("Action"))) - 1;

					Functions::iterator iterator = this->actions().find(ref.second);

					if (iterator != this->actions().end())
					{
						if (actionIndex < trigger->actions().size())
						{
							trigger->actions()[actionIndex] = iterator->second;
						}
						else
						{
							std::cerr << boost::format(_("Invalid action index %1% for default trigger entry %2%.")) % actionIndex % index << std::endl;
						}
					}
					else
					{
						std::cerr << boost::format(_("Default trigger entry %1% has invalid action value \"%2%\".")) % index % ref.second << std::endl;
					}
				}
				else
				{
					std::cerr << boost::format(_("Default trigger entry \"%1%\" has unknown suffix \"%2%\".")) % type << std::endl;
				}
			}
			else
			{
				std::cerr << boost::format(_("Invalid index %1% for default trigger \"%2%\".")) % index % ref.first << std::endl;
			}
		}
	}

	if (numTriggers != actualTriggers)
	{
		std::cerr << boost::format(_("Expected default triggers %1% are not equal to actual ones %2%.")) % numTriggers % actualTriggers << std::endl;
	}

	return size;
}

std::streamsize TriggerData::write(OutputStream &ostream) const
{
	boost::scoped_ptr<Txt> txt(new Txt());
	Txt::Section triggerCategories;
	triggerCategories.name = "TriggerCategories";

	BOOST_FOREACH(Categories::const_reference ref, this->categories())
	{
		const string categoryName = ref->first;
		const Category *category = ref->second;
		stringstream sstream;
		sstream << category->displayText() << "," << category->iconImageFile();

		if (!category->displayName())
		{
			sstream << ",1";
		}

		const string entry = sstream.str();

		triggerCategories.entries.push_back(Txt::Entry(categoryName, entry));
	}

	txt->sections().push_back(triggerCategories);

	Txt::Section triggerTypes;
	triggerTypes.name = "TriggerTypes";

	BOOST_FOREACH(Types::const_reference ref, this->types())
	{
		const string typeName = ref->first;
		const Type *type = ref->second;
		stringstream sstream;
		sstream << type->canBeGlobal() << "," << type->canBeCompared() << "," << type->displayText();

		if (type->baseType() != nullptr)
		{
			sstream << "," << type->baseType()->name();
		}

		const string entry = sstream.str();

		triggerTypes.entries.push_back(Txt::Entry(typeName, entry));
	}

	txt->sections().push_back(triggerTypes);

	// TriggerTypeDefaults
	Txt::Section triggerTypeDefaults;
	triggerTypeDefaults.name = "TriggerTypeDefaults";

	BOOST_FOREACH(Types::const_reference ref, this->types())
	{
		const string typeName = ref->first;
		const Type *type = ref->second;


		if (!type->defaultValue().empty())
		{
			triggerTypeDefaults.entries.push_back(Txt::Entry(typeName, type->defaultValue()));
		}
	}

	txt->sections().push_back(triggerTypeDefaults);

	// TriggerParams
	Txt::Section triggerParams;
	triggerParams.name = "TriggerParams";

	BOOST_FOREACH(Parameters::const_reference ref, this->parameters())
	{
		const string parameterName = ref->first;
		const Parameter *parameter = ref->second;
		stringstream sstream;

		if (parameter->type() != nullptr)
		{
			sstream << "," << parameter->type()->name();
		}
		else
		{
			sstream << ",";
		}

		sstream << "," << parameter->code() << "," << parameter->displayText();

		const string entry = sstream.str();

		triggerParams.entries.push_back(Txt::Entry(parameterName, entry));
	}

	txt->sections().push_back(triggerParams);

	Txt::Section triggerEvents;
	triggerEvents.name = "TriggerEvents";

	BOOST_FOREACH(Functions::const_reference ref, this->events())
	{
		writeFunction(ref->second, triggerEvents);
	}

	txt->sections().push_back(triggerEvents);

	Txt::Section triggerConditions;
	triggerConditions.name = "TriggerConditions";

	BOOST_FOREACH(Functions::const_reference ref, this->conditions())
	{
		writeFunction(ref->second, triggerConditions);
	}

	txt->sections().push_back(triggerConditions);

	Txt::Section triggerActions;
	triggerActions.name = "TriggerActions";

	BOOST_FOREACH(Functions::const_reference ref, this->actions())
	{
		writeFunction(ref->second, triggerActions);
	}

	txt->sections().push_back(triggerActions);

	Txt::Section triggerCalls;
	triggerCalls.name = "TriggerCalls";

	BOOST_FOREACH(Calls::const_reference ref, this->calls())
	{
		writeFunction(ref->second, triggerCalls);
	}

	txt->sections().push_back(triggerCalls);

	// DefaultTriggerCategories
	Txt::Section defaultTriggerCategories;
	defaultTriggerCategories.name = "DefaultTriggerCategories";

	Txt::Entry numCategoriesEntry;
	numCategoriesEntry.setKey("NumCategories");
	numCategoriesEntry.setValue(boost::lexical_cast<string>(this->defaultTriggerCategories().size()));
	defaultTriggerCategories.entries.push_back(numCategoriesEntry);
	std::size_t i = 1;

	BOOST_FOREACH(DefaultTriggerCategories::const_reference ref, this->defaultTriggerCategories())
	{
		Txt::Entry categoryEntry;
		stringstream sstream;
		sstream << "Category";

		if (i < 10)
		{
			sstream << "0";
		}

		sstream << i;
		const string key = sstream.str();
		categoryEntry.setKey(key);
		categoryEntry.setValue(ref);
		defaultTriggerCategories.entries.push_back(categoryEntry);

		++i;
	}

	txt->sections().push_back(defaultTriggerCategories);

	// DefaultTriggers
	Txt::Section defaultTriggers;
	defaultTriggers.name = "DefaultTriggers";

	Txt::Entry numTriggersEntry;
	numTriggersEntry.setKey("NumTriggers");
	numTriggersEntry.setValue(boost::lexical_cast<string>(this->defaultTriggers().size()));
	defaultTriggers.entries.push_back(numTriggersEntry);

	i = 1;

	BOOST_FOREACH(DefaultTriggers::const_reference ref, this->defaultTriggers())
	{
		stringstream sstream;
		sstream << "Trigger";

		if (i < 10)
		{
			sstream << "0";
		}

		sstream << i;
		const string keySuffix = sstream.str();

		Txt::Entry triggerNameEntry;
		triggerNameEntry.setKey(keySuffix + "Name");
		triggerNameEntry.setValue(ref.name());
		defaultTriggers.entries.push_back(triggerNameEntry);

		Txt::Entry triggerCommentEntry;
		triggerCommentEntry.setKey(keySuffix + "Comment");
		triggerCommentEntry.setValue(ref.comment());
		defaultTriggers.entries.push_back(triggerCommentEntry);

		Txt::Entry triggerCategoryEntry;
		triggerCategoryEntry.setKey(keySuffix + "Category");
		triggerCategoryEntry.setValue(ref.triggerCategory()); // TODO index?
		defaultTriggers.entries.push_back(triggerCategoryEntry);

		Txt::Entry triggerEventsEntry;
		triggerEventsEntry.setKey(keySuffix + "Events");
		triggerEventsEntry.setValue(boost::lexical_cast<string>(ref.events().size()));
		defaultTriggers.entries.push_back(triggerEventsEntry);

		std::size_t j = 1;

		BOOST_FOREACH(DefaultTrigger::Functions::const_reference eventRef, ref.events())
		{
			sstream.str("");
			sstream << keySuffix << "Event";

			if (j < 10)
			{
				sstream << "0";
			}

			sstream << j;
			const string eventKey = sstream.str();

			Txt::Entry triggerEventEntry;
			triggerEventEntry.setKey(eventKey);
			triggerEventEntry.setValue(eventRef->code());
			defaultTriggers.entries.push_back(triggerEventEntry);

			++j;
		}

		Txt::Entry triggerConditionsEntry;
		triggerConditionsEntry.setKey(keySuffix + "Conditions");
		triggerConditionsEntry.setValue(boost::lexical_cast<string>(ref.conditions().size()));
		defaultTriggers.entries.push_back(triggerConditionsEntry);

		j = 1;

		BOOST_FOREACH(DefaultTrigger::Functions::const_reference conditionRef, ref.conditions())
		{
			sstream.str("");
			sstream << keySuffix << "Condition";

			if (j < 10)
			{
				sstream << "0";
			}

			sstream << j;
			const string conditionKey = sstream.str();

			Txt::Entry triggerConditionEntry;
			triggerConditionEntry.setKey(conditionKey);
			triggerConditionEntry.setValue(conditionRef->code());
			defaultTriggers.entries.push_back(triggerConditionEntry);

			++j;
		}

		Txt::Entry triggerActionsEntry;
		triggerActionsEntry.setKey(keySuffix + "Actions");
		triggerActionsEntry.setValue(boost::lexical_cast<string>(ref.actions().size()));
		defaultTriggers.entries.push_back(triggerActionsEntry);

		j = 1;

		BOOST_FOREACH(DefaultTrigger::Functions::const_reference actionRef, ref.actions())
		{
			sstream.str("");
			sstream << keySuffix << "Action";

			if (j < 10)
			{
				sstream << "0";
			}

			sstream << j;
			const string actionKey = sstream.str();

			Txt::Entry triggerActionEntry;
			triggerActionEntry.setKey(actionKey);
			triggerActionEntry.setValue(actionRef->code());
			defaultTriggers.entries.push_back(triggerActionEntry);

			++j;
		}

		++i;
	}

	txt->sections().push_back(defaultTriggers);

	return txt->write(ostream);
}

void TriggerData::clear()
{
	this->m_sectionEntries.clear();
	this->m_categories.clear();
	this->m_types.clear();
	this->m_parameters.clear();
	this->m_events.clear();
	this->m_conditions.clear();
	this->m_actions.clear();
	this->m_calls.clear();
	this->m_defaultTriggerCategories.clear();
	this->m_defaultTriggers.clear();
}

const Txt::Entries& TriggerData::sectionEntries(const string &sectionName) const
{
	SectionEntries::const_iterator iterator = this->m_sectionEntries.find(sectionName);

	if (iterator != this->m_sectionEntries.end())
	{
		return *iterator->second;
	}

	throw Exception(boost::format(_("Unknown section: %1%")) % sectionName);
}

}

}
