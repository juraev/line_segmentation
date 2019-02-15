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

}

bool LineSegmentation :: read_image(){
	if(input_path.empty()){
		return false;
	}

	input_img.load(input_path.c_str());

	return true;
}
