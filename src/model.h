/**
 * Biterm topic model(BTM) with Gbbis sampling implementation
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
  string dfile;			// docs file
  string dir;			// result dir
  
  // sample recorders
  Pvec<int> nb_z;	// n(b|z), size K*1
  Pmat<int> nwz;	  // n(w,z), size M*K

public:
  Model(int K, int W, double a, double b, int n_iter, int save_step,
		string dfile, string dir): K(K), W(W), alpha(a), beta(b),
								   dfile(dfile), dir(dir), n_iter(n_iter),
								   save_step(save_step) {srand(time(NULL));}
  
  // run estimate and inference procedures
  void run();
  
protected:  
  // intialize memeber varibles and biterms
  void init();		// load from docs
  void load_docs();
  
  void load_model();   // load from saved model, currently not used
  
  // update estimate of a biterm
  double update_biterm(Biterm& bi);
  
  // reset topic proportions for biterm b
  void reset_biterm_topic(Biterm& bi);
  
  // assign topic proportions for biterm b
  void assign_biterm_topic(Biterm& bi, int k);
  
  // compute condition distribution p(z|b)
  void compute_pz_b(Biterm& bi, Pvec<double>& p);

  void save_all();
  void save_pz();
  void save_pw_z();
  void save_model();			// save nb_z and nwz
};

#endif
