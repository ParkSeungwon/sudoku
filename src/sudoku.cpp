#include<algorithm>
#include<tuple>
#include<random>
#include<iostream>
#include<map>
#include<vector>
#include"base.h"
using namespace std;

Cmat<int, 9, 9> m, Q, A;
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
{
	uniform_int_distribution<int> di1{1, 9}, di2{0, 2};
	random_device rd;
	m.O();
	for(int i=0, n=10; i<9; i++) for(int j=0; j<9; j++) if(!di2(rd)) {//init
		auto v = possible(i, j);
		if(!v.size()) m.O(), i = 0, j = 0;
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
	int filled = 0;
	do {//filled : count of filled numbers this turn
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
		for(int p=0; p<9; p+=3) for(int q=0; q<9; q+=3)
			if(int n = evident(p, p + 2, q, q + 2); n < 0) return false;
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

int main(int ac, char **av)
{//generate sudoku problem and solve it.
	for(int k = 1, not_solved=0; solution != 1; k++, not_solved=0) {
		init(); solution = 0; Q = m; 
		cout << "trying " << k << '\n' << Q;

		if(!evident_solve()) continue;
		cout << "evident" << '\n' << m;
		for(int i=0; i<9; i++) for(int j=0; j<9; j++) if(!m[i][j]) not_solved++;
		if(not_solved > 50) continue;//take too much time for brute force
		
		recur(0, 0);//brute force
		cout << "solution " << solution << '\n';
	}
	cout << A;
}

