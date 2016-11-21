/*$TET$header*/
/*--------------------------------------------------------------------------*/
/*  Copyright 2016 Sergei Vostokin                                          */
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
#include <templet.hpp>

// ������������ ��������� ������
// � �������������� �������� �����

const int P = 10;//����� ������� ��������� (������������ ������ � TEMPLET::stat)
const int N = 10;
double A[N][N], B[N][N], C[N][N];

using namespace std;
/*$TET$*/

using namespace TEMPLET;

// the task state
struct task{
// saving the tasks before sending to a worker process
	void save(saver*s){
/*$TET$task$save*/
		::save(s, &num, sizeof(num)); // ������ num
		::save(s, &A[num], sizeof(double)*N); // ������� A
/*$TET$*/
	}
// restore the task state on a worker process
	void restore(restorer*r){
/*$TET$task$restore*/
		::restore(r, &num, sizeof(num)); // ������ num
		::restore(r, &A[num], sizeof(double)*N); // ������� A
/*$TET$*/
	}
/*$TET$task$data*/
	int num;// ����� ����������� ������ ������� C
/*$TET$*/
};

// the result of task execution
struct result{
// save the result before sending to the master process
	void save(saver*s){
/*$TET$result$save*/
		::save(s, &num, sizeof(num)); // ������ num 
		::save(s, &C[num], sizeof(double)*N); // ������� C
/*$TET$*/
	}
// restore the result on the master process
	void restore(restorer*r){
/*$TET$result$restore*/
		::restore(r, &num, sizeof(num)); // ������ num 
		::restore(r, &C[num], sizeof(double)*N); // ������� C
/*$TET$*/
	}
/*$TET$result$data*/
	int num; // ����� ����������� ������ ������� C
/*$TET$*/
};

// states and methods of the master process
struct bag{
	bag(int argc, char *argv[]){
/*$TET$bag$init*/
		cur = 0; // �������� ���������� � ������� ������
/*$TET$*/
	}
	void run();
	
// task extraction method, if there is no task - it returns false
	bool get(task*t){
/*$TET$bag$get*/
		if (cur<N){ t->num = cur++; return true; }
		else return false;
/*$TET$*/
	}
// placing the result method
	void put(result*r){
/*$TET$bag$put*/
// � ���� ������� �� ������� �����������
/*$TET$*/
	}
// saving worker processes common state method
	void save(saver*s){
/*$TET$bag$save*/
		::save(s, &B, sizeof(double)*N*N); // ������� B
/*$TET$*/
	}
//  restoring worker processes common state method
	void restore(restorer*r){
/*$TET$bag$restore*/
		::restore(r, &B, sizeof(double)*N*N); // ������� B 
/*$TET$*/
	}
/*$TET$bag$data*/
	int cur; // ������ �� ������� ������� C
/*$TET$*/
};

// worker process task execution procedure
void proc(task*t,result*r)
{
/*$TET$proc$data*/
	int i = r->num = t->num;
	for (int j = 0; j<N; j++){// ������������ ���������� ������ ������� C
		C[i][j] = 0.0;
		for (int k = 0; k<N; k++)C[i][j] += A[i][k] * B[k][j];
	}
/*$TET$*/
}

/*$TET$footer*/
int main(int argc, char* argv[])
{
	bag b(argc, argv);

	// �������������
	for (int i = 0; i<N; i++)
		for (int j = 0; j<N; j++)A[i][j] = N*i + j;

	for (int i = 0; i<N; i++)
		for (int j = 0; j<N; j++)B[i][j] = N*i + j;

	// ������������ ��������� ������
	b.run();

	// ����� ���������� ������������� ���������
	// ��������� ��� ������� N
	cout << "\nC(parallel)=\n";
	for (int i = 0; i<N; i++){
		for (int j = 0; j<N; j++){
			cout << C[i][j] << " ";
		}
		cout << '\n';
	}

	// ������� ������� �
	for (int i = 0; i<N; i++)
		for (int j = 0; j<N; j++) C[i][j] = 0.0;

	// ���������������� ��������� ������
	for (int i = 0; i<N; i++){
		for (int j = 0; j<N; j++){
			C[i][j] = 0.0;
			for (int k = 0; k<N; k++)C[i][j] += A[i][k] * B[k][j];
		}
	}

	// ����� ���������� ����������������� ���������
	// ��������� ��� ������� N
	cout << "\nC(serial)=\n";
	for (int i = 0; i<N; i++){
		for (int j = 0; j<N; j++){
			cout << C[i][j] << " ";
		}
		cout << '\n';
	}
	return 0;
}
/*$TET$*/
