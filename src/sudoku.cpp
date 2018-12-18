#include<random>
#include<iostream>
#include<vector>
#include"base.h"
using namespace std;

Cmat<int, 9, 9> m, Q, A;
uniform_int_distribution di1{1, 9}, di2{0, 2};
random_device rd;
int solution = 0;

vector<int> available(int x, int y)
{//return available numbers at x, y
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

void recur(int x, int y)
{//recursive solve sudoku
	if(solution > 1) return;//abort
	if(x == 9) {//recalculate x,y border
		if(y == 8) {
			solution++;
			A = m;
			return;
		} else x = 0, y++;
	}
	if(m[x][y]) recur(x + 1, y);
	else {
		for(int i : available(x, y)) {
			m[x][y] = i;
			recur(x + 1, y);
		}
		m[x][y] = 0;
	}
}

int main(int ac, char **av)
{
	for(int k = 1; solution != 1; k++) {
		m.O();
		for(int i=0, n=10; i<9; i++) for(int j=0; j<9; j++) if(!di2(rd)) {//init
			auto v = available(i, j);
			if(!v.size()) m.O(), i = 0, j = 0;
			else {
				while(n > v.size() - 1) n = di1(rd) - 1;
				m[i][j] = v[n];
			}
			n = 10;
		}
		solution = 0;
		Q = m; 
		cout << "trying " << k << endl << Q << flush;

		vector<seq<int, int,vector<int>>> vv;
		map<int, int> numNfreq;
		for(bool again = true; again;) {//evident solve
			again = false;
			for(int i=0; i<9; i++) for(int j=0; j<9; j++) {
				if(!m[i][j]) {
					auto v = available(i, j);
					if(v.size() == 1) m[i][j] = v[0], again = true;
				}
			}
			for(int p=0; p<3; p++) for(int q=0; q<3; q++) {
				for(int i=0; i<3; i++) for(int j=0; j<3; j++) {
					int x = p * 3 + i; int y = q * 3 + j;
					if(!m[x][y]) {
						auto v = available(x, y);
						vv.push_back(x, y, v);
						for(int n : v) numNfreq[n]++;
					}
				}
				for([int num, int freq] : numNfreq) if(freq == 1) {
					again = true;
					for([int x, int y, auto v] : vv) 
						if(find(v.begin(), v.end(), num) != v.end()) m[x][y] = num;
				}
			}
		}
		cout << "evident" << endl << m << flush;

		recur(0, 0);//brute force
		cout << "solution " << solution << endl;
	}
	cout << A << endl;
}

