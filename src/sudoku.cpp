#include<algorithm>
#include<tuple>
#include<random>
#include<iostream>
#include<map>
#include<vector>
#include<opencv2/opencv.hpp>
#include"base.h"
using namespace std;
using namespace cv;

Cmat<int, 9, 9> m, Q, A, toggle;
int solution = 0;

vector<int> possible(int x, int y)
{//return possible numbers at x, y
	int tmp = m[x][y]; m[x][y] = 0;
	bool exist[10] = {false, };
	for(int i=0; i<9; i++) exist[m[i][y]] = true;
	for(int i=0; i<9; i++) exist[m[x][i]] = true;
	for(int i=0; i<3; i++) for(int j=0; j<3; j++)
		exist[m[x/3*3 + i][y/3*3 + j]] = true;
	m[x][y] = tmp;

	vector<int> v;
	for(int i=1; i<10; i++) if(!exist[i]) v.push_back(i);
	return v;
}

void init() 
{//distribute numbers on board randomly
	uniform_int_distribution<int> di1{1, 9}, di2{0, 2};
	random_device rd;
	m.O();
	for(int i=0, n=10; i<9; i++) for(int j=0; j<9; j++) if(!di2(rd)) {//init
		auto v = possible(i, j);
		if(!v.size()) m.O(), i = 0, j = 0;//same as restarting
		else {
			while(n > v.size() - 1) n = di1(rd) - 1;
			m[i][j] = v[n];
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
		if(!m[i][j]) {
			auto v = possible(i, j);
			vv.push_back({i, j, v});
			for(int n : v) numNfreq[n]++;
		} else vn.push_back(m[i][j]);
	}
	for(auto [num, freq] : numNfreq) if(freq == 1) {
		for(const auto &[x, y, v] : vv) 
			if(find(v.begin(), v.end(), num) != v.end()) m[x][y] = num;
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
			if(!m[i][j]) {
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
			A = m;
			return;
		} else x = 0, y++;
	}
	if(m[x][y]) recur(x + 1, y);
	else {
		for(int i : possible(x, y)) {
			m[x][y] = i;
			recur(x + 1, y);
		}
		m[x][y] = 0;
	}
}

bool toggle_num_shape = false;
void print_sudoku(int x, int y)
{//x, y is current cursor position
	const char *unicode[] = {"\u2780","\u2781","\u2782","\u2783","\u2784",
		"\u2785","\u2786","\u2787","\u2788","\u2789"};
	const char *black_circle2[] = {"\u2776", "\u2777", "\u2778", "\u2779", "\u277a",
		"\u277b", "\u277c", "\u277d", "\u277e", "\u277f"};
	const char *black_circle[] = {"\u278a", "\u278b", "\u278c", "\u278d", "\u278e",
		"\u278f", "\u2790", "\u2791", "\u2792", "\u2793"}; 
	for(int i=0; i<40; i++) cout << '\n';
	for(int j=0; j<9; j++) {
		if(j == 3 || j == 6) cout << "\n---+---+---";
		cout << '\n';
		for(int i=0; i<9; i++) {
			if(i == 3 || i == 6) cout << '|';
			if(!m[i][j]) {
				if(i == x && j == y) cout << "\u25ef";
				else cout << ' ';
			} else if(i == x && j == y) {
				if(!toggle[i][j]) cout << unicode[m[i][j] - 1];
				else cout << unicode[m[i][j] - 1];
			} else {
				if(!toggle[i][j]) cout << m[i][j];
				else cout << black_circle[m[i][j] - 1];
			}
		}
	}
	cout << "\nhjkl to move, d to delete,\n"
		"c to toggle chinese, q to quit, m to match. enter?";
}

Mat M{540, 540, CV_8UC3, {255,255,255}};
bool match = false;
void cv_print_sudoku(int x, int y)
{//opencv 
	M = Scalar(255,255,255);
	circle(M, {30+60*x, 30+60*y}, 25, {0,0,0}, 1);
	for(int i=1; i<9; i++) {//draw lines
		line(M, {i * 60, 0}, {i * 60, 540}, {0,0,0}, i % 3 ? 1 : 3, LINE_8);
		line(M, {0, i * 60}, {540, i * 60}, {0,0,0}, i % 3 ? 1 : 3, LINE_8);
	}
	for(int i=0; i<9; i++) for(int j=0; j<9; j++) if(Scalar color; m[i][j]) {
		if(Q[i][j]) color = {0,0,0};
		else if(match && A[i][j] != m[i][j]) color = {0,0,255};
		else color = toggle[i][j] ? Scalar{0,255,0} : Scalar{255,0,0};
		putText(M, to_string(m[i][j]), {10+60*i, 50+60*j}, FONT_HERSHEY_PLAIN, 4,
				color, 2);
	}
	if(m == A)
		putText(M, "You solved", {50, 300}, FONT_HERSHEY_PLAIN, 5, {0,120,255}, 5);
	imshow("sudoku", M);
}
int main(int ac, char **av)
{//generate sudoku problem and solve it.
	for(int tries=1, not_solved=0; solution != 1; tries++, not_solved=0) {
		init(); solution = 0; Q = m; 
		cout << "trying " << tries << '\n' << Q;

		if(!evident_solve()) continue;
		cout << "evident" << '\n' << m;
		for(int i=0; i<9; i++) for(int j=0; j<9; j++) if(!m[i][j]) not_solved++;
		if(not_solved > 50) continue;//take too much time for brute force
		
		recur(0, 0);//brute force
		cout << "solution " << solution << '\n';
	}
	cout << A;
	cout << "\nhjkl to move, 1-9 to enter numbers d to delete,\n"
		"c to toggle color, q to quit, m to match.";

	int x = 0, y = 0;
	m = Q;
	for(char c; (c = waitKey()) != 'q'; ) {
		switch(c) {
			case 'h': if(x) x--; break;
			case 'k': if(y) y--; break;
			case 'j': if(y != 8) y++; break;
			case 'l': if(x != 8) x++; break;
			case 'd': if(!Q[x][y]) m[x][y] = 0; break;
			case 'c': toggle_num_shape = !toggle_num_shape; break;
		}
		if(c == 'm') match = true;
		else match = false;
		if(c >= '1' && c <= '9' && !Q[x][y])  
			toggle[x][y] = toggle_num_shape, m[x][y] = c - '0';
		cv_print_sudoku(x, y);
	}
}

