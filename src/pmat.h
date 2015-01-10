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
  Pmat(int M, int N);
  Pmat(int M, int N, T v);
  
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
	  cout << 'r' << r << "\t" << array[r].size() << "\t" << ma[r].size() << endl;
	  array[r].resize(ma[r].size());
	  cout << "resize"  << array[r].size() << "\t" << ma[r].size() << endl;
	  for (int c = 0; c < ma[r].size(); ++c) {
		cout << 'c' << c << endl;
		array[r][c] = ma[r][c];
	  }
	}
  }
  
  void resize(int M, int N);
  void resize(int M, int N, T v);
  void rand_init();				// each entry (0,1]
  void load(const string& inf);
  void loadFileStream(ifstream& rf);
  void load_tmat(const string& inf);
  
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

  Pmat<T> operator+(const T & v);
  Pmat<T> operator+(const Pmat<T>& v);
  Pmat<T>& operator+=(const T & v);
  Pmat<T>& operator+=(const Pmat<T>& v);
  Pmat<T> operator-(const T & v);
  Pmat<T> operator-(const Pmat<T>& v);
  Pmat<T>& operator-=(const T & v);
  Pmat<T>& operator-=(const Pmat<T>& v);
  Pmat<T> operator*(const T & v);
  Pmat<T>& operator*=(const T & v);  
  Pmat<T> operator/(const T & v);
  Pmat<T>& operator/=(const T& v);
  
  void add_row(const Pvec<T>& v) {array.push_back(v);}
  void push_back(const Pvec<T>& v) {array.push_back(v);}
  void add_col(const Pvec<T>& v);
  // erase array[start:end)
  void erase_row(int start, int end);
  void erase_col(int start, int end);
  void clear() {array.clear();}
  
  T sum();                // sum of all elements
  T norm();
  void normalize(); 	  // normalize to all entries sum to 1
  void normr(double c=0);	  // row sum 1 normalization
  void normc();	  // col sum 1 normalization

  void add1_log();
  
  Pmat<T> transpose() const;
  string str();  
  void write(const string& pt);
};


template<class T>
Pmat<T>::Pmat(int M, int N):array(M) {
  for (int i = 0; i < M; i++)
	array[i].resize(N);
}

template<class T>
Pmat<T>::Pmat(int M, int N, T v):array(M) {
  for (int i = 0; i < M; i++)
	array[i].resize(N, v);
}

template<class T>
void Pmat<T>::resize(int M, int N) {
  array.resize(M);
  for (int i = 0; i < M; i++)
	array[i].resize(N);
}

template<class T>
void Pmat<T>::resize(int M, int N, T v) {
  array.resize(M);
  for (int i = 0; i < M; i++)
	array[i].resize(N, v);
}

template<class T>
void Pmat<T>::rand_init() {
  for (int i = 0 ; i < rows() ; ++i)
	array[i].rand_init();
}

// input format: v v v ...
// dimensions of the matrix are determinated by input data
template<class T>
void Pmat<T>::load(const string& inf) {
  ifstream rf(inf.c_str());
  if (!rf) 
	EXIT_ERR("file not find:", inf.c_str());
  
  loadFileStream(rf);
}

template<class T>
void Pmat<T>::loadFileStream(ifstream& rf) {
  string line;
  while (getline(rf, line)) {
	// add a new row
	Pvec<T> r(line);
	add_row(r);
  }
}

// load a transpose matrix
template<class T>
void Pmat<T>::load_tmat(const string& inf) {
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

template<class T>
Pmat<T> Pmat<T>::operator+(const T & v){
  Pmat<T> tp(this->rows(), this->cols());
  for (int i=0; i<this->rows(); ++i)
	tp[i] = array[i] + v;
  return tp;
}

template<class T>
Pmat<T>& Pmat<T>::operator+=(const T & v){
  for (int i=0; i<this->rows(); ++i)
	array[i] += v;
  return *this;
}

template<class T>
Pmat<T> Pmat<T>::operator+(const Pmat<T>& v){
  Pmat<T> tp(this->rows(), this->cols());
  for (int i=0; i<this->rows(); ++i)
	tp[i] = array[i] + v[i];
  return tp;
}

template<class T>
Pmat<T>& Pmat<T>::operator+=(const Pmat<T>& v){
  for (int i=0; i<this->rows(); ++i)
	array[i] += v[i];
  return *this;
}


template<class T>
Pmat<T> Pmat<T>::operator-(const T & v){
  Pmat<T> tp(this->rows(), this->cols());
  for (int i=0; i<this->rows(); ++i)
	tp[i] = array[i] - v;
  return tp;
}

template<class T>
Pmat<T>& Pmat<T>::operator-=(const T & v){
  for (int i=0; i<this->rows(); ++i)
	array[i] -= v;
  return *this;
}

template<class T>
Pmat<T> Pmat<T>::operator-(const Pmat<T>& v){
  Pmat<T> tp(this->rows(), this->cols());
  for (int i=0; i<this->rows(); ++i)
	tp[i] = array[i] - v[i];
  return tp;
}

template<class T>
Pmat<T>& Pmat<T>::operator-=(const Pmat<T>& v){
  for (int i=0; i<this->rows(); ++i)
	array[i] -= v[i];
  return *this;
}

template<class T>
Pmat<T> Pmat<T>::operator*(const T & v){
  Pmat<T> tp(this->rows(), this->cols());
  for (int i=0; i<this->rows(); ++i)
	tp[i] = array[i] * v;
  return tp;
}

template<class T>
Pmat<T>& Pmat<T>::operator*=(const T & v){
  for (int i=0; i<this->rows(); ++i)
	array[i] *= v;
  return *this;
}

template<class T>
Pmat<T> Pmat<T>::operator/(const T & v){
  Pmat<T> tp(this->rows(), this->cols());
  for (int i=0; i<this->rows(); ++i)
	tp[i] = array[i] / v;
  return tp;
}

template<class T>
Pmat<T>& Pmat<T>::operator/=(const T & v){
  for (int i=0; i<this->rows(); ++i)
	array[i] /= v;
  return *this;
}

template<class T>
void Pmat<T>::add_col(const Pvec<T>& v) {
  if (array.size() == 0)
	array.resize(v.size());
  else
	assert(array.size() == v.size());

  for (int i=0; i<array.size(); ++i)
	array[i].push_back(v[i]);
}

template<class T>
void Pmat<T>::erase_row(int start, int end) {
  assert(end >= start && end <= array.size());
  array.erase(array.begin() + start, array.begin() + end);
}

template<class T>
void Pmat<T>::erase_col(int start, int end) {
  for (int r = 0; r < array.size(); ++r) {
	assert(end >= start && end <= array[r].size());
	array[r].erase(array[r].begin() + start, array[r].begin() + end);
  }
}
	
template<class T>
T Pmat<T>::sum() {
  T s=0;
  for (int i = 0 ; i < rows() ; ++i)
	s += array[i].sum();
  return s;
}

template<class T>
T Pmat<T>::norm() {
  T s=0;
  for (int i = 0 ; i < rows(); ++i)
	for (int j = 0; j < cols(); ++j)
	  s += array[i][j] * array[i][j];
  return sqrt(s);
}

template<class T>
void Pmat<T>::normalize() {
  double eps = 1e-30;
  double smoother = eps * rows() * cols();
  
  T s = this->sum();
  for (int i=0 ; i<rows() ; ++i)
	for (int j=0; j<cols() ; ++j)
	  array[i][j] = (array[i][j] + eps)/(s + smoother);
}

// normalize elements in a row
template<class T>
void Pmat<T>::normr(double c) {
  for (int i = 0 ; i < rows() ; ++i)
	array[i].normalize(c);
}

// normalize elements in a row
template<class T>
void Pmat<T>::normc() {
  for (int j = 0; j < cols() ; ++j) {
	T s=0;
	for (int i = 0; i < rows(); ++i) 
	  s += array[i][j];
	
	for (int i = 0; i < rows() ; ++i)
	  array[i][j] /= s;
  }
}

template<class T>
void Pmat<T>::add1_log() {
  for (int i=0; i<rows(); ++i)
	array[i].add1_log();
}

template<class T>
Pmat<T> Pmat<T>::transpose() const {
  int N = rows();
  int M = cols();
  Pmat<T> tmat(M, N);

  for (int i=0; i<N; ++i)
	for (int j=0; j<M; ++j)
	  tmat[j][i] = array[i][j];

  return tmat;
}

template<class T>
string Pmat<T>::str() {
  ostringstream os;
  int i;
  for (i=0; i<rows(); ++i) 
	os << array[i].str() << endl;

  return os.str();
}

template<class T>
void Pmat<T>::write(const string& pt) {
  ofstream wf(pt.c_str());
  if (!wf) {
	cout << "Path not exists:" << pt << endl;
	exit(-1);
  }
  
  for (int i=0; i<rows(); ++i) 
	wf << array[i].str() << endl;
}

#endif
