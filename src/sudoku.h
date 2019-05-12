#include<vector>
#include"base.h"

class Sudoku : public Cmat<int, 9, 9>
{
public:
	Sudoku();
	Cmat<int, 9, 9> Q, A;
	
protected:
	int x=0, y=0;
	int solution = 0;
	bool evident_solve();
	void recur();

private:
	std::vector<int> possible(int x, int y);
	void init();
	int evident(int, int, int, int);
};
