#include <iostream>
#include <fstream>
#include <string>
using namespace std;


double kahan_add_then_average(double* in1, double* in2, double* in3,double* in4,
									double* in5, int idx){
	double sum = 0.0;
	double c = 0.0;
	double y, t;	
	
		y = in1[idx] - c;
		t = sum + y;
		c = (t - sum) - y;
		sum = t;

		y = in2[idx] - c;
		t = sum + y;
		c = (t - sum) - y;
		sum = t;

		y = in3[idx] - c;
		t = sum + y;
		c = (t - sum) - y;
		sum = t;

		y = in4[idx] - c;
		t = sum + y;
		c = (t - sum) - y;
		sum = t;

		y = in5[idx] - c;
		t = sum + y;
		c = (t - sum) - y;
		sum = t;

		sum = sum / (double)5.0;
	
	return sum;
}


int main () {
  	string in1[40];
	double d_in1[40];	
	
  	string in2[40];
	double d_in2[40];	

  	string in3[40];
	double d_in3[40];	

  	string in4[40];
	double d_in4[40];	

  	string in5[40];
	double d_in5[40];	

	double in_average[40];


	double speedup[40];
	double efficiency[40];
	double first;
	int idx = 0;

// trial 1 --------------------------------------
  string line1;
  ifstream myfile1 ("re1.txt");
  if (myfile1.is_open())
  {
	while ( getline (myfile1,line1) )
    {
		in1[idx] = line1;
		idx++;     
		//cout << line << '\n';
    }
    myfile1.close();
	idx = 0;
  }
	
  else cout << "Unable to open file"; 


// trial 2 --------------------------------------
  string line2;
  ifstream myfile2 ("re2.txt");
  if (myfile2.is_open())
  {
	while ( getline (myfile2,line2) )
    {
		in2[idx] = line2;
		idx++;     
		//cout << line << '\n';
    }
    myfile2.close();
	idx = 0;
  }

  else cout << "Unable to open file"; 



// trial 3 --------------------------------------
  string line3;
  ifstream myfile3 ("re3.txt");
  if (myfile3.is_open())
  {
	while ( getline (myfile3,line3) )
    {
		in3[idx] = line3;
		idx++;     
		//cout << line << '\n';
    }
    myfile3.close();
	idx = 0;
  }

  else cout << "Unable to open file"; 




// trial 4 --------------------------------------
  string line4;
  ifstream myfile4 ("re4.txt");
  if (myfile4.is_open())
  {
	while ( getline (myfile4,line4) )
    {
		in4[idx] = line4;
		idx++;     
		//cout << line << '\n';
    }
    myfile4.close();
	idx = 0;
  }

  else cout << "Unable to open file"; 

// trial 5 --------------------------------------
  string line5;
  ifstream myfile5 ("re5.txt");
  if (myfile5.is_open())
  {
	while ( getline (myfile5,line5) )
    {
		in5[idx] = line5;
		idx++;     
		//cout << line << '\n';
    }
    myfile5.close();
	idx = 0;
  }

  else cout << "Unable to open file"; 


// convert to double -------------------------------------
	for(int i = 0; i < 40; i++){
		//cout << "------------ " << i << " ------------" << endl;
		//cout << "1" << endl;
		d_in1[i] = stod(in1[i]);
		//cout << "2" << endl;
		d_in2[i] = stod(in2[i]);
		//cout << "3" << endl;
		d_in3[i] = stod(in3[i]);
		//cout << "4" << endl;
		d_in4[i] = stod(in4[i]);
		//cout << "5" << endl;
		d_in5[i] = stod(in5[i]);
	}


// add and take average--------------------------
	for(int i = 0; i < 40; i++){
		in_average[i] = kahan_add_then_average(d_in1, d_in2, d_in3, d_in4, 
																d_in5, i);
	}
	
// -----------------------

	for(int i = 0; i < 40; i++){
		speedup[i] = in_average[0] / in_average[i];
	}
	for(int i = 0; i < 40; i++){
		efficiency[i] = speedup[i]/(double)i;
	}
	ofstream sp;
	ofstream ef;  	
	sp.open ("sp.dat");
	ef.open ("ef.dat");
	for(int i = 0; i < 40; i++){	
		sp << i << " " << speedup[i] << endl;
		ef << i << " " << efficiency[i] << endl;
	}
 	sp.close();
	ef.close();
  return 0;
}
