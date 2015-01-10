#include <cassert>
#include <iostream>
#include <string>
#include <cmath>
#include <limits>
#include <ctime>
#include <algorithm>

#include "sampler.h"
#include "str_util.h"
#include "model.h"

// n_status: number of latent status for each biterm
void Model::init() {
  cout << "init" << endl;
  load_docs();
  
  nwz.resize(W, K);
  nb_z.resize(K);
  
  // random initialize
  for (vector<Biterm>::iterator b = bs.begin(); b != bs.end(); ++b) {
	int k = Sampler::uni_sample(K);
	assign_biterm_topic(*b, k);
  }
}

// input, each line is a doc
// format: wid  wid  wid ...
void Model::load_docs() {
  cout << "load docs: " << dfile << endl;
  ifstream rf( dfile.c_str() );
  if (!rf) {
	cout << "file not find:" << dfile << endl;
	exit(-1);
  }

  string line;
  while(getline(rf, line)) {
	Doc doc(line);
	doc.gen_biterms(bs);
  }
}

void Model::run() {
  init();
  
  cout << "Begin iteration" << endl;
  for (int it = 1; it < n_iter + 1; ++it) {
	cout << "\riter " << it;
	fflush(stdout);
	for (int b = 0; b < bs.size(); ++b) {
	  update_biterm(bs[b]);
	}
	
	if (it % save_step == 0)
	  save_all();
  }

  save_all();
}

// sample procedure for ith biterm 
double Model::update_biterm(Biterm& bi) {
  reset_biterm_topic(bi);
  
  // compute p(z|b)
  Pvec<double> pz;
  compute_pz_b(bi, pz);

  // sample topic for biterm b
  int k = Sampler::mult_sample(pz.to_vector());
  assign_biterm_topic(bi, k);

  return pz.max();
}

// reset topic assignment of biterm i
void Model::reset_biterm_topic(Biterm& bi) {
  int w1 = bi.get_wi();
  int w2 = bi.get_wj();
  int k = bi.get_z();
  
  nb_z[k] -= 1;	// update number of biterms in topic K
  nwz[w1][k] -= 1;	// update w1's occurrence times in topic K
  nwz[w2][k] -= 1;
  assert(nb_z[k] > -10e-7 && nwz[w1][k] > -10e-7 && nwz[w2][k] > -10e-7);
  
  bi.reset_z();
}

// compute p(z|w_i, w_j)
void Model::compute_pz_b(Biterm& bi, Pvec<double>& pz) {
  pz.resize(K);
  int w1 = bi.get_wi();
  int w2 = bi.get_wj();
  
  double pw1k, pw2k;
  for (int k = 0; k < K; ++k) {
	// avoid numerical problem by mutipling W
	double deno1 = W / (2 * nb_z[k] + W * beta);
	double deno2 = W / (2 * nb_z[k] + W * beta);
	pw1k = (nwz[w1][k] + beta) * deno1;
	pw2k = (nwz[w2][k] + beta) * deno2;
	pz[k] = (nb_z[k] + alpha) * pw1k * pw2k;
  }
  pz.normalize();
}

// assign topic k to biterm i
void Model::assign_biterm_topic(Biterm& bi, int k) {
  int w1 = bi.get_wi();
  int w2 = bi.get_wj();
  
  bi.set_z(k);
  nb_z[k] += 1;
  nwz[w1][k] += 1;
  nwz[w2][k] += 1;
}

void Model::save_all() {
  save_pz();
  save_pw_z();
  //save_model();
}

// p(z) is determinated by the overall proportions
// of biterms in it
void Model::save_pz() {
  Pvec<double> pz(K);	          // p(z) = theta
  for (int k = 0; k < K; k++) 
	pz[k] = (nb_z[k] + alpha);
  
  pz.normalize();
  
  string pt = dir + "pz.k" + str_util::itos(K);
  cout << end << "save p(z): " << pt << endl;
  pz.write(pt);
}

void Model::save_pw_z() {
  Pmat<double> pw_z(K, W);   // p(w|z) = phi, size K * M
  for (int k = 0; k < K; k++) {
	for (int m = 0; m < W; m++) 
	  pw_z[k][m] = (nwz[m][k] + beta);

	pw_z[k].normalize();
  }

  string pt = dir + "pw_z.k" + str_util::itos(K);
  cout << "save p(w|z): " << pt << endl;
  pw_z.write(pt);
}

// format:wi   wj    z 
void Model::save_model() {
  string pt = dir + "bz.k" + str_util::itos(K);
  cout << "save bz: " << pt << endl;
  ofstream wf(pt.c_str());
  if (!wf)
	wf.open("bz.txt");

  for (vector<Biterm>::iterator b = bs.begin(); b != bs.end(); ++b)
	wf << b->str() << endl;
}

void Model::load_model() {
  string pt = dir + "bz.k" + str_util::itos(K);
  cout << "load bz: " << pt << endl;

  ifstream rf(pt.c_str());
  string line;
  int max_w = -1;
  while (getline(rf, line)) {
	Biterm bi(line);
	bs.push_back(bi);

	if (max_w < bi.get_wj())
	  max_w = bi.get_wj();
  }
  
  W = max_w + 1;
  printf("load n(b)=%lu, n(w)=%d\n", bs.size(), W);
}
