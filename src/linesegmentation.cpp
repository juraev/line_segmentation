/*
 * linesegmentation.cpp
 *
 *  Created on: Feb 13, 2019
 *      Author: jack
 */

#include "linesegmentation.h"

LineSegmentation :: LineSegmentation(string in, string out):
input_path(in),
output_path(out) {}

void LineSegmentation :: proceed(){
	if(!read_image()){
		//ToDo @juraev think what to do
	}

	process_chunks();

}

bool LineSegmentation :: read_image(){
	if(input_path.empty()){
		return false;
	}

	input_img.load(input_path.c_str());
	int dd = 0;
//	cimg_forXY(input_img, x, y){
//		if(input_img(x, y, 0, 0) == 0)
//			if(input_img(x, y, 0, 1) == 0)
//				if(input_img(x, y, 0, 2) == 0)
//					dd ++;
//		if(input_img(x, y, 0, 0) == 255)
//			if(input_img(x, y, 0, 1) == 255)
//				if(input_img(x, y, 0, 2) == 255)
//					dd ++;
//	}
//	clog << dd << endl;

	if(input_img.empty())
		return false;

	return true;
}

void LineSegmentation :: process_chunks(){
	int st = 0;
	for (int i = 0; i < num_of_chunks; i ++){
		st += input_img._width / num_of_chunks;
		Chunk ch(input_img._height, input_img._width / num_of_chunks, i, st);
		ch.build_hist(this);
		chunks.push_back(ch);
	}

}


//--------------------------------------------------------
//Chunkssss

void Chunk :: build_hist(LineSegmentation * module){
	// running from each pixel of the chunk
	int dd = 0;
	CImg<uchar> temp;
	cimg_for_inXY(module->input_img,
			start_of_the_chunk, 			0,
			start_of_the_chunk + _width, 	_height,
			x, y){
		if(module->input_img(x, y, 0) == 0){
			hist[x] ++;
			dd ++;
		}
	}
	if(ind == 5){ clog << dd << endl;
//		module->input_img.crop(start_of_the_chunk, 0, 0, 0,
//				start_of_the_chunk + _width, _height, 0, 0, 0).save_jpeg(module->output_path.c_str());
	}
	//smoothing the histogram with "Cumulative moving average" method

	hist[0] = (hist[0] + hist[1] + hist[2]) / 3;
	for (int i = 1; i <_height; i ++){
//		if(ind == 5)
//			clog << hist[i] << endl;
		hist[i] = (hist[i + 2] + 4 * hist[i - 1]) / 5;
	}

	//
	for (int i = 1; i < _height; i ++){
		int rr = hist[i-1], cc = hist[i], ll = hist[i+1];
		if(cc >= rr && cc >= ll){
			peaks.push_back(make_pair(i, cc));
		}
	}
}
