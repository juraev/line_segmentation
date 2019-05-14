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
//	cin >> s_in >> s_out;
	s_in = "/home/jack/Downloads/output/a01-000u/sauvola.jpg";
	s_out = "/home/jack/Downloads/output/a01-000u";
	LineSegmentation module = LineSegmentation(s_in, s_out);
	module.proceed();
//	Image<> img = Image<>(s_in);
//	img.getWB()->save_jpeg(s_out.c_str());
}

/*
/home/jack/Downloads/output/a01-000u/
 *
 */
