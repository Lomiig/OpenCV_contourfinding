#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

/*
Removes lone pixels
*/
Mat cleaner(Mat);

/*
Counts how many pixels are different from 0 in a 3x3 area around pixel with coords (i,j)
*/
int count(Mat, int, int);

/*
Apply multiple transformations to return a sharpened image
*/
Mat sharpen(Mat);

/*
Create a Mat that you can imshow after a watershed transform
*/
Mat applywatershed(Mat, vector<vector<Point>>);

int main() {

	string image_path = "E:/photos/street3.png";
	Mat Clrimage = imread(image_path, IMREAD_COLOR);
	Mat image = imread(image_path, IMREAD_GRAYSCALE);
	Mat  combimg, thimg, dist, out, sharp;

	combimg = sharpen(image);
	sharp = sharpen(Clrimage);

	threshold(combimg, thimg, 45, 255, THRESH_BINARY);
	distanceTransform(thimg, dist, DIST_L2, 3);
	normalize(dist, dist, 0, 1, NORM_MINMAX);
	erode(dist, dist, Mat::ones(3, 3, CV_8U), Point(-1, -1), 3);
	threshold(dist, dist, 0.08, 1, THRESH_BINARY);

	Mat dist_8u;
	dist.convertTo(dist_8u, CV_8U);

	vector<vector<Point> > contours;
	findContours(dist_8u, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

	Mat markers = Mat::zeros(dist.size(), CV_32S);
	for (size_t i = 0; i < contours.size(); i++)
	{
		drawContours(markers, contours, static_cast<int>(i), Scalar(static_cast<int>(i) + 1), -1);
	}

	circle(markers, Point(5, 5), 3, Scalar(255), -1);
	Mat markers8u;
	markers.convertTo(markers8u, CV_8U, 10);

	watershed(sharp, markers);
	out = applywatershed(markers, contours);
	imshow("Out", out);

	waitKey(0);

	return 0;
}


Mat cleaner(Mat src) {
	int channels = src.channels();
	int nRows = src.rows;
	int nCols = src.cols;
	int i, j, nlife;
	switch (channels) {
		case 1: {
			for (i = 0; i < nRows; i++) {
				for (j = 0; j < nCols; j++) {
					if (src.at<uchar>(i, j) != 0) {
						nlife = count(src, i, j);
						if (nlife <= 2 || nlife >= 8) {
							src.at<uchar>(i, j) = 0;
						}
					}
				}
			}
			break;
		}
		case 3: {
			Mat_<Vec3b> _src = src;
			for (i = 0; i < nRows; i++) {
				for (j = 0; j < nCols; j++) {
					if (_src(i, j)[0] != 0 || _src(i, j)[1] != 0 || _src(i, j)[2] != 0 ) {
						nlife = count(src, i, j);
						if (nlife <= 2 || nlife >= 8) {
							_src(i, j)[0] = 0;
							_src(i, j)[1] = 0;
							_src(i, j)[2] = 0;
						}
					}
				}
			}
			src = _src;
			break;
		}
	}
	return src;
}

int count(Mat src, int i, int j) {
	int channels = src.channels();
	int k, l;
	int s = 0;
	switch (channels) {
		case 1: {
			if (i == 0) {
				if (j == 0) {
					for (k = 0; k < 2; k++) {
						for (l = 0; l < 2; l++) {
							if (src.at<uchar>(i + k, j + l) != 0) {
								s = s + 1;
							}
						}
					}
				}
				else if (j == src.cols - 1) {
					for (k = 0; k < 2; k++) {
						for (l = -1; l < 1; l++) {
							if (src.at<uchar>(i + k, j + l) != 0) {
								s = s + 1;
							}
						}
					}
				}
				else {
					for (k = 0; k < 2; k++) {
						for (l = -1; l < 2; l++) {
							if (src.at<uchar>(i + k, j + l) != 0) {
								s = s + 1;
							}
						}
					}
				}

			}
			else if (i == src.rows - 1) {
				if (j == 0) {
					for (k = -1; k < 1; k++) {
						for (l = 0; l < 2; l++) {
							if (src.at<uchar>(i + k, j + l) != 0) {
								s = s + 1;
							}
						}
					}
				}
				else if (j == src.cols - 1) {
					for (k = -1; k < 1; k++) {
						for (l = -1; l < 1; l++) {
							if (src.at<uchar>(i + k, j + l) != 0) {
								s = s + 1;
							}
						}
					}
				}
				else {
					for (k = -1; k < 1; k++) {
						for (l = -1; l < 2; l++) {
							if (src.at<uchar>(i + k, j + l) != 0) {
								s = s + 1;
							}
						}
					}
				}
			}
			else if (j == 0) {
				for (k = -1; k < 2; k++) {
					for (l = 0; l < 2; l++) {
						if (src.at<uchar>(i + k, j + l) != 0) {
							s = s + 1;
						}
					}
				}
			}
			else if (j == src.cols - 1) {
				for (k = -1; k < 2; k++) {
					for (l = -1; l < 1; l++) {
						if (src.at<uchar>(i + k, j + l) != 0) {
							s = s + 1;
						}
					}
				}
			}
			else {
				for (k = -1; k < 2; k++) {
					for (l = -1; l < 2; l++) {
						if (src.at<uchar>(i + k, j + l) != 0) {
							s = s + 1;
						}
					}
				}
			}
			break;
		}
		case 3: {
			Mat_<Vec3b> _src = src;
			if (i == 0) {
				if (j == 0) {
					for (k = 0; k < 2; k++) {
						for (l = 0; l < 2; l++) {
							if (_src(i+k, j+l)[0] != 0 || _src(i+k, j+l)[1] != 0 || _src(i+k, j+l)[2] != 0) {
								s = s + 1;
							}
						}
					}
				}
				else if (j == src.cols - 1) {
					for (k = 0; k < 2; k++) {
						for (l = -1; l < 1; l++) {
							if (_src(i + k, j + l)[0] != 0 || _src(i + k, j + l)[1] != 0 || _src(i + k, j + l)[2] != 0) {
								s = s + 1;
							}
						}
					}
				}
				else {
					for (k = 0; k < 2; k++) {
						for (l = -1; l < 2; l++) {
							if (_src(i + k, j + l)[0] != 0 || _src(i + k, j + l)[1] != 0 || _src(i + k, j + l)[2] != 0) {
								s = s + 1;
							}
						}
					}
				}

			}
			else if (i == src.rows - 1) {
				if (j == 0) {
					for (k = -1; k < 1; k++) {
						for (l = 0; l < 2; l++) {
							if (_src(i + k, j + l)[0] != 0 || _src(i + k, j + l)[1] != 0 || _src(i + k, j + l)[2] != 0) {
								s = s + 1;
							}
						}
					}
				}
				else if (j == src.cols - 1) {
					for (k = -1; k < 1; k++) {
						for (l = -1; l < 1; l++) {
							if (_src(i + k, j + l)[0] != 0 || _src(i + k, j + l)[1] != 0 || _src(i + k, j + l)[2] != 0) {
								s = s + 1;
							}
						}
					}
				}
				else {
					for (k = -1; k < 1; k++) {
						for (l = -1; l < 2; l++) {
							if (_src(i + k, j + l)[0] != 0 || _src(i + k, j + l)[1] != 0 || _src(i + k, j + l)[2] != 0) {
								s = s + 1;
							}
						}
					}
				}
			}
			else if (j == 0) {
				for (k = -1; k < 2; k++) {
					for (l = 0; l < 2; l++) {
						if (_src(i + k, j + l)[0] != 0 || _src(i + k, j + l)[1] != 0 || _src(i + k, j + l)[2] != 0) {
							s = s + 1;
						}
					}
				}
			}
			else if (j == src.cols - 1) {
				for (k = -1; k < 2; k++) {
					for (l = -1; l < 1; l++) {
						if (_src(i + k, j + l)[0] != 0 || _src(i + k, j + l)[1] != 0 || _src(i + k, j + l)[2] != 0) {
							s = s + 1;
						}
					}
				}
			}
			else {
				for (k = -1; k < 2; k++) {
					for (l = -1; l < 2; l++) {
						if (_src(i + k, j + l)[0] != 0 || _src(i + k, j + l)[1] != 0 || _src(i + k, j + l)[2] != 0) {
							s = s + 1;
						}
					}
				}
			}
			break;
		}
	}
	return s;
}

Mat sharpen(Mat src) {
	Mat blur, laplimg, combimg, thimg, dilimg;
	int ksize = 3;
	int scaleF = 3;
	int delta = 0;
	medianBlur(src, blur, 5);
	Laplacian(blur, laplimg, -1, ksize, scaleF, delta);
	threshold(laplimg, thimg, 90, 255, THRESH_BINARY);

	cleaner(thimg);

	dilate(thimg, dilimg, Mat::ones(3, 3, CV_8U), Point(-1, -1), 1);

	combimg = blur - dilimg; 
	return combimg;
}

Mat applywatershed(Mat markers, vector<vector<Point> > contours){
	Mat mark;
	markers.convertTo(mark, CV_8U);
	bitwise_not(mark, mark);

	vector<Vec3b> colors;
	for (size_t i = 0; i < contours.size(); i++)
	{
		int b = 0;
		int g = 255 - 10 * (int)i;
		int r = 10 * (int)i;

		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}

	Mat dst = Mat::zeros(markers.size(), CV_8UC3);

	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{
			int index = markers.at<int>(i, j);
			if (index > 0 && index <= static_cast<int>(contours.size()))
			{
				dst.at<Vec3b>(i, j) = colors[index - 1];
			}
		}
	}
	return dst;
}
