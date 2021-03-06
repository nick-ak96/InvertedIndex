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
#include <limits>
#include <time.h>


bool posting_comparator(const Posting &p1, const Posting &p2) {
    for (int i = ALPHABET_SIZE - 1; i >= 0; i--) {
        if (p1.data[i] ^ p2.data[i])
            return p2.data[i];
    }
    return false;
	//return p1.data.to_ullong() < p2.data.to_ullong();
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
		}
	}
}


std::bitset<ALPHABET_SIZE>* create_set_from_string(const std::string &set_string) {
	std::bitset<ALPHABET_SIZE>* set = new std::bitset<ALPHABET_SIZE>();
	char delimiter = ',';
	std::string el;
	std::istringstream stream(set_string);
	while (std::getline(stream, el, delimiter)) {
		set->set(std::stoi(el) - 1);
	}
	return set;
}

std::string create_string_from_set(const std::bitset<ALPHABET_SIZE> &set) {
	std::string result;
	for (size_t i = 0; i < set.size(); i++) {
		if (!set.test(i))
			continue;
		result += std::to_string(i) + ",";
	}
	result.pop_back();
	return result;
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

void InvertedIndex::sort_postings() {
    std::unordered_map<int, std::vector<Posting>>::iterator it;
    for (it = this->index.begin(); it != this->index.end(); it++){
        sort(it->second.begin(), it->second.end(), posting_comparator);
    }
}


// ---------------------------------------------------------------------------
// Main functions
// ---------------------------------------------------------------------------

// helper function
size_t intersect_postings(const std::vector<std::vector<Posting>*> &postings, const size_t &position) {
	std::vector<Posting>* smallest_posting_list = postings.at(position);
	std::vector<Posting> intersection = std::vector<Posting>();
	for (int i = 0; i < postings.size(); i++) {
		intersection.clear();
		
		std::set_intersection(smallest_posting_list->begin(), smallest_posting_list->end(), postings.at(i)->begin(), postings.at(i)->end(), std::back_inserter(intersection), posting_comparator);
		
		if (intersection.size() == 0)
			break;
		else if (intersection.size() > 0 && intersection.size() < smallest_posting_list->size()) {
			std::vector<Posting> temp = std::vector<Posting>(intersection);
			smallest_posting_list = &temp;
		}
	}
	return intersection.size();
}


/* Existence of subset */
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


/* Get all subsets */
size_t InvertedIndex::subset_get_all(const std::bitset<ALPHABET_SIZE> &test_set) {
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
	size_t result = 0;
	for (auto &el : hasher) {
		if (el.first.count() == el.second)
			result++;
	}
	return result;
}


/* Existence of supersets */
bool InvertedIndex::superset_exists(const std::bitset<ALPHABET_SIZE> &test_set) {
	// get lists of postings for each value in the set
	std::vector<std::vector<Posting>*> temp = std::vector<std::vector<Posting>*>();
	std::unordered_map<int, std::vector<Posting>>::iterator it;
	size_t min_posting_size = std::numeric_limits<int>::max(),
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
	return intersect_postings(temp, min_posting_position) > 0;
}


/* Get all supersets */
size_t InvertedIndex::superset_get_all(const std::bitset<ALPHABET_SIZE> &test_set) {
	// get lists of postings for each value in the set
	std::vector<std::vector<Posting>*> temp = std::vector<std::vector<Posting>*>();
	std::unordered_map<int, std::vector<Posting>>::iterator it;
	size_t min_posting_size = std::numeric_limits<int>::max(),
	min_posting_position = 0;
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (test_set[i]) {
			it = this->index.find(i);
			if (it == index.end()) {
				return 0;
			}
			temp.push_back(&(it->second));
			if (it->second.size() < min_posting_size) {
				min_posting_size = it->second.size();
				min_posting_position = temp.size() - 1;
			}
		}
	}
	return intersect_postings(temp, min_posting_position);
}


/* Set existence */
bool InvertedIndex::exists(const std::bitset<ALPHABET_SIZE> &test_set) {
	// get lists of postings for each value in the set
	std::vector<std::vector<Posting>*> temp = std::vector<std::vector<Posting>*>();
	std::unordered_map<int, std::vector<Posting>>::iterator it;
	size_t min_posting_size = std::numeric_limits<int>::max(),
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


// ---------------------------------------------------------------------------
// Benchmark
// ---------------------------------------------------------------------------

typedef std::chrono::high_resolution_clock Clock;

template<typename TUnits>
long calculate_time(const Clock::time_point &start, const Clock::time_point &stop) {
	return std::chrono::duration_cast<TUnits>(stop - start).count();
}

void timespec_diff(struct timespec *stop, struct timespec *start,
		struct timespec *result) {
	result->tv_sec  = stop->tv_sec  - start->tv_sec;
	result->tv_nsec = stop->tv_nsec - start->tv_nsec;
	if (result->tv_nsec < 0) {
		--result->tv_sec;
		result->tv_nsec += 1000000000L;
	}
}


std::string InvertedIndex::run_test(const std::string &set_string, const std::string &test_mode) {
	std::string result;
	auto test_set = create_set_from_string(set_string);
	struct timespec start, stop, res;
	//Clock::time_point start, stop;
	
	if (test_mode.compare("asb") == 0) {
		//start = Clock::now();
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		size_t sets = subset_get_all(*test_set);
		//stop = Clock::now();
		clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
		result = "sets=" + std::to_string(sets) + ";";
	}
	else if (test_mode.compare("sb") == 0) {
		//start = Clock::now();
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		bool sb_res = subset_exists(*test_set);
		//stop = Clock::now();
		clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
		result = sb_res ? "val=true;" : "val=false;";
	}
	else if (test_mode.compare("asp") == 0) {
		//start = Clock::now();
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		size_t sets = superset_get_all(*test_set);
		//stop = Clock::now();
		clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
		result = "sets=" + std::to_string(sets) + ";";
	}
	else if (test_mode.compare("sp") == 0) {
		//start = Clock::now();
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		bool sp_res = superset_exists(*test_set);
		//stop = Clock::now();
		clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
		result = sp_res ? "val=true;" : "val=false;";
	}
	else {
		//start = Clock::now();
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);
		result = exists(*test_set);
		//stop = Clock::now();
		clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
	}
	timespec_diff(&stop, &start, &res);
	return
		"set=" + set_string + ";" +
		result +
		"cnt=0;" + // hardcoded value just to keep the format
		"nsc=" + std::to_string((unsigned long)(res.tv_sec * 1e+9 + res.tv_nsec));
}
