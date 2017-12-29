#include "csapp.h"
#include <opencv2/opencv.hpp>

using namespace cv;

int process() {

	Mat image, gray_image;
	image = imread( "CopiedImage.jpg", CV_LOAD_IMAGE_COLOR );
 	cvtColor( image, gray_image, COLOR_BGR2GRAY );
 	imwrite( "Gray_Image.jpg", gray_image );
 	return 0;
}