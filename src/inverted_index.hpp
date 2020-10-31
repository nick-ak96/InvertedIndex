//
//  inverted_index.hpp
//  inverted_index
//
//  Created by Mikita Akulich on 15/03/2018.
//  Copyright © 2019 Mikita Akulich. All rights reserved.
//

#ifndef inverted_index_hpp
#define inverted_index_hpp

#include <unordered_map>
#include <vector>
#include <string>
#include <bitset>

const int ALPHABET_SIZE = 300;

class Posting {
public:
	std::bitset<ALPHABET_SIZE> data;
};

class InvertedIndex {
private:
	std::unordered_map<int, std::vector<Posting>> index;
	
	void add_posting(const Posting *posting);
	
	bool subset_exists(const std::bitset<ALPHABET_SIZE> &test_set);
	size_t subset_get_all(const std::bitset<ALPHABET_SIZE> &test_set);
	bool superset_exists(const std::bitset<ALPHABET_SIZE> &test_set);
	size_t superset_get_all(const std::bitset<ALPHABET_SIZE> &test_set);
	bool exists(const std::bitset<ALPHABET_SIZE> &test_set);
public:
	void insert_set(const std::string &set_string);
    void sort_postings();
	std::string run_test(const std::string &set_string, const std::string &test_mode);
};


#endif /* inverted_index_hpp */
