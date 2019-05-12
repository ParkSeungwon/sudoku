#include<iostream>
#include<tuple>
#include<algorithm>
#include<map>
#include<random>
#include"sudoku.h"
using namespace std;

vector<int> Sudoku::possible(int x, int y)
{//return possible numbers at x, y
	int tmp = (*this)[x][y]; (*this)[x][y] = 0;
	bool exist[10] = {false, };
	for(int i=0; i<9; i++) exist[(*this)[i][y]] = true;
	for(int i=0; i<9; i++) exist[(*this)[x][i]] = true;
	for(int i=0; i<3; i++) for(int j=0; j<3; j++)
		exist[(*this)[x/3*3 + i][y/3*3 + j]] = true;
	(*this)[x][y] = tmp;

	vector<int> v;
	for(int i=1; i<10; i++) if(!exist[i]) v.push_back(i);
	return v;
}

void Sudoku::init() 
{//distribute numbers on board randomly
	uniform_int_distribution<int> di1{1, 9}, di2{0, 2};
	random_device rd;
	O();
	for(int i=0, n=10; i<9; i++) for(int j=0; j<9; j++) if(!di2(rd)) {//init
		auto v = possible(i, j);
		if(!v.size()) O(), i = 0, j = 0;//same as restarting
		else {
			while(n > v.size() - 1) n = di1(rd) - 1;
			(*this)[i][j] = v[n];
		}
		n = 10;
	}
}

int Sudoku::evident(int x1, int x2, int y1, int y2)//vertical, horizontal, 3x3 exclusive solve
{//return 0 : no more possible solve, -1 : not solvable, + : count of solved
	vector<tuple<int, int,vector<int>>> vv;//x,y,possible numbers 
	map<int, int> numNfreq;
	vector<int> vn;//numbers already solved and given
	int r = 0;
	for(int i=x1; i<=x2; i++) for(int j=y1; j<=y2; j++) {
		if(!(*this)[i][j]) {
			auto v = possible(i, j);
			vv.push_back({i, j, v});
			for(int n : v) numNfreq[n]++;
		} else vn.push_back((*this)[i][j]);
	}
	for(auto [num, freq] : numNfreq) if(freq == 1) {
		for(const auto &[x, y, v] : vv) 
			if(find(v.begin(), v.end(), num) != v.end()) (*this)[x][y] = num;
		r++;
	}
	for(int i=1; i<10; i++) 
		if(!numNfreq[i] && find(vn.begin(), vn.end(), i) == vn.end()) r = -1;
	return r;
}
bool Sudoku::evident_solve()
{//repeat evident solve for every row and column and 3x3, return false if not solvable
	int filled = 0;//filled : count of filled numbers this turn
	do {
		filled = 0;
		for(int i=0; i<9; i++) for(int j=0; j<9; j++) {
			if(!(*this)[i][j]) {
				auto v = possible(i, j);
				if(v.empty()) return false;
				if(v.size() == 1) (*this)[i][j] = v[0], filled++;
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

void Sudoku::recur(int x, int y)
{//recursively solve sudoku, brute force
	if(solution > 1) return;//abort if more than 2 Answer is possible
	if(x == 9) {//recalculate x,y border
		if(y == 8) {
			solution++;
			A = *this;
			return;
		} else x = 0, y++;
	}
	if((*this)[x][y]) recur(x + 1, y);
	else {
		for(int i : possible(x, y)) {
			(*this)[x][y] = i;
			recur(x + 1, y);
		}
		(*this)[x][y] = 0;
	}
}

Sudoku::Sudoku()
{
	for(int tries=1, not_solved=0; solution != 1; tries++, not_solved=0) {
		init(); solution = 0; Q = *this; 
		cout << "trying " << tries << '\n' << Q;

		if(!evident_solve()) continue;
		cout << "evident" << '\n' << m;
		for(int i=0; i<9; i++) for(int j=0; j<9; j++) if(!(*this)[i][j]) not_solved++;
		if(not_solved > 50) continue;//take too much time for brute force
		
		recur(0, 0);//brute force
		cout << "solution " << solution << '\n';
	}
	cout << A;
	A = *this;
	cout << "\n1-9 to enter numbers d to delete,\n"
		"c to toggle color, q to quit, m to match." << endl;
}
