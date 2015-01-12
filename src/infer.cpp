#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>

#include "str_util.h"
#include "infer.h"

void Infer::run(string docs_pt, string model_dir) {
  load_para(model_dir);
  
  cout << "Infer p(z|d) for docs in: " << docs_pt << endl;
  ifstream rf(docs_pt.c_str());
  assert(rf);

  string pt = model_dir + "k" + str_util::itos(K) + ".pz_d";
  ofstream wf(pt.c_str());
  assert(wf);
  cout << "write p(z|d): " << pt << endl;
  
  string line;
  while (getline(rf, line)) {
	Doc doc(line);
	Pvec<double> pz_d(K);
	doc_infer(doc, pz_d);
	// write p(z|d) for d, a doc a time
	wf << pz_d.str() << endl;
  }
}

void Infer::load_para(string model_dir) {
  string pt = model_dir + "k" + str_util::itos(K) + ".pz";
  cout << "load p(z):" << pt <<endl;
  pz.loadFile(pt);
  assert(abs(pz.sum() - 1) < 1e-4);
 
  string pt2 = model_dir + "k" + str_util::itos(K) + ".pw_z";
  cout << "load p(w|z):" << pt2 <<endl;
  pw_z.load(pt2);
  printf("n(z)=%d, n(w)=%d\n", pw_z.rows(), pw_z.cols());
  assert(pw_z.rows() > 0 && abs(pw_z[0].sum() - 1) < 1e-4);
}

void Infer::doc_infer(const Doc& doc, Pvec<double>& pz_d) {
  if (type == "sum_b")
	doc_infer_sum_b(doc, pz_d);
  else if (type == "sub_w")
	doc_infer_sum_w(doc, pz_d);
  else if (type == "mix")
	doc_infer_mix(doc, pz_d);
  else {
	cout << "[Err] unkown infer type:" << type << endl;
	exit(1);
  }
}


// p(z|d) = \sum_b{ p(z|b)p(b|d) }
void Infer::doc_infer_sum_b(const Doc& doc, Pvec<double>& pz_d) {
  pz_d.assign(K, 0);
  
  if (doc.size() == 1) {
	// doc is a single word, p(z|d) = p(z|w) \propo p(z)p(w|z)
	for (int k = 0; k < K; ++k) 
	  pz_d[k] = pz[k] * pw_z[k][doc.get_w(0)];
  }
  else {
	// more than one words
	vector<Biterm> bs;
	doc.gen_biterms(bs);

	int W = pw_z.cols();	
	for (int b = 0; b < bs.size(); ++b) {	  
	  int w1 = bs[b].get_wi();
	  int w2 = bs[b].get_wj();

	  // filter out-of-vocabulary words
	  if (w2 >= W) continue;
	  
	  // compute p(z|b) \propo p(w1|z)p(w2|z)p(z)
	  Pvec<double> pz_b(K);
	  for (int k = 0; k < K; ++k) {
		assert(pw_z[k][w1]>0 && pw_z[k][w2]>0);
		pz_b[k] = pz[k] * pw_z[k][w1] * pw_z[k][w2];
	  }
	  pz_b.normalize();
	
	  // sum for b, p(b|d) is unifrom
	  for (int k = 0; k < K; ++k) 
		pz_d[k] += pz_b[k];
	}
  }
  
  pz_d.normalize();
}

// p(z|d) = \sum_w{ p(z|w)p(w|d) }
void Infer::doc_infer_sum_w(const Doc& doc, Pvec<double>& pz_d) {
  pz_d.assign(K, 0);
  
  int W = pw_z.cols();	
  const vector<int>& ws = doc.get_ws();
  
  for (int i = 0; i < ws.size(); ++i) {
	int w = ws[i];
	if (w >= W) continue;
	
	// compute p(z|w) \propo p(w|z)p(z)
	Pvec<double> pz_w(K);
	for (int k = 0; k < K; ++k) 
	  pz_w[k] = pz[k] * pw_z[k][w];
	
	pz_w.normalize();
	
	// sum for b, p(b|d) is unifrom
	for (int k = 0; k < K; ++k) 
	  pz_d[k] += pz_w[k];
  }
  pz_d.normalize();
}

void Infer::doc_infer_mix(const Doc& doc, Pvec<double>& pz_d) {
  pz_d.resize(K);
  for (int k = 0; k < K; ++k) 
	pz_d[k] = pz[k];

  const vector<int>& ws = doc.get_ws();
  int W = pw_z.cols();
  for (int i = 0; i < ws.size(); ++i) {
	int w = ws[i];
	if (w >= W) continue;

	for (int k = 0; k < K; ++k) 
	  pz_d[k] *= (pw_z[k][w] * W);
  }
  
	// sum for b, p(b|d) is unifrom
  pz_d.normalize();
}

// compute p(z|d, w) \proto p(w|z)p(z|d)
void Infer::compute_pz_dw(int w, const Pvec<double>& pz_d, Pvec<double>& p) {
  p.resize(K);
  
  for (int k = 0; k < K; ++k) 
	p[k] = pw_z[k][w] * pz_d[k];
  
  p.normalize();
}
