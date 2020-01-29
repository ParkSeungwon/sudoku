#include<utility>
#include<deque>
#include<opencv2/opencv.hpp>
#define UP 82
#define DOWN 84
#define RIGHT 83
#define LEFT 81

cv::Mat_<char> board;

class View : public cv::Mat
{
public:
	View();
	void show();
	void show2();

protected:
	static const int zoom_scale_ = 20;
};

class Snake
{
public:
	bool move();
	void direction(int d);
	void score();

protected:
	int direction_ = RIGHT;
	std::deque<cv::Point> body_ = {{10, 10}};//x, y coordinates of body, first is head
};
