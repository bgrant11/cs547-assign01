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

using namespace std;

struct thread_info{
	long thread_num;	
	double max;
	double min;
	long num_samples;
	double * input;	
	double * results;
	double sum;
	long samples_taken;	
	uniform_real_distribution<double> dist;
	default_random_engine engine;
};


double a, b, domain, section_len;







double fn(double x);
//double rand_d(const double min, const double max); 
void thread_info_ctor(thread_info* ti, double min, double max, 
											long num_samples, long thread_num);
void thread_info_dtor(thread_info* ti);
void print_info(thread_info* ti);
void* take_samples(void* ti_);
double kahan_sum(thread_info* it);
double kahan_final(double* sums, long n_threads);

int main(int argc, char **argv) {
	if(argc > 5){
		cout << "invocation must be of the form:" << endl;
		cout << "\t./integrate a b n n_threads" << endl;
	}

	//double a, b, domain, section_len;
	long n, n_threads, n_per_thread, remainder;
	string n_string = argv[3];

	a = stod(argv[1]);
	b = stod(argv[2]);
	n = stoi(argv[3]);
	n_threads = stoi(argv[4]);
	n_per_thread = n/n_threads;
	remainder = n%n_threads;
	domain = b-a;
	section_len = domain / (double)n_threads;
	
	/*
	{	// debug
		cout << "a\t\t" << setprecision(10) << a << endl;
		cout << "b \t\t" << setprecision(10) << b << endl;
		cout << "n\t\t" << n << endl;
		cout << "n_threads\t" << n_threads << endl;
		cout << "n_per_thread\t" << n_per_thread << endl;
		cout << "remainder\t" << remainder << endl;
		cout << "domain\t" << setprecision(10) << domain << endl;	
		cout << "section_len\t" << setprecision(10) << section_len << endl;	
		//cout << "dom\t" << section_len*(double)n_threads << endl;	
		//cout << fn(1.2) << endl;
		cout << "rand" << endl;
		double lower_bound = 0;
  	 	double upper_bound = 10000;
  		std::uniform_real_distribution<double> unif;
		unif = uniform_real_distribution<double>(lower_bound,upper_bound);
 		std::default_random_engine re;
  		//double a_random_double = unif(re);
		//for(int i = 0; i < 99; i++){
		//	cout << unif(re) << endl;;
		//}
	}
	*/
	
	// each thread is randomly assigned a section to randomize distribution	
	
	//	
	

	long * sample_count = new (nothrow) long[n_threads];
		
	for(long i = 0; i < n_threads; i++){
		sample_count[i] = n_per_thread;
	}
	for(long i = 0; i < remainder; i++){
		sample_count[i]++;
	}
	/*
	{ // debug
		cout << "before shuffle" << endl;
		for(long i = 0; i < n_threads; i++){
			cout << sample_count[i] << endl;
		}
	}
	*/
	shuffle(sample_count, sample_count+n_threads, default_random_engine());
	/*
	{ // debug
		cout << "after shuffle" << endl;
		for(long i = 0; i < n_threads; i++){
			cout << sample_count[i] << endl;
		}
	}
	 */
	thread_info *threads = new (nothrow) thread_info[n_threads];
	
	double low, high;
	low = a;
	for(long i = 0; i < n_threads; i++){
		high = low + section_len;
		//if(i == (n_threads-1)){
		//	high = b;
		//}		
		thread_info_ctor(&(threads[i]), low, high, sample_count[i], i);		
		low = high;
	}
	
	/*
	{ // debug
		for(long i = 0; i < n_threads; i++){
			print_info(&(threads[i]));	
		}

	}
	*/
	int rv;
	pthread_t* tids = new pthread_t[n_threads];
	for(long i = 0; i < n_threads; i++){
		//take_samples(&(threads[i]));
		rv = pthread_create(&(tids[i]), NULL, take_samples, (void*)&(threads[i]) );
		assert(rv == 0); 
	}
	for(long i = 0; i < n_threads; i++){
		rv = pthread_join(tids[i], NULL);
	}
	

	/*
	{ // debug
		cout << "after samples taken" << endl;		
		for(long i = 0; i < n_threads; i++){
			print_info(&(threads[i]));	
		}

	}
	*/
	double total_sum = 0.0;
	for(int i = 0; i < n_threads; i++){
		total_sum += threads[i].sum;
	}	
	double result = total_sum / ((double)n);
	cout << "total_sum " << total_sum << endl;
	cout << "result " << result << endl;
	
	cout << "kahan" << endl;
	double* kahan_result = new (nothrow) double[n_threads];
	for(int i = 0; i < n_threads; i++){
		kahan_result[i] = kahan_sum(&threads[i]);
	}
	double final_sum = kahan_final(kahan_result, n_threads);
	double last = final_sum / ((double)n);	
	cout << "last " << last << endl;
}

double fn(double x){
	return sin(x)/x;
}

//double rand_d(const double min, const double max){
	
//}

void thread_info_ctor(thread_info* ti, double min, double max, 
											long num_samples, long thread_num){
	ti->max = max;
	ti->min = min;
	ti->num_samples = num_samples;
	ti->input = new (nothrow) double[num_samples];	 
	ti->results = new (nothrow) double[num_samples];
	ti->sum = 0.0;
	ti->samples_taken = 0;
	ti->dist = uniform_real_distribution<double>(min,max);
	ti->thread_num = thread_num;
}

void thread_info_dtor(thread_info* ti){
	delete[] ti->results;
}
// TODO are they as precise as they should be
void print_info(thread_info* ti){
	cout << "-----------------------------" << endl;	
	cout << "thread_num " << ti->thread_num << endl;	
	cout << "min " << ti->min << endl;	
	cout << "max " << ti->max << endl;
	cout << "num_samples " << ti->num_samples << endl;
	/*
	cout << "inputss ";
	for(int i = 0; i < ti->samples_taken; i++){
		cout << ti->input[i] << " ";
	}	
	cout << endl;	
	cout << "results ";
	for(int i = 0; i < ti->samples_taken; i++){
		cout << ti->results[i] << " ";
	}
	cout << endl;
	*/
	
	cout << "input\t\tresults" << endl;
	for(int i = 0; i < ti->samples_taken; i++){
		cout << ti->input[i] << "\t\t" << ti->results[i] << endl;
	}
	cout << "sum " << ti->sum << endl;
	cout << "samples_taken " << ti->samples_taken << endl;
	cout << "-----------------------------" << endl;
}

void* take_samples(void* ti_){
	thread_info* ti = (thread_info*)ti_;	
	double in;
	double result;	
	for(; ti->samples_taken < ti->num_samples; ti->samples_taken++){
		//in = ti->dist(ti->engine);
		
		uniform_real_distribution<double> unif(a,b);
   		default_random_engine re;		
   		in = unif(re);
				
		if(in == 0){
			result = 1;
		} else{		
			result = fn(in);
		}
		ti->sum += result;
		ti->input[ti->samples_taken] = in;
		ti->results[ti->samples_taken] = result;
		//ti->sum += fn(ti->dist(ti->engine));
	}
	sort(ti->results, ti->results + ti->num_samples);
	return NULL;
}

double kahan_sum(thread_info* it){
	double sum = 0.0;
	double c = 0.0;
	double y, t;	
	for(int i = 0; i < it->num_samples; i++){
		y = it->results[i] - c;
		t = sum + y;
		c = (t - sum) - y;
		sum = t;
	}
	return sum;
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



