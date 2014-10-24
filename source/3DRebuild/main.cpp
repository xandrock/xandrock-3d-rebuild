#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "opencv2/opencv.hpp"
#include <QMap>
#include <QList>
#include <QFile>

struct PointCC
{
	QPoint leftDot;
	QPoint rightDot;
	CVector realDot;
	unsigned char color[3];
};
struct BarChorp
{
	quint32 bX;
	quint32 ex;
	quint32 barIdx;
};
QList<PointCC> finalPoints;
QVector<int> gareCode;

quint32 GareToInt(quint32 gare)
{
	return gareCode.indexOf(gare);
}



quint32 IntToGare(quint32 val)
{
	return gareCode.indexOf(val);
}

void QSetImageGrayCode(quint32 idx, quint32 y, IplImage *bars[], IplImage *rBars[], IplImage *pGrayImg, quint32 fac, quint32 xb, quint32 xe)
{
	QList<quint32> lightorDark;
	QList<QPair<quint32, quint32> > LineBars;
	uchar* const pLBinData = (uchar *)(pGrayImg->imageData);
	qint32 txb = -1;
	qint32 txe = -1;
	qint32 curIdx = -1;;
	QPair<qint32, qint32> barBE(-1, -1);
	for(int i = xb; i <= xe; i++)
	{
		quint32 valuable = pLBinData[(y * 1280 + i) * 3 + 2];
		if(valuable)
		{
			if(txb == -1)
			{
				txb = i;
			}
			txe = i;
			uchar barPixel = *(bars[idx]->imageData + y * 1280 + i);
			uchar rBarPixel = *(rBars[idx]->imageData + y * 1280 + i);
			qint32 tbidx = 0;
			if(barPixel > rBarPixel)
			{
				tbidx = 1;
			}
			if(tbidx != curIdx)
			{
				lightorDark.append(tbidx);
				barBE.first = i;
				barBE.second = i;
				LineBars.append(barBE);
			}else
			{
				if(LineBars.count())
				{
					LineBars[LineBars.count() - 1].second = i;
				}
			}
		}
	}
	assert(lightorDark.count() == LineBars.count());
	if(lightorDark.count() > 2)
	{
		QMap<quint32, quint32> ChangedAndIndex;
		QList<quint32> changedsWidth;
		for(int i = 0; i < lightorDark.count(); i++)
		{
			quint32 changed = 0;
			for(int pi = 0; pi < i; pi++)
			{
				if(lightorDark[pi] == lightorDark[i])
				{
					changed += LineBars[pi].second - LineBars[pi].first + 1;
				}
			}
			for(int pi = i + 1; pi < lightorDark.count(); pi++)
			{
				if(lightorDark[pi] != lightorDark[i])
				{
					changed += LineBars[pi].second - LineBars[pi].first + 1;
				}
			}
			changedsWidth.append(changed);
		}
		QList<quint32> minlessChanged;
		for(int i = 0; i < changedsWidth.count(); i++)
		{
			if(minlessChanged.count() == 0)
			{
				minlessChanged.append(i);
			}else if(changedsWidth[minlessChanged[0]] > changedsWidth[i])
			{
				minlessChanged.clear();
				minlessChanged.append(i);
			}else if(changedsWidth[minlessChanged[0]] == changedsWidth[i])
			{
				minlessChanged.append(i);
			}
		}
		if(minlessChanged.count() > 1)
		{

		}
		//if(minlessChanged.count() == 1)
		//{
		//	for(int pi = 0; pi < minlessChanged.at(0); pi++)
		//	{
		//		if(lightorDark[pi] == lightorDark[minlessChanged.at(0)])
		//		{
		//			changed += LineBars[pi].second - LineBars[pi].first + 1;
		//		}
		//	}
		//	for(int pi = i + 1; pi < lightorDark.count(); pi++)
		//	{
		//		if(lightorDark[pi] != lightorDark[i])
		//		{
		//			changed += LineBars[pi].second - LineBars[pi].first + 1;
		//		}
		//	}

		//}
	}
}

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
	//init garecode
	gareCode.fill(0, 256);

	quint32 wid = 512;
	for(int i = 0; i < 8; i++)
	{
		for(int x = 0; x < 256; x++)
		{
			int yu = x % wid;
			if( yu >= wid / 4 && yu < wid / 4 * 3)
			{
				gareCode[x] += wid / 4;
			}
		}
		wid /= 2;
		qDebug() << "Line Processed";
	}

	foreach(int val, gareCode)
	{
		QByteArray num = QByteArray::number(val, 2);
		while(num.length() < 8)
			num.prepend("0");
		qDebug() << num;
	}

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
	//IplImage *lCanny[12] = {NULL};
	//IplImage *lRCanny[12] = {NULL};
	//IplImage *rCanny[12] = {NULL};
	//IplImage *rRCanny[12] = {NULL};
	//IplImage *lAndCanny[12] = {NULL};
	//IplImage *rAndCanny[12] = {NULL};



	//create a window
	cvNamedWindow("ImageView", CV_WINDOW_AUTOSIZE);

	// first load all images
	lDark = cvLoadImage("ImageL13.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	lAll = cvLoadImage("ImageL14.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	rDark = cvLoadImage("ImageR13.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	rAll = cvLoadImage("ImageR14.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	colorImg = cvLoadImage("ImageL14.bmp", CV_LOAD_IMAGE_COLOR);
	lBinMap = cvCreateImage(cvGetSize(lAll), IPL_DEPTH_8U, 3);
	rBinMap = cvCreateImage(cvGetSize(rAll), IPL_DEPTH_8U, 3);
	cvSetZero(lBinMap);
	cvSetZero(rBinMap);
	for(int i = 12; i > 0; i--)
	{
		lBars[12 - i] = cvLoadImage(QString("ImageL%1.bmp").arg(i).toLatin1(), CV_LOAD_IMAGE_GRAYSCALE);
		rBars[12 - i] = cvLoadImage(QString("ImageR%1.bmp").arg(i).toLatin1(), CV_LOAD_IMAGE_GRAYSCALE);
		lRBars[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		rRBars[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		//lCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		//rCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		//lRCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		//rRCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);
		//lAndCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);;
		//rAndCanny[12 - i] = cvCreateImage(cvGetSize(lBars[12 - i]), IPL_DEPTH_8U, 1);;
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

	CvSize lSize = cvGetSize(lAll);
	CvSize rSize = cvGetSize(rAll);

	uchar* const pLBinData = (uchar *)(lBinMap->imageData);
	uchar* const pRBinData = (uchar *)(rBinMap->imageData);
	

	quint32 proTreshhold = 8;
	for(int i = 0; i < lSize.width * lSize.height; i++)
	{
		unsigned char allPix;
		allPix = *(lAll->imageData + i);
		if(allPix > proTreshhold)
		{
			pLBinData[i * 3 + 2] = 128;
		}
		allPix = *(rAll->imageData + i);
		if(allPix > proTreshhold)
		{
			pRBinData[i * 3 + 2] = 128;
		}

	}


	// mark pixel's blue channel with gray code bar index;
	unsigned char fac = 128;
	for(int idx = 0; idx < 8; idx++)
	{
		//CvScalar blackValue = cvScalarAll(0);
		unsigned char barPixel;
		unsigned char rBarPixel;
		unsigned char allPix;
		for(int i = 0; i < lSize.width * lSize.height; i++)
		{
			allPix = *(lAll->imageData + i);
			if(allPix > proTreshhold)
			{
				barPixel = *(lBars[idx]->imageData + i);
				rBarPixel = *(lRBars[idx]->imageData + i);
				if(barPixel > rBarPixel)
				{
					pLBinData[i * 3 + 0] += fac;
				//}else if(barPixel == rBarPixel)
				//{
				//	pLBinData[i * 3 + 0] = 0;
				//	pLBinData[i * 3 + 1] = 0;
				//	pLBinData[i * 3 + 2] = 0;
				}
			}
			allPix = *(rAll->imageData + i);
			if(allPix > proTreshhold)
			{
				barPixel = *(rBars[idx]->imageData + i);
				rBarPixel = *(rRBars[idx]->imageData + i);
				if(barPixel > rBarPixel)
				{
					pRBinData[i * 3 + 0] += fac;
				//}else if(barPixel == rBarPixel)
				//{
				//	pRBinData[i * 3 + 0] = 0;
				//	pRBinData[i * 3 + 1] = 0;
				//	pRBinData[i * 3 + 2] = 0;
				}

			}
		}
		fac /= 2;
	}
	// change gray code to normal index;
	for(int i = 0; i < lSize.width * lSize.height; i++)
	{
		pLBinData[i * 3 + 0] = GareToInt(pLBinData[i * 3 + 0]);
		pRBinData[i * 3 + 0] = GareToInt(pRBinData[i * 3 + 0]);		
	}
	////compare pixel's index to it's left pixel, if smaller
	//for(int y = 0; y < 960; y++)
	//{
	//	quint32 lastRCode = 0;
	//	quint32 lastLCode = 0;
	//	for(int x = 0; x < 1280; x++)
	//	{
	//		quint32 idx = (y * 1280 + x) * 3;
	//		if(pLBinData[idx + 2] > 0)
	//		{
	//			if(pLBinData[idx] > lastLCode)
	//			{
	//				lastLCode = pLBinData[idx];
	//			}else if(pLBinData[idx] < lastLCode)
	//			{
	//				qDebug() << QString("Remove Left Pixel x:%1 y:%2 index:%3 lastIndex:%4 ").arg(x).arg(y).arg(pLBinData[idx]).arg(lastLCode) ;
	//				pLBinData[idx] = 0;
	//				pLBinData[idx + 2] = 0;
	//			}
	//		}
	//		if(pRBinData[idx + 2] > 0)
	//		{
	//			if(pRBinData[idx] > lastRCode)
	//			{
	//				lastRCode = pRBinData[idx];
	//			}else if(pRBinData[idx] < lastRCode)
	//			{
	//				qDebug() << QString("Remove Right Pixel x:%1 y:%2 index:%3 lastIndex:%4 ").arg(x).arg(y).arg(pRBinData[idx]).arg(lastRCode) ;
	//				pRBinData[idx] = 0;
	//				pRBinData[idx + 2] = 0;
	//			}
	//		}
	//	}
	//}




	for(int idx = 8; idx < 2; idx++)
	{
		IplImage *lCalBin = cvCreateImage(cvGetSize(lAll), IPL_DEPTH_8U, 3);
		cvCopy(lBinMap, lCalBin);
		IplImage *rCalBin = cvCreateImage(cvGetSize(rAll), IPL_DEPTH_8U, 3);
		cvCopy(rBinMap, rCalBin);
		uchar* const pLCalData = (uchar *)(lCalBin->imageData);
		uchar* const pRCalData = (uchar *)(rCalBin->imageData);
		unsigned char barPixel;
		unsigned char rBarPixel;
		unsigned char allPix;
		for(int i = 0; i < lSize.width * lSize.height; i++)
		{
			allPix = *(lAll->imageData + i);
			if(allPix > proTreshhold)
			{
				barPixel = *(lBars[idx]->imageData + i);
				rBarPixel = *(lRBars[idx]->imageData + i);
				if(barPixel > rBarPixel)
				{
					pLCalData[i * 3 + 1] += 64;
				}
			}
			allPix = *(rAll->imageData + i);
			if(allPix > proTreshhold)
			{
				barPixel = *(rBars[idx]->imageData + i);
				rBarPixel = *(rRBars[idx]->imageData + i);
				if(barPixel > rBarPixel)
				{
					pRCalData[i * 3 + 1] += 64;
				}
			}
		}

		for(int y = 3; y < lSize.height; ++y)
		{

		}
		//QMap< baridx, QMap<Y, QPair<QList<LeftX>, QList<RightX> > > >
		QMap<quint32, QMap<quint32,  QPair<QList<quint32>, QList<quint32> > > > bars;
		for(int i = 0; i < lSize.width * lSize.height; i++)
		{
			int y = i / lSize.width;
			int x = i % lSize.width;
			if(y < 3/* || x < 250 || x > 880*/)
			{
				continue;
			}
			uchar r = pLCalData[i * 3 + 2 ];
			uchar g = pLCalData[i * 3 + 1 ];
			uchar b = pLCalData[i * 3 + 0 ];

			quint32 val = r << 16 | b << 8 | g;

			if(bars[val][y].first.count() < 2)
			{
				bars[val][y].first.append(x);
			}else
			{
				bars[val][y].first[1] = x;
			}
			r = pRCalData[i * 3 + 2 ];
			g = pRCalData[i * 3 + 1 ];
			b = pRCalData[i * 3 + 0 ];

			val = r << 16 | b << 8 | g;
				
			if(bars[val][y - 3].second.count() < 2)
			{
				bars[val][y - 3].second.append(x);
			}else
			{
				bars[val][y - 3].second[1] = x;
			}
		}
		//foreach(quint32 bidx, bars.keys())
		//{
		//
		//	foreach(quint32 y, bars[bidx].keys())
		//	{
		//		QPair<QList<quint32>, QList<quint32> > pts = bars[bidx][y];
		//		if(pts.first.isEmpty() || pts.second.isEmpty())
		//		{
		//			bars[bidx].remove(y);
		//			continue;
		//		}
		//		// math the left edge of this bar
		//		CVector lScreenPos(pts.first.first(), 959.0 - y, 1.0);
		//		CVector rScreenPos(pts.second.first(), 962.0 - y, 1.0);
		//		CVector lRealPos = Matrix::UnprojectPoint(lScreenPos, matLMv, matPrj, viewport);
		//		CVector rRealPos = Matrix::UnprojectPoint(rScreenPos, matRMv, matPrj, viewport);
		//		CVector b1, b2;
		//		CVector::GetShortestBridge(leftCameraPos, lRealPos, rightCameraPos, rRealPos, b1, b2);
		//		if((b1 - b2).Length() < 10.0)
		//		{
		//			CVector realPos = (b1 + b2) / 2.0;
		//			PointCC pxc;
		//			pxc.realDot = realPos;
		//			pxc.leftDot = QPoint(pts.first.first(), y);
		//			pxc.rightDot = QPoint(pts.first.first(), y - 3);
		//			pxc.color[0] = cvGet2D(colorImg, y, pts.first.first()).val[2];
		//			pxc.color[1] = cvGet2D(colorImg, y, pts.first.first()).val[1];
		//			pxc.color[2] = cvGet2D(colorImg, y, pts.first.first()).val[0];
		//			finalPoints.append(pxc);
		//		}
		//		////// then math the right edge
		//		//if(pts.first.count() == 1 && pts.second.count() == 1)
		//		//{
		//		//	continue;
		//		//}
		//		//lScreenPos = CVector(pts.first.last(), 959.0 - y, 1.0);
		//		//rScreenPos = CVector(pts.second.last(), 962.0 - y, 1.0);
		//		//lRealPos = Matrix::UnprojectPoint(lScreenPos, matLMv, matPrj, viewport);
		//		//rRealPos = Matrix::UnprojectPoint(rScreenPos, matRMv, matPrj, viewport);
		//		//CVector::GetShortestBridge(leftCameraPos, lRealPos, rightCameraPos, rRealPos, b1, b2);
		//		//if((b1 - b2).Length() < 10.0)
		//		//{
		//		//	CVector realPos = (b1 + b2) / 2.0;
		//		//	PointCC pxc;
		//		//	pxc.realDot = realPos;
		//		//	pxc.leftDot = QPoint(pts.first.last(), y);
		//		//	pxc.rightDot = QPoint(pts.first.last(), y - 3);

		//		//	pxc.color[0] = cvGet2D(colorImg, y, pts.first.last()).val[2];
		//		//	pxc.color[1] = cvGet2D(colorImg, y, pts.first.last()).val[1];
		//		//	pxc.color[2] = cvGet2D(colorImg, y, pts.first.last()).val[0];
		//		//	finalPoints.append(pxc);
		//		//}
		//	}
		//}
		//	//
		qDebug() << bars;
		QFile qf("textOupt.txt");
		qf.open(QIODevice::WriteOnly);
		foreach(quint32 bidx, bars.keys())
		{
			if(bidx == 0)
			{
				continue;
			}
			qf.write(QString("Bar%1\n").arg(bidx).toLatin1());
			foreach(quint32 y, bars[bidx].keys())
			{
				if(bars[bidx][y].first.isEmpty() || bars[bidx][y].second.isEmpty())
				{
					continue;
				}
				qf.write(QString("    In Left Row%1 And Right Row%2\n").arg(y).arg(y - 3).toLatin1());
				qf.write(QString("        Left %1 - %2  Right %3 - %4 \n")
					.arg(bars[bidx][y].first.first())
					.arg(bars[bidx][y].first.last())
					.arg(bars[bidx][y].second.first())
					.arg(bars[bidx][y].second.last()).toLatin1());
			}
		}
		qf.close();

		cvReleaseImage(&lCalBin);
		cvReleaseImage(&rCalBin);

	}

	//}

	//Save("x.ds3d");






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
	//cvSmooth(lBinMap, lBinMap, CV_GAUSSIAN, 10, 0, 0);
	cvShowImage("ImageView", rBinMap);


	//for(int i = 0; i < 1280; i++)
	//{
	//	uchar r = pLBinData[(1280 * 520 + i) * 3 + 2 ];
	//	uchar g = pLBinData[(1280 * 520 + i) * 3 + 1 ];
	//	uchar b = pLBinData[(1280 * 520 + i) * 3 + 0 ];

	//	quint32 xxx = r << 16 | b << 8 | g;
	//	qDebug() << i << QByteArray::number(xxx, 16);
	//}


	for(int i = 0; i < 12; i++)
	{
		cvReleaseImage(&lBars[i]);
		cvReleaseImage(&rBars[i]);
		cvReleaseImage(&lRBars[i]);
		cvReleaseImage(&rRBars[i]);
		//cvReleaseImage(&lCanny[i]);
		//cvReleaseImage(&rCanny[i]);
		//cvReleaseImage(&lRCanny[i]);
		//cvReleaseImage(&rRCanny[i]);
		//cvReleaseImage(&lAndCanny[i]);
		//cvReleaseImage(&rAndCanny[i]);
	}
	cvReleaseImage(&lAll);
	cvReleaseImage(&lDark);
	cvReleaseImage(&rAll);
	cvReleaseImage(&rDark);
	cvReleaseImage(&lBinMap);
	cvReleaseImage(&rBinMap);
	qDebug() << "Finished!!!!!!!!!!!";
	return a.exec();
}