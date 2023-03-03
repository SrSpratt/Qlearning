#ifndef QL_H

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

#include <cstdlib>
#include <clocale>
#include <ctime>


#define PRECISION 4
#define SPACES 9
#define ACTIONS 4
#define STATES  56// é preciso ser um múltiplo de quatro.
#define SETSEED srand(time(NULL))
#define LANG setlocale(LC_ALL,"Portuguese")
#define SPACEW setw(SPACES)

#define QL_H

using namespace std;

extern double Q[STATES][ACTIONS];

class QL{

	private:	
		//termos da equação dos Q-valores
		float gamma;
		float epsilon;
		float epsilonmin;
		float epsilon_dec;
		float lrate;
		float lratemin;
		float lrate_dec;
		
		// variáveis constantes
		static const int nstates = STATES; // estados
		static const int ncol = 4; // colunas
		
		// arquivo da LOG
		ofstream QTABLE, LOG;
		
		//variáveis da classe
		int cstate, nstate; // current state, next state
		int act; // ação
		int X [nstates]; // vetor de estados. Printá-lo-ei para mostrar o avanço do boneco no espaço
		int p; // contador de passos num episódio
			
		double r; // recompensa
			
		vector <int>act_seq; 
		vector <int>states_seq;

	public:
		//métodos da classe
		int init ();
		void initialize (int *);
		void printMap (int *, int);
		void printQ();
		bool out_of_bounds (int);
		int randomact(int);
		int greedySelection(int);
		int chooseAnAction();
		int move (int, int, double &);
		void learn(int, int, double, int);
		void episodeEpsilon ();
		void printseq (vector<int> &);
		void epsilon_calc();
		void lrate_calc();
		QL();
		~QL();
};



#endif
