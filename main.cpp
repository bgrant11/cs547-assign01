#include <iostream>
#include <cmath>


using namespace std;

double fn(double x);

int main(int argc, char **argv) {
	if(argc > 5){
		cout << "invocation must be of the form:" << endl;
		cout << "\t./integrate a b n n_threads" << endl;
	}
	double a, b, domain, section_len;
	long n, n_threads, n_per_thread, remainder;
	a = stod(argv[1]);
	b = stod(argv[2]);
	n = stoi(argv[3]);
	n_threads = stoi(argv[4]);
	n_per_thread = n/n_threads;
	remainder = n%n_threads;
	domain = b-a;
	section_len = domain / (double)n_threads;
	cout << "a\t\t" << a << endl;
	cout << "b \t\t" << b << endl;
	cout << "n\t\t" << n << endl;
	cout << "n_threads\t" << n_threads << endl;
	cout << "n_per_thread\t" << n_per_thread << endl;
	cout << "remainder\t" << remainder << endl;
	cout << "domain\t" << domain << endl;	
	cout << "section_len\t" << section_len << endl;	
	cout << "dom\t" << section_len*(double)n_threads << endl;	
	cout << fn(1.2) << endl;
	
	// each thread is randomly assigned a section to randomize distribution	
	int * sample_count = new (nothrow) int[n_threads];
	for(int i = 0; i < n_threads; i++){
		sample_count[i] = n_per_thread;
	}
	for(int i = 0; i < remainder; i++){
		sample_count[i]++;
	}

	for(int i = 0; i < n_threads; i++){
		cout << sample_count[i] << endl;
	}
	


}

double fn(double x){
	return sin(x)/x;

}
