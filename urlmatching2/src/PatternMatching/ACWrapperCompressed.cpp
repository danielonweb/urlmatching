/*
 * ACWrapperCompressed.cpp
 *
 *  Created on: 7 ���� 2014
 *      Author: Daniel
 */


/*
 * AhoCrosarikClassic.cpp
 *
 *  Created on: 14 ���� 2014
 *      Author: Daniel
 */

#include "../PatternMatching/ACWrapperCompressed.h"
#include <string.h>
#include <assert.h>
#include <map>
#include "../UrlDictionay.h"
#include "MatchingFunctions.h"
#include "../UrlDictionaryTypes.h"

extern "C" {
#include "../Common/Types.h"
}


//#define DBG(__str__) std::cout<<__str__


//uint32_t getStringFromMap(char* out_buff, uint32_t max_size) {
//	ACWrapperClassic ac = ACWrapperClassic::getInstance();
//	std::string* str= ac.nextPattern();
//	if (str==NULL)
//		return 0;
//	const char* chars= str->c_str();
//	strncpy(out_buff,chars,max_size);
//	return strlen(chars);
//}
//
//uint32_t getStringFromList(char* out_buff, uint32_t max_size, void* list_struct) {
//	stringlistType* list_describtor = (stringlistType*) list_struct;
//	if (list_describtor->index == list_describtor->size)
//		return 0;
//	const char* chars= list_describtor->list[list_describtor->index]->c_str();
//	list_describtor->index = list_describtor->index+1;
//	strncpy(out_buff,chars,max_size);
//	return strlen(chars);
//}

int special_handle_pattern(char* str,uint32_t idx, void* data) {
	urlMatchingType* url_module = (urlMatchingType*) data;
	assert(url_module!=NULL);

	DBG("at idx="<<idx<<", pattern="<<str);
	//update module with the pattern
	return 1;
}



ACWrapperCompressed::ACWrapperCompressed() :
		is_loaded(false),
		_patternsList(NULL),
		_patternsMap(NULL),
		_machine(NULL)
{
	_symbolsTable.table=NULL;
	_symbolsTable.size=0;
	for (int i=0; i < MAX_CHAR; i++) {
		_char_to_symbol[i]=S_NULL;
	}
}

ACWrapperCompressed::~ACWrapperCompressed() {
	if (_machine != NULL) {
		destroyStateMachine(_machine);
		_machine = NULL;
	}
	if (_patternsMap!=NULL) {
		delete _patternsMap;
		_patternsMap = NULL;
	}

	for (uint32_t i = 0 ; i < _symbolsTable.size; i++ ) {
		if (_symbolsTable.table[i] == NULL)
			continue;
		uint32_t counter = 0;
		for (counter = 0; _symbolsTable.table[i][counter] != NULL; counter++) {
			delete[] _symbolsTable.table[i][counter];
		}
		delete[] _symbolsTable.table[i];
	}
	if (_symbolsTable.table != NULL )
		delete[] _symbolsTable.table;

}


bool ACWrapperCompressed::load_patterns(std::string filepath) {
	assert(false); // not supported
	const char* tmp = filepath.c_str();
	_machine = createStateMachine(tmp,100,100,0);
	make_pattern_to_symbol_list();
	is_loaded = true;
	return true;

}

bool ACWrapperCompressed::load_patterns(Symbol2pPatternVec* patternsList, uint32_t patternsList_size) {
	assert(!isLoaded());
	//make a copy for Symbol2PatternType list
	_patternsList = patternsList;
	//convert Symbol2PatternType to StringListType
	StringListType list = new std::string*[patternsList_size];
	uint32_t idx=0;				//store how many string we entered the list
	_patternsMap = new patternsMapType();

	for (symbolT i=1; i< patternsList_size; i++) {	// 0 is reserved pattern as NULL
		//go over all patterns, if pattern is single char - set it into _char_to_symbol array
		// if pattern is longer than 1 char - add it to patterns list so AC will load them
		if (strlen ( (*patternsList)[i]->_str.c_str() ) == 1) {
			const uint32_t c = (uint32_t) (*patternsList)[i]->_str.at(0);
			_char_to_symbol[c] = i;
		} else {
			list[idx]=&((*patternsList)[i]->_str);
			const char* s = (*patternsList)[i]->_str.c_str();
			_patternsMap->insert(std::make_pair(s,i));
			idx++;
		}
	}

	DBG("--- START printing all patterns: ---");
	StringListDBWithIdxType db={list,0,idx};
	for (uint32_t i=0; i < db.size; i++ ){
		DBG(DVAL(i)<<": "<<*(db.list[i]));
	}
	DBG("--- FINISH printing all patterns: ---");
	_machine = createStateMachineFunc(getStringFromList,&db,1000,1000,0);
	_machine->handlePatternFunc = handle_pattern;

	delete[] list;

//	make_pattern_to_symbol_list();
	is_loaded = true;
	return true;
}

inline
symbolT* ACWrapperCompressed::create_symb_string (const char* c_string) {
	const char* delimiter = ";";

	const char* c = c_string;
	uint32_t length = 2;	//symb_string = { symb, S_NULL } i.e length of 2
	while (*c != '\0') {
		if (*c == *delimiter) {
			length++;
		}
		c++;
	}
	symbolT* symb_string = new symbolT[length];
	assert(symb_string != NULL) ;
	symb_string[length-1] = S_NULL;

	char buffer[300];
	char* cpy =buffer;
	bool to_delete = false;
	if (strlen(c_string) >= 300) {
		cpy = new char[strlen(c_string)];
		to_delete = true;
	}
	strcpy(cpy,c_string);
	char* tk = strtok(cpy, delimiter);
	uint32_t counter = 0;
	while (tk != NULL) {
		patternsMapType::iterator itr = _patternsMap->find(tk);
		if (itr == _patternsMap->end()) {
			ON_DEBUG_ONLY(std::cout<<"error not found \"" << DVAL(tk) << "\" from \"" << DVAL (c_string)<< "\" " << DVAL (counter)<<STDENDL);
			ASSERT(itr == _patternsMap->end());
		}
		symb_string[counter] = itr->second;

		ASSERT(strcmp ( (*_patternsList)[itr->second]->_str.c_str(), tk) == 0 );

		counter++;
		tk = strtok(NULL, delimiter);
	}
	if ((length-1) != counter) {	//lenght includes the S_NULL
		ON_DEBUG_ONLY(std::cout<<"error lenght "<<DVAL(length)<<" "<<DVAL(counter)<<STDENDL);
		ASSERT((length-1) != counter);
	}

	if (to_delete) {
		delete[] cpy;
	}
	return symb_string;
}

/* use this function to build a complimentary patterns table for symbols*/
void ACWrapperCompressed::make_pattern_to_symbol_list() {
	char ***patterns;
	patterns=_machine->patternTable->patterns;

	uint32_t size =_machine->patternTable->size;

	//patterns_as_symbols is an array of array of "symbolT string" i.e S_NULL terminated
	// patterns_as_symbols[i] is an array of dynamic size where the last pointer is NULL
	// 	i.e patterns_as_symbols[i] 		= {symbolT*,symbolT*,..,NULL}
	//		patterns_as_symbols [i][j] 	= {symbol1,symbol2,..,S_NULL}
	symbolT*** symbolsTable = new symbolT**[size];
	for (uint32_t i = 0; i < size; i++ ) {
		symbolsTable[i]=NULL;
	}

	std::cout<<"Print cached patterns table of size "<< size<<std::endl;
	for (uint32_t i = 0; i < size; i++) {
		if (patterns[i] == NULL) {
			symbolsTable[i]=NULL;
			ON_DEBUG_ONLY(std::cout<<"patterns["<<i<<"]=NULL"<<std::endl);
			continue;
		}
		uint32_t num_of_j = 0;	//location of the terminating NULL
		//count patterns
		char* pp =patterns[i][num_of_j];
		while ( pp != NULL) {		// [0,1,2,3,NULL] j=4 is null, j returns with 5
			num_of_j++; //[11]
			pp= patterns[i][num_of_j];
		}
		symbolsTable[i] = new symbolT*[num_of_j+1];
		symbolsTable[i][num_of_j] = NULL;

		if (num_of_j == 0) {
			ON_DEBUG_ONLY(std::cout<<"patterns["<<i<<"][0]=EMPTY"<<std::endl);
			continue;
		}

		//assign symbols
		for (uint32_t j=0;j<num_of_j;j++) {
			symbolT* symb_string = create_symb_string (patterns[i][j]);
#if BUILD_DEBUG
			std::cout<< "patterns["<<i<<"]["<<j<<"  /"<<num_of_j<<"]=" << patterns[i][j] << "->";
			for(symbolT* sym = symb_string; *sym != S_NULL; sym++) {
				std::cout<< *sym<< ":"<<(*_patternsList)[*sym]->_str <<"; ";
			}
			std::cout<<std::endl;
#endif
			symbolsTable[i][j] = symb_string;
		}
	}
	_symbolsTable.table = symbolsTable;
	_symbolsTable.size = size;
	ON_DEBUG_ONLY(std::cout<<"FINISHED Print cached patterns table"<<std::endl);
}

bool ACWrapperCompressed::find_patterns(std::string input_str, symbolT* result) {
	assert(isLoaded());

	if ( (input_str.length() == 0 )||
			(input_str == "") ){
		result[0]=S_NULL;
		return true;
	}
	//prepare metadata
	urlMatchingType module;
	initModule(module);
	module.patternDB = _patternsMap;
	module.symbolsTable = &_symbolsTable;

	module.char_to_symbol = _char_to_symbol;
	module.list = _patternsList;

	const char* str = input_str.c_str();

	//TODO: insert the following into input string to init
	module.input_string = (char *) str;
	uint32_t first_char_of_string =  str[0];
	symbolT symbol_of_first_char = _char_to_symbol[first_char_of_string];
	module.matching_symbols_arr[module.matching_symbols_idx] = symbol_of_first_char;
	module.matching_symbols_idx++;

	MachineStats stats;
	_machine->handlePatternFunc = handle_pattern;
	_machine->handlePatternData = &module;
	//to support multithreaded - make a copy of _machine and set its handlePatternData member
	match(_machine,/*(char *)*/ str, strlen(str)
			, 1 /* verbose to make matching call handlePatternFunc when a pattern is found*/
			, &stats);
	finalize_result(module,result);

	return true;
}

void ACWrapperCompressed::printDB(std::ostream& os) {
	os<<"ACWrapperCompressed::printDB\n";
	return;
}
