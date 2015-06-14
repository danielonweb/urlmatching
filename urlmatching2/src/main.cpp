/*
 * main.cpp
 *
 *  Created on: 1 2014
 *      Author: Daniel
 */

#include <cstring>
#include <iostream>
#include <map>
#include <string>

#include "Huffman.h"
#include "PatternMatching/ACWrapperClassic.h"
#include "tester.h"
#include "UrlDictionay.h"
#include "cmd_line_options.h"
#include "easylogging++.h"


#ifdef BUILD_DEBUG
INITIALIZE_EASYLOGGINGPP
#endif


int main(int argc, char* argv[])
{

	std::string path;
	getWorkingDirectory(path);
	std::cout<<"running from path="<<path<<std::endl;

    CmdLineOptions options(argc, argv);
    if (options.logger_config == "") {
    	options.logger_config = "src/easylogging.conf";
    }

#ifdef BUILD_DEBUG
	// Load configuration from file
	std::string logger_config_file(options.logger_config.c_str());
	logger_config_file = path + "/" + logger_config_file;
    el::Configurations conf(logger_config_file.c_str());
    // Reconfigure single logger
//    el::Loggers::reconfigureLogger("default", conf);
    // Actually reconfigure all loggers instead
    el::Loggers::reconfigureAllLoggers(conf);

    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
    // Now all the loggers will use configuration from file
#endif


//	RUN_TEST(test_aho_compressed);
//	RUN_TEST(test_url_matching);
//	RUN_TEST(test_url_dictionary);
//	RUN_TEST(test_LLH);
//	RUN_TEST(test_url_dictionary_load_from_url_txt_file);
    clock_t begin,end;
    begin = std::clock(); ;
	test_main(options);
	end = std::clock();;
	double test_time = double(end - begin) / (CLOCKS_PER_SEC);
	std::cout<<"TEST ENDED, took " << test_time / 60 << " min."<< std::endl;

	return 0;
}
