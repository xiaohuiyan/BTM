#ifndef _INFERLDA_H
#define _INFERLDA_H

#include <fstream>
#include <string>

#include "pvec.h"
#include "pmat.h"

#include "doc.h"

using namespace std;

class Infer {
private:
  int K;
  string type;			// infer type
  
  string dir;			// parameters dir
  string dfile;			// inference docs
  Pvec<double> pz;	    // p(z) = theta
  Pmat<double> pw_z;   // p(w|z) = phi, size K * M

  ofstream wf;					// hander of p(z|d) writer

private:
  void load_para();
  
  void doc_infer(const Doc& doc, Pvec<double>& pz_d);  
  void doc_infer_sum_b(const Doc& doc, Pvec<double>& pz_d);
  void doc_infer_sum_w(const Doc& doc, Pvec<double>& pz_d);
  void doc_infer_lda(const Doc& doc, Pvec<double>& pz_d);
  void doc_infer_mix(const Doc& doc, Pvec<double>& pz_d);
  
  // compute condition distribution p(z|w, d) with p(w|z) fixed
  void compute_pz_dw(int w, const Pvec<double>& pz_d, Pvec<double>& p); 
  
public:
  Infer(string type, int K, string dfile, string dir);

  void run();
};

#endif
