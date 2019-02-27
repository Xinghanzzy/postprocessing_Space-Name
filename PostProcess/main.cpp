#define _CRT_SECURE_NO_WARNINGS
#include "post_processor.h"
#include <iostream>

using namespace std;
int main()
{
	cout << "Begin" << endl;
	post_processor::PostProcessor instance("en2zh");
	std::ifstream infile_src, infile_tst;
	std::ofstream outfile_post;
	string line_src, line_tst;

	// Load configuration file
	instance.Init("config.ini");

	cout << "-------------------------------------" << endl;

	infile_src.open("input");
	infile_tst.open("tst");
	if (!infile_src || !infile_tst)
	{
		cout << "The input file does not exist!" << endl;
		exit(1);
	}
	outfile_post.open("tst.new");
	while (!infile_src.eof() && !infile_tst.eof())
	{
		getline(infile_src, line_src);
		getline(infile_tst, line_tst);
		string temp = instance.Process(line_src, line_tst);
		outfile_post << temp << endl;
	} //while readfile

	infile_src.close();
	infile_tst.close();
	outfile_post.close();
	cout << "Done!" << endl;
	return 0;
}