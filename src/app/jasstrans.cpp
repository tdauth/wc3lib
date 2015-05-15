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

/**
 * \file
 * The jasstrans executable allows you to parse JASS input files as well as string input files such as .wts files
 * and extracts all external strings from the JASS files trying to match them with existing entries in the string
 * files.
 *
 * If no such entries exist it will create new ones.
 *
 * Finally it is able to generate an output string file with all extracted strings.
 * This output file can be used by any translator to translate the strings.
 *
 * When doing the extraction it will replace the external string references in the JASS input files by the correct ids from the generated
 * string file.
 *
 * The program is very similar to "xgettext" which does string extraction for more general programming languages.
 *
 * The most basic use case is to run jasstrans on a map which extracts all externalized strings from the map script and joins them into the map's
 * war3map.wts file.
 * Besides it generates a new output JASS file with all GetLocalizedString() calls having a replaced key.
 */

#include <vector>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "../jass.hpp"
#include "../i18n.hpp"
#include "../map.hpp"
#include "../core.hpp"

#include "../spirit.hpp"

namespace
{

const char *version = "0.1";

}

/**
 * \brief Collects all string references from a JASS AST.
 */
struct AstVisitor
{
	public:
		typedef wc3lib::map::MapStrings::Entries Entries;

		AstVisitor(const std::string &translationFunctionIdentifier = "GetLocalizedString") : m_translationFunctionIdentifier(translationFunctionIdentifier), m_key(0)
		{
		}

		void visit(wc3lib::jass::jass_ast &ast)
		{
			std::cerr << "Files " << ast.files.size() << std::endl;

			BOOST_FOREACH(wc3lib::jass::jass_files::reference file, ast.files)
			{
				this->visit(file);
			}
		}

		void visit(wc3lib::jass::jass_file &file)
		{
			std::cout << "Visiting file " << file.path << std::endl;
                        this->visit(file.declarations);

			BOOST_FOREACH(wc3lib::jass::jass_functions::reference function, file.functions)
			{
				std::cout << "Visiting function" << std::endl;
				this->visit(function);
			}
		}

		void visit(wc3lib::jass::jass_declarations &declarations)
                {
			BOOST_FOREACH(wc3lib::jass::jass_globals::reference global, declarations.globals)
			{
				this->visit(global);
			}
                }

		void visit(wc3lib::jass::jass_function_call &call)
		{
			std::cerr << "Call" << std::endl;

			bool isTranslationFunction = false;

			switch (call.function.whichType())
			{
				case wc3lib::jass::jass_function_reference::Type::Declaration:
				{
					const wc3lib::jass::jass_function_declaration *declaration = boost::get<const wc3lib::jass::jass_function_declaration*>(call.function.variant);

					std::cerr << "Identifier: " << declaration->identifier << std::endl;

					if (declaration->identifier == this->m_translationFunctionIdentifier)
					{
						isTranslationFunction = true;
					}

					break;
				}

				case wc3lib::jass::jass_function_reference::Type::String:
				{
					const wc3lib::string &identifier = boost::get<wc3lib::string>(call.function.variant);

					std::cerr << "Identifier: " << identifier << std::endl;

					if (identifier == this->m_translationFunctionIdentifier)
					{
						isTranslationFunction = true;
					}

					break;
				}
			}

			if (isTranslationFunction)
			{
				std::vector<wc3lib::string> args;

				BOOST_FOREACH(wc3lib::jass::jass_function_args::reference arg, call.arguments)
				{
					wc3lib::jass::jass_expression &expression = arg.get();

					switch (expression.whichType())
					{
						case wc3lib::jass::jass_expression::Type::Constant:
						{
							wc3lib::jass::jass_const &jass_const = boost::get<wc3lib::jass::jass_const>(expression.variant);

							switch (jass_const.whichType())
							{
								case wc3lib::jass::jass_const::Type::String:
								{
									const wc3lib::string value = boost::get<wc3lib::string>(jass_const.variant);

									if (!boost::algorithm::starts_with(value, "TRIGSTRING"))
									{
										args.push_back(value);

										wc3lib::ostringstream ostream;
										// TODO fill 0
										ostream << "TRIGSTRING_" << this->m_key;
										jass_const.variant = ostream.str();
									}

									break;
								}
							}

							break;
						}
					}

					// only take the first argument
					break;
				}

				if (!args.empty())
				{
					Entries::value_type entry;
					entry.key = this->m_key;
					entry.comment = ""; // line and column position and file
					entry.value = args[0];
					this->m_entries.push_back(entry);

					this->m_key++;
				}
			}
		}

		void visit(wc3lib::jass::jass_expression &expression)
		{
			switch (expression.whichType())
			{
				case wc3lib::jass::jass_expression::Type::FunctionCall:
				{
					wc3lib::jass::jass_function_call &call = boost::get<wc3lib::jass::jass_function_call>(expression.variant);
					this->visit(call);

					break;
				}
			}
		}

		void visit(wc3lib::jass::jass_function &function)
		{
			BOOST_FOREACH(wc3lib::jass::jass_locals::reference local, function.locals)
			{
				this->visit(local);
			}

			BOOST_FOREACH(wc3lib::jass::jass_statements::reference statement, function.statements)
			{
				this->visit(statement);
			}
		}

		void visit(wc3lib::jass::jass_global &global)
		{
			visit(global.declaration);
		}

		void visit(wc3lib::jass::jass_var_declaration &declaration)
		{
			bool isStringAssignment = false;

			switch (declaration.type.whichType())
			{
				case wc3lib::jass::jass_type_reference::Type::Declaration:
				{
					isStringAssignment = boost::get<const wc3lib::jass::jass_type*>(declaration.type.variant)->identifier == "string";

					break;
				}

				case wc3lib::jass::jass_type_reference::Type::String:
				{
					isStringAssignment = boost::get<wc3lib::string>(declaration.type.variant) == "string";

					break;
				}
			}

			if (isStringAssignment && declaration.assignment.is_initialized())
			{
				this->visit(declaration.assignment.get());
			}
		}

		void visit(wc3lib::jass::jass_statement &statement)
		{
			switch (statement.whichType())
			{
				case wc3lib::jass::jass_statement::Type::Call:
				{
					this->visit(boost::get<wc3lib::jass::jass_call>(statement.variant));

					break;
				}

				case wc3lib::jass::jass_statement::Type::Return:
				{
					this->visit(boost::get<wc3lib::jass::jass_return>(statement.variant));

					break;
				}
			}
		}

		void visit(wc3lib::jass::jass_call &call)
		{
			this->visit(call.arguments);
		}

		void visit(wc3lib::jass::jass_function_args &args)
		{
			BOOST_FOREACH(wc3lib::jass::jass_function_args::reference arg, args)
			{
				this->visit(arg.get());
			}
		}

		void visit(wc3lib::jass::jass_return &return_statement)
		{
			std::cerr << "Return" << std::endl;

			if (return_statement.expression.is_initialized())
			{
				this->visit(return_statement.expression.get().get());
			}
		}

		/**
		 * \note Sets the internal starting key to the maximum key + 1 from \p mapStrings so keys won't interfere.
		 */
		void visit(wc3lib::map::MapStrings &mapStrings)
		{
			BOOST_FOREACH(wc3lib::map::MapStrings::Entries::const_reference entry, mapStrings.entries())
			{
				this->m_entries.push_back(entry);

				if (entry.key > this->m_key)
				{
					this->m_key = entry.key + 1;
				}
			}
		}

		const Entries& entries() const
		{
			return this->m_entries;
		}

	private:
		std::string m_translationFunctionIdentifier;
		int m_key;
		Entries m_entries;
};

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("jasstrans", LOCALE_DIR);
	textdomain("jasstrans");

	typedef std::vector<std::string> Strings;
	Strings inputFiles;
	bool merge = false;
	bool force = false;
	std::string keyword;
	std::string prefix;
	std::string suffix;
	boost::filesystem::path outputFile;
	boost::filesystem::path jassOutputFile;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of jasstrans."))
	("help,h", _("Shows this text."))
	("join-existing,j", _("Joins the output file if it does already exists."))
	("force,f", _("Overwrites an existing output file (for example the JASS output file or the WTS output file). -j has always a higher priority."))
	("keyword,k", boost::program_options::value<std::string>(&keyword)->default_value("GetLocalizedString"), _("Sets the identifier of the function which marks localized strings. The first argument is extracted by default."))
	("prefix,p", boost::program_options::value<std::string>(&prefix)->default_value(""), _("Uses the string as prefix for the extracted message. This only works when extracting into an FDF file."))
	("suffix,s", boost::program_options::value<std::string>(&suffix)->default_value(""), _("Uses the string as suffix for the extracted message. This only works when extracting into an FDF file."))
	("i", boost::program_options::value<Strings>(&inputFiles), _("Input files."))
	("output,o", boost::program_options::value<boost::filesystem::path>(&outputFile), _("Output file (WTS or FDF)."))
	("jass-output,O", boost::program_options::value<boost::filesystem::path>(&jassOutputFile), _("Specifies the resulting JASS output file which contains the replaced strings."))
	;

	boost::program_options::positional_options_description p;
	p.add("i", -1);

	boost::program_options::variables_map vm;

	try
	{
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	}
	catch (std::exception &exception)
	{
		std::cerr << boost::format(_("Error while parsing program options: \"%1%\"")) % exception.what() << std::endl;

		return EXIT_FAILURE;
	}

	boost::program_options::notify(vm);

	if (vm.count("version"))
	{
		std::cout <<
		boost::format(_("jasstrans %1%.")) % version
		<< std::endl <<
		_(
		"Copyright © 2009 Tamino Dauth\n"
		"License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law."
		) << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("help"))
	{
		std::cout << _("Usage: jasstrans [options] [input files]") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << _("\nReport bugs to tamino@cdauth.eu or on https://wc3lib.org") << std::endl;

		return EXIT_SUCCESS;
	}

	if (vm.count("join-existing"))
	{
		merge = true;
	}

	if (vm.count("force"))
	{
		force = true;
	}


	if (outputFile.empty())
	{
		std::cerr << _("Missing output file.") << std::endl;

		return EXIT_FAILURE;
	}

	if (jassOutputFile.empty())
	{
		std::cerr << _("Missing JASS output file.") << std::endl;

		return EXIT_FAILURE;
	}

	if (boost::filesystem::exists(outputFile) && !merge && !force)
	{
		std::cerr << _("Output file does already exist.") << std::endl;

		return EXIT_FAILURE;
	}

	if (boost::filesystem::exists(jassOutputFile) && !force)
	{
		std::cerr << _("Output JASS file does already exist.") << std::endl;

		return EXIT_FAILURE;
	}

	if (inputFiles.empty())
	{
		std::cerr << _("Missing input files.") << std::endl;

		return EXIT_FAILURE;
	}

	wc3lib::map::MapStrings strings;

	/**
	 * Merge with existing strings file.
	 */
	if (merge && boost::filesystem::exists(outputFile))
	{
		boost::filesystem::ifstream in(outputFile);

		if (!in)
		{
			std::cerr << boost::format(_("Unable to open output file %1%.")) % outputFile << std::endl;

			return EXIT_FAILURE;
		}

		try
		{
			strings.read(in);
		}
		catch (const wc3lib::Exception &e)
		{
			std::cerr << e.what() << std::endl;

			return EXIT_FAILURE;
		}
	}

	/*
	 * TODO
	 * find string literals in a JASS AST and create WTS or FDF files.
	 */
	wc3lib::jass::Grammar grammar;
	wc3lib::jass::jass_ast ast;
	bool error = false;

	wc3lib::spiritTraceLog.open("jasstrans_traces.xml");

	BOOST_FOREACH(Strings::const_reference ref, inputFiles)
	{
		std::cout << boost::format(_("Processing file \"%1%\".")) % ref << std::endl;

		boost::filesystem::path file(ref);

		if (boost::filesystem::is_regular_file(file))
		{
			wc3lib::ifstream ifstream(file);

			if (ifstream)
			{
				try
				{
					if (!grammar.parse(ifstream, ast, ref))
					{
						error = true;

						std::cerr << boost::format(_("Error while parsing file \"%1%\".")) % ref << std::endl;
					}
				}
				catch (wc3lib::Exception &e)
				{
					error = true;

					std::cerr << boost::format(_("Error while parsing file \"%1%\": %2%")) % ref % e.what() << std::endl;
				}
			}
			else
			{
				std::cerr << boost::format(_("Error on opening input file \"%1%\".")) % ref << std::endl;
			}
		}
		else
		{
			std::cerr << boost::format(_("File \"%1%\" is no regular file.")) % ref << std::endl;
		}
	}

	wc3lib::spiritTraceLog.close();

	/*
	 * If an error did occur when parsing the grammar we should not check the AST since
	 * it will be corrupted anyways.
	 * So do only check the AST for types etc. if it had been parsed properly.
	 */
	if (!error)
	{
		std::cerr << "AST files " << ast.files.size() << std::endl;

		AstVisitor visitor(keyword);
		visitor.visit(strings);
		visitor.visit(ast);
		/*
		 * Create an output file depending on the specified file extension.
		 * By default use a wts file.
		 */
		bool isWts = outputFile.extension() != "fdf";

		boost::filesystem::ofstream out(outputFile);

		if (out)
		{
			std::cout << boost::format(_("Writing output file %1%.")) % outputFile << std::endl;

			if (isWts)
			{
				wc3lib::map::MapStrings mapStrings;
				mapStrings.setEntries(visitor.entries());

				try
				{
					mapStrings.write(out);
				}
				catch (const wc3lib::Exception &e)
				{
					std::cerr << boost::format(_("Error on writing map strings %1%: %2%")) % outputFile % e.what() << std::endl;
				}
			}
			// TODO else
		}
		else
		{
			std::cerr << boost::format(_("Error on generating file %1%.")) % outputFile << std::endl;
		}

		/*
		 * Now generate the JASS file with the complete AST but with replaced string values.
		 */
		out.close();
		out.open(jassOutputFile);

		if (out)
		{
			std::cout << boost::format(_("Writing output file %1%.")) % jassOutputFile << std::endl;

			wc3lib::jass::Generator generator;

			try
			{
				if (!generator.generate(out, ast))
				{
					std::cerr << boost::format(_("Error on writing JASS output file %1%.")) % jassOutputFile << std::endl;
				}
			}
			catch (const wc3lib::Exception &e)
			{
				std::cerr << boost::format(_("Error on writing JASS output %1%: %2%")) % jassOutputFile % e.what() << std::endl;
			}
		}
		else
		{
			std::cerr << boost::format(_("Error on generating file %1%.")) % jassOutputFile << std::endl;
		}
	}

	return 0;
}
