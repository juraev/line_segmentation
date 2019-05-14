/*
 * linesegmentation.hh
 *
 *  Created on: Feb 13, 2019
 *      Author: jack
 */

#ifndef LINESEGMENTATION_H_
#define LINESEGMENTATION_H_
#include "CImg.h"
#include <vector>
#include <map>
#include <math.h>
#include <iostream>
#include <set>
#define uchar unsigned char
#define index first
#define hgt second

using namespace cimg_library;
using namespace std;

class Chunk;


class LineSegmentation{
	friend class Chunk;

public:
	LineSegmentation(string, string);

	void proceed();

	bool read_image();

	void process_chunks();

	void draw_initial_lines();

	int dist(int, int, int, int);




private:
	//paths for input ans output images
	string input_path;
	string output_path;

	//
	CImg<uchar> input_img;

	//chunks
	vector<Chunk> chunks;

	//number of chunks
	const int num_of_chunks = 20;
};

class Chunk{
	friend class LineSegmentation;
private:
	//parameters of the chunk
	int _width;
	int _height;
	int start_of_the_chunk;
	int avg_height;
	int num_of_lines;
	int num_of_valleys;
	int num_of_peaks;

	//index of the chunk
	int ind;

	//The projection profile of the chunk
	vector<int> hist;
	vector<int> sm_hist;

public:
	Chunk(int, int, int, int);

	void build_hist(LineSegmentation*);

	void prepare_peaks();

	//peaks and valleys
	vector<pair<int, int> > peaks;
	vector<int> valleys;
};

#endif
