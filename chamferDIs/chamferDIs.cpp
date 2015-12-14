// chamferDIs.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include "ChamferDisCal.h"
using namespace cv;

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
	IplImage* grayImg = cvCreateImage(cvSize(inputImg->width, inputImg->height), 8, 1);
	copyImg3to1(inputImg, grayImg);
	CChamferDisCal myEndDetect;
	DRect rect;
	rect.left = 0;
	rect.right = grayImg->width - 1;
	rect.top = 0;
	rect.bottom = grayImg->height - 1;
	int EDp_x = 0;
	int EDp_y = 0;
	int EDp_z = 0;
	myEndDetect.detectEndPoint_new(grayImg,rect, EDp_x, EDp_y, EDp_z, TRUE);

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

