#pragma once
#include<vector>
#include<sstream>
#include<iomanip>
#include<cassert>
#include<sstream>
#include<string>
#include<cmath>
#include<type_traits>

template<class T, unsigned W, unsigned H> struct Cmat;
template<class T, unsigned W, unsigned H> struct CmatBase
{//compile time matrix base class
	std::array<std::array<T, H>, W> arr_;//if fundamental type no init
	CmatBase() = default;
	CmatBase(std::initializer_list<T> li) {
		//assert(li.size() == W * H);
		auto it = li.begin();
		for(int j=0; j<H; j++) for(int i=0; i<W; i++) (*this)[i][j] = *it++;
	}
	int width() const { return width_; }
	int height() const { return height_; }

	CmatBase& O() {
		for(int i=0; i<W; i++) for(int j=0; j<H; j++) arr_[i][j] = 0;
		return *this;
	}

	const std::array<T, H>& operator[](unsigned n) const { return arr_[n]; } 
	std::array<T, H>& operator[](unsigned n) { return arr_[n]; }//need arr_ public
	template<unsigned R> Cmat<T,R,H> operator*(const CmatBase<T,R,W>& r) const {
		Cmat<T, R, H> m;
		m.O();
		for(int i=0; i<R; i++) for(int j=0; j<H; j++) 
			for(int k=0; k<W; k++) m[i][j] += (*this)[k][j] * r[i][k];
		return m;
	}
	Cmat<T, W, H> operator+(const CmatBase<T, W, H>& r) const {
		Cmat<T, W, H> m;
		for(int i=0; i<W; i++) for(int j=0; j<H; j++) 
			m[i][j] = (*this)[i][j] + r[i][j];
		return m;
	}
	Cmat<T, W, H> operator-(const CmatBase<T, W, H>& r) const {
		Cmat<T, W, H> m;
		for(int i=0; i<W; i++) for(int j=0; j<H; j++) 
			m[i][j] = (*this)[i][j] - r[i][j];
		return m;
	}
	CmatBase<T, W, H>& operator+=(const CmatBase<T, W, H>& r) {
		return *this = *this + r;
	}
	CmatBase<T, W, H>& operator-=(const CmatBase<T, W, H>& r) {
		return *this = *this - r;
	}
	bool operator==(const CmatBase<T, W, H>& r) const {
		for(int i=0; i<W; i++) for(int j=0; j<H; j++) 
			if((*this)[i][j] != r[i][j]) return false;
		return true;
	}
	bool operator!=(const CmatBase<T, W, H>& r) const{
		return !(*this == r);
	}
	Cmat<T, H, W> transpose() const {
		Cmat<T, H, W> m;
		for(int i=0; i<W; i++) for(int j=0; j<H; j++) m[j][i] = (*this)[i][j];
		return m;
	}
	Cmat<T, W, H> operator*(T mul) const {
		Cmat<T, W, H> m{*this};
		for(int i=0; i<W; i++) for(int j=0; j<H; j++) m[i][j] = m[i][j] * mul;
		return m;
	}
	CmatBase<T, W, H>& operator*=(T mul) {
		return *this = *this * mul;
	}
	Cmat<T, W, H> operator/(T div) const {
		Cmat<T, W, H> m{*this};
		for(int i=0; i<W; i++) for(int j=0; j<H; j++) m[i][j] = m[i][j] / div;
		return m;
	}
	CmatBase<T, W, H>& operator/=(T div) {
		return *this = *this / div;
	}
	friend Cmat<T, W, H> operator*(T mul, const CmatBase<T, W, H>& r) {
		return r * mul;
	}
private:
	int width_ = W;
	int height_ = H;
};

template<class T, unsigned W, unsigned H> struct Cmat : public CmatBase<T,W,H>
{//Cmat template, made base class to avoid repeatition in specialized class definition
	Cmat() = default;
	Cmat(std::initializer_list<T> li) : CmatBase<T,W,H>{li} {}
	Cmat(CmatBase<T, W, H> r) { this->arr_ = r.arr_; }
	Cmat(const Cmat<T, W, H>& r) = default;
};


template <class T, unsigned W, unsigned H>
std::ostream& operator<<(std::ostream& o, const CmatBase<T, W, H>& r)
{//stream output
	int gap[W] {0,};
	for(int y=0; y<H; y++) for(int x=0; x<W; x++) {
		std::stringstream ss;
		ss << r[x][y];
		int sz = ss.str().length();
		if(gap[x] < sz) gap[x] = sz;
	}

	o << "\u23a1" << ' ';
	for(int x=0; x<W; x++) o << std::setw(gap[x]) << r[x][0] << ' ';
	o << "\u23a4" << std::endl;
	for(int y=1; y<H-1; y++) {
		o << "\u23a2" << ' ';
		for(int x=0; x<W; x++) o << std::setw(gap[x]) << r[x][y] << ' ';
		o << "\u23a5" << std::endl;
	}
	if(H > 1) {
		o << "\u23a3" << ' ';
		for(int x=0; x<W; x++) o << std::setw(gap[x]) << r[x][H-1] << ' ';
		o << "\u23a6" << std::endl;
	}
	return o;
}


