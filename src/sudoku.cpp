#include<iostream>
#include<thread>
#include"base.h"
using namespace std;

bool toggle_num_shape = false, match = false, finished = false;
array<Cmat<int, 9, 9>, 2> sudokuQA();
void print_frame(), show(), complete();
void print_xy(int number, int x, int y, array<uint8_t, 3> rgb);
char getkey();
Cmat<int, 9, 9> toggle, Q, A, m;

void show_time()
{
	for(int i=0; !finished; i++) {
		this_thread::sleep_for(1s);
		cout << i/60 << "분" << i%60 << "초 경과\r" << flush;
	}
	cout << endl;
}

array<uint8_t, 3> get_color(int i, int j)
{//return color according to circumstances
	uint8_t r, g, b;
	if(Q[i][j]) r = 0, g = 0, b = 0;
	else if(match && A[i][j] != m[i][j]) r = 255, g = 0, b = 0;
	else if(toggle[i][j]) r = 0, g = 255, b = 0;
	else r = 0, g = 0, b = 255;
	return {r, g, b};
}

void print_all()
{//opencv 
	for(int i=0; i<9; i++) for(int j=0; j<9; j++) if(m[i][j])
		print_xy(m[i][j], i, j, get_color(i, j));
	if(m == A) complete();
	show();
}

extern int x, y;
int main()
{//generate sudoku problem and solve it.
	auto a = sudokuQA();
	Q = a[0]; A = a[1]; m = Q;
	print_frame();
	print_all();
	show();
	thread th{show_time};
	for(char c; (c = getkey()) != 'q'; ) {
		switch(c) {
			case 'd': if(!Q[x][y]) m[x][y] = 0;
					  print_xy(0, x, y, {0, 0, 0});
					  show();
					  break;
			case 'c': toggle_num_shape = !toggle_num_shape; break;
			case 'm': match = !match, print_all(), show();
		}
		if(c >= '1' && c <= '9' && !Q[x][y]) { 
			toggle[x][y] = toggle_num_shape, m[x][y] = c - '0';
			print_xy(m[x][y], x, y, get_color(x, y));
			if(m == A) complete();
			show();
		}
	}
	finished = true;
	th.join();
}

