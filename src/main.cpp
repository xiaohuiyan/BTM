#include <cstdlib>
#include <string.h>
#include <string>
#include <cstdlib>
#include <string.h>
#include <string>
#include <iostream>
#include <ctime>

#include "model.h"
#include "infer.h"

using namespace std;

void usage() {
  cout << "Usage:\n"
       << "btm est K alpha beta pt_input pt_outdir n_iter\n"
       << "\tK\tint, number of topics, like 20\n"
       << "\talpha\tdouble, Pymmetric Dirichlet prior of P(z), like 1.0\n"
       << "\tbeta\tdouble, Pymmetric Dirichlet prior of P(w|z), like 0.01\n"
       << "\tpt_input\ttraining biterms file\n"
       << "\tpt_outdir\toutput directory\n"
       << "btm inf K pt_input pt_outdir\n"
       << "\tpt_input\tinference biterms file\n"
       << "\tpt_outdir\tparameters/output directory\n";
}

int main(int argc, char* argv[]) {
  // test_main();
  //// load parameters from std input
  if (argc < 4) {
    usage();
    return 1;
  }  

  if (strcmp(argv[1], "est")==0) {
    int K = atoi(argv[2]);                  // topic num
	int W = atoi(argv[3]);
    double alpha = atof(argv[4]);    // hyperparameters of p(z)
    double beta = atof(argv[5]);     // hyperparameters of p(w|z)
    int n_iter = atoi(argv[6]);
	int save_step = atoi(argv[7]);
    string dfile(argv[8]);
    string dir(argv[9]);

    cout << "Run BTM, K=" << K << ", W=" << W << ", alpha=" << alpha << ", beta=" << beta << ", n_iter=" << n_iter << ", save_step=" << save_step << " ====" << endl;	

    // load training data from file
	clock_t start = clock();
	Model model(K, W, alpha, beta, n_iter, save_step, dfile, dir);
	model.run();
	clock_t end = clock();
	printf("cost %fs\n", double(end - start)/CLOCKS_PER_SEC);
	
  } else if (strcmp(argv[1], "inf")==0) {
	string type(argv[2]);
    int K = atoi(argv[3]);                  // topic num
    string dfile(argv[4]);
    string dir(argv[5]);
    cout << "Run infence:K=" << K << ", type " << type << " ====" << endl;
    Infer inf(type, K, dfile, dir);
    inf.run();
  } else {
	cout << "Wrong common:" << argv[0] << " " << argv[1] << endl;
  }
}
