/*
 * linesegmentation.cpp
 *
 *  Created on: Feb 13, 2019
 *      Author: jack
 */

#include "linesegmentation.h"
#include <fstream>

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
	if(input_img.empty())
		return false;

	return true;
}

void LineSegmentation :: process_chunks(){
	int st = 0;
	int wi = input_img._width / num_of_chunks;
	int hi = input_img._height;
	for (int i = 0; i < num_of_chunks; i ++){
		Chunk ch = Chunk(hi, wi, i, st);
		st += wi;
		ch.build_hist(this);
		chunks.push_back(&ch);
	}

}


//--------------------------------------------------------
//Chunkssss

void Chunk :: build_hist(LineSegmentation * module){
	// running from each pixel of the chunk
	//	ofstream ff("/home/khurshid/Pictures/log.txt", std::ios::app);


	cimg_for_inXY(module->input_img, start_of_the_chunk, 0,
			start_of_the_chunk + _width, _height, x, y){
		if((int)module->input_img(x, y, 0) < 100) hist[y] ++;
	}


	string s = (string)(module->output_path + to_string(ind) + ".jpg");
	if(ind == 10)
		module->input_img.crop(start_of_the_chunk, 0, 0,
				start_of_the_chunk + _width, _height, 0).save_jpeg(s.c_str());


	//smoothing the histogram with "Cumulative moving average" method
	sm_hist.push_back((hist[0]+hist[1]+hist[3]+1)/3);
	sm_hist.push_back((hist[0]+hist[1]+hist[3] + hist[4] + 2) / 4);

	for (int i = 2; i <_height; i ++){
		sm_hist.push_back((hist[i-2]+hist[i-1]+hist[i]+hist[i+1]+hist[i+2] + 2)/5);
	}

	for(auto x : sm_hist){
		int tmp_he = 0;
		if(x){
			tmp_he ++;
		} else {
			if(tmp_he){
				avg_height += tmp_he;
				num_of_lines ++;
				tmp_he = 0;
			}
		}
	}

	if(num_of_lines) avg_height /= num_of_lines;
	avg_height = max(30, int(avg_height + avg_height / 2.0));
	prepare_peaks();

}

void Chunk::prepare_peaks(){
	//Search for peaks
	for (int i = 1; i < _height; i ++){
		while(sm_hist[i] == 0 && i < _height){
			i ++;
		}
		int in = -1, mxval = -1;

		while(sm_hist[i] != 0 && i < _height){
			if(sm_hist[i] >= mxval && i - in <= avg_height / 2){

				in = i, mxval = sm_hist[i];
				if(!peaks.empty())peaks.back()={in, mxval};
				else peaks.push_back({in, mxval}), num_of_lines++;

			} else if(sm_hist[i] >= mxval){

				in = i, mxval = sm_hist[i];
				peaks.push_back({in, mxval});
				num_of_lines++;
			}
			i ++;
		}
	}


}
