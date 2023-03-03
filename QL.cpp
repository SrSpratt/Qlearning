#include "QL.h"

using namespace std;

double Q[STATES][ACTIONS];

QL::QL(){
	//termos da equa��o dos Q-valores
		gamma = 0.8;
		epsilon = 1;
		epsilonmin = 0.001;
		epsilon_dec = 0.97;
		lrate = 0.33;
		lratemin = 0.001;
		lrate_dec = 0.9999;
		
		// vari�veis constantes
		act = 0; // a��o
		p = 0; // contador de passos num epis�dio
		r = 0; // recompensa
}

QL::~QL(){
	
}

int QL::init () {

	LANG;
	SETSEED;
	
	initialize(X);
	
	cstate = 0;
	//r = 0;
	
	LOG.open("LOGII.txt", ios::out);	
	
	for (int n = 0; n < 100; n++){
		
		p = 0;
		//printMap(X, cstate);
		
		episodeEpsilon();
		
		LOG << "\n\n\n" << p << " passos foram tomados."<< endl;
		
		LOG << "A sequ�ncia de estados foi:" << endl << "0" << endl;
		printseq(states_seq);
	
	
		LOG << "A sequ�ncia de a��es foi:" << endl;
		printseq(act_seq);
	
		cstate = 0;
	
		epsilon_calc();
		lrate_calc();
	}
	
	LOG.close();
	
	QTABLE.open("QTABLE.txt", ios::out);
	printQ();
	QTABLE.close();
	
}

void QL::initialize (int *X){
	for (int i = 0; i< nstates; i++){
		X[i] = 0;
		int j =0;
		do {
			// lembrar que, na matriz Q, i representa os estados do vetor e j representa as a��es

			if ( ( ((i%ncol) == 0) && (j == 2) ) || ( ((i%ncol) == (ncol-1) ) && (j == 3) ) || ( ((i/ncol) == 0) && (j == 0) ) || ( ((i/ncol) == (nstates-1)/ncol) && (j == 1) ) ) // restringe o movimento � esquerda, � direita, acima e abaixo, respectivamente
				Q[i][j] = -10.000;
			else
				Q[i][j] = 0.000;
		
			j++;
		} while (j < ACTIONS);
	}
}

void QL::printMap (int *X, int act){
	
	LOG << "\n\n"<< SPACEW << "MAPA" << SPACEW << "\n\n";
	LOG << "|";
	for (int i=0; i<nstates; i++){
		if (act == i)
			LOG << SPACEW << " " << SPACEW;
		else
			LOG << SPACEW << X[i] << SPACEW;
		if ((i%ncol) == (ncol-1)){
			if ((i/ncol) == ((nstates/ncol)-1))
				LOG << "|\n\n";
			else 
				LOG << "|\n|";
		}
	}
}

void QL::printQ(){
	for (int i = 0; i<nstates; i++){
		QTABLE << "|";
		for (int j = 0; j<ACTIONS; j++){
			QTABLE << setprecision(PRECISION);
			QTABLE << SPACEW << Q[i][j] << SPACEW;
		}
		QTABLE << "|" << endl;
	}
	QTABLE << endl;

}
//int out_of_bounds (int cstate, int nstates){
//	return ( ((cstate > (nstates-1)) || (cstate < 0)) ? true : false);
//}

bool QL::out_of_bounds (int opt){
	switch (opt){
		case 0:
			if ( (cstate/ncol) == 0)
				return true;
			break;	
		case 1:
			if ( (cstate/ncol) == (nstates-1)/ncol)
					return true;
			break;
		case 2:
			if ( (cstate%ncol) == 0)
				return true;
			break;
		case 3:
			if ( (cstate%ncol) == ncol-1)
				return true;
			break;
	}
	return false;
}

int QL::randomact(int cstate){
	
	// 0: sobe, 1: desce, 2: vai � esquerda, 3: vai � direita
	
	int o; // a��o
	bool can = true; // verifica a possibilidade da a��o
	
	do {
		o = rand()%ACTIONS;
	} while (out_of_bounds(o));
		
	return o;
}

int QL::greedySelection(int cstate){
	
	double biggest = -1000000000.0;
    int bestAction = -1;
    int numTies = 0;
	
    for (int i = 0; i < ACTIONS; i++){
		if (out_of_bounds(i))	
			continue;
			
		if (biggest < Q[cstate][i]){
        	biggest = Q[cstate][i];
            bestAction = i;
        } else if (biggest == Q[cstate][i]){
	            numTies++; // n�mero de empates
	//            	if (i == (ncol-1)){
	//            		bestAction = randomact(cstate);
	//            		LOG << "\nAgi aleatoriamente por falta de dados\n";
	//            	}
            if (rand() % (numTies + 1) == 0){ // se o n�mero de empates for demasiado, escolher qualquer a��o dentre as encolatadas
                biggest = Q[cstate][i];
                bestAction = i;
            }
        }
    }
	return bestAction;    
}

int QL::chooseAnAction(){
				
	if (((float)rand()/RAND_MAX) < epsilon){
		act = randomact(cstate);
		LOG << "\nA PR�XIMA A��O SER� ALEAT�RIA\n\n\n\n---------------------------------------------------";
	} else {
		act = greedySelection (cstate);
		LOG << "\nA PR�XIMA A��O SER� INTELIGENTE\n\n\n\n--------------------------------------------------";
	}
				
	return act;
}

int QL::move (int cstate, int o, double &r){

	int astate = cstate;
	int na, nc;

	r = 0;	

	switch (o){
		case 0:
			cstate -= ncol;
			break;
		case 1:
			cstate += ncol;
			break;
		case 2:
			cstate --;
			break;
		case 3:
			cstate ++;
			break;
		default:
			cstate = cstate;
			break;
	}
	//	nc = ( ((nstates-1)/ncol)-(cstate/ncol) )+( (ncol-1)-(cstate%ncol) ); // (nlin - slin) - (ncol - scol) -> nx, n�mero total; cx, n�mero do estado
	//	na = ( ((nstates-1)/ncol)-(astate/ncol) )+( (ncol-1)-(astate%ncol) );
	//	if (astate == 19)
	//		a = 5;
	//	else if (cstate == 19)
	//		a = 1;
	//	else 
	//	a = 1.0/nc; // aumenta quando se aproxima do objetivo, diminui quando se distancia;
	//	LOG << ((nstates-1)/ncol) << endl;
	//	LOG << (cstate/ncol) << endl;
	//	LOG << (ncol-1) << endl;
	//	LOG << (cstate%ncol) << endl;
	//	LOG << endl << astate << "  " <<  cstate << endl;
	//	LOG << endl << na << "  " <<  nc<< endl;
	//sleep(1);
	
	//LOG << "\n\n\n VALOR DE A: " << a << "\n\n\n";
	
	//if ( na < nc/*( ((astate/ncol))-((ncol-1)-(astate%ncol)) ) < ( ((cstate/ncol))-((ncol-1)-(cstate%ncol)) )*/) // se a dist�ncia anterior for maior do que a nova, o movimento foi errado;
		//r -= a; // objective - displacement to objective | puni��o
	//else
		//r += a;
	if (cstate == (nstates-1))
		r = 1;
	//else if ( (cstate == (nstates-1)/ncol) || (cstate == (nstate-(nstate/ncol)-1)) )
		//r = -1;
	else
		r = 0;
	//LOG << "\n\n\n RECOMPENSA: " << r << "\n\n\n";

	LOG << "\nESTIVE EM: " <<  astate << "\n";
	LOG << "\nVIM PARA: " <<  cstate << "\n";
	
	return cstate;
}

void QL::learn(int u, int v, double r, int a){
	double foretell;
	foretell = Q[u][a];
	double target = r + gamma * Q[v][greedySelection(v)];

	Q[u][a] += lrate * (target - foretell);
}

void QL::printseq (vector<int> &vec){

	for (int c = 0; c < p; c++) 
		LOG << vec[c] << endl;
	LOG << endl;
	
	states_seq.clear();
}

void QL::episodeEpsilon (){
	do { // Escolhe a a��o at� o epis�dio terminar
		
		act = chooseAnAction (); // escolhe uma a��o
			
		nstate = move(cstate, act, r); // prev� o resultado da a��o
		
		act_seq.push_back(act); // adiciona a �ltima a��o ao vetor de a��es

		learn(cstate, nstate, r, act); // atualiza a LOG
		
		cstate = nstate; // atualiza o currentstate
		
		states_seq.push_back(cstate); // adiciona o �ltimo estado ao vetor de estados
		
		printMap(X, cstate);
		
		p++;
		
	//	sleep(1);
	} while ((p < 150) && (cstate != nstates-1));

}

void QL::epsilon_calc(){
	if (epsilon > epsilonmin)
			epsilon *= epsilon_dec;
}

void QL::lrate_calc(){
	if (lrate > lratemin)
			lrate *= lrate_dec;
}
