#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
//#include "Vector.h"
//#include "Quaternion.h"
//#include "Matrix.h"
#include "opencv2/opencv.hpp"
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	//CVector vec;
	//Quaternion qut;
	//Matrix matPrj;
	//matPrj.LoadIdentity();
	//matPrj.Perspective(22.5, 1.333333333, 50.0, 2000.0);
	//Matrix matLMv;
	//matLMv.LoadIdentity();
	//matLMv.Rotate(15, CVector(0.0f, 1.0f, 0.0f));
	//matLMv.Translate(CVector(100, 0, 0));
	//Matrix matRMv;
	//matRMv.LoadIdentity();
	//matRMv.Rotate(-7, CVector(0.0f, 1.0f, 0.0f));
	//matRMv.Translate(CVector(-100, 0, 0));
	//CVector screenPoint(610, 951, 1.0);
	//const int viewport[4] = {0, 0, 1280, 960};
	//CVector vecF = Matrix::UnprojectPoint(screenPoint, matLMv, matPrj, viewport);


	//CVector planeNormal(1, 1, 1);
	//CVector planePoint(0, -1, 0);
	//CVector lineDir(1, 2, 3);
	//CVector linePoint(1, 1, 3);
	//CVector pdt, pdt2;
	//CVector::GetShortestBridge(planePoint, planeNormal, linePoint, lineDir, pdt, pdt2);

	IplImage *lAll = NULL;
	IplImage *lDark = NULL;
	IplImage *rAll = NULL;
	IplImage *rDark = NULL;
	IplImage *lBars[12] = {NULL};
	IplImage *rBars[12] = {NULL};
	IplImage *lRBars[12] = {NULL};
	IplImage *rRBars[12] = {NULL};
	IplImage *lCanny[12] = {NULL};
	IplImage *lRCanny[12] = {NULL};
	IplImage *rCanny[12] = {NULL};
	IplImage *rRCanny[12] = {NULL};
	IplImage *lAndCanny[12] = {NULL};
	IplImage *rAndCanny[12] = {NULL};

	//create a window
	cvNamedWindow("ImageView", CV_WINDOW_AUTOSIZE);

	// first load all images
	lDark = cvLoadImage("ImageL13.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	lAll = cvLoadImage("ImageL14.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	rDark = cvLoadImage("ImageR13.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	rAll = cvLoadImage("ImageR14.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	for(int i = 12; i > 0; i--)
	{
		lBars[12 - i] = cvLoadImage(QString("ImageL%1.bmp").arg(i).toLatin1(), CV_LOAD_IMAGE_GRAYSCALE);
		rBars[12 - i] = cvLoadImage(QString("ImageR%1.bmp").arg(i).toLatin1(), CV_LOAD_IMAGE_GRAYSCALE);
		lRBars[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		rRBars[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		lCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		rCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		lRCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		rRCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		lAndCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);;
		rAndCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);;
	}
	//GAUSSIAN smooth all image
	cvSmooth(rAll, rAll, CV_GAUSSIAN, 3, 0, 0);
	cvSmooth(lAll, lAll, CV_GAUSSIAN, 3, 0, 0);
	cvSmooth(lDark, lDark, CV_GAUSSIAN, 3, 0, 0);
	cvSmooth(rDark, rDark, CV_GAUSSIAN, 3, 0, 0);
	for(int i = 12; i > 0; i--)
	{
		cvSmooth(lBars[12 - i], lBars[12 - i], CV_GAUSSIAN, 3, 0, 0);
		cvSmooth(rBars[12 - i], rBars[12 - i], CV_GAUSSIAN, 3, 0, 0);
	}

	//remove pixels that never changed
	cvSub(lAll, lDark, lAll);
	cvSub(rAll, rDark, rAll);
	for(int i = 0; i < 12; i++)
	{
		cvSub(lBars[i], lDark, lBars[i]);
		cvSub(rBars[i], rDark, rBars[i]);
	}
	//create reverse-bar image
	for(int i = 0; i < 12; i++)
	{
		cvSub(lAll, lBars[i], lRBars[i]);
		cvSub(rAll, rBars[i], rRBars[i]);
	}
	int cannyThreshold = 10;
	for(int i = 0; i < 12; i++)
	{
		cvCanny(lBars[i], lCanny[i], cannyThreshold, cannyThreshold * 2, 3);
		cvCanny(rBars[i], rCanny[i], cannyThreshold, cannyThreshold * 2, 3);
		cvCanny(lRBars[i], lRCanny[i], cannyThreshold, cannyThreshold * 2, 3);
		cvCanny(rRBars[i], rRCanny[i], cannyThreshold, cannyThreshold * 2, 3);
		cvAnd(lCanny[i], lRCanny[i], lAndCanny[i]);
		cvAnd(rCanny[i], rRCanny[i], rAndCanny[i]);
	}

	//int threshold = 20;
	//CvSize lSize = cvGetSize(lAll);
	//CvSize rSize = cvGetSize(rAll);
	//QElapsedTimer etimer;
	//etimer.start();
	//for(int i = 0; i < lSize.width * lSize.height; i++)
	//{
	//	CvScalar blackValue = cvScalarAll(0);
	//	int barPixel;
	//	int lAllPixel = *(lAll->imageData + i);
	//	int rAllPixel = *(rAll->imageData + i);
	//	for(int idx = 0; idx < 12; idx++)
	//	{
	//		barPixel = *(lBars[idx]->imageData + i);
	//		if(abs(lAllPixel - barPixel) > threshold)
	//		{
	//			*(lBars[idx]->imageData + i) = 0;
	//		}
	//		barPixel = *(lRBars[idx]->imageData + i);
	//		if(abs(lAllPixel - barPixel) > threshold)
	//		{
	//			*(lRBars[idx]->imageData + i) = 0;
	//		}
	//		barPixel = *(rBars[idx]->imageData + i);
	//		if(abs(rAllPixel - barPixel) > threshold)
	//		{
	//			*(rBars[idx]->imageData + i) = 0;
	//		}
	//		barPixel = *(rRBars[idx]->imageData + i);
	//		if(abs(rAllPixel - barPixel) > threshold)
	//		{
	//			*(rRBars[idx]->imageData + i) = 0;
	//		}
	//	}
	//}
	//qDebug() << etimer.elapsed();

	cvShowImage("ImageView", rRBars[7]);

	for(int i = 0; i < 12; i++)
	{
		cvReleaseImage(&lBars[i]);
		cvReleaseImage(&rBars[i]);
		cvReleaseImage(&lRBars[i]);
		cvReleaseImage(&rRBars[i]);
		cvReleaseImage(&lCanny[i]);
		cvReleaseImage(&rCanny[i]);
		cvReleaseImage(&lRCanny[i]);
		cvReleaseImage(&rRCanny[i]);
		cvReleaseImage(&lAndCanny[i]);
		cvReleaseImage(&rAndCanny[i]);
	}
	cvReleaseImage(&lAll);
	cvReleaseImage(&lDark);
	cvReleaseImage(&rAll);
	cvReleaseImage(&rDark);
	return a.exec();
}
