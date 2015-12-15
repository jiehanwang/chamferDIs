#include "stdafx.h"
#include "ChamferDisCal.h"


CChamferDisCal::CChamferDisCal(void)
{
}


CChamferDisCal::~CChamferDisCal(void)
{
}

DRect CChamferDisCal::getTheBoundingBox(IplImage* inputImg)
{
	int x, y;
	int width = inputImg->width;
	int height = inputImg->height;

	DRect boundingBox;
	boundingBox.bottom = 0;
	boundingBox.top = 0;
	boundingBox.left = 0;
	boundingBox.right = 0;
	int pixelNumThre = 1;

	// The left
	for (x=0; x<width; x++)
	{
		int noZeroCount = 0;
		for (y=0; y<height; y++)
		{
			double dPixelVal = cvGetReal2D( inputImg, y, x );
			if (dPixelVal >0)
			{
				noZeroCount++;
			}
		}
		if (noZeroCount > pixelNumThre)
		{
			boundingBox.left = x;
			break;
		}
	}

	// The right
	for (x=width-1; x>=0; x--)
	{
		int noZeroCount = 0;
		for (y=0; y<height; y++)
		{
			double dPixelVal = cvGetReal2D( inputImg, y, x );
			if (dPixelVal >0)
			{
				noZeroCount++;
			}

		}
		if (noZeroCount> pixelNumThre)
		{
			boundingBox.right = x;
			break;
		}
	}

	// The top
	for (y=0; y<height; y++)
	{
		int noZeroCount = 0;
		for (x=0; x<width; x++)
		{
			double dPixelVal = cvGetReal2D( inputImg, y, x );
			if (dPixelVal >0)
			{
				noZeroCount++;
			}
		}
		if (noZeroCount > pixelNumThre )
		{
			boundingBox.top = y;
			break;
		}


	}
	for (y=height-1; y>=0; y--)
	{
		int noZeroCount = 0;
		for (x=0; x<width; x++)
		{
			double dPixelVal = cvGetReal2D( inputImg, y, x );
			if (dPixelVal >0)
			{
				noZeroCount++;
			}
		}
		if (noZeroCount > pixelNumThre)
		{
			boundingBox.bottom = y;
			break;
		}
	}
	 
	return boundingBox;
}

bool CChamferDisCal::process(IplImage* inputImg, vector<CvPoint> &ChamferPath)
{
	int width = inputImg->width;
	int height = inputImg->height;

	DRect boundingBox = getTheBoundingBox(inputImg);
	bool isRight = TRUE;
	bool isDetect = detectEndPoint_new(inputImg,boundingBox,ChamferPath, isRight);
	

	return isDetect;
}

bool CChamferDisCal::detectEndPoint_new(IplImage* Src_Img,DRect rect, vector<CvPoint> &ChamferPath, bool isRight)
{
	int xOut, yOut, zOut;
	int i, j;  // For loop
	CvSize img_size = cvGetSize(Src_Img);
	int Remove_Num=0;
	int loopTime = 0;  // The loop time will be limited to 100. 

	// Extract the bone of arm
	do 
	{
		loopTime+=1;
		Remove_Num=0;
		for (j = rect.top; j < rect.bottom; j++)
		{
			for(i = rect.left; i < rect.right; i++)
			{
				BYTE gray_value = ((BYTE*)(Src_Img->imageData + Src_Img->widthStep*j))[i];
				if ( gray_value && IsContourP( i, j, Src_Img))
				{
					((BYTE*)(Src_Img->imageData + Src_Img->widthStep*j))[i]=(UCHAR)0;
					Remove_Num++;
				}  
			}  
		}  
	} while( Remove_Num && loopTime<100); // What is the function of "Remove_Num"?

	// Define the seed point. "isRight" indicates the right or left arm is considered. 
	int rootX = -1;
	int rootY = rect.top;
	int startP = rect.left;
	int endP = rect.right;

	for (i=rect.left; i<rect.right; i++)
	{
		double dPixelVal = cvGetReal2D( Src_Img, rect.top, i );
		if (dPixelVal > 0)
		{
			startP = i;
			break;
		}
	}
	for (i = rect.right; i>rect.left; i--)
	{
		double dPixelVal = cvGetReal2D( Src_Img, rect.top, i );
		if (dPixelVal > 0)
		{
			endP = i;
			break;
		}
	}
	rootX = int((startP + endP)/2);
	cvCircle(Src_Img,cvPoint(rootX,rootY),10,CV_RGB(255,255,255),2,1,0);
	if (rootX == -1)
	{
		return FALSE;
	}

	// Find all the endpoints
	int endX[1000];
	int endY[1000];
	int endLength[100];
	int count = 0;
	for (j = rect.top; j < rect.bottom; j++)
	{
		for (i = rect.left+5; i < rect.right-5; i++)
		{
			BYTE gray_value = ((BYTE*)(Src_Img->imageData + Src_Img->widthStep*(j)))[i];
			if (gray_value > 250)
			{
				// The 8 directions. 
				BYTE gray_value1 = ((BYTE*)(Src_Img->imageData + Src_Img->widthStep*(j-1)))[i-1];
				BYTE gray_value2 = ((BYTE*)(Src_Img->imageData + Src_Img->widthStep*j))[i-1];
				BYTE gray_value3 = ((BYTE*)(Src_Img->imageData + Src_Img->widthStep*(j+1)))[i-1];
				BYTE gray_value4 = ((BYTE*)(Src_Img->imageData + Src_Img->widthStep*(j-1)))[i];
				BYTE gray_value5 = ((BYTE*)(Src_Img->imageData + Src_Img->widthStep*(j+1)))[i];
				BYTE gray_value6 = ((BYTE*)(Src_Img->imageData + Src_Img->widthStep*(j-1)))[i+1];
				BYTE gray_value7 = ((BYTE*)(Src_Img->imageData + Src_Img->widthStep*j))[i+1];
				BYTE gray_value8 = ((BYTE*)(Src_Img->imageData + Src_Img->widthStep*(j+1)))[i+1];
				byte gray = gray_value1+gray_value2+gray_value3+gray_value4+gray_value5+gray_value6
					+gray_value7+gray_value8;
				if (gray == 255)
				{
					endX[count] = i;
					endY[count] = j;
					cvCircle(Src_Img,cvPoint(i,j),3,CV_RGB(255,255,255),1,1,0);
					count++;
				}
			}
			if (count>100) break;
		}
		if (count>100) break;
	}
	// Choose the farthest endpoint as the final one. 
	// This method need to be improved.
	int maxDistance = 0;
	for (i=0; i<count; i++)
	{
		if ((abs(rootX-endX[i])+abs(rootY-endY[i]))>maxDistance)
		{
			maxDistance = (abs(rootX-endX[i])+abs(rootY-endY[i]));
			xOut = endX[i];
			yOut = endY[i];
		}
	}
	// 	if (abs(m_iDHPreviousEdp.x-xOut)+abs(m_iDHPreviousEdp.y-yOut)<300)
	// 	{
	// 		xOut = m_iDHPreviousEdp.x;
	// 		yOut = m_iDHPreviousEdp.y;
	// 	}
	// 	else
	// 	{
	// 		m_iDHPreviousEdp.x = xOut;
	// 		m_iDHPreviousEdp.y = yOut;
	// 	}
	cvCircle(Src_Img,cvPoint(xOut,yOut),10,CV_RGB(255,255,255),2,1,0);
	// Set zOut to 0 temporally. 
	zOut = 0; // *(m_pDHFrameBits+m_iWidth*yOut+xOut);


	ChamferPath.push_back(cvPoint(rootX, rootY));
	ChamferPath.push_back(cvPoint(xOut, yOut));
	// If succeed, return TRUE.
	return TRUE;
	
}


bool CChamferDisCal::IsContourP(int x, int y, IplImage* Src_Img)
{
	BOOL p[10] ={0};
	int LineBytes =Src_Img->widthStep;
	BYTE *lpPtr= (BYTE*)(Src_Img->imageData+LineBytes*y)+x;

	p[2]=*(lpPtr-LineBytes) ? true:false;
	p[3]=*(lpPtr-LineBytes+1) ? true:false;
	p[4]=*(lpPtr+1) ? true:false;
	p[5]=*(lpPtr+LineBytes+1) ? true:false;
	p[6]=*(lpPtr+LineBytes) ? true:false;
	p[7]=*(lpPtr+LineBytes-1) ? true:false;
	p[8]=*(lpPtr-1) ? true:false;
	p[9]=*(lpPtr-LineBytes-1) ? true:false;

	int Np=0;
	int Tp=0;
	for (int i=2; i<10; i++)
	{
		Np += p[i];
		int k= (i<9) ? (i+1) : 2;
		if ( p[k] -p[i]>0)
		{
			Tp++;
		}
	}
	int p246= p[2] && p[4] && p[6];
	int p468= p[4] && p[6] && p[8];

	int p24= p[2] && !p[3] && p[4] && !p[5] && !p[6] && !p[7] && !p[8] && !p[9];
	int p46= !p[2] && !p[3] && p[4] && !p[5] && p[6] && !p[7] && !p[8] && !p[9];
	int p68= !p[2] && !p[3] && !p[4] && !p[5] && p[6] && !p[7] && p[8] && !p[9];
	int p82= p[2] && !p[3] && !p[4] && !p[5] && !p[6] && !p[7] && p[8] && !p[9];

	int p782= p[2] && !p[3] && !p[4] && !p[5] && !p[6] && p[7] && p[8] && !p[9];
	int p924= p[2] && !p[3] && p[4] && !p[5] && !p[6] && !p[7] && !p[8] && p[9];
	int p346= !p[2] && p[3] && p[4] && !p[5] && p[6] && !p[7] && !p[8] && !p[9];
	int p568= !p[2] && !p[3] && !p[4] && p[5] && p[6] && !p[7] && p[8] && !p[9];

	int p689= !p[2] && !p[3] && !p[4] && !p[5] && p[6] && !p[7] && p[8] && p[9];
	int p823= p[2] && p[3] && !p[4] && !p[5] && !p[6] && !p[7] && p[8] && !p[9];
	int p245= p[2] && !p[3] && p[4] && p[5] && !p[6] && !p[7] && !p[8] && !p[9];
	int p467= !p[2] && !p[3] && p[4] && !p[5] && p[6] && p[7] && !p[8] && !p[9];

	int p2468= p24 || p46 || p68 || p82;
	int p3333= p782 || p924 || p346 || p568 || p689 || p823 || p245 || p467;

	return ( !p246 && !p468 && (Np<7) && (Np>1) && (Tp==1) ) || p2468 || p3333; 

}