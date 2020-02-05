#include <iostream>
#include <cmath>
#include <string>
#include "time.h"
#include <unistd.h>
#include <random>
#include <algorithm>
#include <iomanip>
#include <pthread.h>
#include <assert.h>
#include <thread>

using namespace std;

struct thread_info{
	double a;
	double b;	
	long thread_num;	
	long num_samples;
	double sum;
};


double fn(double x);
void thread_info_ctor(thread_info* ti, long num_samples, long thread_num, 
															double a, double b);
void thread_info_dtor(thread_info* ti);
void print_info(thread_info* ti);
void* take_samples(void* ti_);
double kahan_final(double* sums, long n_threads);

int main(int argc, char **argv) {
	if(argc > 5){
		cout << "invocation must be of the form:" << endl;
		cout << "\t./integrate a b n n_threads" << endl;
	}

	long n, n_threads, n_per_thread, remainder;
	string n_string = argv[3];
	double a, b;
	a = stod(argv[1]);
	b = stod(argv[2]);
	n = stoi(argv[3]);
	n_threads = stoi(argv[4]);
	n_per_thread = n/n_threads;
	remainder = n%n_threads;

	long * sample_count = new (nothrow) long[n_threads];
		
	for(long i = 0; i < n_threads; i++){
		sample_count[i] = n_per_thread;
	}
	for(long i = 0; i < remainder; i++){
		sample_count[i]++;
	}
	random_device rd;
	default_random_engine re(rd());
	shuffle(sample_count, sample_count+n_threads, re);

	thread_info *threads = new (nothrow) thread_info[n_threads];

	for(long i = 0; i < n_threads; i++){
		thread_info_ctor(&(threads[i]), sample_count[i], i, a, b);		
	}

	int rv;
	pthread_t* tids = new pthread_t[n_threads];
	



	for(long i = 0; i < n_threads; i++){
		rv = pthread_create(&(tids[i]), NULL, take_samples, (void*)&(threads[i]) );
		assert(rv == 0); 
	}
	for(long i = 0; i < n_threads; i++){
		rv = pthread_join(tids[i], NULL);
	}
	
	double* kahan_result = new (nothrow) double[n_threads];
	for(int i = 0; i < n_threads; i++){
		kahan_result[i] = threads[i].sum;
	}
	double final_sum = kahan_final(kahan_result, n_threads);
	double last = final_sum / ((double)n);	
	//cout << "n_threads " << n_threads << endl;
	cout << setprecision(20) << last << endl;
	
	delete[] kahan_result;
	delete[] sample_count;
	delete[] threads;
	delete[] tids;
}

double fn(double x){
	return sin(x)/x;
}

void thread_info_ctor(thread_info* ti, long num_samples, long thread_num, 
															double a, double b){
	ti->a = a;	
	ti->b = b;
	ti->num_samples = num_samples;
	ti->sum = 0.0;
	ti->thread_num = thread_num;
}

void print_info(thread_info* ti){
	cout << "-----------------------------" << endl;	
	cout << "thread_num " << ti->thread_num << endl;	
	cout << "num_samples " << ti->num_samples << endl;
	cout << "sum " << setprecision(20) << ti->sum << endl;
	cout << "-----------------------------" << endl;
}

// l_ signifies local version of struct data
void* take_samples(void* ti_){
	thread_info* ti = (thread_info*)ti_;	
		
	// set affinity
	int rc;
	const unsigned int num_cpus = thread::hardware_concurrency();
	//cout << "num_cpus " << num_cpus << endl;
	//cout << pthread_self() << endl;
	cpu_set_t cpuset;
   	CPU_ZERO(&cpuset);
   	CPU_SET( (ti->thread_num % num_cpus ), &cpuset);
	rc = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0){
      	std::cerr << "Error setting affinity" << '\n';
   	}
	
	double in;
	double result;
	long l_samples_taken = 0;
	long l_num_samples = ti->num_samples;	
	double l_a = ti->a;	
	double l_b = ti->b;
	double l_sum = 0;	
	double * local_results = new double[l_num_samples];	
	uniform_real_distribution<double> unif(l_a,l_b);
	random_device de;
	for(; l_samples_taken < l_num_samples; l_samples_taken++){
   		//default_random_engine re;	
        //mt19937_64 re(de());
		minstd_rand re(de());	
   		in = unif(re);
		if(in == 0){
			result = 1;
		} else{		
			result = fn(in);
		}
		local_results[l_samples_taken] = result;
	}
	sort(local_results, local_results + l_num_samples);
	l_sum = kahan_final(local_results, l_num_samples);	
	ti->sum = l_sum;
	delete[] local_results;
	return NULL;
}

double kahan_final(double* sums, long n_threads){
	double sum = 0.0;
	double c = 0.0;
	double y, t;	
	for(int i = 0; i < n_threads; i++){
		y = sums[i] - c;
		t = sum + y;
		c = (t - sum) - y;
		sum = t;
	}
	return sum;
}

