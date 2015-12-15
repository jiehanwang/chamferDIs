// chamferDIs.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <opencv2\opencv.hpp>
#include "ChamferDisCal.h"
using namespace cv;
using namespace std;

void copyImg3to1(IplImage* src, IplImage* des)
{
	//
	// copy depth image from 3 channels to 1 channel
	//
	int height = src->height;
	int width = src->width;
	for (int rows=0; rows<height; rows++)
	{
		uchar* des_ptr = (uchar*)(des->imageData + rows*des->widthStep);
		uchar* src_ptr_depth = (uchar*)(src->imageData + rows*src->widthStep);
		for (int cols=0; cols<width; cols++)
		{
			des_ptr[cols] = src_ptr_depth[3*cols + 0];
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	IplImage* inputImg = cvLoadImage("..\\data\\testImg.jpg", 1);
	IplImage* grayImg = cvCreateImage(cvGetSize(inputImg), 8, 1);
	bool isRight = TRUE; //FALSE TRUE

	copyImg3to1(inputImg, grayImg);
	CChamferDisCal myEndDetect;

	vector<CvPoint> ChamferPath;
	myEndDetect.process(grayImg, ChamferPath);

	for(vector<CvPoint>::iterator iter = ChamferPath.begin(); iter != ChamferPath.end(); iter++)  
	{  
		cout<<(*iter).x<<" "<<(*iter).y<<endl; 
	}  

	cvNamedWindow("show", 1);
	cvShowImage("show", grayImg);

	cvNamedWindow("ori",1);
	cvShowImage("ori", inputImg);
	cvWaitKey(10000);

	cvDestroyWindow("show");
	cvDestroyWindow("ori");

	cvReleaseImage(&inputImg);
	cvReleaseImage(&grayImg);

	return 0;
}

