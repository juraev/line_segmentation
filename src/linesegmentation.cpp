
/*
 * linesegmentation.cpp
 *
 *  Created on: Feb 13, 2019
 *      Author: juraev
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
		Chunk ch(hi, wi, i, st);
		st += wi;
		ch.build_hist(this);
		chunks.push_back(ch);
	}
}

void LineSegmentation :: draw_initial_lines(){
	for (int i = 0; i < chunks[5].num_of_valleys; i ++)
		cout << chunks[5].valleys[i] << " ";
	cout << endl;
	for (int i = 0; i < chunks[6].num_of_valleys; i ++)
		cout << chunks[6].valleys[i] << " ";
	//	clog << "drawing started" << endl;
	for (int i = 1; i < num_of_chunks; i ++){

		int n = chunks[i].num_of_valleys;
		int n_1 = chunks[i-1].num_of_valleys;
		if(n_1 == 0 || n == 0) continue;
		vector<int> cnct;
		for(int j = 0; j < n_1; j ++) cnct.push_back(-1);

		int j = 0, j_1 = 0;

		//bind valleys on phi(i) with valleys on phi(i-1)

		for (; j < n && j_1 < n_1;){
			if(dist(i-1, j_1, i, j) > dist(i-1, j_1, i, j + 1)){
				j ++;
			} else if(dist(i-1, j_1, i, j) > dist(i-1, j_1 + 1, i, j)){
				j_1 ++;
			} else {
				cnct[j_1] = j;
				j_1 ++;
				j ++;
			}
		}

		if(i - 1 == 5){
			cout << endl;
			for(int g = 0; g < cnct.size(); g ++){
				cout << chunks[i-1].valleys[g] << " " << chunks[i].valleys[cnct[g]] << endl;
			}
		}

		const uchar col[] = {0, 0, 0};
		for (int z = 0; z < n_1; z ++){

			input_img.draw_line(chunks[i-1].start_of_the_chunk, chunks[i-1].valleys[z],
					chunks[i].start_of_the_chunk, chunks[i-1].valleys[z], col, 1);

			if(cnct[z] != -1){

				input_img.draw_line(chunks[i].start_of_the_chunk, chunks[i-1].valleys[z],
						chunks[i].start_of_the_chunk, chunks[i].valleys[cnct[z]], col, 1);

			}
		}
	}
	input_img.save_jpeg((output_path + "lines.jpg").c_str());
}

int LineSegmentation :: dist(int from, int i, int to, int j){
	return abs(chunks[from].valleys[i] - chunks[to].valleys[j]);
}

//--------------------------------------------------------
//Chunkssss

void Chunk :: build_hist(LineSegmentation * module){
	//to test
	//	ofstream ff("/home/jack/Pictures/log.txt", std::ios::app);

	CImg<int> img = module->input_img.get_crop(start_of_the_chunk, 0,
			start_of_the_chunk + _width, _height);

	// running from each pixel of the chunk
	cimg_forXY(img, x, y){
		if(img.atXY(x, y, 0) < 100){
			hist[y] ++;
		}
	}

	//		string s = (string)(module.output_path + to_string(ind) + ".jpg");
	//		img.save_jpeg(s.c_str());

	//smoothing the histogram with "Cumulative moving average" method
	sm_hist[0] = ((hist[0]+hist[1]+hist[3]+1)/3);
	sm_hist[1] = ((hist[0]+hist[1]+hist[3] + hist[4] + 2) / 4);
	sm_hist[_height - 1] = ((hist[_height - 1]+hist[_height - 2]+hist[_height - 3]+1)/3);
	sm_hist[_height - 2] = ((hist[_height - 1]+hist[_height - 2]+hist[_height - 3] + hist[_height - 4] + 2) / 4);


	for (int i = 2; i <_height - 2; i ++){
		sm_hist[i] = ((hist[i-2]+hist[i-1]+hist[i]+hist[i+1]+hist[i+2])/5);
		//		if(ind == 10) ff << sm_hist[i] << endl;
	}

	int nn = 0;
	int tmp_he = 0;
	for(int x : sm_hist){
		if(x > 0){
			tmp_he ++;
		} else {
			if(tmp_he > 0){
				avg_height += tmp_he;
				nn ++;
				tmp_he = 0;
			}
		}
	}

	if(nn) avg_height = avg_height / nn;
	avg_height = max(30, int(avg_height + avg_height / 2.0));
	prepare_peaks();
}

void Chunk :: prepare_peaks(){
	//Search for peaks and valleys
	int start = 0;
	for (int i = 0; i < _height; i ++){
		start = i;

		while(sm_hist[i] == 0 && i < _height){
			i ++;
		}

		if(i < _height){
//			if((start + i)/2==-1) cout << "xaxa" << endl;
			valleys.push_back((start + i) / 2);
		}

		int in = -1, mxval = -1;//max value on nonzero region
		int ii = -1, mnval = 1e9;//min value on this region

		while(sm_hist[i] != 0 && i < _height){

			if(sm_hist[i] >= mxval && i - in <= avg_height / 1.5){

				in = i, mxval = sm_hist[i];
				if(!peaks.empty()) peaks.back()={in, mxval};
				else peaks.push_back({in, mxval}), num_of_peaks++;
				mnval = 1e9; ii = -1;

			} else if(sm_hist[i] >= mxval){

				if(ii >= 0){				//minimal value between two peaks is found
					valleys.push_back(ii);
				}

				mnval = 1e9; ii = -1;	//reset values

				in = i, mxval = sm_hist[i];
				peaks.push_back({in, mxval});
				num_of_peaks++;
			}
			if(sm_hist[i] < mnval) {
				mnval = sm_hist[i], ii = i;
			}
			i ++;
		}
	}
	num_of_valleys = valleys.size();
}
//21 70 104 141 186 230 277 300 327 355 394 434 473 508 547 582 613 648 675 688 718

Chunk::Chunk(int a, int b, int in, int st){
	_height = a;
	_width = b;
	ind = in;
	start_of_the_chunk = st;
	hist = vector<int>(_height + 2, 0);
	sm_hist = vector<int>(_height, 0);
	avg_height = 0;
	num_of_lines = 0;
	peaks = vector<pair<int, int> >();
	valleys = vector<int>();
	num_of_valleys = 0;
	num_of_peaks = 0;
};
