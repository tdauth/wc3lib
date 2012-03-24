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

#include <getopt.h>

#include "../wc3lib.hpp"

namespace
{

const char *version = "0.1";

enum Format
{
	Blp,
	Jpeg,
	Tga,
	Png,
	Mdl,
	Mdx,
	Blend,
	Max,
	Mpq,
	Tar,
	MaxFormats,
	InvalidFormat
};

const char *formatExpression[MaxFormats] =
{
	"blp",
	"jpeg",
	"tga",
	"png",
	"mdl",
	"mdx",
	"blend",
	"3ds",
	"mpq",
	"tar"
};

const char *formatDescription[MaxFormats] =
{
	_("Blizzard's texture format."),
	_("The common JPEG format."),
	_("The common TGA format."),
	_("The common PNG format."),
	_("Blizzard's text model format."),
	_("Blizzard's binary model format."),
	_("The common BLEND format."),
	_("The common 3ds Max format."),
	_("Blizzard's archive format."),
	_("The common TAR format.")
};

const char *formatExtension[MaxFormats] =
{
	"blp",
	"jpeg",
	"tga",
	"png",
	"mdl",
	"mdx",
	"blend",
	"3ds",
	"mpq",
	"tar"
};

const bool formatIsBinary[MaxFormats] =
{
	true,
	true, // JPEG
	true, // TGA
	true, // PNG
	false, // MDL
	true, // MDX
	true, // BLEND
	true, // MAX
	true, // MPQ
	true // TAR
};

const bool formatConvertibility[MaxFormats][MaxFormats] =
{
	// blp   jpeg   tga    png    mdl    mdx   blend  3ds   mpq   tar
	{
		true,
		true,
		true,
		true,
		false,
		false,
		false,
		false,
		false,
		false
	},

	{
		true,
		true,
		true,
		true,
		false,
		false,
		false,
		false,
		false,
		false
	},

	{
		true,
		true,
		true,
		true,
		false,
		false,
		false,
		false,
		false,
		false
	},

	{
		true,
		true,
		true,
		true,
		false,
		false,
		false,
		false,
		false,
		false
	},

	// mdl
	{
		false,
		false,
		false,
		false,
		true,
		true,
		true,
		true,
		false,
		false
	},

	// mdx
	{
		false,
		false,
		false,
		false,
		true,
		true,
		true,
		true,
		false,
		false
	},

	{
		false,
		false,
		false,
		false,
		true,
		true,
		true,
		true,
		false,
		false
	},

	{
		false,
		false,
		false,
		false,
		true,
		true,
		true,
		true,
		false,
		false
	},

	// mpq
	{
		false,
		false,
		false,
		false,
		false,
		false,
		false,
		false,
		true,
		true
	},

	// TAR
	{
		false,
		false,
		false,
		false,
		false,
		false,
		false,
		false,
		true,
		true
	}
};

inline std::string getFormatExpression(enum Format format)
{
	return formatExpression[format];
}

inline enum Format getFormatByExpression(const char *formatString)
{
	for (int i = 0; i < MaxFormats; ++i)
	{
		if (strcmp(formatString, formatExpression[i]) == 0)
			return Format(i);
	}

	return InvalidFormat;
}

inline std::string getFormatExtension(enum Format format)
{
	return formatExtension[format];
}

inline bool isFormatBinary(enum Format format)
{
	return formatIsBinary[format];
}

inline bool checkFormatConvertibility(enum Format format0, enum Format format1)
{
	return formatConvertibility[format0][format1];
}

bool addFilePath(const boost::filesystem::path &path, std::list<boost::filesystem::path> &filePaths, bool recursive, std::string extension, bool quiet)
{
	if (!boost::filesystem::exists(path))
	{
		if (!quiet)
		{
			std::cerr << boost::format(_("Error, path \"%1%\" does not exist.\nContinue process (y/n)?")) % path.string() << std::endl;

			return wc3lib::expectInput();
		}
		else
			return true;
	}

	if (boost::filesystem::is_directory(path))
	{
		if (recursive)
		{
			boost::filesystem::directory_iterator endIterator;

			for (boost::filesystem::directory_iterator iterator(path); iterator != endIterator; ++iterator)
				addFilePath(iterator->path(), filePaths, recursive, extension, quiet);
		}
		else
		{
			if (!quiet)
			{
				std::cerr << boost::format(_("Error, path \"%1%\" seems to be a directory and option -R is not used.\nContinue process (ignore file, y/n)?")) % path.string() << std::endl;

				return wc3lib::expectInput();
			}
			else
				return true;
		}
	}
	else if (extension != "*")
	{
		std::string::size_type index = path.string().find_last_of(extension);

		if (index == std::string::npos)
			return false;
	}

	filePaths.push_back(path);

	return true;
}

void convertBlp(const boost::filesystem::path &path, wc3lib::ifstream &in, wc3lib::ofstream &out, enum Format inputFormat, enum Format outputFormat, bool verbose, bool readonly) throw (class wc3lib::Exception)
{
	boost::scoped_ptr<wc3lib::blp::Blp> blp(new wc3lib::blp::Blp());

	switch (inputFormat)
	{
		case Blp:
		{
			std::streamsize bytes = blp->read(in);

			if (verbose)
				std::cout << boost::format(_("Read BLP file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Jpeg:
		{
			wc3lib::blp::dword bytes = 0;//blp.readJpeg(ifstream);

			if (verbose)
				std::cout << boost::format(_("Read JPEG file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Tga:
		{
			wc3lib::blp::dword bytes = 0;//blp.readTga(ifstream);

			if (verbose)
				std::cout << boost::format(_("Read TGA file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Png:
		{
			wc3lib::blp::dword bytes = 0;//blp.readPng(ifstream);

			if (verbose)
				std::cout << boost::format(_("Read PNG file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		default:
			throw wc3lib::Exception(boost::str(boost::format(_("File \"%1%\" is not converted with a valid input BLP format.\nUsed input format is %2%.")) % path.string() % inputFormat));
	}

	if (readonly)
		return;

	switch (outputFormat)
	{
		case Blp:
		{
			std::streamsize bytes = blp->write(out);

			if (verbose)
				std::cout << boost::format(_("Wrote BLP file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Jpeg:
		{
			wc3lib::blp::dword bytes = 0;//blp.writeJpeg(ofstream);

			if (verbose)
				std::cout << boost::format(_("Wrote JPEG file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Tga:
		{
			wc3lib::blp::dword bytes = 0;//blp.writeTga(ofstream);

			if (verbose)
				std::cout << boost::format(_("Wrote TGA file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Png:
		{
			wc3lib::blp::dword bytes = 0;//blp.writePng(ofstream);

			if (verbose)
				std::cout << boost::format(_("Wrote PNG file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		default:
			throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted with a valid output BLP format.\nUsed input format is %2%.")) % path.string() % inputFormat);
	}
}

void convertMdlx(const boost::filesystem::path &path, std::ifstream &ifstream, std::ofstream &ofstream, enum Format inputFormat, enum Format outputFormat, bool verbose, bool readonly) throw (class wc3lib::Exception)
{
	boost::scoped_ptr<wc3lib::mdlx::Mdlx> mdlx(new wc3lib::mdlx::Mdlx());

	switch (inputFormat)
	{
		case Mdl:
		{
			std::streamsize bytes = mdlx->readMdl(ifstream);

			if (verbose)
				std::cout << boost::format(_("Read MDL file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Mdx:
		{
			std::streamsize bytes = mdlx->readMdx(ifstream);

			if (verbose)
				std::cout << boost::format(_("Read MDX file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Blend:
		{
			wc3lib::mdlx::long32 bytes = 0;//mdlx.readBlend(ifstream);

			if (verbose)
				std::cout << boost::format(_("Read Blender file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Max:
		{
			wc3lib::mdlx::long32 bytes = 0;//mdlx.read3ds(ifstream);

			if (verbose)
				std::cout << boost::format(_("Read 3ds Max file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		default:
			throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted with a valid input MDLX format.\nUsed input format is %2%.")) % path.string() % inputFormat);
	}

	if (readonly)
		return;

	switch (outputFormat)
	{
		case Mdl:
		{
			std::streamsize bytes = mdlx->writeMdl(ofstream);

			if (verbose)
				std::cout << boost::format(_("Wrote MDL file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Mdx:
		{
			std::streamsize bytes = mdlx->writeMdx(ofstream);

			if (verbose)
				std::cout << boost::format(_("Wrote MDX file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Blend:
		{
			wc3lib::mdlx::long32 bytes = 0;//= mdlx.writeBlend(ofstream);

			if (verbose)
				std::cout << boost::format(_("Wrote Blender file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Max:
		{
			wc3lib::mdlx::long32 bytes = 0;//mdlx.write3ds(ofstream);

			if (verbose)
				std::cout << boost::format(_("Wrote 3ds Max file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		default:
			throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted with a valid output MDLX format.\nUsed output format is %2%.")) % path.string() % outputFormat);
	}
}

void convertMpq(const boost::filesystem::path &path, wc3lib::ifstream &ifstream, wc3lib::ofstream &ofstream, enum Format inputFormat, enum Format outputFormat, bool verbose, bool readonly) throw (class wc3lib::Exception)
{
	boost::scoped_ptr<wc3lib::mpq::Mpq> mpq(new wc3lib::mpq::Mpq());

	switch (inputFormat)
	{
		case Mpq:
		{
			std::streamsize bytes = mpq->open(path);

			if (verbose)
				std::cout << boost::format(_("Read MPQ file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Tar:
		{
			std::streamsize bytes = 0;//mpq.openTar(path);

			if (verbose)
				std::cout << boost::format(_("Read TAR file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		default:
			throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted with a valid input MPQ format.\nUsed input format is %2%.")) % path.string() % inputFormat);
	}

	if (readonly)
		return;

	switch (outputFormat)
	{
		case Mpq:
		{
			std::streamsize bytes = mpq->write(ofstream);

			if (verbose)
				std::cout << boost::format(_("Wrote MPQ file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		case Tar:
		{
			std::streamsize bytes = 0;//mpq.writeTar(ofstream);

			if (verbose)
				std::cout << boost::format(_("Wrote TAR file successfully. %1%.\n")) % wc3lib::sizeStringBinary(bytes) << std::endl;

			break;
		}

		default:
			throw wc3lib::Exception(boost::format(_("File \"%1%\" is not converted with a valid output MPQ format.\nUsed output format is %2%.")) % path.string() % outputFormat);
	}
}

void convertFile(const boost::filesystem::path &path, const boost::filesystem::path &dirPath, enum Format inputFormat, enum Format outputFormat, bool verbose, bool readonly) throw (class wc3lib::Exception)
{
	std::ios_base::openmode openMode = std::ifstream::in;

	if (isFormatBinary(inputFormat))
		openMode |= std::ifstream::binary;

	if (verbose)
		std::cout << boost::format(_("Reading file \"%1%\".")) % path.string() << std::endl;

	boost::filesystem::ifstream ifstream(path, openMode);

	if (!ifstream)
		throw wc3lib::Exception(boost::format(_("Error while opening file \"%1%\". Continuing with next one.")) % path.string());

	boost::filesystem::ofstream ofstream;

	if (!readonly)
	{
		openMode = std::ofstream::out;

		if (isFormatBinary(outputFormat))
			openMode |= std::ofstream::binary;

		boost::filesystem::path filePath = path.string();

		if (!dirPath.string().empty())
			filePath = dirPath / filePath.filename();

		filePath.replace_extension(getFormatExtension(outputFormat));
		ofstream.open(filePath, openMode);

		if (!ofstream)
			throw wc3lib::Exception(boost::format(_("Error while opening file \"%1%\". Continuing with next one.")) % filePath);
	}

	switch (inputFormat)
	{
		case Blp:
		case Jpeg:
		case Tga:
		case Png:
			//convertBlp(path, ifstream, ofstream, inputFormat, outputFormat, verbose, readonly);

			break;

		case Mdl:
		case Mdx:
		case Blend:
		case Max:
			convertMdlx(path, ifstream, ofstream, inputFormat, outputFormat, verbose, readonly);

			break;

		case Mpq:
		case Tar:
			convertMpq(path, ifstream, ofstream, inputFormat, outputFormat, verbose, readonly);

			break;
	}

	ifstream.close();

	if (!readonly)
		ofstream.close();
}

}

int main(int argc, char *argv[])
{
	// Set the current locale.
	setlocale(LC_ALL, "");
	// Set the text message domain.
	bindtextdomain("wc3converter", LOCALE_DIR);
	textdomain("wc3converter");

	static struct option options[] =
	{
		{"version",     no_argument,             0, 'v'},
		{"help",        no_argument,             0, 'h'},
		{"formats",     no_argument,             0, 'f'},
		{"iformat",     required_argument,       0, 'i'},
		{"oformat",     required_argument,       0, 'o'},
		{"dir",         required_argument,       0, 'd'},
		{"recursive",   no_argument,             0, 'R'},
		{"verbose",     no_argument,             0, 'V'}, /// @todo Probably reserved for --version, too
		{"readonly",    no_argument,             0, 'O'},
		{"quiet",       no_argument,             0, 'Q'},
		{0, 0, 0, 0}
	};

	bool optionFormats = false;
	Format optionIformat = InvalidFormat;
	Format optionOformat = InvalidFormat;
	boost::filesystem::path optionDir;
	bool optionRecursive = false;
	bool optionVerbose = false;
	bool optionReadonly = false;
	bool optionQuiet = false;
	std::list<boost::filesystem::path> optionFiles;
	int optionShortcut;

	while (true)
	{
		int optionIndex = 0;
		optionShortcut = getopt_long(argc, argv, "vhfi:o:d:RVOQ", options, &optionIndex);

		if (optionShortcut == -1)
			break;

		switch (optionShortcut)
		{
			case 'v':
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

			case 'h':
			{
				std::cout <<
				_("converter\n") <<
				_("\nUsage:\n") <<
				_("\tconverter [-vhioRV] <input files/directories>\n") <<
				_("\nOptions:\n") <<
				_("\t-v, --version             Shows the current version of mdlxtest.\n") <<
				_("\t-f, --formats             Lists all possible input and output formats.\n") <<
				("\t-h, --help                 Shows this text.\n") <<
				_("\t-i, --iformat <arg>       <arg> has to be replaced by input files format.\n") <<
				_("\t-o, --oformat <arg>       <arg> has to be replaced by output files format.\n") <<
				_("\t-d, --dir <arg>           <arg> has to be replaced by output directory.\n") <<
				_("\t-R, --recursive           If some of the input files are directories they will be iterated recursively and searched for other files with the input format extension.\n") <<
				_("\t-V, --verbose             Enables verbose mode and shows more detailed output information.\n") <<
				_("\t-O, --readonly            Enables read-only mode. Output format is not necessary in read-only mode. This mode can be useful if you just want to try some format reading.\n") <<
				_("\t-Q, --quiet               Disables interactive mode. You won't be asked anything while the program runs.\n") <<
				_("\nReport bugs to tamino@cdauth.eu or on https://wc3lib.org") <<
				std::endl;

				return EXIT_SUCCESS;
			}

			case 'f':
			{
				optionFormats = true;

				break;
			}

			case 'i':
			{
				if (getFormatByExpression(optarg) == InvalidFormat)
				{
					std::cerr << boost::format(_("Invalid format \"%1%\".")) % optarg << std::endl;

					return EXIT_FAILURE;
				}

				optionIformat = getFormatByExpression(optarg);

				break;
			}

			case 'o':
			{
				if (getFormatByExpression(optarg) == InvalidFormat)
				{
					std::cerr << boost::format(_("Invalid format \"%1%\".")) % optarg << std::endl;

					return EXIT_FAILURE;
				}

				optionOformat = getFormatByExpression(optarg);

				break;
			}

			case 'd':
			{
				if (!boost::filesystem::is_directory(optarg))
				{
					std::cerr << boost::format(_("Invalid directory \"%1%\".")) % optarg << std::endl;

					return EXIT_FAILURE;
				}

				optionDir = optarg;

				break;
			}

			case 'R':
			{
				optionRecursive = true;

				break;
			}

			case 'V':
			{
				optionVerbose = true;

				break;
			}

			case 'O':
			{
				optionReadonly = true;

				break;
			}

			case 'Q':
			{
				optionQuiet = true;

				break;
			}
		}
	}

	if (optind < argc)
	{
		while (optind < argc)
		{
			boost::filesystem::path path = argv[optind++];

			if (!addFilePath(path, optionFiles, optionRecursive, getFormatExtension(optionIformat), optionQuiet))
			{
				std::cerr << _("Canceled process.") << std::endl;

				return EXIT_FAILURE;
			}
		}

	}
	else if (!optionFormats)
	{
		std::cerr << _("Missing file arguments.") << std::endl;

		return EXIT_FAILURE;
	}

	if (optionFormats)
	{
		std::cout << boost::format(_("Listing available formats (%1%):")) % MaxFormats << std::endl;

		for (std::size_t i = 0; i < MaxFormats; ++i)
			std::cout << boost::format(_("* \"%1%\" - %2%")) % formatExpression[i] % formatDescription[i] << std::endl;

		if (optionFiles.empty())
			return EXIT_SUCCESS;
	}

	if (optionIformat == InvalidFormat)
	{
		std::cerr << _("Input format was not defined.") << std::endl;

		return EXIT_FAILURE;
	}

	if (optionOformat == InvalidFormat && !optionReadonly)
	{
		std::cerr << _("Output format was not defined.") << std::endl;

		return EXIT_FAILURE;
	}

	if (!optionReadonly && !checkFormatConvertibility(optionIformat, optionOformat))
	{
		std::cerr << boost::format(_("Format \"%1%\" can not be converted into format \"%2%\".")) % getFormatExpression(optionIformat) % getFormatExpression(optionOformat) << std::endl;

		return EXIT_FAILURE;
	}

	if (optionFiles.empty())
	{
		std::cerr << _("No input files were chosen.") << std::endl;

		return EXIT_FAILURE;
	}


	/// @todo If it's multithreaded make sure that there aren't doubled file paths
	BOOST_FOREACH(boost::filesystem::path path, optionFiles)
	{
		std::cout << "Loop" << std::endl;

		try
		{
			convertFile(path, optionDir, optionIformat, optionOformat, optionVerbose, optionReadonly);
		}
		catch (const class wc3lib::Exception &exception)
		{
			std::cerr << boost::format(_("Error occured when converting file \"%1%\": \"%2%\".\nSkipping file.")) % path % exception.what() << std::endl;

			continue;
		}

		/*
		try
		{
			boost::thread thread = boost::thread(convertFile, path, optionDir, optionIformat, optionOformat, optionVerbose, optionReadonly);
		}
		catch (boost::thread_interrupted &threadInterrupted)
		{

		}
		catch (...)
		{
			std::cerr << _("Unknown exception. Canceling.") << std::endl;

			break;
		}
		*/
	}

	return EXIT_SUCCESS;
}
