#pragma once
#include <opencv2\opencv.hpp>
#include <atlstr.h>
using namespace std;
using namespace cv;

struct DRect
{
	int top;
	int bottom;
	int left;
	int right;
};

class CChamferDisCal
{
public:
	CChamferDisCal(void);
	~CChamferDisCal(void);

	bool process(IplImage* inputImg, vector<CvPoint> &ChamferPath);
	DRect getTheBoundingBox(IplImage* inputImg);
	bool detectEndPoint_new(IplImage* Src_Img,DRect rect, vector<CvPoint> &ChamferPath);
	bool IsContourP(int x, int y, IplImage* Src_Img);
};

