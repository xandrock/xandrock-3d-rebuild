#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "opencv2/opencv.hpp"
#include <QMap>
#include <QList>

struct PointCC
{
	QPoint leftDot;
	QPoint rightDot;
	CVector realDot;
	unsigned char color[3];
};
QList<PointCC> finalPoints;



bool Save(const char *filename)
{
	//wchar_t *p,*n;
	//wchar_t tfile[256];
	//unsigned char tmpbuff = 1;
	//char boneId = 0;

	FILE *fp = fopen(filename, "wb");
	if (!fp)
		return false;

	fseek(fp, 0, SEEK_SET);

	char id[11] = "DS3D000000";
	fwrite(id, sizeof(char), 10, fp);

	int version = 4;
	fwrite(&version, sizeof(int), 1, fp);

	// vertices
	unsigned int numVertices = finalPoints.size();
	fwrite(&numVertices, sizeof(unsigned int), 1, fp);
	for(int i = 0; i < numVertices; i++)
	{
		float x = finalPoints[i].realDot.x;
		float y = finalPoints[i].realDot.y;
		float z = finalPoints[i].realDot.z;
		unsigned char vcolor[3] = {255};
		fwrite(&x, sizeof(float), 1, fp);
		fwrite(&y, sizeof(float), 1, fp);
		fwrite(&z, sizeof(float), 1, fp);
		fwrite(finalPoints[i].color, sizeof(char), 3, fp);
	}
	// triangles
	unsigned short numTriangles = 0;
	fwrite(&numTriangles, sizeof(unsigned short), 1, fp);
	// groups
	unsigned short numGroups = 0;
	fwrite(&numGroups, sizeof(unsigned short), 1, fp);

	// materials
	unsigned short numMaterials = 0;
	fwrite(&numMaterials, sizeof(unsigned short), 1, fp);
	fclose(fp);

}
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	CVector vec;
	Quaternion qut;
	Matrix matPrj;
	matPrj.LoadIdentity();
	matPrj.Perspective(22.5, 1.333333333, 50.0, 2000.0);
	Matrix matLMv;
	matLMv.LoadIdentity();
	matLMv.Rotate(15, CVector(0.0f, 1.0f, 0.0f));
	matLMv.Translate(CVector(100, 0, 0));
	Matrix matRMv;
	matRMv.LoadIdentity();
	matRMv.Rotate(-7, CVector(0.0f, 1.0f, 0.0f));
	matRMv.Translate(CVector(-100, 0, 0));
	const int viewport[4] = {0, 0, 1280, 960};
	CVector leftCameraPos(-100, 0, 0);
	CVector rightCameraPos(100, 0, 0);
	//CVector screenPoint(610, 951, 1.0);
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
	IplImage *lBinMap = NULL;
	IplImage *rBinMap = NULL;
	IplImage *colorImg = NULL;
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
	colorImg = cvLoadImage("ImageL14.bmp", CV_LOAD_IMAGE_COLOR);
	lBinMap = cvCreateImage(cvGetSize(lAll), IPL_DEPTH_16U, 1);
	rBinMap = cvCreateImage(cvGetSize(rAll), IPL_DEPTH_16U, 1);
	cvSetZero(lBinMap);
	cvSetZero(rBinMap);
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

	int threshold = 20;
	CvSize lSize = cvGetSize(lAll);
	CvSize rSize = cvGetSize(rAll);
	QElapsedTimer etimer;
	etimer.start();
	unsigned int fac = 1280;
	for(int idx = 0; idx < 8; idx++)
	{
		//CvScalar blackValue = cvScalarAll(0);
		unsigned char barPixel;
		unsigned char rBarPixel;
		unsigned char allPix;
		for(int i = 0; i < lSize.width * lSize.height; i++)
		{
			allPix = *(lAll->imageData + i);
			if(allPix >0)
			{
				barPixel = *(lBars[idx]->imageData + i);
				rBarPixel = *(lRBars[idx]->imageData + i);
				if(barPixel > rBarPixel)
				{
					unsigned short *pData = (unsigned short*)(lBinMap->imageData);
					pData += i;
					*(pData) += fac;
				}
			}
			allPix = *(rAll->imageData + i);
			if(allPix > 0)
			{
				barPixel = *(rBars[idx]->imageData + i);
				rBarPixel = *(rRBars[idx]->imageData + i);
				if(barPixel > rBarPixel)
				{
					unsigned short *pData = (unsigned short*)(rBinMap->imageData);
					pData += i;
					*(pData) += fac;
				}
			}
		}
		fac /= 2;
	}

	for(int idx = 8; idx < 9; idx++)
	{
		//IplImage *lCalBin = cvCreateImage(cvGetSize(lAll), IPL_DEPTH_16U, 1);
		//IplImage *rCalBin = cvCreateImage(cvGetSize(rAll), IPL_DEPTH_16U, 1);
		//cvCopy(lBinMap, lCalBin);;
		//cvCopy(rBinMap, rCalBin);;
		unsigned char barPixel;
		unsigned char rBarPixel;
		unsigned char allPix;
		//QMap< baridx, QMap<Y, QPair<QList<LeftX>, QList<RightX> > > >
		QMap<quint32, QMap<quint32,  QPair<QList<quint32>, QList<quint32> > > > bars;
		for(int i = 0; i < lSize.width * lSize.height; i++)
		{
			int y = i / lSize.width;
			int x = i % lSize.width;
			if(y < 3)
			{
				continue;
			}
			allPix = *(lAll->imageData + i);
			if(allPix >0)
			{
				barPixel = *(lBars[idx]->imageData + i);
				rBarPixel = *(lRBars[idx]->imageData + i);
				unsigned short *pData = (unsigned short*)(lBinMap->imageData);
				pData += i;
				unsigned short val = *pData;
				if(barPixel > rBarPixel)
				{
					val++;
				}
				bars[val][y].first.append(x);
			}
			allPix = *(rAll->imageData + i);
			if(allPix > 0)
			{
				barPixel = *(rBars[idx]->imageData + i);
				rBarPixel = *(rRBars[idx]->imageData + i);
				unsigned short *pData = (unsigned short*)(rBinMap->imageData);
				pData += i;
				unsigned short val = *pData;
				if(barPixel > rBarPixel)
				{
					val++;
				}
				bars[val][y - 3].second.append(x);
			}
		}
		foreach(quint32 bidx, bars.keys())
		{
			foreach(quint32 y, bars[bidx].keys())
			{
				QPair<QList<quint32>, QList<quint32> > pts = bars[bidx][y];
				if(pts.first.isEmpty() || pts.second.isEmpty())
				{
					continue;
				}
				// math the left edge of this bar
				CVector lScreenPos(pts.first.first(), 959.0 - y, 1.0);
				CVector rScreenPos(pts.second.first(), 962.0 - y, 1.0);
				CVector lRealPos = Matrix::UnprojectPoint(lScreenPos, matLMv, matPrj, viewport);
				CVector rRealPos = Matrix::UnprojectPoint(rScreenPos, matRMv, matPrj, viewport);
				CVector b1, b2;
				CVector::GetShortestBridge(leftCameraPos, lRealPos, rightCameraPos, rRealPos, b1, b2);
				if((b1 - b2).Length() < 10.0)
				{
					CVector realPos = (b1 + b2) / 2.0;
					PointCC pxc;
					pxc.realDot = realPos;
					pxc.leftDot = QPoint(pts.first.first(), y);
					pxc.rightDot = QPoint(pts.first.first(), y - 3);
					pxc.color[0] = cvGet2D(colorImg, y, pts.first.first()).val[2];
					pxc.color[1] = cvGet2D(colorImg, y, pts.first.first()).val[1];
					pxc.color[2] = cvGet2D(colorImg, y, pts.first.first()).val[0];
					finalPoints.append(pxc);
				}
				// then math the right edge
				if(pts.first.count() == 1 && pts.second.count() == 1)
				{
					continue;
				}
				lScreenPos = CVector(pts.first.last(), 959.0 - y, 1.0);
				rScreenPos = CVector(pts.second.last(), 962.0 - y, 1.0);
				lRealPos = Matrix::UnprojectPoint(lScreenPos, matLMv, matPrj, viewport);
				rRealPos = Matrix::UnprojectPoint(rScreenPos, matRMv, matPrj, viewport);
				CVector::GetShortestBridge(leftCameraPos, lRealPos, rightCameraPos, rRealPos, b1, b2);
				if((b1 - b2).Length() < 10.0)
				{
					CVector realPos = (b1 + b2) / 2.0;
					PointCC pxc;
					pxc.realDot = realPos;
					pxc.leftDot = QPoint(pts.first.last(), y);
					pxc.rightDot = QPoint(pts.first.last(), y - 3);

					pxc.color[0] = cvGet2D(colorImg, y, pts.first.last()).val[2];
					pxc.color[1] = cvGet2D(colorImg, y, pts.first.last()).val[1];
					pxc.color[2] = cvGet2D(colorImg, y, pts.first.last()).val[0];
					finalPoints.append(pxc);
				}
			}
		}


		//


		//cvReleaseImage(&lCalBin);
		//cvReleaseImage(&rCalBin);
	}

	Save("x.ds3d");






	//int cannyThreshold = 10;
	//for(int i = 0; i < 12; i++)
	//{
	//	cvCanny(lBars[i], lCanny[i], cannyThreshold, cannyThreshold * 2, 3);
	//	cvCanny(rBars[i], rCanny[i], cannyThreshold, cannyThreshold * 2, 3);
	//	cvCanny(lRBars[i], lRCanny[i], cannyThreshold, cannyThreshold * 2, 3);
	//	cvCanny(rRBars[i], rRCanny[i], cannyThreshold, cannyThreshold * 2, 3);
	//	cvAnd(lCanny[i], lRCanny[i], lAndCanny[i]);
	//	cvAnd(rCanny[i], rRCanny[i], rAndCanny[i]);
	//}

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

	//cvShowImage("ImageView", lBinMap);


	//for(int i = 0; i < 1280; i++)
	//{
	//	unsigned short *pData = (unsigned short*)(lBinMap->imageData);
	//	pData += 1280 * 520 + i;
	//	qDebug() << *(pData);
	//}


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
	cvReleaseImage(&lBinMap);
	cvReleaseImage(&rBinMap);
	return a.exec();
}
