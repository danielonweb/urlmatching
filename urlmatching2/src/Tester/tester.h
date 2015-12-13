/*
 * tester.h
 *
 *  Created on: 18 December 2014
 *      Author: Daniel Krauthgamer
 *
 *  Tester implementation for the module
 */

#ifndef TESTER_H_
#define TESTER_H_

#ifdef _WIN32
    #include <direct.h>
	#include <windows.h>
    #define GetCurrentDir _getcwd
	#define sleep(n) Sleep(1000 * n)
#elif defined  __unix__
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#include "cmd_line_options.h"

#define MAX_PATH_2 1000

//Helpers
inline
void getWorkingDirectory(std::string& out ) {
	char pBuf[MAX_PATH_2];
	char* p = GetCurrentDir(pBuf, MAX_PATH_2);
	out.assign(p);
}


#define GETTIMING double(end - begin) / (CLOCKS_PER_SEC)
#define START_TIMING 	do {begin = std::clock();} while(0)
#define STOP_TIMING 	do {end = std::clock();} while(0)
#define PREPARE_TIMING clock_t begin,end


#define RUN_TEST(name) PREPARE_TIMING; \
	std::cout<<"TEST \""<<#name<<"\" STARTED now" << std::endl; \
	START_TIMING ; \
	name(); \
	STOP_TIMING; \
	double test_time = GETTIMING; \
	std::cout<<"TEST \""<<#name<<"\" ENDED, took " << test_time / 60 << " min."<< std::endl;


class UrlMatchingModule;


void run_cmd_main(CmdLineOptions& options);

//cmds:
void test_main						(CmdLineOptions& options);
void test_build_dictionary_to_file	(CmdLineOptions& options);
void test_encode					(CmdLineOptions& options);
void test_compress					(CmdLineOptions& options);
void test_extract					(CmdLineOptions& options);
void test_hashtable					(CmdLineOptions& options);
void test_article					(CmdLineOptions& options);


//The cmd tester (Main tester)
void take_a_break(int seconds, std::string why);
bool sanityTesting(UrlMatchingModule& urlc , bool verbose = false);

//Other testers (need to set manually in main.cpp)
void test_LLH();
void test_url_dictionary_load_from_url_txt_file();




#endif /* TESTER_H_ */