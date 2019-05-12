#include<thread>
#include<opencv2/opencv.hpp>
#include"base.h"
using namespace std;

const int scale = 60;
bool toggle_num_shape = false, match = false, finished = false;
void show_time()
{
	for(int i=0; !finished; i++) {
		this_thread::sleep_for(1s);
		cout << i/60 << "분" << i%60 << "초 경과\r" << flush;
	}
	cout << endl;
}

using namespace cv;
Mat M{scale*9, scale*9, CV_8UC3, {255,255,255}};
int x, y;
void callback(int event, int i, int j, int, void*)
{
	if(event == EVENT_MOUSEMOVE) x = i/scale, y = j/scale;
}

using smat = Cmat<int, 9, 9>;
smat toggle;
void cv_print_sudoku(const smat &m, const smat &Q, const smat &A)
{//opencv 
	M = Scalar(255,255,255);
	for(int i=1; i<9; i++) {//draw lines
		line(M, {i * scale, 0}, {i * scale, scale*9}, {0,0,0}, i % 3 ? 1 : 3, LINE_8);
		line(M, {0, i * scale}, {scale*9, i * scale}, {0,0,0}, i % 3 ? 1 : 3, LINE_8);
	}
	for(int i=0; i<9; i++) for(int j=0; j<9; j++) if(Scalar color; m[i][j]) {
		if(Q[i][j]) color = {0,0,0};
		else if(match && A[i][j] != m[i][j]) color = {0,0,255};
		else color = toggle[i][j] ? Scalar{0,255,0} : Scalar{255,0,0};
		putText(M, to_string(m[i][j]), {10 + scale*i, scale - 10 + scale*j},
				FONT_HERSHEY_PLAIN, scale / 10 - 2, color, 2);
	}
	if(m == A) {
		putText(M, "You solved", {scale, scale*5}, FONT_HERSHEY_PLAIN, scale/12, {120,120,255}, 5);
		finished = true;
	}
	imshow("sudoku", M);
}

array<Cmat<int, 9, 9>, 2> sudokuQA();

int main(int ac, char **av)
{//generate sudoku problem and solve it.
	auto [Q, A] = sudokuQA();
	namedWindow("sudoku");
	setMouseCallback("sudoku", callback);
	Cmat<int, 9, 9> m, toggle;
	m = Q;
	cv_print_sudoku(m, Q, A);
	thread th{show_time};
	for(char c; (c = waitKey()) != 'q'; ) {
		switch(c) {
			case 'd': if(!Q[x][y]) m[x][y] = 0; break;
			case 'c': toggle_num_shape = !toggle_num_shape; break;
			case 'm': match = !match;
		}
		if(c >= '1' && c <= '9' && !Q[x][y])  
			toggle[x][y] = toggle_num_shape, m[x][y] = c - '0';
		cv_print_sudoku(m, Q, A);
	}
	finished = true;
	th.join();
}

