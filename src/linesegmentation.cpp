
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

	//here, we build up chunks and their projection profiles
	//then we smooth the profile and get peaks as well as valleys
	process_chunks();

	draw_initial_lines();

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

void LineSegmentation :: draw_initial_lines(){
	clog << "drawing started" << endl;
	for (int i = 1; i < num_of_chunks; i ++){

		int n = chunks[i]->valleys.size();
		int n_1 = chunks[i-1]->valleys.size();
		vector<int> cnct(chunks[i-1]->valleys.size(), -1);
		int j = 0, mn = 1e9;

		//bind valleys on phi(i) with valleys on phi(i-1)
		for (int x = 0; x < n; x ++){
			while(j < n_1 && mn > dist(i, x, i-1, j)) mn = dist(i, x, i-1, j), j ++;
			if(!cnct.empty()){
				if(cnct[j] != -1 && dist(i, cnct[j], i-1, j) > dist(i, x, i-1, j)){
					cnct[j] = x;
				} else if(cnct[j] == -1) cnct[j] = x;
				else if(dist(i, x, i - 1, j) == dist(i, x, i - 1, j + 1)){
					cnct[j+1]=x;
				}
			}
		}
		clog << "connecting is finished" << endl;

		const unsigned char color[3] = {70, 70, 70};
		for (j = 0; j < n_1; j ++){
			if(cnct[j] != -1)
				input_img.draw_line(chunks[i-1]->start_of_the_chunk, chunks[i-1]->valleys[j],
						chunks[i]->start_of_the_chunk, chunks[i]->valleys[cnct[j]], color, 1);
			else
				input_img.draw_line(chunks[i-1]->start_of_the_chunk, chunks[i-1]->valleys[j],
						chunks[i]->start_of_the_chunk, chunks[i-1]->valleys[j], color, 1);
			input_img.draw_line(chunks[i]->start_of_the_chunk, chunks[i-1]->valleys[j],
					chunks[i]->start_of_the_chunk, chunks[i-1]->valleys[j], color, 1);
		}
	}
	input_img.save_jpeg(output_path.c_str());
}

int LineSegmentation :: dist(int from, int i, int to, int j){
	return abs(chunks[from]->valleys[i] - chunks[to]->valleys[j]);
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

void Chunk :: prepare_peaks(){
	//Search for peaks and valleys
	for (int i = 0; i < _height; i ++){
		if(i != 0)
			valleys.push_back(i);
		while(sm_hist[i] == 0 && i < _height){
			i ++;
		}
		valleys.push_back(i - 1);
		int in = -1, mxval = -1;//max value on nonzero region
		int ii = -1, mnval = 1e9;//min value on this region

		while(sm_hist[i] != 0 && i < _height){

			if(sm_hist[i] >= mxval && i - in <= avg_height / 2){

				in = i, mxval = sm_hist[i];
				if(!peaks.empty())peaks.back()={in, mxval};
				else peaks.push_back({in, mxval}), num_of_lines++;
				mnval = 1e9; ii = -1;

			} else if(sm_hist[i] >= mxval){

				if(ii >= 0)				//minimal value between two peaks found
					valleys.push_back(ii);

				mnval = 1e9; ii = -1;	//reset values

				in = i, mxval = sm_hist[i];
				peaks.push_back({in, mxval});
				num_of_lines++;
			}
			if(sm_hist[i] < mnval) {
				mnval = sm_hist[i], ii = i;
			}
			i ++;
		}
	}
}
