/*
 * Positive vector
 */
#ifndef _PVEC_H
#define _PVEC_H
#include <vector>
#include <string>
#include <cassert>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <algorithm> 

#define EXIT_ERR( s1, s2 ) {printf("[Error] %s%s\n", s1, s2);	\
	exit(EXIT_FAILURE);}

using namespace std;

template<class T>
class Pvec {
private:
  vector<T> p;
    
public:
  Pvec(){}
  Pvec(size_t n): p(n) {}
  Pvec(size_t n, T v): p(n, v) {}

  template<class T2>
  Pvec(const vector<T2>& v) {
	for (int i = 0; i < p.size(); ++i)
	  p[i] = v[i];
  }

  template<class T2>
  Pvec(const Pvec<T2>& v) {
	p.resize(v.size());
	for (int i = 0; i < v.size(); ++i)
	  p[i] = v[i];
  }

  Pvec(const string& line) {
	loadString(line);
  }
  
  size_t size() const {return p.size();}
  void resize(size_t n) {p.resize(n);}
  void resize(size_t n, const T& v) {p.resize(n, v);}
  void assign(size_t n, const T& v) {p.assign(n, v);}

  void rand_init() {
	srand(time(NULL));
	for (size_t i = 0; i < p.size(); ++i) 
	  p[i] = rand() % 100 + 1;

	normalize();
  }

  void fill(T v) {
	for( size_t i = 0 ; i < p.size() ; ++i ) 
	  p[i] = v;
  }
  
  void uniform_init() {
	for( size_t i = 0 ; i < p.size() ; ++i ) 
	  p[i] = double(1) / p.size();
  }
  
  // p[0] = v, others is (1-v)/(p.size - 1)
  void bias_init(double v) {
	assert( v < 1);
	p[0] = v;
	for( size_t i = 1 ; i < p.size() ; ++i ) 
	  p[i] = (double)((1-v) / ((int)p.size()-1));
  }
  
  void push_back(T v) {p.push_back(v);}

  void extend(const Pvec<T>& vec) {
	p.insert(p.end(), vec.p.begin(), vec.p.end());
  }
  
  void loadString(const string& line) {
	p.clear();
	istringstream iss(line);
	T v;
	while (iss >> v) {
	  p.push_back(v);
	}
  }

  // load a varible an line, make sure no empty lines
  // the number of rows determinates the dimension of vector
  void loadFile(const string& inf) {
	p.clear();
	ifstream rf(inf.c_str());
	if (!rf) 
	  EXIT_ERR("file not find:", inf.c_str());
	loadFileStream(rf);
  }

  void loadFileStream(ifstream& rf) {
	p.clear();
	T v;
	while (rf >> v) {
	  p.push_back(v);
	}
  }
  
  T sum() const{
	T s = 0;
	for( size_t i = 0 ; i < p.size() ; ++i ) 
	  s += p[i];
	return s;
  }

  T norm() const{
	T s = 0;
	for( size_t i = 0 ; i < p.size() ; ++i ) 
	  s += p[i]*p[i];
	return sqrt(s);
  }

  void normalize(double smoother = 0.0) {
	T s = sum();
	assert(s>=0);
  
	int K = p.size();
	// avoid numerical problem
	for( size_t i = 0 ; i < K ; ++i ) {
	  p[i] = (p[i] + smoother)/(s + K*smoother);
	}
  }

  // normalize to all exponents of entries sum to 1
  void exp_normalize() {
	vector<T> tmp(p);
	for (size_t i = 0; i < p.size(); ++i ) {
	  double s = 0.0;
	  for (size_t j = 0; j < p.size(); ++j ) 
		s += exp( tmp[j] - tmp[i] );
	
	  assert(s>=1);
	  p[i] = 1/s;
	}
  }

  void smooth(double smoother) {
	for( size_t i = 0 ; i < p.size() ; ++i )
	  if (p[i] < smoother)
		p[i] = smoother;
  }

  template<class T2>
  Pvec<T>& operator=(const vector<T2>& v) {
	p.resize(v.size());
	copy(v.begin(), v.end(), p.begin());
	return *this;
  }
  
  T &operator[](int i) {
	if (i >= p.size())
	  cout << "ERR: index=" << i << ", size=" << p.size() << endl;
	assert(i < p.size());
	return p[i]; 
  }

  const T& operator[](int i) const{assert(i<p.size()); return p[i];}

  Pvec<T> operator+(const T & v){
	Pvec<T> tp(p.size());
	for (int i=0; i<p.size(); ++i)
	  tp[i] = p[i] + v;
	return tp;
  }

  Pvec<T> operator+(const Pvec& v){
	Pvec<T> tp(p.size());
	for (int i=0; i<p.size(); ++i)
	  tp[i] = p[i] + v[i];
	return tp;
  }

  Pvec<T>& operator+=(const T & v){
	for (int i=0; i<p.size(); ++i)
	  p[i] += v;
	return *this;
  }


  Pvec<T>& operator+=(const Pvec& v){
	for (int i=0; i<p.size(); ++i)
	  p[i] += v[i];
	return *this;
  }

  Pvec<T> operator-(const T & v){
	Pvec<T> tp(p.size());
	for (int i=0; i<p.size(); ++i)
	  tp[i] = p[i] - v;
	return tp;
  }

  Pvec<T> operator-(const Pvec& v){
	Pvec<T> tp(p.size());
	for (int i=0; i<p.size(); ++i)
	  tp[i] = p[i] - v[i];
	return tp;
  }

  Pvec<T>& operator-=(const T & v){  
	for (int i=0; i<p.size(); ++i)
	  p[i] -= v;
	return *this;
  }

  Pvec<T>& operator-=(const Pvec& v) { 
	for (int i=0; i<p.size(); ++i)
	  p[i] -= v[i];
	return *this;
  }

  Pvec<T> operator*(const T & v) {
	Pvec<T> tp(p.size());
	for (int i=0; i<p.size(); ++i)
	  tp[i] = p[i] * v;
	return tp;
  }

  Pvec<T>& operator*=(const T& v) { 
	for (int i=0; i<p.size(); ++i)
	  p[i] *= v;
	return *this;
  }

  Pvec<T> operator/(const T & v) {
	Pvec<T> tp(p.size());
	for (int i=0; i<p.size(); ++i)
	  tp[i] = p[i] / v;
	return tp;
  }

  Pvec<T>& operator/=(const T& v) { 
	assert(v > 0);
	for (int i=0; i<p.size(); ++i)
	  p[i] /= v;
	return *this;
  }

  // element-wise log
  void add1_log() {
	for (int i=0; i<p.size(); ++i)
	  p[i] = log(1 + p[i]);
  }

  T max() const {
	T max_v = -10000000;
	for (int i=0; i<p.size(); ++i) {
	  if (p[i] > max_v) {
		max_v = p[i];
	  }
	}
	return max_v;
  }

  int max_idx() const {
	T max_v = -10000000;
	int idx = 0;
	for (int i=0; i<p.size(); ++i) {
	  if (p[i] > max_v) {
		max_v = p[i];
		idx = i;
	  }
	}
	return idx;
  }

  // erase v[start:end)
  void erase(int start, int end) {
	assert(end >= start && end <= p.size());
	p.erase(p.begin() + start, p.begin() + end);
  }

  void clear() {p.clear();}
    
  vector<T> to_vector() {return p;}
  Pvec<double> toDouble() {return Pvec<double>(*this);}

  string str(char delim = ' ') const{
	ostringstream os;
	size_t i;
	for( i = 0 ; i < p.size(); ++i ) 
	  os << p[i] << delim;
  
	return os.str();
  }

  // filter small values less than v
  string sparse_str(double v) const{
	ostringstream os;
	size_t i;
	for( i = 0 ; i < p.size(); ++i ) {
	  if (p[i] > v)
		os << i << ':' << p[i] << ' ';
	}
  
	return os.str();
  }

  void write(const string& pt, char delim = ' ') {
	ofstream wf(pt.c_str());
	if (!wf) {
	  cout << "Path not exists:" << pt << endl;
	  exit(-1);
	}
  
	wf << str(delim);
  }
};

#endif
