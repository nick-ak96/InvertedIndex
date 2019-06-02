//
//  inverted_index.cpp
//  inverted_index
//
//  Created by Mikita Akulich on 15/03/2018.
//  Copyright © 2018 Mikita Akulich. All rights reserved.
//

#include "inverted_index.hpp"
#include <sstream>
#include <chrono>
#include <algorithm>


bool posting_comparator(const Posting &p1, const Posting &p2) {
	return p1.data.to_ullong() < p2.data.to_ullong();
}

void InvertedIndex::add_posting(const Posting *posting) {
	// iterate through each element in the posting (set)
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (posting->data[i]) {
			if (this->index.find(i) != this->index.end()) {
				this->index.at(i).push_back(*posting);
			}
			else {
				std::vector<Posting> postings = { *posting };
				this->index.insert(std::make_pair(i, postings));
			}
			sort(std::begin(this->index.at(i)), std::end(this->index.at(i)), posting_comparator);
		}
	}
}


std::bitset<ALPHABET_SIZE>* create_set_from_string(const std::string &set_string) {
	std::bitset<ALPHABET_SIZE>* set = new std::bitset<ALPHABET_SIZE>();
	char delimiter = ',';
	std::string el;
	std::istringstream stream(set_string);
	while (std::getline(stream, el, delimiter)) {
		set->set(std::stoi(el));
	}
	return set;
}


Posting* create_posting(const std::string &set_string) {
	Posting* posting = new Posting();
	auto set = create_set_from_string(set_string);
	posting->data = *set;
	return posting;
}


void InvertedIndex::insert_set(const std::string &set_string) {
	auto posting = create_posting(set_string);
	this->add_posting(posting);
}


// ---------------------------------------------------------------------------
// BENCHMARK
// ---------------------------------------------------------------------------

std::vector<Posting>* intersect_postings(const std::vector<std::vector<Posting>*> &postings, const size_t &position) {
	auto smallest_posting_list = postings.at(position);
	std::vector<Posting> intersection = std::vector<Posting>();
	for (int i = 0; i < postings.size(); i++) {
		intersection.clear();
		
		std::set_intersection(smallest_posting_list->begin(), smallest_posting_list->end(), postings.at(i)->begin(), postings.at(i)->end(), std::back_inserter(intersection), posting_comparator);
		
		if (intersection.size() == 0)
			break;
		else if (intersection.size() > 0 && intersection.size() < smallest_posting_list->size()) {
			smallest_posting_list = new std::vector<Posting>(intersection);
		}
	}
	return new std::vector<Posting>(intersection);
}


bool InvertedIndex::subset_exists(const std::bitset<ALPHABET_SIZE> &test_set) {
	std::unordered_map<std::bitset<ALPHABET_SIZE>, int> hasher = std::unordered_map<std::bitset<ALPHABET_SIZE>, int>();
	std::unordered_map<int, std::vector<Posting>>::iterator it;
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (test_set[i]) {
			it = this->index.find(i);
			if (it == index.end()) {
				continue; // some elements of the query set can be skiped, since we search for subsets
			}
			for (auto &el : it->second) {
				auto hasher_it = hasher.find(el.data);
				if (hasher_it != hasher.end()) {
					hasher_it->second++;
				}
				else {
					hasher.insert(std::make_pair(el.data, 1));
				}
			}
		}
	}
	for (auto &el : hasher) {
		if (el.first.count() == el.second)
			return true;
	}
	return false;
}


bool InvertedIndex::superset_exists(const std::bitset<ALPHABET_SIZE> &test_set) {
	// get lists of postings for each value in the set
	std::vector<std::vector<Posting>*> temp = std::vector<std::vector<Posting>*>();
	std::unordered_map<int, std::vector<Posting>>::iterator it;
	size_t min_posting_size = INT_MAX,
		min_posting_position = 0;
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (test_set[i]) {
			it = this->index.find(i);
			if (it == index.end()) {
				return false;
			}
			temp.push_back(&(it->second));
			if (it->second.size() < min_posting_size) {
				min_posting_size = it->second.size();
				min_posting_position = temp.size() - 1;
			}
		}
	}
	std::vector<Posting>* intersection = intersect_postings(temp, min_posting_position);
	return intersection->size() > 0;
}


bool InvertedIndex::exists(const std::bitset<ALPHABET_SIZE> &test_set) {
	// get lists of postings for each value in the set
	std::vector<std::vector<Posting>*> temp = std::vector<std::vector<Posting>*>();
	std::unordered_map<int, std::vector<Posting>>::iterator it;
	size_t min_posting_size = INT_MAX,
	min_posting_position = 0;
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (test_set[i]) {
			it = this->index.find(i);
			if (it == index.end()) {
				return false;
			}
			temp.push_back(&(it->second));
			if (it->second.size() < min_posting_size) {
				min_posting_size = it->second.size();
				min_posting_position = temp.size() - 1;
			}
		}
	}
	for (auto &el : *temp.at(min_posting_position)) {
		if (el.data == test_set)
			return true;
	}
	return false;
}


template<typename TUnits>
long calculate_time(const std::chrono::system_clock::time_point &start, const std::chrono::system_clock::time_point &stop) {
	return std::chrono::duration_cast<TUnits>(stop - start).count();
}


std::string InvertedIndex::run_test(const std::string &set_string, const std::string &test_mode) {
	bool result;
	auto test_set = create_set_from_string(set_string);
	std::chrono::system_clock::time_point start, stop;
	
	if (test_mode.compare("sb") == 0) {
		start = std::chrono::system_clock::now();
		result = subset_exists(*test_set);
		stop = std::chrono::system_clock::now();
	}
	else if (test_mode.compare("sp") == 0) {
		start = std::chrono::system_clock::now();
		result = superset_exists(*test_set);
		stop = std::chrono::system_clock::now();
	}
	else {
		start = std::chrono::system_clock::now();
		result = exists(*test_set);
		stop = std::chrono::system_clock::now();
	}
	return
		set_string + ";" +
		std::to_string(result) + ";" +
		std::to_string(calculate_time<std::chrono::nanoseconds>(start, stop));
}