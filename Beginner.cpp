#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
	string image_path = "E:/photos/street1.png";
	Mat Clrimage = imread(image_path, IMREAD_COLOR);
	Mat image = imread(image_path, IMREAD_GRAYSCALE);
	Mat blur,laplimg,combimg,thimg,dist,imgcont,out,comb_8u;

	int ksize = 3;
	int scaleF = 3;
	int delta = 0;

	
	medianBlur(image, blur, 7);
	Laplacian(blur, laplimg, -1, ksize,scaleF,delta);
	threshold(laplimg, laplimg, 90, 255, THRESH_BINARY);

	combimg = blur - laplimg;

	threshold(combimg, thimg, 45, 255, THRESH_BINARY);

	distanceTransform(thimg, dist, DIST_L2, 3);
	normalize(dist, dist, 0, 1, NORM_MINMAX);
	erode(dist, dist, Mat::ones(3, 3, CV_8U), Point(-1, -1),3);
	threshold(dist, dist, 0.04, 1, THRESH_BINARY);

	Mat dist_8u;
	dist.convertTo(dist_8u, CV_8U);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(dist_8u, contours, hierarchy,RETR_LIST, CHAIN_APPROX_SIMPLE);

	Mat markers = Mat::zeros(dist.size(), CV_8UC3);
	for (size_t i = 0; i < contours.size(); i++)
	{
		drawContours(markers, contours, static_cast<int>(i), Scalar(0, 255-(int)i, (int)i), -1);
	}

	Mat mark_out;
	markers.convertTo(mark_out, image.type());

	double alpha = 0.8; double beta;
	beta = (1.0 - alpha);
	addWeighted(mark_out, alpha, Clrimage, beta, 0.0, out);

	imshow("Out", out);

	waitKey(0);

	return 0;
}