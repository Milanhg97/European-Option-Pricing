#include <iostream>
#include <string>   // pour les chaines de caracteres
#include <vector>   // pour les tableaux
#include <fstream>  // pour les manipulations de fichiers
#include <iomanip>  // pour setprecision
#include <cmath>    // pour les maths
#include <stdlib.h> // pour atof() pour convertir string -> double
#include <time.h>   // time
#include <random> 
#include <tuple>
using namespace std;

double cdf_function(double x){
  double result = 0.5 * (1 + erf(x/sqrt(2)));
  return result;
}

struct param{
  double d1;
  double d2;
};

class BS{
  public:
    BS(double S, double K, double sig, double r, double T, double div): S(S), K(K), sig(sig), r(r), T(T), div(div) {};

    // Method 1 : Parameters

    param parameters(){
      param lesparametres;

      lesparametres.d1 = (log(S/K) + (r-div+pow(sig,2)/2))/(sig*T);
      lesparametres.d2 = lesparametres.d1 - sig * sqrt(T);
      return lesparametres;
    };

    // Method 2 : Pricing

    double option(bool opt){
      double d1 = parameters().d1;
      double d2 = parameters().d2;
      if (opt == true){
          return S * cdf_function(d1) - K*exp(-r*T)*cdf_function(d2); // Pricing d'un call
      } else {
        return S * (cdf_function(d1)-1) - K*exp(-r*T)*(cdf_function(d2)-1); // Pricing d'un put 
      }

    }

  private:
      double S;
      double K;
      double sig;
      double r;
      double T;
      double div;
};

vector <double> linspace(double begin, double end, int times){
  vector <double> liste;
  if (times > 0){
    double equivalent = (end - begin)/(times-1);

    for (int i = 0; i < times ; i++){
      liste.push_back(begin + i*equivalent);
    }
  }
  return liste;
};

int main() {

  double begin = 0.8;
  double end = 1.2;
  int times = 150;
  vector <double> x = linspace(begin,end,times);
  vector <double> thepricing = linspace(begin,end,times);

  for (int i = 0; i < x.size(); i++){
    BS call(x[i], 1, 0.15,0.05,1,0.03);
    double resultat = call.option(true);
    thepricing[i] = resultat;
    cout << thepricing[i] << endl;
  }

  return 0;
}