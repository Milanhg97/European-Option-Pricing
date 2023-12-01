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
#include <algorithm> // Pour max

using namespace std;

double cdf_function(double x){
  // This method allows to compute the cumulative distribution function of a normal distribution.
  double result = 0.5 * (1 + erf(x/sqrt(2)));
  return result;
}

struct param{
  // Structure to memorize both d1 and d2 parameters of class BS
  double d1;
  double d2;
};

class BS{
  public:
    BS(double S, double K, double sig, double r, double T): S(S), K(K), sig(sig), r(r), T(T) {};

    // Method 1 : Parameters

    param parameters(){ // Venant de la structure
      param lesparametres; // Appelle de la structure, et on renomme lesparametres

      lesparametres.d1 = (log(S/K) + (r+pow(sig,2)/2))/(sig*sqrt(T));
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

    // Method 3 : Pricing conclusion

    void pricing_conclusion(bool opt){
      if (opt == true){
        cout << "The call option's price by BS : " << option(opt) << "€" << endl;
      } else {
        cout << "The put option's price by BS : " << option(opt) << "€" << endl;
      };
     };
  private:
      double S;
      double K;
      double sig;
      double r;
      double T;
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

//struct pathes{
//  vector <vector<double>> allpathes;
//};

class MC{
  public:
    MC(double S0, double K, double sig, double mu, double r, double T): S0(S0), K(K), sig(sig), mu(mu), r(r), T(T) {};

  double random_number(){
    //https://en.cppreference.com/w/cpp/numeric/random/normal_distribution
    // Initialization of the random number's generator
    random_device rd;
    mt19937 gen(rd());
    // Normal distribution standard (Mean = 0, Standard-deviation = 1)
    normal_distribution<double> distribution(0.0, 1.0);
    
    return distribution(gen);
  };

  vector <double> wienner(int nb_times){
    // This method returns a vector in which inside, there are the sum of past sum of the wienner's function (as a brownian motion in time t)
    double dt = T/nb_times;
    double somme = 0;
    vector <double> cumul = {0};
    for (int i = 0; i < nb_times; i++){
      double rand_number = random_number(); 
      somme += rand_number;
      cumul.push_back(sqrt(dt) * somme);
    };
    return cumul;
  };

  vector <vector <double>> generator_BM(int nb_times, int nb_path){
    // This method allows to generate many paths of Brownian Motion
    double dt = T/nb_times;
    vector <double> S = {S0};
    vector <vector<double>> pathes;
    for (int h = 0; h < nb_path; h++){ // Number of simulations
      vector <double> W = wienner(nb_times); // Here, else, the Geometric Brownian Motions will be the same.
      for (int i = 0; i < W.size(); i++){ // Number of observation per trajectory
        S.push_back(S0 * exp((mu - 0.5 * pow(sig,2))*dt + sig * W[i]));
      };
      pathes.push_back(S);
    };
    return pathes;
  };

  double MonteCarlo(int nb_times, int nb_path, bool type_opt){
    // Type_opt : Pricing of call option if true, put else.
    vector <vector <double>> pathes = generator_BM(nb_times, nb_path);
    double somme = 0;
    if (type_opt){
      for (int i = 0; i < pathes.size();i++){
        if(pathes[i].back() - K > 0){
          somme += pathes[i].back() - K;
        } else {
          somme += 0;
        };

      }
    } else {
        for (int i = 0; i < pathes.size(); i++){
          if (K-pathes[i].back() > 0){
            somme += K-pathes[i].back();
          } else {
            somme += 0;
          };
        };
    };
    int nb_simul = pathes.size();
    double mean = somme/nb_simul;
    return mean*exp(-r*T);
  };

  void pricing_conclusion(int nb_times, int nb_path, bool type_opt){
    if (type_opt == true){
      cout << "The call option's price with MC is : " << MonteCarlo(nb_times, nb_path, type_opt) << "€" << endl;
    } else {
      cout << "The put option's price with MC is : " << MonteCarlo(nb_times, nb_path, type_opt) << "€" << endl;
    };
  };

  private:
    double S0;
    double K; 
    double sig;
    double mu;
    double r;
    double T;
};


int main() {

  double S0 = 100;
  double K = 90;
  double sig = 0.25;
  double mu = 0.03;
  double r = 0.03;
  double T = 0.25;

  // PRICING with BS
  BS callBS(S0, K, sig, r, T);
  bool opt = true;
  //double resultatBS = callBS.option(opt);
  //cout << "Le prix d'un call avec BS : " << resultatBS << "€" << endl;
  callBS.pricing_conclusion(opt);

  // PRICING with MONTE CARLO
  int nb_path = 1200;
  int nb_times = 150;
  bool type_opt = true;
  MC callMC(S0, K, sig, mu, r, T);
  //double resultatMC = callMC.MonteCarlo(nb_times, nb_path, type_opt);
  //cout << "Le prix d'un call avec MC : " << resultatMC << "€" << endl;
  callMC.pricing_conclusion(nb_times, nb_path, type_opt);

  return 0;
}