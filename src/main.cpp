//
//  main.cpp
//  inverted_index
//
//  Created by Mikita Akulich on 15/03/2018.
//  Copyright © 2019 Mikita Akulich. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "inverted_index.hpp"


int main(int argc, const char * argv[]) {
	// check if test file is specified
	if (argc != 3) {
		std::cerr<<"Test file or benchmark mode is not specified"<<std::endl;
		return EXIT_FAILURE;
	}
	// check the mode
	std::string test_mode = argv[2];
	if (test_mode.compare("sb") != 0 &&
			test_mode.compare("asb") != 0 &&
			test_mode.compare("eq") != 0 &&
			test_mode.compare("sp") != 0 &&
			test_mode.compare("asp") != 0) {
		std::cerr<<"Invalid test mode. Supported modes: sb, eq, sp"<<std::endl;
		return EXIT_FAILURE;
	}
	
	std::unique_ptr<InvertedIndex> index = std::make_unique<InvertedIndex>();
	// fill index from stdin
	try {
		std::string input_set;
		while (std::getline(std::cin, input_set)) {
			index->insert_set(input_set);
		}
        index->sort_postings();
	} catch (...) {
		return EXIT_FAILURE;
	}
	
	// benchmark
	
	// sb - subset
	// eq - equal
	// sp - superset
	std::ifstream test_file;
	try {
		test_file.open(argv[1]);
		if (!test_file.is_open()) {
			std::cerr<<"Error opening test file";
			throw;
		}
		std::string test_set;
		int line_cnt = 1;
		while (std::getline(test_file, test_set)) {
			try {
				// write result to stdout
				std::cout<<index->run_test(test_set, test_mode)<<std::endl;
				line_cnt++;
			} catch (...) {
				std::cerr<<"Error running test at line "<<line_cnt<<std::endl;
				throw;
			}
		}
		test_file.close();
	} catch (...) {
		if (test_file.is_open()) {
			test_file.close();
		}
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
