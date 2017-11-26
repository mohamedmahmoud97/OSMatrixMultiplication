#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void startRandommat();
void initRandomMat(double** X,int r,int c,int limit);
void nonThreadedMatMult(double**A, double**B, FILE*outFile);
void readMatrixFiles(FILE*inFile1, FILE*inFile2);
int threadedMatMultPerElement(char *argv[], FILE*outFile);
void *elementCalc(void*param);
int threadedMatMultPerRow(char *argv[], FILE*outFile);
void *rowCalc(void*param);
int fpeek(FILE *stream);

int x=0, y_1=0, y2=0, z=0;
int a=0,b=0,c=0;
double **A, **B, **C;

typedef struct threadargs{
    int arg1;
    int arg2;
}threadargs;

int main(int argc, char *argv[])
{
	FILE *inFile1;
	FILE *inFile2;
	FILE *outFile;

	int noOfTinE, noOfTinR;

	startRandommat();

	//readMatrixFiles(inFile1, inFile2);	
	nonThreadedMatMult(A,B,outFile);
	noOfTinE = threadedMatMultPerElement(argv, outFile);
	noOfTinR = threadedMatMultPerRow(argv, outFile);
	printf("# of threads in elements used: %d\n", noOfTinE);
	printf("# of threads in rows used: %d\1", noOfTinR);

	free(A);
	free(B);
	//fclose(inFile1);
    //fclose(inFile2);
    //fclose(outFile);

	return 0;
}

void startRandommat()
{
	x=3000,y_1=3000,y2=3000,z=3000;
	A = (double **)malloc(2*x * sizeof(double*));
	for(int i = 0; i < x; i++)
	{
	    A[i] = (double *)malloc(2*y_1 * sizeof(double));
	}

	B = (double **)malloc(2*y2 * sizeof(double*));
	for(int i = 0; i < y2; i++)
	{
	    B[i] = (double *)malloc(2*z * sizeof(double));
	}
	initRandomMat(A,x,y_1,50);
	initRandomMat(B,y2,z,50);
}

void initRandomMat(double** X,int r,int c,int limit)
{
    time_t seed;
    srand((unsigned) time(&seed));
    for (int i = 0; i < r;++i) {
        for (int j = 0; j < c; ++j) {
            X[i][j] = rand() % limit;
        }
    }
}

void readMatrixFiles(FILE*inFile1, FILE*inFile2)
{
	bool fileFound = false;
	bool validSize = false;

	int i,j,k;
	int count;

	double temp;

	//open files and set mode to read 
	do
	{
    	inFile1 = fopen("matA.txt","r");
    	if(!inFile1)
    	{
        	printf("File Not Found!!\n");
           	fileFound=true;
    	}
    	else
        	fileFound=false;

	}while(fileFound);

	do
	{
    	inFile2 = fopen("matB.txt","r");
    	if(!inFile2)
    	{
            printf("File Not Found!!\n");
            fileFound=true;
    	}
    	else
        	fileFound=false;

	}while(fileFound);

	//////////////////////////////////////////////////////////////////////////////////////

	//Read matrix A of unknown sizefrom file 1
	while(!feof(inFile1))
	{
		count = 0;
		while(fscanf(inFile1, "%lf", &temp) && fpeek(inFile1)!='\n' && !feof(inFile1))
		{
			if(x==0)
			{
				y_1++;	
			}
			else
			{
				if(count>y_1)
				{
					printf("There is an error in line number: %d\n", x);
					return;
				}
				count++;
			}
		}
		if (x==0)
			y_1++;
		x++;
	}

	fseek(inFile1,0,0);

	A = (double **)malloc(2*x * sizeof(double*));
	for(i = 0; i < x; i++)
	{
	    A[i] = (double *)malloc(2*y_1 * sizeof(double));
	}

	for(i=0;i<x;i++)
    {
        for(j=0;j<y_1;j++){
            fscanf(inFile1, "%lf", &A[i][j]);
        }
    }


	//Read matrix B of unknown sizefrom file 2
	while(!feof(inFile2))
	{
		count = 0;
		while(fscanf(inFile2, "%lf", &temp) && fpeek(inFile2)!='\n' && !feof(inFile2))
		{
			if(y2==0)
			{
				z++;	
			}
			else
			{
				if(count>z)
				{
					printf("There is an error in line number: %d\n", y2);
					return;
				}
				count++;
			}
		}
		if (y2==0)
			z++;
		y2++;
	}

	fseek(inFile2,0,0);

	B = (double **)malloc(2*y2 * sizeof(double*));
	for(i = 0; i < y2; i++)
	{
	    B[i] = (double *)malloc(2*z * sizeof(double));
	}

	for(i=0;i<y2;i++)
    {
        for(j=0;j<z;j++){
            fscanf(inFile2, "%lf", &B[i][j]);
        }
    }

	//check if they have valid sizes to be multipilcated
	if(y_1!=y2)
    	validSize = false;
	else
    	validSize = true;//

    if(validSize == false)
	{
    	printf("Matrix Sizes are not valid you Fugget!! \nCant multiply those two matrices \n");
    	fclose(inFile1);
    	fclose(inFile2);
    	return;
	}
	else
	{
		printf("matrices sizes are valid\n");
		
	    //////////////////////////////////////////////////
	    printf("\n\n");
	    //print matrix 1
	   // printf("MatA:\n");
	   // for(i=0;i<x;i++)
	   // {
	   //     for(j=0;j<y_1;j++)
	   //         printf("%lf\t",A[i][j]);
//
	   //     printf("\n");
	   // }
		////////////////////////////////////////////////
	   // printf("\n\n");
	    //print matrix 2
	   // printf("MatB:\n");
	   // for(i=0;i<y2;i++)
	   // {
	   //     for(j=0;j<z;j++)
	   //         printf("%lf\t",B[i][j]);
//
	   //     printf("\n");
	   // }
		/////////////////////////////////////////////
	}
	printf("\nMatrices are read successfully\n\n");
	return;
}

void nonThreadedMatMult(double**A, double**B, FILE*outFile)
{
	clock_t start, end;
 	double cpu_time_used;

 	start = clock();

 	C = (double **)malloc(2*x * sizeof(double*));
		for(int i = 0; i < x; i++)
		    C[i] = (double *)malloc(2*z * sizeof(double));

	int i=0, j=0, k=0;
	bool fileFound = false;

	for (i = 0; i < x; ++i)
	{
		for (j = 0; j < z; ++j)
		{
			for (k = 0; k < y_1; ++k)
			{
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}

	//open files and set mode to read 
	do
	{
    	outFile = fopen("matC.txt","w");
    	if(!outFile)
    	{
        	printf("File Not Found!!\n");
           	fileFound=true;
    	}
    	else
        	fileFound=false;

	}while(fileFound);

	for(i=0;i<x;i++)
	{
	    for(j=0;j<z;j++)
	        fprintf(outFile, "%lf\t",C[i][j]);
	    fprintf(outFile, "\n");
	}

	printf("MatC:\n");
	for(i=0;i<x;i++)
	{
        for(j=0;j<z;j++)
        printf("%lf\t",C[i][j]);
	
        printf("\n");
    }
    printf("\n");

    end = clock();
 	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
 	printf("time taken for multiplication without threads: %lf sec\n\n", cpu_time_used);
 	free(C);
	return;
}

int threadedMatMultPerElement(char *argv[], FILE*outFile)
{
	clock_t start, end;
 	double cpu_time_used;

 	start = clock();

 	C = (double **)malloc(2*x * sizeof(double*));
		for(int i = 0; i < x; i++)
		    C[i] = (double *)malloc(2*z * sizeof(double));

	int i=0,j=0;
	int E = x*z;
	pthread_t elementThread[E];

	for (a = 0; a < x; ++a)
	{
		for (b = 0; b < z; ++b)
		{
			pthread_t x;
			threadargs *args = malloc(sizeof(struct threadargs));
			args->arg1 = a;
			args->arg2 = b;
			pthread_create(&x,NULL,elementCalc,(void *)args);
			pthread_join(x,NULL);
			i++;
		}
	}

	outFile = fopen("matC.txt","w");

	for(i=0;i<x;i++)
	{
	    for(j=0;j<z;j++)
	        fprintf(outFile, "%lf\t",C[i][j]);
	    fprintf(outFile, "\n");
	}

	//printf("MatC:\n");
	//for(i=0;i<x;i++)
	//{
    //    for(j=0;j<z;j++)
    //    printf("%lf\t",C[i][j]);
	//
    //    printf("\n");
    //}

    end = clock();
 	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
 	printf("Time of using thread per element: %lf sec\n\n", cpu_time_used);
 	free(C);
	return E;
}

void *elementCalc(void*argsE)
{
	threadargs *args = argsE;
	C[args->arg1][args->arg2] = 0;
	for (c = 0; c < y_1; ++c)
	{
		C[args->arg1][args->arg2] += A[args->arg1][c] * B[c][args->arg2];
	}

	//free(args);
	free(argsE);

	pthread_exit(0);
}

int threadedMatMultPerRow(char *argv[], FILE*outFile)
{
	clock_t start, end;
 	double cpu_time_used;

 	start = clock();

 	C = (double **)malloc(2*x * sizeof(double*));
		for(int i = 0; i < x; i++)
		    C[i] = (double *)malloc(2*z * sizeof(double));

    int i=0,j=0;
	int E = x;
	pthread_t elementThread[E];
	pthread_attr_t attr;

	for (a = 0; a < x; ++a)
	{
		pthread_t x;
		threadargs *args = malloc(sizeof(struct threadargs));
		args->arg1 = a;
		pthread_create(&x,NULL,rowCalc,(void *)args);
		pthread_join(x,NULL);
		i++;
	}

	outFile = fopen("matC.txt","w");

	for(i=0;i<x;i++)
	{
	    for(j=0;j<z;j++)
	        fprintf(outFile, "%lf\t",C[i][j]);
	    fprintf(outFile, "\n");
	}

	//printf("MatC:\n");
	//for(i=0;i<x;i++)
	//{
    //   for(j=0;j<z;j++)
    //    printf("%lf\t",C[i][j]);
	//
    //    printf("\n");
    //}

    end = clock();
 	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
 	printf("Time of using thread per row: %lf sec\n\n", cpu_time_used);
 	free(C);
 	return E;
}

void *rowCalc(void*argsE)
{
	threadargs *args = argsE;

	for (int b = 0; b < z; ++b)
	{
		C[args->arg1][b] = 0;
		for (c = 0; c < y_1; ++c)
		{
			C[args->arg1][b] += A[args->arg1][c] * B[c][b];
		}
	}

	//free(args);
	free(argsE);

	pthread_exit(0);
}

int fpeek(FILE *stream)
{
    int c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}