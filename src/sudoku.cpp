#include<iostream>
#include<thread>
#include"base.h"
using namespace std;

array<Cmat<int, 9, 9>, 2> sudokuQA();
void print_frame(), show(), complete();
void print_xy(int number, int x, int y, array<uint8_t, 3> rgb);
char getkey();
Cmat<int, 9, 9> toggle, Q, A, m;
bool finished = false, toggle_color = false, match = false;

void show_time()
{
	for(int i=0; !finished; i++) {
		this_thread::sleep_for(1s);
		cout << i/60 << "분" << i%60 << "초 경과\r" << flush;
	}
	cout << endl;
}
array<uint8_t, 3> get_color(int i, int j)
{//return rgb color according to circumstances
	if(Q[i][j]) return {0,0,0};
	if(match && A[i][j] != m[i][j]) return {255,0,0};
	if(toggle[i][j]) return {0,255,0};
	return {0,0, 255};
}


void print_all()
{//opencv 
	for(int i=0; i<9; i++) for(int j=0; j<9; j++) if(m[i][j])
		print_xy(m[i][j], i, j, get_color(i, j));
	if(m == A) complete();
	show();
}

int main()
{//generate sudoku problem and solve it.
	extern int x, y;
	auto [q, a] = sudokuQA();
	m = Q = move(q); A = move(a);
	print_frame();
	print_all();
	thread th{show_time};
	for(char c; (c = getkey()) != 'q'; ) {
		switch(c) {
			case 'd': if(!Q[x][y]) m[x][y] = 0;
					  print_xy(0, x, y, {0, 0, 0});
					  show();
					  break;
			case 'c': toggle_color = !toggle_color; break;
			case 'm': match = !match;
					  print_all();
		}
		if(c >= '1' && c <= '9' && !Q[x][y]) { 
			toggle[x][y] = toggle_color;
			m[x][y] = c - '0';
			print_xy(m[x][y], x, y, get_color(x, y));
			if(m == A) complete(), finished = true;
			show();
		}
	}
	finished = true;
	th.join();
}

