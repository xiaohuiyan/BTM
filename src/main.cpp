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
  cout << "Training Usage:" << endl
       << "btm est <K> <W> <alpha> <beta> <n_iter> <save_step> <docs_pt> <model_dir>\n"
       << "\tK  int, number of topics, like 20" << endl
       << "\tW  int, size of vocabulary" << endl
       << "\talpha   double, Pymmetric Dirichlet prior of P(z), like 1.0" << endl
       << "\tbeta    double, Pymmetric Dirichlet prior of P(w|z), like 0.01" << endl
       << "\tn_iter  int, number of iterations of Gibbs sampling" << endl
       << "\tsave_step   int, steps to save the results" << endl
       << "\tdocs_pt     string, path of training docs" << endl
       << "\tmodel_dir   string, output directory" << endl
       << "Inference Usage:" << endl
       << "btm inf <K> <docs_pt> <model_dir>" << endl
       << "\tK  int, number of topics, like 20" << endl
       << "\tdocs_pt     string, path of training docs" << endl
       << "\tmodel_dir  string, output directory" << endl;
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
     usage();
     return 1;
   }

  //// load parameters from std input
  int i = 1;
  if (strcmp(argv[i++], "est")==0) {
    int K = atoi(argv[i++]);                  // topic num
	int W = atoi(argv[i++]);
    double alpha = atof(argv[i++]);    // hyperparameters of p(z)
    double beta = atof(argv[i++]);     // hyperparameters of p(w|z)
    int n_iter = atoi(argv[i++]);
	int save_step = atoi(argv[i++]);
    string docs_pt(argv[i++]);
    string dir(argv[i++]);

    cout << "Run BTM, K=" << K << ", W=" << W << ", alpha=" << alpha << ", beta=" << beta << ", n_iter=" << n_iter << ", save_step=" << save_step << " ====" << endl;	
    // load training data from file
	clock_t start = clock();
	Model model(K, W, alpha, beta, n_iter, save_step);
	model.run(docs_pt, dir);
	clock_t end = clock();
	printf("cost %fs\n", double(end - start)/CLOCKS_PER_SEC);	
  } else if (strcmp(argv[1], "inf")==0) {
	string type(argv[2]);
    int K = atoi(argv[3]);                  // topic num
    string docs_pt(argv[4]);
    string dir(argv[5]);
    cout << "Run inference:K=" << K << ", type " << type << " ====" << endl;
    Infer inf(type, K);
    inf.run(docs_pt, dir);
  } else {
	cout << "Wrong common:" << argv[0] << " " << argv[1] << endl;
  }
}
