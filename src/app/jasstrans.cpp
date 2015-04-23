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

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "../jass.hpp"
#include "../i18n.hpp"
#include "../map.hpp"
#include "../core.hpp"

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
		
		void visit(const wc3lib::jass::jass_ast &ast)
		{
			BOOST_FOREACH(wc3lib::jass::jass_files::const_reference file, ast.files)
			{
				this->visit(file)
			}
		}
		
		void visit(const wc3lib::jass::jass_file &file)
		{
			BOOST_FOREACH(wc3lib::jass::jass_declarations::const_reference declaration, file.declarations)
			{
				this->visit(declaration);
			}
			
			BOOST_FOREACH(wc3lib::jass::jass_functions::const_reference function, file.functions)
			{
				this->visit(function);
			}
		}
		
		void visit(const wc3lib::jass::jass_global_declaration &declaration)
		{
			BOOST_FOREACH(wc3lib::jass::jass_globals::const_reference global, declaration.globals)
			{
				this->visit(global);
			}
		}
		
		void visit(const wc3lib::jass::jass_function &function)
		{
			BOOST_FOREACH(wc3lib::jass::jass_locals::const_reference local, function.locals)
			{
			}
			
			BOOST_FOREACH(wc3lib::jass::jass_statements::const_reference statement, function.statements)
			{
			}
		}
		
		void visit(const wc3lib::jass::jass_global &global)
		{
			bool isStringAssignment = false;
					
			switch (global.declaration.type.whichType())
			{
				case wc3lib::jass::jass_type_reference::Type::Declaration:
				{
					isStringAssignment = boost::get<const wc3lib::jass::jass_type*>(global.declaration.type.variant)->identifier == "string";
					
					break;
				}
				
				case wc3lib::jass::jass_type_reference::Type::String:
				{
					isStringAssignment = boost::get<wc3lib::string>(global.declaration.type.variant) == "string";
					
					break;
				}
			}
			
			if (isStringAssignment && global.declaration.assignment.is_initialized())
			{
				// TODO extract string data from assignment/jass expression
			}
		}
		
		void visit(const wc3lib::map::MapStrings &mapStrings)
		{
			BOOST_FOREACH(wc3lib::map::MapStrings::Entries::const_reference entry, mapStrings.entries())
			{
				entry.push_back(entry);
			}
		}
		
		const Entries& entries() const
		{
			return this->m_entries;
		}
		
	private:
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
	boost::filesystem::path outputFile;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of jassc."))
	("help,h", _("Shows this text."))
	("merge,m", boost::program_options::value<bool>(&merge), _("Merges the output file if it does already exists."))
	("i", boost::program_options::value<Strings>(&inputFiles), _("Input files."))
	("o", boost::program_options::value<boost::filesystem::path>(&outputFile), _("Output file (WTS or FDF)."))
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
	
	if (outputFile.empty())
	{
		std::cerr << _("Missing output file.") << std::endl;

		return EXIT_FAILURE;
	}
	
	boost::filesystem::path out(outputFile);
		
	if (boost::filesystem::exists(out) && !merge)
	{
		std::cerr << _("Output file does already exist.") << std::endl;
		
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
	if (merge && boost::filesystem::exists(out))
	{
		boost::filesystem::ifstream in(out);
		
		if (!out)
		{
			std::cerr << boost::format(_("Unable to open output file %1%.")) % out << std::endl;
			
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

	BOOST_FOREACH(Strings::const_reference ref, inputFiles)
	{
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
		}
		else
		{
			std::cerr << boost::format(_("File \"%1%\" is no regular file.")) % ref << std::endl;
		}
	}

	/*
	 * If an error did occur when parsing the grammar we should not check the AST since
	 * it will be corrupted anyways.
	 * So do only check the AST for types etc. if it had been parsed properly.
	 */
	if (!error)
	{
		AstVisitor visitor;
		visitor.visit(strings);
		visitor.visit(ast);
	}

	return 0;
}
