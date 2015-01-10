#include <iostream>
#include <sstream>
#include <cmath>

#include "str_util.h"
#include "infer.h"

Infer::Infer(string type, int K, string dfile, string dir): 
  type(type), K(K), dfile(dfile), dir(dir){
  string pt = dir + "pz_d.k" + str_util::itos(K);

  cout << "write p(z|d): " << pt << endl;
  wf.open(pt.c_str());
  
  if (!wf) {
	cout << "[Error]Wirte:" << pt.c_str() << endl;
	exit(-1);
  }
}

void Infer::run() {
  load_para();
  
  cout << "Infer p(z|d) for dfile: " << dfile << endl;
  string line;
  
  ifstream rf(dfile.c_str());
  if (!rf) {
	cout << "[Error]not found:" << dfile.c_str() << endl;
	exit(-1);
  }
  
  while (getline(rf, line)) {
	Doc doc(line);
	Pvec<double> pz_d(K);
	doc_infer(doc, pz_d);
	// write p(z|d) for d, a doc a time
	wf << pz_d.str() << endl;
  }
}

void Infer::load_para() {
  if (type != "lda") {
	string pt = dir + "pz.k" + str_util::itos(K);
	cout << "load p(z):" << pt <<endl;
	pz.loadFile(pt);
	assert(abs(pz.sum() - 1) < 1e-4);
  }
 
  string pt2 = dir + "pw_z.k" + str_util::itos(K);
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
  else if (type == "lda")
	doc_infer_lda(doc, pz_d);
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

// pure inference without changing training parameters
// test doc random initialize, and then iteratively update topic assignments
// of words in test documents
// compute p(z|d) = \sum_{w \in d} p(z|d,w) -- words are i.i.d in a documents
// inference smooth not enough!!!
void Infer::doc_infer_lda(const Doc& doc, Pvec<double>& pz_d) {
  // random init p(z|d)
  pz_d.rand_init();
    
  int W = pw_z.cols();
  
  const vector<int>& ws = doc.get_ws();
  int n_iter = 10;
 
  for (int it = 1; it < n_iter + 1; ++it) {
	//update pz_d
	Pvec<double> new_pz_d(K);
  
	for (int i = 0; i < ws.size(); ++i) {
	  int w = ws[i];
	  if (w >= W) continue;
	  
	  Pvec<double> pz_dw(K);
	  
	  compute_pz_dw(w, pz_d, pz_dw);
	  new_pz_d += pz_dw;
	}

	new_pz_d.normalize();
	pz_d = new_pz_d;
  }
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
