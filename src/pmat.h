/*
 * (dense) Positive Matrix, each row is a Pvec
 *
 *  Created on: 2012-7-31
 *      Author: xhcloud@gmail.com
 */
#ifndef _PMAT_H
#define _PMAT_H

#include <vector>
#include <string>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <exception>
#include <ctime>

#include "pvec.h"

#define EXIT_ERR(s1, s2) {printf("[Error] %s%s\n", s1, s2);	\
	exit(EXIT_FAILURE);}

using namespace std;

template<class T>
class Pmat
{
private:
  vector<Pvec<T> > array;

public:
  Pmat(){}

  Pmat(int M, int N):array(M) {
	for (int i = 0; i < M; i++)
	  array[i].resize(N);
  }

  Pmat(int M, int N, T v):array(M) {
	for (int i = 0; i < M; i++)
	  array[i].resize(N, v);
  }

  template<class T2>
  Pmat(const Pmat<T2>& ma) {
	resize(ma.rows(), ma.cols());
	for (int m = 0; m < ma.rows(); ++m)
	  for (int n = 0; n < ma.cols(); ++n)
		array[m][n] = ma[m][n];
  }

  template<class T2>
  Pmat(const vector<vector<T2> >& ma) {
	array.resize(ma.size());
	for (int r = 0; r < ma.size(); ++r) {
	  array[r].resize(ma[r].size());
	  for (int c = 0; c < ma[r].size(); ++c) {
		array[r][c] = ma[r][c];
	  }
	}
  }

  void resize(int M, int N) {
	array.resize(M);
	for (int i = 0; i < M; i++)
	  array[i].resize(N);
  }

  void resize(int M, int N, T v) {
	array.resize(M);
	for (int i = 0; i < M; i++)
	  array[i].resize(N, v);
  }

  void rand_init() {
	for (int i = 0 ; i < rows() ; ++i)
	  array[i].rand_init();
  }

  void fill(T v) {
	for (int i = 0; i < array.size(); ++i)
	  array[i].fill(v);
  }

  // input format: v v v ...
  // dimensions of the matrix are determinated by input data
  void load(const string& inf) {
	ifstream rf(inf.c_str());
	if (!rf) 
	  EXIT_ERR("file not find:", inf.c_str());
  
	loadFileStream(rf);
  }

  void loadFileStream(ifstream& rf) {
	string line;
	while (getline(rf, line)) {
	  // add a new row
	  Pvec<T> r(line);
	  add_row(r);
	}
  }

  // load a transpose matrix
  void load_tmat(const string& inf) {
	ifstream rf(inf.c_str());
	if (!rf) 
	  EXIT_ERR("file not find:", inf.c_str());
  
	try {
	  string line;
	  while (getline(rf, line)) {
		// add a new row
		Pvec<T> r(line);
		add_col(r);
	  }
	}
	catch (...) {
	  EXIT_ERR("Err file:", inf.c_str());
	}
  }
  
  const int rows() const {return array.size();}
  const int size() const {return rows();}
  const int cols() const {return rows()?array[0].size():0;}

  Pvec<T> &operator[] (int m){
	if (m >= array.size())
	  cout << "ERR Row(i):" << m << ' ' << array.size() << endl;
	return array[m];
  }

  const Pvec<T> &operator[] (int m) const {return array[m];}

  template<class T2>
  Pmat<T>& operator=(const vector<vector<T2> >& ma) {
	array.resize(ma.size());
	for (int i = 0; i < array.size(); ++i) {
	  array[i].resize(ma[i].size());
	  for (int j = 0; j < array[i].size(); ++j)
		array[i][j] = ma[i][j];
	}
	return *this;
  }

  Pmat<T> operator+(const T & v){
	Pmat<T> tp(this->rows(), this->cols());
	for (int i=0; i<this->rows(); ++i)
	  tp[i] = array[i] + v;
	return tp;
  }

  Pmat<T>& operator+=(const T & v){
	for (int i=0; i<this->rows(); ++i)
	  array[i] += v;
	return *this;
  }

  Pmat<T> operator+(const Pmat<T>& v){
	Pmat<T> tp(this->rows(), this->cols());
	for (int i=0; i<this->rows(); ++i)
	  tp[i] = array[i] + v[i];
	return tp;
  }

  Pmat<T>& operator+=(const Pmat<T>& v){
	for (int i=0; i<this->rows(); ++i)
	  array[i] += v[i];
	return *this;
  }


  Pmat<T> operator-(const T & v){
	Pmat<T> tp(this->rows(), this->cols());
	for (int i=0; i<this->rows(); ++i)
	  tp[i] = array[i] - v;
	return tp;
  }

  Pmat<T>& operator-=(const T & v){
	for (int i=0; i<this->rows(); ++i)
	  array[i] -= v;
	return *this;
  }

  Pmat<T> operator-(const Pmat<T>& v){
	Pmat<T> tp(this->rows(), this->cols());
	for (int i=0; i<this->rows(); ++i)
	  tp[i] = array[i] - v[i];
	return tp;
  }

  Pmat<T>& operator-=(const Pmat<T>& v){
	for (int i=0; i<this->rows(); ++i)
	  array[i] -= v[i];
	return *this;
  }

  Pmat<T> operator*(const T & v){
	Pmat<T> tp(this->rows(), this->cols());
	for (int i=0; i<this->rows(); ++i)
	  tp[i] = array[i] * v;
	return tp;
  }

  Pmat<T>& operator*=(const T & v){
	for (int i=0; i<this->rows(); ++i)
	  array[i] *= v;
	return *this;
  }

  Pmat<T> operator/(const T & v){
	Pmat<T> tp(this->rows(), this->cols());
	for (int i=0; i<this->rows(); ++i)
	  tp[i] = array[i] / v;
	return tp;
  }

  Pmat<T>& operator/=(const T & v){
	for (int i=0; i<this->rows(); ++i)
	  array[i] /= v;
	return *this;
  }

  void add_row(const Pvec<T>& v) {array.push_back(v);}
  void push_back(const Pvec<T>& v) {array.push_back(v);}
  
  void add_col(const Pvec<T>& v) {
	if (array.size() == 0)
	  array.resize(v.size());
	else
	  assert(array.size() == v.size());

	for (int i=0; i<array.size(); ++i)
	  array[i].push_back(v[i]);
  }

  void erase_row(int start, int end) {
	assert(end >= start && end <= array.size());
	array.erase(array.begin() + start, array.begin() + end);
  }

  void erase_col(int start, int end) {
	for (int r = 0; r < array.size(); ++r) {
	  assert(end >= start && end <= array[r].size());
	  array[r].erase(array[r].begin() + start, array[r].begin() + end);
	}
  }

  void clear() {array.clear();}

  T sum() {
	T s=0;
	for (int i = 0 ; i < rows() ; ++i)
	  s += array[i].sum();
	return s;
  }
  
  Pvec<T> rowSum() {
	Pvec<T> s(array.size());
	for (int i = 0 ; i < rows() ; ++i)
	  s[i] = array[i].sum();
	return s;
  }

  T norm() {
	T s=0;
	for (int i = 0 ; i < rows(); ++i)
	  for (int j = 0; j < cols(); ++j)
		s += array[i][j] * array[i][j];
	return sqrt(s);
  }

  // normalize to all entries sum to 1
  void normalize() {
	double eps = 1e-30;
	double smoother = eps * rows() * cols();
  
	T s = this->sum();
	for (int i=0 ; i<rows() ; ++i)
	  for (int j=0; j<cols() ; ++j)
		array[i][j] = (array[i][j] + eps)/(s + smoother);
  }

  // normalize elements in a row
  void normr(double c = 0.0) {
	for (int i = 0 ; i < rows() ; ++i)
	  array[i].normalize(c);
  }

  // normalize elements in a row
  void normc(double c = 0.0) {
	for (int j = 0; j < cols() ; ++j) {
	  T s=0;
	  for (int i = 0; i < rows(); ++i) 
		s += array[i][j] + c;
	
	  for (int i = 0; i < rows() ; ++i)
		array[i][j] = (array[i][j] + c)/s;
	}
  }

  void add1_log() {
	for (int i=0; i<rows(); ++i)
	  array[i].add1_log();
  }
  
  Pmat<T> transpose() const {
	int N = rows();
	int M = cols();
	Pmat<T> tmat(M, N);

	for (int i=0; i<N; ++i)
	  for (int j=0; j<M; ++j)
		tmat[j][i] = array[i][j];

	return tmat;
  }

  Pmat<double> toDouble() {
	return Pmat<double>(*this);
  }

  string str() {
	ostringstream os;
	int i;
	for (i=0; i<rows(); ++i) 
	  os << array[i].str() << endl;

	return os.str();
  }

  void write(const string& pt) {
	ofstream wf(pt.c_str());
	if (!wf) {
	  cout << "Path not exists:" << pt << endl;
	  exit(-1);
	}
  
	for (int i=0; i<rows(); ++i) 
	  wf << array[i].str() << endl;
  }
};
	
#endif
