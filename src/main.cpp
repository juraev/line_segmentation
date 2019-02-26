/*
 * main.cpp
 *
 *  Created on: Feb 11, 2019
 *      Author: jack
 */
#include "linesegmentation.h"
//#include "Image.h"

int main(int argc, char **argv) {
	string s_in, s_out;
	cin >> s_in >> s_out;
	LineSegmentation module = LineSegmentation(s_in, s_out);
	module.proceed();
//	Image<> img = Image<>(s_in);
//	img.getWB()->save_jpeg(s_out.c_str());
}

/*
/home/jack/eclipse-workspace/output/b01-000/otsu.jpg
/home/jack/eclipse-workspace/output/b01-000/img.jpg
 *
 */
