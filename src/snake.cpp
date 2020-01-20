#include<iostream>
#include<thread>
#include<random>
#include<chrono>
#include"snake.h"
#define WIDTH 50
#define HEIGHT 50
using namespace std;
using namespace cv;

View::View()
	: Mat{zoom_scale_ * board.cols, zoom_scale_ * board.rows, CV_8UC3, {255, 255, 255}}
{ }

void View::show()
{
	Mat::operator=(Scalar{255, 255, 255});
	for(int x = 0; x < board.cols; x++) for(int y = 0; y < board.rows; y++) {
		putText(*this, string{board.at<char>(y, x)}, 
				{5 + zoom_scale_*x, -5 + zoom_scale_ + zoom_scale_*y},
				FONT_HERSHEY_PLAIN, zoom_scale_ / 15, {0, 0, 0}, 1.5);
	}
	imshow("game", *this);
}

bool Snake::move()
{//if false : dead
	auto p = body_.front();
	switch(direction_) {
		case UP: p.y--; break;
		case DOWN: p.y++; break;
		case RIGHT: p.x++; break;
		case LEFT: p.x--; break;
	}
	if(board.at<char>(p) != 'f') {
		board.at<char>(body_.back()) = ' ';
		body_.pop_back();//no fruit
	} 
	if(p.x <= 0 || p.y <= 0 || p.x >= board.cols-1 || p.y >= board.rows-1)
		return false;
	for(const auto &a : body_) if(p == a) return false;
	board.at<char>(p) = 'O';
	body_.push_front(p);
	return true;
}

void Snake::direction(int d)
{
	direction_ = d;
}

bool game_end = false;

void generate_fruit()
{
	uniform_int_distribution<int> di_x{1, board.rows - 2}, di_y{1, board.cols -2};
	random_device rd;
	while(!game_end) {
		int x = di_x(rd), y = di_y(rd);
		this_thread::sleep_for(3s);
		if(board.at<char>(y, x) == ' ') board.at<char>(y, x) = 'f';
	}
}

int main()
{
	board.create(WIDTH, HEIGHT);
	for(int i=0; i<WIDTH; i++) for(int j=0; j<HEIGHT; j++) 
		board.at<char>(j, i) = ' ';
	for(int i=0; i<WIDTH; i++) {
		board.at<char>(HEIGHT -1, i) = '#';
		board.at<char>(0, i) = '#';
	}
	for(int i=0; i<HEIGHT; i++) {
		board.at<char>(i, WIDTH -1) = '#';
		board.at<char>(i, 0) = '#';
	}
	thread th{generate_fruit};
	View screen;
	Snake snake;
	while(1) {
		if(int c = waitKey(100); c == 'q') break;
		else if(c > 80 && c < 85) snake.direction(c); 
		else if(!snake.move()) break;
		screen.show();
	}
	game_end = true;
	th.join();
}
