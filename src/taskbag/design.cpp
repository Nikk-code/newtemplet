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

// ��������� ������
struct task{
// ���������� ������ ����� ��������� �������� ��������
	void save(saver*s){
/*$TET$task$save*/
		::save(s, &num, sizeof(num)); // ������ num ������� A
		::save(s, &A[num], sizeof(double)*N);
/*$TET$*/
	}
// �������������� ��������� ������ �� ������� ��������
	void restore(restorer*r){
/*$TET$task$restore*/
		::restore(r, &num, sizeof(num)); // ������ num ������� A
		::restore(r, &A[num], sizeof(double)*N);
/*$TET$*/
	}
/*$TET$task$data*/
	int num;// ����� ����������� ������ ������� C
/*$TET$*/
};

// ��������� ���������� ������
struct result{
// ���������� ���������� ����� ��������� ������������ ��������
	void save(saver*s){
/*$TET$result$save*/
		::save(s, &num, sizeof(num)); // ������ num 
		::save(s, &C[num], sizeof(double)*N); // ������� C
/*$TET$*/
	}
// �������������� ���������� �� ����������� ��������
	void restore(restorer*r){
/*$TET$result$restore*/
		::restore(r, &num, sizeof(num)); // ������ num 
		::restore(r, &C[num], sizeof(double)*N); // ������� C
/*$TET$*/
	}
/*$TET$result$data*/
	int num; // ����� ����������� ������ ������� c
/*$TET$*/
};

// ��������� � ������ ������������ ��������
struct bag{
	bag(int argc, char *argv[]){
/*$TET$bag$init*/
		cur = 0;
/*$TET$*/
	}
	void run();
	void delay(double);
// ����� ���������� ������, ���� ������ ��� -- ���������� false
	bool get(task*t){
/*$TET$bag$get*/
		if (cur<N){ t->num = cur++; return true; }
		else return false;
/*$TET$*/
	}
// ����� ��������� ���������� ���������� ������
	void put(result*r){
/*$TET$bag$put*/
/*$TET$*/
	}
// ���������� ���������, ������ ��� ������� ���������
	void save(saver*s){
/*$TET$bag$save*/
		::save(s, &B, sizeof(double)*N*N); // ������� B
/*$TET$*/
	}
// �������������� ������ ��������� �� ������� ���������
	void restore(restorer*r){
/*$TET$bag$restore*/
		::restore(r, &B, sizeof(double)*N*N); // ������� B 
/*$TET$*/
	}
/*$TET$bag$data*/
	int cur;//����� ������� ������ � ������� �
/*$TET$*/
};

void delay(double);

// ��������� ���������� ������ �� ������� ��������
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

	double T1, Tp, Smax, Sp;
	int Pmax;

	if (TEMPLET::stat(&b, &T1, &Tp, &Pmax, &Smax, P, &Sp)){
		std::cout << "T1 = " << T1 << ", Tp = " << Tp << ", Pmax = " << Pmax << ", Smax = " << Smax << ", P = " << P << ", Sp = " << Sp;
	}

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
