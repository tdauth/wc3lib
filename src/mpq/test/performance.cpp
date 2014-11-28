#define BOOST_TEST_MODULE PerformanceTest
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>
#include <chrono>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

/*
 * This test uses the StormLib to compare performances.
 */
#include <StormLib.h>
#include <StormPort.h>

#include "../archive.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

#ifndef WC3_DIR
#error Define WC3_DIR.
#endif

using namespace wc3lib;

/*
 * Reads all files from war3.mpq.
 *
 * This test compares the file read speed of wc3lib and StormLib.
 * Note that StormLib provides more efficient ways of reading the whole archive and iterating through all files than opening each file by its name.
 * Since the listfile entry extraction is excluded from the timer only the iterating blocks next to each other without massive seeking in the file would be much faster in StormLib than using this solution.
 */
BOOST_AUTO_TEST_CASE(ReadWar3)
{
	/*
	 * Copy original war3.mpq.
	 * Since the original war3.mpq may not be distributed due to licensing, this test can only be run if Warcraft III: Reign of Chaos is installed.
	 * The installation directory must be specified in "WC3_DIR" which is an environment variable.
	 */
	boost::filesystem::path wc3DirPath = WC3_DIR;
	BOOST_REQUIRE(boost::filesystem::exists(wc3DirPath));
	BOOST_REQUIRE(boost::filesystem::is_directory(wc3DirPath));

	const boost::filesystem::path war3Path = wc3DirPath / "war3.mpq";

	BOOST_REQUIRE(boost::filesystem::exists(war3Path));

	const  boost::filesystem::path workingDir = boost::filesystem::current_path();

	BOOST_REQUIRE(boost::filesystem::exists(workingDir));
	BOOST_REQUIRE(boost::filesystem::is_directory(workingDir));

	const  boost::filesystem::path target = workingDir / "war3.mpq";

	/*
	 * Overwrite old copy to get a fresh unmodified file.
	 */
	if (boost::filesystem::exists(target))
	{
		BOOST_REQUIRE(boost::filesystem::is_regular_file(target));
		BOOST_REQUIRE(boost::filesystem::remove(target));
	}

	std::cerr << "Copying " << war3Path << " to " << target << std::endl;

	boost::system::error_code ec;
	boost::filesystem::copy(war3Path, target, ec);

	std::cerr << "Error code: " << ec.message() << std::endl;

	BOOST_REQUIRE(!ec);

	mpq::Archive archive;
	archive.open(target);

	BOOST_REQUIRE(archive.isOpen());
	BOOST_REQUIRE(archive.containsListfileFile());

	stringstream tmpStream0;
	const mpq::Listfile::Entries entries = archive.listfileFile().entries();
	std::size_t count0 = 0;
	const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

	/*
	for (mpq::Listfile::Entries::const_iterator iterator = entries.begin(); iterator != entries.end(); ++iterator)
	{
		if (!iterator->empty())
		{
			mpq::File file = archive.findFile(*iterator);

			std::cerr << "File: " << *iterator << std::endl;

			BOOST_REQUIRE(file.isValid());

			file.writeData(tmpStream0);

			++count0;
		}
	}
	*/

	const std::chrono::system_clock::time_point later = std::chrono::system_clock::now();
	const std::chrono::system_clock::duration duration = later - now;

	archive.close();

	std::cerr << "Duration wc3lib: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms" << std::endl;

	// TODO test with StormLib to compare results
	HANDLE stormArchive;
	BOOST_REQUIRE(SFileOpenArchive(target.c_str(), 0, MPQ_FLAG_READ_ONLY, &stormArchive));
	BOOST_REQUIRE(SFileCloseArchive(stormArchive));
	std::size_t count1 = 0;
	stringstream tmpStream1;
	const std::chrono::system_clock::time_point now1 = std::chrono::system_clock::now();

	for (mpq::Listfile::Entries::const_iterator iterator = entries.begin(); iterator != entries.end(); ++iterator)
	{
		if (!iterator->empty())
		{
			if (SFileHasFile(stormArchive, iterator->c_str()))
			{
				std::cerr << "File: " << *iterator << std::endl;

				HANDLE file;
				BOOST_REQUIRE(SFileOpenFileEx(stormArchive, iterator->c_str(), 0, &file));
				BOOST_REQUIRE(file != 0);
				DWORD fileSize = 0;
				DWORD fileSizeHigh = 0;
				fileSize = SFileGetFileSize(file, &fileSizeHigh);

				std::cerr << "File size: " << fileSize << " file size high: " << fileSizeHigh << std::endl;

				BOOST_REQUIRE(fileSize > 0);

				void *buffer = 0;
				void *overlapped = 0;
				BOOST_REQUIRE(SFileReadFile(file, buffer, fileSize, &fileSizeHigh, overlapped));
				BOOST_REQUIRE(buffer != 0);

				tmpStream1.write(reinterpret_cast<const byte*>(buffer), fileSize);
				// TODO write file size high and overlapped buffer.

				delete buffer;

				if (overlapped != 0)
				{
					delete overlapped;
				}

				BOOST_REQUIRE(SFileCloseFile(file));

				++count1;
			}
		}
	}

	const std::chrono::system_clock::time_point later1 = std::chrono::system_clock::now();
	const std::chrono::system_clock::duration duration1 = later1 - now1;

	std::cerr << "Duration StormLib: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration1).count() << " ms" << std::endl;

	BOOST_REQUIRE(count0 == count1);

}
