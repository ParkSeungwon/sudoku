#include<thread>
#include<algorithm>
#include<tuple>
#include<random>
#include<iostream>
#include<map>
#include<vector>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat_<int> m(9,9), Q(9,9), A(9,9), toggle(9,9);//Q question, A answer
int solution = 0;

vector<int> possible(int x, int y)
{//return possible numbers at x, y
	int tmp = m.at<int>(x,y); m.at<int>(x,y) = 0;
	bool exist[10] = {false, };
	for(int i=0; i<9; i++) exist[m.at<int>(i,y)] = true;
	for(int i=0; i<9; i++) exist[m.at<int>(x,i)] = true;
	for(int i=0; i<3; i++) for(int j=0; j<3; j++)
		exist[m[x/3*3 + i][y/3*3 + j]] = true;
	m.at<int>(x,y) = tmp;

	vector<int> v;
	for(int i=1; i<10; i++) if(!exist[i]) v.push_back(i);
	return v;
}

void init() 
{//distribute numbers on board randomly
	uniform_int_distribution<int> di1{1, 9}, di2{0, 2};
	random_device rd;
	m = 0;
	for(int i=0, n=10; i<9; i++) for(int j=0; j<9; j++) if(!di2(rd)) {//init
		auto v = possible(i, j);
		if(!v.size()) m = 0, i = 0, j = 0;//same as restarting
		else {
			while(n > v.size() - 1) n = di1(rd) - 1;
			m.at<int>(i,j) = v[n];
		}
		n = 10;
	}
}

int evident(int x1, int x2, int y1, int y2)//vertical, horizontal, 3x3 exclusive solve
{//return 0 : no more possible solve, -1 : not solvable, + : count of solved
	vector<tuple<int, int,vector<int>>> vv;//x,y,possible numbers 
	map<int, int> numNfreq;
	vector<int> vn;//numbers already solved and given
	int r = 0;
	for(int i=x1; i<=x2; i++) for(int j=y1; j<=y2; j++) {
		if(!m.at<int>(i,j)) {
			auto v = possible(i, j);
			vv.push_back({i, j, v});
			for(int n : v) numNfreq[n]++;
		} else vn.push_back(m.at<int>(i,j));
	}
	for(auto [num, freq] : numNfreq) if(freq == 1) {
		for(const auto &[x, y, v] : vv) 
			if(find(v.begin(), v.end(), num) != v.end()) m.at<int>(x,y) = num;
		r++;
	}
	for(int i=1; i<10; i++) 
		if(!numNfreq[i] && find(vn.begin(), vn.end(), i) == vn.end()) r = -1;
	return r;
}
bool evident_solve()
{//repeat evident solve for every row and column and 3x3, return false if not solvable
	int filled = 0;//filled : count of filled numbers this turn
	do {
		filled = 0;
		for(int i=0; i<9; i++) for(int j=0; j<9; j++) {
			if(!m.at<int>(i,j)) {
				auto v = possible(i, j);
				if(v.empty()) return false;
				if(v.size() == 1) m[i][j] = v[0], filled++;
			}
		}
		for(int i=0; i<9; i++) {
			if(int n = evident(i, i, 0, 8); n < 0) return false;
			else filled += n;
			if(int n = evident(0, 8, i, i); n < 0) return false;
			else filled += n;
		}
		for(int i=0; i<9; i+=3) for(int j=0; j<9; j+=3)
			if(int n = evident(i, i + 2, j, j + 2); n < 0) return false;
			else filled += n;
	} while(filled > 0);
	return true;
}

void recur(int x, int y)
{//recursively solve sudoku, brute force
	if(solution > 1) return;//abort if more than 2 Answer is possible
	if(x == 9) {//recalculate x,y border
		if(y == 8) {
			solution++;
			m.copyTo(A);
			return;
		} else x = 0, y++;
	}
	if(m.at<int>(x,y)) recur(x + 1, y);
	else {
		for(int i : possible(x, y)) {
			m.at<int>(x,y) = i;
			recur(x + 1, y);
		}
		m.at<int>(x,y) = 0;
	}
}

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

Mat M{scale*9, scale*9, CV_8UC3, {255,255,255}};
int x, y;
void callback(int event, int i, int j, int, void*)
{
	if(event == EVENT_MOUSEMOVE) x = i/scale, y = j/scale;
}
void cv_print_sudoku()
{//opencv 
	M = Scalar(255,255,255);
	for(int i=1; i<9; i++) {//draw lines
		line(M, {i * scale, 0}, {i * scale, scale*9}, {0,0,0}, i % 3 ? 1 : 3, LINE_8);
		line(M, {0, i * scale}, {scale*9, i * scale}, {0,0,0}, i % 3 ? 1 : 3, LINE_8);
	}
	for(int i=0; i<9; i++) for(int j=0; j<9; j++) if(Scalar color; m.at<int>(i,j)) {
		if(Q.at<int>(i,j)) color = {0,0,0};
		else if(match && A.at<int>(i,j) != m.at<int>(i,j)) color = {0,0,255};
		else color = toggle.at<int>(i,j) ? Scalar{0,255,0} : Scalar{255,0,0};
		putText(M, to_string(m.at<int>(i,j)), {10 + scale*i, scale - 10 + scale*j},
				FONT_HERSHEY_PLAIN, scale / 10 - 2, color, 2);
	}
	if(!countNonZero(m != A)) {//m == A
		putText(M, "You solved", {scale, scale*5}, FONT_HERSHEY_PLAIN, scale/12, {120,120,255}, 5);
		finished = true;
	}
	imshow("sudoku", M);
}

int main(int ac, char **av)
{//generate sudoku problem and solve it.
	for(int tries=1, not_solved=0; solution != 1; tries++, not_solved=0) {
		init(); solution = 0;  m.copyTo(Q); 
		cout << "trying " << tries << '\n' << Q << '\n';

		if(!evident_solve()) continue;
		cout << "evident" << '\n' << m << '\n';
		for(int i=0; i<9; i++) for(int j=0; j<9; j++)
			if(!m.at<int>(i,j)) not_solved++;
		if(not_solved > 50) continue;//take too much time for brute force
		
		recur(0, 0);//brute force
		cout << "solution " << solution << '\n';
	}
	cout << A << '\n';
	cout << "\n1-9 to enter numbers d to delete,\n"
		"c to toggle color, q to quit, m to match." << endl;

	namedWindow("sudoku");
	setMouseCallback("sudoku", callback);
	Q.copyTo(m);
	cv_print_sudoku();
	thread th{show_time};
	for(char c; (c = waitKey()) != 'q'; ) {
		switch(c) {
			case 'd': if(!Q.at<int>(x,y)) m.at<int>(x,y) = 0; break;
			case 'c': toggle_num_shape = !toggle_num_shape; break;
			case 'm': match = !match;
		}
		if(c >= '1' && c <= '9' && !Q.at<int>(x,y))  
			toggle.at<int>(x,y) = toggle_num_shape, m.at<int>(x,y) = c - '0';
		cv_print_sudoku();
	}
	finished = true;
	th.join();
}

