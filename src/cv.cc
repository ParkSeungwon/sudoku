#include<string>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int x, y;
namespace {
	const int scale = 60;
	Mat M{scale*9, scale*9, CV_8UC3, {255,255,255}};

	void callback(int event, int i, int j, int, void*)
	{
		if(event == EVENT_MOUSEMOVE) x = i/scale, y = j/scale;
	}

	int init() 
	{
		namedWindow("sudoku");
		setMouseCallback("sudoku", callback);
		return 0;
	}

	int k = init();
}

void print_frame()
{
	M = Scalar(255,255,255);
	for(int i=1; i<9; i++) {//draw lines
		line(M, {i * scale, 0}, {i * scale, scale*9}, {0,0,0}, i % 3 ? 1 : 3, LINE_8);
		line(M, {0, i * scale}, {scale*9, i * scale}, {0,0,0}, i % 3 ? 1 : 3, LINE_8);
	}
}

void print_xy(int number, int x, int y, array<uint8_t, 3> rgb)
{
	const int border = 3;
	Rect rt{x*scale + border, y*scale + border, scale - border*2, scale - border*2};
	auto roi = M(rt);
	roi = Scalar{255,255,255};
	if(auto [r, g, b] = rgb; number) 
		putText(roi, to_string(number), {border + 5 , scale - border -5},
				FONT_HERSHEY_PLAIN, scale / 10 - 2, {b, g, r}, 2);
}

void complete()
{
	putText(M, "You solved", {scale, scale*5}, FONT_HERSHEY_PLAIN, 
			scale/12, {255,223,0}, 5);
}
void show() 
{
	imshow("sudoku", M);
}

char getkey()
{
	return waitKey();
}
