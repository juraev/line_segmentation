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

using namespace cimg_library;
using namespace std;

class Chunk;


class LineSegmentation{

	LineSegmentation(string, string);

public:
	void proceed();

	void save_lines();

	bool read_image();

private:
	//paths for input ans output images
	string input_path;
	string output_path;

	//
	CImg<uchar> input_img;

	//chunks
	vector<Chunk*> chunks;
};


#endif
