/*************     #include    *************/
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <direct.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>

/***************    using     **************************************************************************************************************/
using namespace cv;
using namespace std;

/***************    Struct    *************************************************************************************************************/

struct color
{
	int red;
	int green;
	int blue;
};
struct block
{
	color RGB;
	string textureName;
	string blockName;
};

/***************  friend var  ********************************************************************************************************/
vector<block> blocks;

/***************  functions   ********************************************************************************************************/

/*Get the average color from img pixels*/
string padLeft(string str, int totalWidth, char paddingChar) {return string(totalWidth - str.length(), '0') + str;}

int invertNum(int num, int maxNum) { 
	//return num;
	// uncomment and remove prior line when done testing
	return abs(num - maxNum); 
}
int hexToDec(string HEX) {
	int dec;
	stringstream ss(HEX);
	ss >> hex >> dec;
	return dec;
}

string decToHex(int dec) {
	stringstream HEX;
	HEX << hex << dec;
	return HEX.str();
}

color getAvgColor(string img){
	Mat IMG;
	IMG = imread("files\\block\\" + img + ".png", CV_LOAD_IMAGE_COLOR);
	cout << "Getting Average Color of \"" << img << "\""<< endl;
	color tmp;
	tmp.red = 0;
	tmp.green = 0;
	tmp.blue = 0;
	for (int x = 0; x < IMG.rows; x++) {
		for (int y = 0; y < IMG.cols; y++) {
			tmp.blue += IMG.at<Vec3b>(x, y)[0];
			tmp.green += IMG.at<Vec3b>(x, y)[1];
			tmp.red += IMG.at<Vec3b>(x, y)[2];
		}
	}
	int imgSize = (IMG.rows * IMG.cols);
	tmp.blue = tmp.blue / imgSize;
	tmp.green = tmp.green / imgSize;
	tmp.red = tmp.red / imgSize;
	return tmp;
}

block getBlockinfo(string line) {
	string lnTmp = line;
	string tmp[3];
	int found, i = 0;
	while (lnTmp != "\0"){
		found = lnTmp.find("|");
		for (int f = 0; f < found; f++) {
			tmp[i] += lnTmp[f];
		}
		lnTmp.erase(0, found + 1);
		i++;
	}
	block tmpBlock;
	tmpBlock.textureName = tmp[0];
	tmpBlock.blockName= tmp[1];
	if (tmp[2] == "-10000") {
		color RGB;
		RGB = getAvgColor(tmpBlock.textureName);
		tmpBlock.RGB = RGB;
	}
	else {
		color RGB;
		RGB.red = hexToDec(tmp[2].substr(0, 2));
		RGB.green = hexToDec(tmp[2].substr(2, 2));
		RGB.blue = hexToDec(tmp[2].substr(4, 2));
		tmpBlock.RGB = RGB;
	}
	return tmpBlock;
}

void makeBlockTxt(string txt, string BlockColorTxt) {
	string str;
	ifstream fle;
	ofstream newFile;
	fle.open(txt);
	newFile.open(BlockColorTxt);
	while (getline(fle, str)){
		block tBlock;
		tBlock = getBlockinfo(str);
		stringstream ss[3];
		string clr;
		ss[0] << hex << tBlock.RGB.red;
		ss[1] << hex << tBlock.RGB.green;
		ss[2] << hex << tBlock.RGB.blue;
		newFile << (tBlock.textureName + '|' + tBlock.blockName + '|' + padLeft(decToHex(tBlock.RGB.red), 2, '0') + padLeft(decToHex(tBlock.RGB.green), 2, '0') + padLeft(decToHex(tBlock.RGB.blue), 2, '0')  + '|' + "\n");
}
	fle.close();
	newFile.close();
}

vector<block> getBlocks(string txt) {
	string str;
	ifstream fle;
	vector<block> blockList;
	fle.open(txt);
	while (getline(fle, str)
) {blockList.push_back(getBlockinfo(str));}
	fle.close();
	return blockList;
}

int getBestBlock(color RGB) {
	int bestRGB = 1023; int bestBlock = 0;
	for (int i = 0; i < blocks.size(); i++) {
		int R, G, B;
		R = (abs(blocks.at(i).RGB.red - RGB.red));
		G = (abs(blocks.at(i).RGB.green - RGB.green));
		B = (abs(blocks.at(i).RGB.blue - RGB.blue));
		if (bestRGB > (R + G + B)) {
			bestRGB = (R + G + B);
			bestBlock = i;
		}
	}
	return bestBlock;
}

vector<vector<int>> blockImage(Mat img) {
	int width, height; width = img.rows; height = img.cols;
	vector<vector<int>> pxGrid;
	for (int y = 0; y < height; y++){
		vector<int> pxLine;
		for (int x = 0; x < width; x++) {
			color pxl;
			pxl.blue  = img.at<Vec3b>(x, y)[0];
			pxl.green = img.at<Vec3b>(x, y)[1];
			pxl.red  = img.at<Vec3b>(x, y)[2];
			pxLine.push_back(getBestBlock(pxl));
		}
		pxGrid.push_back(pxLine);
	}
	return pxGrid;
}

void makeDataFiles() {
//	mkdir("files\\datapacks\\ITMPA\\");
}

void convertToPixelArt(string image, string imgName) {
	//makeDataFiles();
	ofstream imgData;
	Mat imag;
	imag = imread(image, CV_LOAD_IMAGE_COLOR);
	imgData.open("files\\datapacks\\ITMPA\\" + imgName + ".mcfunction");
	vector<vector<int>> readBlockImg;
	readBlockImg = blockImage(imag);
	for (int y = 0; y < readBlockImg.size(); y++) {
		for (int x = 0; x < readBlockImg.at(y).size(); x++) {
			imgData << ("setblock ~" + to_string(invertNum(x, readBlockImg.at(y).size())) + " ~" + to_string(invertNum(y, readBlockImg.size())) + " ~ "  + blocks.at(readBlockImg.at(x).at(y)).blockName) << endl;
		}
	}
	imgData.close();
	cout << "Image done processing." << endl;
}

/***************   main function   ********************************************************************************************************/
int main()
{
	//makeBlockTxt("files\\blocks.txt", "files\\blocks_color.txt");
	blocks = getBlocks("files\\blocks_color.txt");
	//cout << to_string(blocks.size()) << endl;
	/*for (int x = 0; x < blocks.size(); x++) {
		cout << blocks.at(x).blockName << ", " << to_string(blocks.at(x).RGB.red)<< ", " << to_string(blocks.at(x).RGB.green) << ", " << to_string(blocks.at(x).RGB.blue) << endl;
	}*/
	convertToPixelArt("C:\\Users\\ausde\\source\\repos\\Project3\\Project3\\files\\testimg.png", "hat_in_time");
	system("pause");
	return 0;
}