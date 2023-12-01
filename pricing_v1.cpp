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
  // https://fr.wikipedia.org/wiki/Fonction_d%27erreur
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
      if (opt == true){ // Call option
          return S * cdf_function(d1) - K*exp(-r*T)*cdf_function(d2); 
      } else { // Put option
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

  BS test(100, 110, 0.25, 0.05, 1, 0);
  bool opt = false;
  double theoption = test.option(opt);

  if (opt){
    cout << "The call option's price is equal to " << theoption << "€"<< endl;
  } else {
    cout << "The put option's price is equal to " << theoption << "€" << endl;
  }


  return 0;
}