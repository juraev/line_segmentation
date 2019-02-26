/*
 * linesegmentation.hh
 *
 *  Created on: Feb 13, 2019
 *      Author: jack
 */

#ifndef LINESEGMENTATION
#define LINESEGMENTATION
#include "CImg.h"
#include <vector>
#include <map>
#include <math.h>
#include <iostream>
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

	void save_lines();

	bool read_image();

	void process_chunks();


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

	//index of the chunk
	int ind;

	//The projection profile of the chunk
	vector<int> hist;

public:
	Chunk(int a, int b, int in, int st){
		_height = a;
		_width = b;
		ind = in;
		start_of_the_chunk = st;
//		for (int i = 0; i < 23; i ++) hist.push_back(0);
		hist = vector<int>(_height + 2, 0);
	};

	void build_hist(LineSegmentation*);

	//peaks
	vector<pair<int, int> > peaks;

};

#endif
