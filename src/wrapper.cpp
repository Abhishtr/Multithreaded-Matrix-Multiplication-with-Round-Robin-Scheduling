// compile this file as "g++ script.cpp -o script.out"

#include<bits/stdc++.h>
#include<unistd.h>
using namespace std;
int main(int argc, char *argv[])
{
	if (argc != 7)
	{
		printf("Number of Arguments do not match, execute the script as\n./script.out i j k in1.txt in2.txt out.txt\n");
		exit(-1);
	}
	int x = atoi(argv[1]);
	int y = atoi(argv[2]);
	int z = atoi(argv[3]);
	string inputFile1 = argv[4];
	string inputFile2 = argv[5];
	string outputFile = argv[6];
	
	/*-----Comment this block if you dont want the wrapper to call MatrixGen.py and supply the input files yourself-------*/
	string executeMatrixGen="python3 MatrixGen.py";
	executeMatrixGen+=" "+ to_string(x) + " " + to_string(y) + " " + to_string(z) + " " + inputFile1 + " " + inputFile2;
	system(executeMatrixGen.data());
	sleep(1);
	/*------------------------------------*/

	int threadForReadingMatrix1 = 20;
	int threadForReadingMatrix2 = 20;
	int threadForCalculatingProductMatrix= 20;
	
	//create the customeheader
	FILE *fp;
    fp = fopen("customHeader.h","w");
	fprintf(fp,"#define x %d\n",x);
	fprintf(fp,"#define y %d\n",y);
	fprintf(fp,"#define z %d\n",z);
	fprintf(fp,"#define threadForReadingMatrix1 %d\n",threadForReadingMatrix1);
	fprintf(fp,"#define threadForReadingMatrix2 %d\n",threadForReadingMatrix2);	
	fprintf(fp,"#define threadForCalculatingProductMatrix %d\n",threadForCalculatingProductMatrix);
	fclose(fp);
	
	//compile the files
	system("gcc P1.c -lpthread -o P1.out");
	system("gcc P2.c -lpthread -o P2.out");
	system("gcc Scheduler.c -o S.out");
	
	//Building Execution arguments for S.out
	string executeS="./S.out";
	executeS += " " + inputFile1 + " " + inputFile2 + " " + outputFile;
	//cout<<executeS<<'\n';
	
	//Executing S.out
	system(executeS.data());
	//S.out will in turn start P1.out and P2.out and pass the respective arguments
	
	return 0;
}
