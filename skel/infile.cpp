/*$TET$templet$header*/

const int N=10;
double a[N][N],b[N][N],c[N][N];

/*$TET$*/


struct Result{
    void save(ostream&s){
/*$TET$save$result*/
	 s<<num; for(int j=0;j<N;j++)s<<c[num][j];
/*$TET$*/ 
    }
    void rest(istream&s){
/*$TET$restore$result*/
         s>>num; for(int j=0;j<N;j++)s>>c[num][j];
/*$TET$*/
    }
/*$TET$result*/
    int num;// ����� ����������� ������
/*$TET$*/
};

struct Task{
    void save(ostream&s){
/*$TET$save$task*/
	s<<num; 
/*$TET$*/
    }
    void rest(istream&s){
/*$TET$restore$task*/
        s>>num;
/*$TET$*/ 
    }
/*$TET$task*/
    int num;// ����� ����������� ������
/*$TET$*/
};

void Proc(Task&t,Result&r){
/*$TET$Proc*/
    int i=t.num;
    for(int j=0;j<N;j++){// ������������ ���������� ������ ������� C
        c[i][j]=0.0;
        for(int k=0;k<N;k++)c[i][j]+=a[i][k]*b[k][j];
    }
    r.num=i;
/*$TET$*/
}

struct Bag{
    Bag(int argc, char* argv[]){
/*$TET$bag$constructor*/
/*$TET$*/
    }
    void run();

    bool isTask(){
/*$TET$bag$isTask*/
           return cur<N;
/*$TET$*/
    }
    void put(Result&){
/*$TET$bag$put*/
/*$TET$*/
    }
    void get(Task&t){
/*$TET$bag$get*/
       t.num=cur++;
/*$TET$*/
    }

/*$TET$bag*/
    int cur;//����� ������� ������ � ������� �
/*$TET$*/
};

int main(int argc, char* argv[])
{
    Bag bag(argc,argv);

/*$TET$input*/
    // �������������
    input(a,b);
    bag.cur=0;

    // ������������ ��������� ������
/*$TET$*/ 
    bag.run();
/*$TET$output*/
    // ����� ���������� ������������� ���������
    output(c);
/*$TET$*/
    return 0;
}