#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	Mat M{540, 540, CV_8UC3, Scalar(255,255,255)};
	for(int i=1; i<9; i++) {
		line(M, {i * 60, 0}, {i * 60, 540}, Scalar(0,0,0), i % 3 ? 1 : 3, LINE_8);
		line(M, {0, i * 60}, {540, i * 60}, Scalar(0,0,0), i % 3 ? 1 : 3, LINE_8);
	}
	putText(M, "9", {10, 50}, FONT_HERSHEY_PLAIN, 4, Scalar(0,0,0), 2);
	circle(M, {30, 30}, 25, Scalar(0,0,0), 1);
	imshow("sudoku", M);
	waitKey(0);
}

