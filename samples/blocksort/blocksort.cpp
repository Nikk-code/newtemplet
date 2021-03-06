/*$TET$actor*/
/*--------------------------------------------------------------------------*/
/*  Copyright 2017 Sergei Vostokin                                          */
/*                                                                          */
/*  Licensed under the Apache License, Version 2.0 (the "License");         */
/*  you may not use this file except in compliance with the License.        */
/*  You may obtain a copy of the License at                                 */
/*                                                                          */
/*  http://www.apache.org/licenses/LICENSE-2.0                              */
/*                                                                          */
/*  Unless required by applicable law or agreed to in writing, software     */
/*  distributed under the License is distributed on an "AS IS" BASIS,       */
/*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*/
/*  See the License for the specific language governing permissions and     */
/*  limitations under the License.                                          */
/*--------------------------------------------------------------------------*/

#include <iostream>

//#define  PARALLEL_EXECUTION
//#define  USE_OPENMP

#define SIMULATED_EXECUTION
const double TRADEOFF = 0.0;

#include <templet.hpp>

#include <omp.h>
#include <algorithm>

#include <stdlib.h>

using namespace std;

// 1
//const int NUM_BLOCKS = 2;
//const int BLOCK_SIZE = 64000000;
// 2
//const int NUM_BLOCKS = 4;
//const int BLOCK_SIZE = 32000000;
// 3
//const int NUM_BLOCKS = 8;
//const int BLOCK_SIZE = 16000000;
// 4
//const int NUM_BLOCKS = 16;
//const int BLOCK_SIZE = 8000000;
// 5
//const int NUM_BLOCKS = 32;
//const int BLOCK_SIZE = 4000000;
// 6
//const int NUM_BLOCKS = 64;
//const int BLOCK_SIZE = 2000000;
// 7
const int NUM_BLOCKS = 128;
const int BLOCK_SIZE = 1000000;

int block_array[NUM_BLOCKS*BLOCK_SIZE];

int num_of_block_sort = 0;
int num_of_block_merge = 0;

void block_sort(int block_num)
{
	std::sort(&block_array[block_num*BLOCK_SIZE], &block_array[(block_num+1)*BLOCK_SIZE]);

	num_of_block_sort++;
}

void block_merge(int less_block_num, int more_block_num)
{
	int* tmp_array = (int*)malloc(sizeof(int)*(2*BLOCK_SIZE));

	if (!tmp_array) {
		std::cout << "Memory allocation failed!!!\n";
		exit(1);
	}

	std::merge(&block_array[less_block_num*BLOCK_SIZE], &block_array[(less_block_num + 1)*BLOCK_SIZE],
		&block_array[more_block_num*BLOCK_SIZE], &block_array[(more_block_num + 1)*BLOCK_SIZE],
		&tmp_array[0]);
	std::copy(&tmp_array[0], &tmp_array[BLOCK_SIZE], &block_array[less_block_num*BLOCK_SIZE]);
	std::copy(&tmp_array[BLOCK_SIZE], &tmp_array[2*BLOCK_SIZE], &block_array[more_block_num*BLOCK_SIZE]);

	free(tmp_array);

	num_of_block_merge++;
}

bool is_sorted()
{
	int prev = block_array[0];
	for (int i = 1; i < NUM_BLOCKS*BLOCK_SIZE; i++) {
		if (prev > block_array[i])return false;
		prev = block_array[i];
	}
	return true;
}

/*$TET$*/

using namespace TEMPLET;

#pragma templet ~mes=

struct mes : message_interface{
/*$TET$mes$$data*/
	int i;
/*$TET$*/
};

#pragma templet *sorter(out!mes)+

struct sorter : actor_interface{
	sorter(engine_interface&){
/*$TET$sorter$sorter*/
/*$TET$*/
	}

	mes out;

	void start(){
/*$TET$sorter$start*/
#if defined(SIMULATED_EXECUTION)
		double time;
		time = omp_get_wtime();
#endif
		block_sort(i); 
#if defined(SIMULATED_EXECUTION)
		time = omp_get_wtime() - time;
		delay(time*(1+TRADEOFF));
#endif
		out.send();
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$sorter$out*/
/*$TET$*/
	}

/*$TET$sorter$$code&data*/
	int i;
/*$TET$*/
};

#pragma templet *producer(in?mes,out!mes)

struct producer : actor_interface{
	producer(engine_interface&){
/*$TET$producer$producer*/
		bc = NUM_BLOCKS;
		i = 0;
/*$TET$*/
	}

	void in(mes&){}
	mes out;

	void in_handler(mes&m){
/*$TET$producer$in*/
		bc--;
		if (!bc) out_handler(m);
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$producer$out*/
		if (i == NUM_BLOCKS) return;
		out.i = i++;
		out.send();
/*$TET$*/
	}

/*$TET$producer$$code&data*/
	int i,bc;
/*$TET$*/
};

#pragma templet *merger(in?mes,out!mes)

struct merger : actor_interface{
	merger(engine_interface&){
/*$TET$merger$merger*/
		is_first=true;
		_in=0;
/*$TET$*/
	}

	void in(mes&){}
	mes out;

	void in_handler(mes&m){
/*$TET$merger$in*/
		_in=&m;
		merge();
/*$TET$*/
	}

	void out_handler(mes&m){
/*$TET$merger$out*/
		merge();
/*$TET$*/
	}

/*$TET$merger$$code&data*/
	void merge(){
		if (!(access(_in) && access(out)))return;       

		if(is_first){
			is_first=false;	j = _in->i;
			_in->send();
		}
		else{
#if defined(SIMULATED_EXECUTION)
			double time;
			time = omp_get_wtime();
#endif
			block_merge(j,_in->i);
#if defined(SIMULATED_EXECUTION)
			time = omp_get_wtime() - time;
			delay(time*(1 + TRADEOFF));
#endif
			out.i = _in->i;
			_in->send();out.send();
		}                                              
	}

	int  j;
	bool is_first;
	mes* _in;
/*$TET$*/
};

#pragma templet *stopper(in?mes)

struct stopper : actor_interface{
	stopper(engine_interface&){
/*$TET$stopper$stopper*/
/*$TET$*/
	}

	void in(mes&){}

	void in_handler(mes&m){
/*$TET$stopper$in*/
		stop();
/*$TET$*/
	}

/*$TET$stopper$$code&data*/
/*$TET$*/
};

int main(int argc, char *argv[])
{
	engine_interface e(argc, argv);
/*$TET$footer*/
	system("uname -a");// for Linux

	std::cout << "\nNUM_BLOCKS = " << NUM_BLOCKS << endl
		<< "BLOCK_SIZE = " << BLOCK_SIZE << endl
		<< "OMP_NUM_PROCS = " << omp_get_num_procs() << endl;

	srand(1); for (int i = 0; i < NUM_BLOCKS*BLOCK_SIZE; i++)	block_array[i] = rand();
	
	double time = omp_get_wtime();
	std::sort(&block_array[0], &block_array[NUM_BLOCKS*BLOCK_SIZE]);                
	time = omp_get_wtime() - time;

	std::cout << "\nSequential sort time is " << time << " sec\n";

	//////////////////// sequential blocksort /////////////////////
	srand(1); for (int i = 0; i < NUM_BLOCKS*BLOCK_SIZE; i++)	block_array[i] = rand();
	
	time = omp_get_wtime();

	for (int i = 0; i<NUM_BLOCKS; i++) block_sort(i);                                 
	for (int i = 1; i<NUM_BLOCKS; i++) for (int j = 0; j<i; j++) block_merge(j, i);   
	
	time = omp_get_wtime() - time;

	if (!is_sorted())std::cout << "\nSomething went wrong in the sequential block-sort!!!\n";
	else std::cout << "Sequential block-sort time is " << time << " sec\n";
	///////////////////////////////////////////////////////////////

	/////////////////// parallel actor blocksort //////////////////
	producer a_producer(e);
	stopper a_stoper(e);

	sorter** a_sorter = new sorter*[NUM_BLOCKS];
	for (int i = 0; i < NUM_BLOCKS; i++) { 
		a_sorter[i] = new sorter(e); 
		a_sorter[i]->i = i;
		a_producer.in(a_sorter[i]->out);
	}

	merger** a_merger = new merger*[NUM_BLOCKS-1];
	for(int i=0;i<NUM_BLOCKS-1;i++)a_merger[i]=new merger(e);
	
	mes* prev=&a_producer.out;
	for(int i=0;i<NUM_BLOCKS -1;i++){
		a_merger[i]->in(*prev);
		prev=&(a_merger[i]->out);
	}
	a_stoper.in(*prev);

	srand(1); for (int i = 0; i < NUM_BLOCKS*BLOCK_SIZE; i++)	block_array[i] = rand();
	
	num_of_block_sort = 0;
	num_of_block_merge = 0;

	time = omp_get_wtime();
	e.run();
	time = omp_get_wtime() - time;

	if (!is_sorted())std::cout << "\nSomething went wrong in the parallel actor block-sort!!!\n";
	else std::cout << "\nParallel block-sort time is " << time << " sec\n";

#if defined(SIMULATED_EXECUTION)
	double T1, Tp;
	int Pmax;
	double Smax;
	int P=1;
	double Sp;
	
	std::cout << "num_of_block_sort = " << num_of_block_sort << "  num_of_block_merge = " << num_of_block_merge << " total tasks = " <<
		num_of_block_sort + num_of_block_merge << "\n";

	if (TEMPLET::stat(&e, &T1, &Tp, &Pmax, &Smax, P, &Sp)) {
		std::cout << "\nSimulated sequential time = " << T1 / (1 + TRADEOFF) << " sec\n"
			<< "Simulated parallel time = " << Tp << " sec\n" 
			<< "Number of procs in the simulation = " << Pmax << endl
			<< "TRADEOFF is " << TRADEOFF*100. << " percents";
	}
#endif
	return 0;
	/////////////////////////////////////////////////////////////////
	
/*$TET$*/
}
