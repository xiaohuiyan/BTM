#ifndef _SAMPLER_H
#define _SAMPLER_H
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

namespace Sampler {

inline void init() {
	srand(1);
} //time(NULL));}

// uniform sample Mult(1/K), result [0, K-1]
inline int uni_sample(int K) {
	int k = rand() % K;
	return k;
}

// sample from [0,1)
inline double uni_sample() {
	double t = rand() % 10000 / 10000;
	return t;
}

// sample from Mult(p)
inline int mult_sample(vector<double> p) {
	int K = p.size();
	for (int i = 1; i < K; i++)
		p[i] += p[i - 1];

	double u = double(rand()) / RAND_MAX;
	int k;  						// record sampled index
	for (k = 0; k < K; k++)
		if (p[k] >= u * p[K - 1])
			break;

	if (k == K)
		--k;

	return k;
}

// p is p(1)
inline bool Bern_sample(float p) {
	double u = double(rand()) / RAND_MAX;
	return (u < p);
}

// return counts sampling from multinormial distribution p
// the counts sum to N
inline void systematic_sample(const vector<double>& p, int N,
		vector<int>& counts) {
	counts.resize(p.size(), 0);

	vector<double> u(N);
	u[0] = Sampler::uni_sample() / N;
	for (int n = 1; n < N; ++n)
		u[n] = u[0] + double(n) / N;

	int i = 0;
	double s1 = 0;
	double s2 = p[0];
	for (int n = 0; n < N - 1; ++n) {
		while (i < N && u[i] < s2) {
			++i;
			++counts[n];
		}

		s1 = s2;
		s2 += p[n + 1];
	}

	counts[N - 1] = N - i;
}
}
#endif
