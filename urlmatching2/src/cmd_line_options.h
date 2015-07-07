/* 
 * File:   cmd_line_parser.h
 * Author: golanp
 *
 * Created on December 7, 2013, 4:46 PM
 * Edited by Michal
 * Added the option of string use.
 */

#ifndef CMD_LINE_PARSER_H
#define	CMD_LINE_PARSER_H

#include <string>
#include <iostream>
#include <fstream>


#define CMD_FULLTEST        "test"
#define CMD_BUILDDIC        "build"
#define CMD_ENCODE          "encode"
#define CMD_HASHTABLE 		"testhash"
#define CMD_COMPRESS        "compress"
#define CMD_EXTRACT         "extract"


class CmdLineOptions {

public:
	CmdLineOptions(int argc, char* argv[]);
	void PrintParameters(std::ostream& log);
	void usage();

	std::string getDictionaryFilename() ; //return the dictionary filename that should be used


private:
	void _parse_command_line(int argc, char* argv[]);

public:

	std::string cmd;
	std::string input_urls_file_path;
	bool		use_dictionary_file;
	std::string dictionary_file;
	bool		add_header_to_output_file;
	bool 		custom_output_file;
	std::string output_file_path;
	std::string logger_config;
	size_t      kgram_size;
	int         n1;
	int         n2;
	float       r;
	bool		test_decoding;	//LPM=Longest Prefix Match
	bool		split_for_LPM;	//LPM=Longest Prefix Match
	bool		print_dicionary;
	std::string print_dicionary_file;
	int			break_time;

	char        line_del;

};

#endif	/* CMD_LINE_PARSER_H */

