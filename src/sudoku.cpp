#include<thread>
#include"sudoku.h"
using namespace std;

bool toggle_num_shape = false, match = false, finished = false;
void show_time()
{
	for(int i=0; !finished; i++) {
		this_thread::sleep_for(1s);
		cout << i/60 << "분" << i%60 << "초 경과\r" << flush;
	}
	cout << endl;
}

int main(int ac, char **av)
{//generate sudoku problem and solve it.
	Sudoku sd;
	cv_print_sudoku();
	thread th{show_time};
	for(char c; (c = waitKey()) != 'q'; ) {
		switch(c) {
			case 'd': if(!Q[x][y]) m[x][y] = 0; break;
			case 'c': toggle_num_shape = !toggle_num_shape; break;
			case 'm': match = !match;
		}
		if(c >= '1' && c <= '9' && !Q[x][y])  
			toggle[x][y] = toggle_num_shape, m[x][y] = c - '0';
		cv_print_sudoku();
	}
	finished = true;
	th.join();
}

