/*
 * main.cpp
 *
 *  Created on: Feb 11, 2019
 *      Author: jack
 */
#include "linesegmentation.h"
using namespace std;

int main(int argc, char **argv) {
	string s_in, s_out;
	cin >> s_in >> s_out;
	LineSegmentation module = LineSegmentation(s_in, s_out);
	module.proceed();

}


