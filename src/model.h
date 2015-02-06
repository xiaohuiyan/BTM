/**
 * Biterm topic model(BTM) with Gbbis sampling 
 * Author: Xiaohui Yan(xhcloud@gmail.com)
 * 2012-9-25
 */
#ifndef _MODEL_H
#define _MODEL_H

#include <vector>
#include <fstream>
#include "biterm.h"
#include "doc.h"
#include "pvec.h"
#include "pmat.h"

using namespace std;

class Model {
public:
  vector<Biterm> bs;

protected:
  int W;				// vocabulary size
  int K;				// number of topics
  int n_iter;			// maximum number of iteration of Gibbs Sampling
  int save_step;

  double alpha;			// hyperparameters of p(z)
  double beta;			// hyperparameters of p(w|z)
  
  // sample recorders
  Pvec<int> nb_z;	// n(b|z), size K*1
  Pmat<int> nwz;	  // n(w,z), size K*W

  Pvec<double> pw_b;   // the background word distribution  

  // If true, the topic 0 is set to a background topic that 
  // equals to the emperiacal word dsitribution. It can filter 
  // out common words
  bool has_background; 

public:
  Model(int K, int W, double a, double b, int n_iter, int save_step,
		bool has_b = false): 
	K(K), W(W), alpha(a), beta(b), 
	n_iter(n_iter), has_background(has_b),
	save_step(save_step) {
	pw_b.resize(W);
	nwz.resize(K, W);
	nb_z.resize(K);
  }
  
  // run estimate procedures
  void run(string docs_pt, string res_dir);
  
private:
  // intialize memeber varibles and biterms
  void model_init();		// load from docs
  void load_docs(string docs_pt);
  
  // update estimate of a biterm
  void update_biterm(Biterm& bi);
  
  // reset topic proportions for biterm b
  void reset_biterm_topic(Biterm& bi);
  
  // assign topic proportions for biterm b
  void assign_biterm_topic(Biterm& bi, int k);
  
  // compute condition distribution p(z|b)
  void compute_pz_b(Biterm& bi, Pvec<double>& p);

  void save_res(string res_dir);
  void save_pz(string pt);
  void save_pw_z(string pt);
};

#endif
