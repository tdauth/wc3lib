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

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <list>
#include <fstream>

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
//#include <boost/thread.hpp>
#include <boost/ptr_container/ptr_map.hpp>

// XML support
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <boost/program_options.hpp>

#include "../core.hpp"
#include "../blp.hpp"
#include "../map.hpp"
#include "../mdlx.hpp"
#include "../mpq.hpp"

namespace
{

const char *version = "0.1";

class ConvFormat
{
	public:
		typedef boost::ptr_map<std::string, ConvFormat> Formats;
		static Formats formats;

		static void append(const std::string &extension, const std::string &description, bool isBinary, const std::string &group)
		{
			std::auto_ptr<ConvFormat> format(new ConvFormat(extension, description, isBinary, group));
			formats.insert(extension, format);
		}

		const std::string& extension() const
		{
			return this->m_extension;
		}

		const std::string& description() const
		{
			return this->m_description;
		}

		bool isBinary() const
		{
			return this->m_isBinary;
		}

		const std::string& group() const
		{
			return this->m_group;
		}

	private:
		ConvFormat(const std::string &extension, const std::string &description, bool isBinary, const std::string &group) : m_extension(extension), m_description(description), m_isBinary(isBinary), m_group(group)
		{
		}

		std::string m_extension;
		std::string m_description;
		bool m_isBinary;
		std::string m_group;
};

ConvFormat::Formats ConvFormat::formats;

typedef std::list<boost::filesystem::path> FilePaths;

bool addFilePath(const boost::filesystem::path &path, FilePaths &filePaths, bool recursive, std::string extension)
{
	if (!boost::filesystem::exists(path))
	{
		std::cerr << boost::format(_("%1% does not exist.")) % path << std::endl;

		return false;
	}

	if (boost::filesystem::is_directory(path))
	{
		if (recursive)
		{
			boost::filesystem::directory_iterator endIterator;

			for (boost::filesystem::directory_iterator iterator(path); iterator != endIterator; ++iterator)
				addFilePath(iterator->path(), filePaths, recursive, extension);
		}
		else
		{
			std::cerr << boost::format(_("Skipping directory %1% since --recursive is not used.")) % path << std::endl;

			return false;
		}
	}
	else if (extension != "" && boost::filesystem::extension(path) != extension)
	{
		std::cerr << boost::format(_("Path %1% has not extension \"%2%\".")) % path % extension << std::endl;

		return false;
	}

	filePaths.push_back(path);

	return true;
}

void convertBlp(const boost::filesystem::path &path, wc3lib::ifstream &in, wc3lib::ofstream &out, const ConvFormat &inputFormat, const ConvFormat &outputFormat, bool verbose) throw (wc3lib::Exception)
{
	boost::scoped_ptr<wc3lib::blp::Blp> blp(new wc3lib::blp::Blp());

	if (inputFormat.extension() == "blp")
	{
		std::streamsize bytes = blp->read(in);

		if (verbose)
			std::cout << boost::format(_("Read BLP file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;
	}
	else
	{
		throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted with a valid input BLP format.\nUsed input format is %2%.")) % path.string() % inputFormat.extension());
	}

	if (outputFormat.extension() == "blp")
	{
		std::streamsize bytes = blp->write(out);

		if (verbose)
			std::cout << boost::format(_("Wrote BLP file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;
	}
	else
	{
		throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted into a valid output BLP format.\nUsed output format is %2%.")) % path.string() % outputFormat.extension());
	}
}

void convertMdlx(const boost::filesystem::path &path, wc3lib::ifstream &in, wc3lib::ofstream &out, const ConvFormat &inputFormat, const ConvFormat &outputFormat, bool verbose) throw (wc3lib::Exception)
{
	boost::scoped_ptr<wc3lib::mdlx::Mdlx> mdlx(new wc3lib::mdlx::Mdlx());

	if (inputFormat.extension() == "mdx")
	{
		std::streamsize bytes = mdlx->readMdx(in);

		if (verbose)
			std::cout << boost::format(_("Read MDX file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;
	}
	else if (inputFormat.extension() == "mdl")
	{
		std::streamsize bytes = mdlx->readMdl(in);

		if (verbose)
			std::cout << boost::format(_("Read MDL file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;
	}
	else
	{
		throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted with a valid input MDLX format.\nUsed input format is %2%.")) % path.string() % inputFormat.extension());
	}

	if (outputFormat.extension() == "mdx")
	{
		std::streamsize bytes = mdlx->writeMdx(out);

		if (verbose)
			std::cout << boost::format(_("Wrote MDX file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;
	}
	else if (outputFormat.extension() == "mdl")
	{
		std::streamsize bytes = mdlx->writeMdl(out);

		if (verbose)
			std::cout << boost::format(_("Wrote MDL file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;
	}
	else
	{
		throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted into a valid output MDLX format.\nUsed output format is %2%.")) % path.string() % outputFormat.extension());
	}
}

void convertText(const boost::filesystem::path &path, wc3lib::ifstream &in, wc3lib::ofstream &out, const ConvFormat &inputFormat, const ConvFormat &outputFormat, bool verbose) throw (wc3lib::Exception)
{
	boost::scoped_ptr<wc3lib::map::Txt> txt(new wc3lib::map::Txt());

	if (inputFormat.extension() == "txt")
	{
		std::streamsize bytes = txt->read(in);

		if (verbose)
			std::cout << boost::format(_("Read TXT file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;
	}
	else if (inputFormat.extension() == "xml")
	{
		boost::archive::xml_iarchive ia(in);
		ia >> BOOST_SERIALIZATION_NVP(*txt);
	}
	else
	{
		throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted with a valid input text format.\nUsed input format is %2%.")) % path.string() % inputFormat.extension());
	}

	if (outputFormat.extension() == "xml")
	{
		boost::archive::xml_oarchive oa(out);
		oa << BOOST_SERIALIZATION_NVP(*txt);
		//std::streamsize bytes = mdlx->writeMdx(out);

		//if (verbose)
		//	std::cout << boost::format(_("Wrote MDX file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;
	}
	else if (outputFormat.extension() == "txt")
	{
		std::streamsize bytes = txt->write(out);

		if (verbose)
			std::cout << boost::format(_("Wrote TXT file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;
	}
	else
	{
		throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted into a valid output text format.\nUsed output format is %2%.")) % path.string() % outputFormat.extension());
	}
}

void convertFile(const boost::filesystem::path &path, const boost::filesystem::path &outputPath, const ConvFormat &inputFormat, const ConvFormat &outputFormat, bool verbose, bool overwrite) throw (wc3lib::Exception)
{
	std::ios_base::openmode openMode = std::ifstream::in;

	if (inputFormat.isBinary())
		openMode |= std::ifstream::binary;

	if (verbose)
		std::cout << boost::format(_("Reading file \"%1%\".")) % path.string() << std::endl;

	boost::filesystem::ifstream ifstream(path, openMode);

	if (!ifstream)
		throw wc3lib::Exception(boost::format(_("Error while opening file \"%1%\". Continuing with next one.")) % path.string());

	boost::filesystem::ofstream ofstream;

	openMode = std::ofstream::out;

	if (outputFormat.isBinary())
		openMode |= std::ofstream::binary;

	if (!overwrite && boost::filesystem::exists(outputPath))
		throw wc3lib::Exception(boost::format(_("File \"%1%\" does already exist. Continuing with next one.")) % outputPath);

	ofstream.open(outputPath, openMode);

	if (!ofstream)
		throw wc3lib::Exception(boost::format(_("Error while opening file \"%1%\". Continuing with next one.")) % outputPath);

	if (inputFormat.group() == "blp")
		convertBlp(path, ifstream, ofstream, inputFormat, outputFormat, verbose);
	else if (inputFormat.group() == "mdlx")
		convertMdlx(path, ifstream, ofstream, inputFormat, outputFormat, verbose);
	else if (inputFormat.group() == "text")
		convertText(path, ifstream, ofstream, inputFormat, outputFormat, verbose);
	else
		throw wc3lib::Exception(boost::format(_("Input format \"%1%\" doesn't belong to any format group.")) % inputFormat.extension());

	ifstream.close();
	ofstream.close();
}

ConvFormat* formatByExtension(const boost::filesystem::path &path)
{
	if (boost::filesystem::extension(path).empty() || boost::filesystem::extension(path).size() == 1)
	{
		std::cerr << boost::format(_("Format \"%1%\" not found!")) % boost::filesystem::extension(path) << std::endl;

		return 0;
	}

	ConvFormat::Formats::iterator iterator = ConvFormat::formats.find(boost::filesystem::extension(path).substr(1));

	if (iterator == ConvFormat::formats.end())
	{
		std::cerr << boost::format(_("Format \"%1%\" not found!")) % boost::filesystem::extension(path) << std::endl;

		return 0;
	}

	return iterator->second;
}

}

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("wc3converter", LOCALE_DIR);
	textdomain("wc3converter");

	std::string inputFormatExtension;
	std::string outputFormatExtension;
	typedef std::vector<std::string> Strings;
	Strings inputFiles;
	boost::filesystem::path outputFile;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
	("version,V", _("Shows current version of mpq."))
	("help,h",_("Shows this text."))
	// options
	("formats,f", _("Lists all possible input and output formats."))
	("iformat", boost::program_options::value<std::string>(&inputFormatExtension)->default_value(""), _("<arg> has to be replaced by input files format."))
	("oformat", boost::program_options::value<std::string>(&outputFormatExtension)->default_value(""), _("<arg> has to be replaced by output files format."))
	("recursive,R", _("If some of the input files are directories they will be iterated recursively and searched for other files with the input format extension."))
	("overwrite", _("Overwrites existing files and directories when creating or extracting files."))
	("i", boost::program_options::value<Strings>(&inputFiles), _("Input files."))
	("o", boost::program_options::value<boost::filesystem::path>(&outputFile), _("Output file or directory (for multiple files)."))
	;

	boost::program_options::positional_options_description p;
	p.add("o", 1); // first input is output path
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
		boost::format(_("converter %1%.")) % version
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
		std::cout << _("Usage: wc3converter [options] [output file/directory] [input files]") << std::endl << std::endl;
		std::cout << desc << std::endl;
		std::cout << _("\nReport bugs to tamino@cdauth.eu or on https://wc3lib.org") << std::endl;

		return EXIT_SUCCESS;
	}

	ConvFormat::append("blp", "Blizzard Entertainment's texture format.", true, "blp");
	ConvFormat::append("mdx", "Blizzard Entertainment's binary model format.", true, "mdlx");
	ConvFormat::append("mdl", "Blizzard Entertainment's human-readable model format.", false, "mdlx");
	ConvFormat::append("txt", "Blizzard Entertainment's TXT files.", false, "text");
	ConvFormat::append("xml", "Extensible Markup Language", false, "text");

	if (vm.count("formats"))
	{
		BOOST_FOREACH(ConvFormat::Formats::value_type v, ConvFormat::formats)
			std::cout << boost::format(_("%1% - %2%")) % v.second->extension() % v.second->description() << std::endl;

		return EXIT_SUCCESS;
	}

	if (!inputFormatExtension.empty() && ConvFormat::formats.find(inputFormatExtension) == ConvFormat::formats.end())
	{
		std::cerr << boost::format(_("Format \"%1%\" does not exist.")) % inputFormatExtension << std::endl;

		return EXIT_FAILURE;
	}

	if (!outputFormatExtension.empty() && ConvFormat::formats.find(outputFormatExtension) == ConvFormat::formats.end())
	{
		std::cerr << boost::format(_("Format \"%1%\" does not exist.")) % outputFormatExtension << std::endl;

		return EXIT_FAILURE;
	}

	ConvFormat *inputFormat = 0;
	ConvFormat *outputFormat = 0;

	if (!inputFormatExtension.empty())
		inputFormat = ConvFormat::formats.find(inputFormatExtension)->second;

	if (!outputFormatExtension.empty())
		outputFormat = ConvFormat::formats.find(outputFormatExtension)->second;

	FilePaths inputFilePaths;

	BOOST_FOREACH(Strings::const_reference ref, inputFiles)
	{
		boost::filesystem::path path = ref;

		addFilePath(path, inputFilePaths, vm.count("recursive"),  inputFormat == 0 ? "" : inputFormat->extension());
	}

	if (inputFilePaths.empty())
	{
		std::cerr << _("No valid input files.") << std::endl;

		return EXIT_FAILURE;
	}

	// output path has to be a directory
	if (inputFilePaths.size() > 1)
	{
		if (!boost::filesystem::is_directory(outputFile) && !boost::filesystem::create_directory(outputFile))
		{
			std::cerr << boost::format(_("Unable to create directory %1% which is required because of multiple input files.")) % outputFile << std::endl;

			return EXIT_FAILURE;
		}
	}

	ConvFormat *realOutputFormat = outputFormat;

	if (realOutputFormat == 0)
		realOutputFormat = formatByExtension(outputFile);

	if (realOutputFormat == 0)
	{
		std::cerr << boost::format(_("Output file format couldn't be determined by extension of %1%.")) % outputFile << std::endl;

		return EXIT_FAILURE;
	}

	BOOST_FOREACH(FilePaths::reference path, inputFilePaths)
	{
		try
		{
			ConvFormat *realInputFormat = inputFormat;

			if (realInputFormat == 0)
				realInputFormat = formatByExtension(path);

			if (realOutputFormat == 0)
				throw wc3lib::Exception(boost::format(_("Input file format couldn't be determined by extension of %1%.")) % path);

			if (realInputFormat->group() != realOutputFormat->group())
				throw wc3lib::Exception(boost::format(_("\"%1%\" and \"%2%\" are not convertible.")) % realInputFormat->extension() % realOutputFormat->extension());

			boost::filesystem::path realOutputFile = outputFile;

			if (boost::filesystem::is_directory(outputFile))
				realOutputFile /= path.stem().string() + "." + realOutputFormat->extension();

			convertFile(path, realOutputFile, *realInputFormat, *realOutputFormat, vm.count("verbose"), vm.count("overwrite"));
		}
		catch (const class wc3lib::Exception &exception)
		{
			std::cerr << boost::format(_("Error occured when converting file %1%: \"%2%\".\nSkipping file.")) % path % exception.what() << std::endl;

			continue;
		}
	}

	return EXIT_SUCCESS;
}
