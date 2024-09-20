
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sudoku_array[9][9]; //array that has specific number of rows and columns

int array[]={1,2,3,4,5,6,7,8,9};  //array that has numbers 1 to 9

pthread_t t[11];


struct param
{
	int rows;    //initialized the variables in integer 
	int columns;
};

int final_check[11];

void *column_worker(void *arg)
{
	struct param *p=(struct param*)arg;
	int test_array[9];
	int rw,cl,x,y,b,valid_determiner=1; //creating array out of the numbers
	
	for(cl=p->columns;cl<9;cl++)  //for both rows and columns 
	{
		for(rw=0;rw<9;rw++)
		{
			test_array[rw]=sudoku_array[rw][cl];
		}

	    for(x=0;x<9;x++)
	    {
			for(y=x+1;y<9;y++)
			{
				if(test_array[x]>test_array[y])
				{
					b=test_array[x];
                    test_array[x]=test_array[y];
                    test_array[y]=b;
            	}
        	}
    	}

    	printf("column sorted array %d: ",cl);
	    for(x=0;x<9;x++)
	        printf("%d, ",test_array[x]);
	    printf("\n");

    	for(x=0;x<9;x++)
    		if(test_array[x]!=array[x])
    			valid_determiner=0;	
	}

	if(valid_determiner==0)
		pthread_exit((int*)0);
	pthread_exit((int*)1); 
} 

void *row_worker(void *arg)
{
	struct param *p=(struct param*)arg;
	int test_array[9];
	int rw,cl,x,y,b,valid_determiner=1;

	for(rw=p->rows;rw<9;rw++)
	{
		for(cl=0;cl<9;cl++)
		{
			test_array[cl]=sudoku_array[rw][cl];
		}

	    for(x=0;x<9;x++)
	    {
			for(y=x+1;y<9;y++)
			{
				if(test_array[x]>test_array[y])
				{
					b=test_array[x];
                    test_array[x]=test_array[y];
                    test_array[y]=b;
            	}
        	}
    	}

    	printf("row sorted array %d: ",rw);
	    for(x=0;x<9;x++)
	        printf("%d, ",test_array[x]);
	    printf("\n");

    	for(x=0;x<9;x++)
    		if(test_array[x]!=array[x])
    			valid_determiner=0;	
	}

	if(valid_determiner==0)
		pthread_exit((int*)0);
	pthread_exit((int*)1); 
} 

void *subgrid_worker(void *arg)
{
	struct param *p=(struct param*)arg;
	int test_array[9];
	int index=0;
	int rw,cl,rows_stop=p->rows+3,column_stop=p->columns+3,x,y,b;

	for(rw=p->rows;rw<rows_stop;rw++)
	{
		for(cl=p->columns;cl<column_stop;cl++){
			test_array[index]=sudoku_array[rw][cl];
			index++;
		}
	}

	    for(x=0;x<9;++x)
	    {
			for(y=x+1;y<9;++y)
			{
				if(test_array[x]>test_array[y])
				{
					b=test_array[x];
                    test_array[x]=test_array[y];
                    test_array[y]=b;
            }
        }
    }

    printf("subgrid sorted array: ");
    for(x=0;x<9;x++)
        printf("%d, ",test_array[x]);
    printf("\n");

    for(x=0;x<9;x++)
		if(test_array[x]!=array[x])
			pthread_exit((int*)0);
	pthread_exit((int*)1);
}

int main(int argc, char *argv[])
{

	if (argc == 1)
	 {
		fprintf(stderr,"No input argument.");		
		return -1;
	}

	const char *file_name;
	if(argc==2)
		file_name=argv[1];

	FILE *sudoku_numbers;
	sudoku_numbers=fopen(file_name,"r");

	
	int e,f,g;

	if(sudoku_numbers==NULL)
	{
		printf(" there has been an error reading the File\n");
		exit(0);
	}

	for(e=0;e<9;e++)
	{
	
		for(f=0;f<9;f++)
		{
			fscanf(sudoku_numbers,"%d",&sudoku_array[e][f]);
		}
	}

			for(e=0;e<9;e++)
			{
		for(f=0;f<9;f++)
		{
			printf("%d, ",sudoku_array[e][f]);
		}
	}
	printf("\n");

	struct param p[11];
	p[0].rows=0;p[0].columns=0;
	p[1].rows=0;p[1].columns=0;
	p[2].rows=0;p[2].columns=0;
	p[3].rows=0;p[3].columns=3;
	p[4].rows=0;p[4].columns=6;
	p[5].rows=3;p[5].columns=0;
	p[6].rows=3;p[6].columns=3;
	p[7].rows=3;p[7].columns=6;
	p[8].rows=6;p[8].columns=0;
	p[9].rows=6;p[9].columns=3;
	p[10].rows=6;p[10].columns=6;

	// threads are being created
	pthread_create(&t[0],NULL,column_worker,&p[0]);
	pthread_create(&t[1],NULL,row_worker,&p[1]);
	pthread_create(&t[2],NULL,subgrid_worker,&p[2]);
	pthread_create(&t[3],NULL,subgrid_worker,&p[3]);
	pthread_create(&t[4],NULL,subgrid_worker,&p[4]);
	pthread_create(&t[5],NULL,subgrid_worker,&p[5]);
	pthread_create(&t[6],NULL,subgrid_worker,&p[6]);
	pthread_create(&t[7],NULL,subgrid_worker,&p[7]);
	pthread_create(&t[8],NULL,subgrid_worker,&p[8]);
	pthread_create(&t[9],NULL,subgrid_worker,&p[9]);
	pthread_create(&t[10],NULL,subgrid_worker,&p[10]);
	
	//waiting for all the threads to finish executing
	pthread_join(t[0],&final_check[0]);
	pthread_join(t[1],&final_check[1]);
	pthread_join(t[2],&final_check[2]);
	pthread_join(t[3],&final_check[3]);
	pthread_join(t[4],&final_check[4]);
	pthread_join(t[5],&final_check[5]);
	pthread_join(t[6],&final_check[6]);
	pthread_join(t[7],&final_check[7]);
	pthread_join(t[8],&final_check[8]);
	pthread_join(t[9],&final_check[9]);
	pthread_join(t[10],&final_check[10]);
	
	//displaying the values of the array 
	int final_result=1;
	printf("Here are the returns from all the threads:\n");
	for(g=0;g<11;g++)
	{
	
		printf("%d, ",final_check[g]);
		if(final_check[g]==0)
			final_result=0;
	}
	if(final_result==1)
		printf("\nThe Sudoku solution that has been printed is valid");
	else
		printf("\nThe Sudoku solution that has been printed is not valid");

	return 0;
}


