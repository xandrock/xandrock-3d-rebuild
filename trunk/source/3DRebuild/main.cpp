#include <QCoreApplication>
#include <QDebug>
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "opencv2/opencv.hpp"
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
	CVector screenPoint(610, 951, 1.0);
	const int viewport[4] = {0, 0, 1280, 960};
	CVector vecF = Matrix::UnprojectPoint(screenPoint, matLMv, matPrj, viewport);


	//CVector planeNormal(1, 1, 1);
	//CVector planePoint(0, -1, 0);
	//CVector lineDir(1, 2, 3);
	//CVector linePoint(1, 1, 3);
	//CVector pdt, pdt2;
	//CVector::GetShortestBridge(planePoint, planeNormal, linePoint, lineDir, pdt, pdt2);

	IplImage* leftLine = cvLoadImage("ImageRLine.bmp", CV_LOAD_IMAGE_GRAYSCALE);


	return a.exec();
}
