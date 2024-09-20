#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* these may be any values >= 0 */
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allow[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// initialize the mutex
pthread_mutex_t mutex;

// initialize the arrays for the banker
int total[NUMBER_OF_RESOURCES]={0};
int banker_available[NUMBER_OF_RESOURCES];
int temp_max[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int temp_allow[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int temp_need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int finished[NUMBER_OF_CUSTOMERS]={0};

char string[NUMBER_OF_RESOURCES*2]={0};
int safeSequence[NUMBER_OF_CUSTOMERS]={0};

// declare the functions
int bankerAlgorithm(int customer_number,int request[]);
void printState();

// function to release the resources
int release_resources(int customer_number){
    printf("Customer %d releases all the resources\n",customer_number+1);
    for(int j=0;j<NUMBER_OF_RESOURCES;j++) {
        available[j]=available[j]+allow[customer_number][j];
        allow[customer_number][j]=0;
    }
    return 0;
}

// function to request resources
int request_resources(int customer_number, int request[]){
    int return_value=-1;
	
    // CRITICAL SECTION
	// make printing and accessing global variable in critical section to prevent race condition
    pthread_mutex_lock(&mutex);
    printf("\nCustomer %d requests for ",customer_number+1);
    
	for(int i=0;i<NUMBER_OF_RESOURCES;i++) {
        printf("%d ",request[i]);
    }

    printf("\n");
	
	// check if there are enough resources available for the request
    for(int i=0;i<NUMBER_OF_RESOURCES;i++) {   
        // if there is not, return -1
        if(request[i]>available[i]){
            printf("Customer %d is waiting for the resources...\n",customer_number+1);
			// before the thread returns, unlock the mutex
            pthread_mutex_unlock(&mutex);
            return -1;
        }
    }
	
	// execute banker's algorithm
    return_value = bankerAlgorithm(customer_number,request);
    if(return_value==0) { 
        int mustbe_zero=1;
        printf("A safe sequence has been found as follows: ");
        for(int i=0;i<NUMBER_OF_CUSTOMERS;i++) {
            printf("Customer %d ",safeSequence[i]+1 );
        }
        printf("\nCustomer %d's request has been granted\n",customer_number+1);
       
	   // allocate resources to the thread
        for(int j=0;j<NUMBER_OF_RESOURCES;j++) { 
            allow[customer_number][j]=allow[customer_number][j]+request[j];
            available[j]=available[j]-request[j];
            need[customer_number][j]=need[customer_number][j]-request[j];
			// to check if need is zero
            if(need[customer_number][j] != 0){
                mustbe_zero = 0;
            }
        }

        if(mustbe_zero){
	// if need is zero, mark the thread finished
            finished[customer_number]=1; 
	// release resources when a thread finishes
            release_resources(customer_number);
        }

        printState();
    }
	
    else {
        printf("Unable to find a safe sequence.\n");
    
    }
	// unlock the mutex
    pthread_mutex_unlock(&mutex); 
    return return_value;
}

void *thread_func(void* Total_Customer_Number) {
    int *c=(int*)Total_Customer_Number;
    int customer_number= *c;
    int Request_Total=0;
	// the whilie loop stops when the thread has finished and its need becomes zero
    while(!finished[customer_number]) {   
        Request_Total=0;
        int request[NUMBER_OF_RESOURCES]={0};
        for(int i=0;i<NUMBER_OF_RESOURCES;i++){
			// generate a request below its need randomly
            request[i]=rand()%(need[customer_number][i]+1); 
            Request_Total=Request_Total+request[i];
        }
		// to make sure it doesn't reqeust for 0 resources
        if(Request_Total!=0)  
		// only when the request has been granted succesfully will the loop terminates, otherwise it will keep making the same request.
            while(request_resources(customer_number,request) == -1);

    }
    return 0;
}

// bankers algorithm
int bankerAlgorithm(int customer_number,int request[]){
    int finished[NUMBER_OF_CUSTOMERS]={0};
	// copy the matrices
    for(int i=0;i<NUMBER_OF_RESOURCES;i++) {
        banker_available[i]=available[i];
        for(int j=0;j<NUMBER_OF_CUSTOMERS;j++) {
            temp_allow[j][i]=allow[j][i];
            temp_max[j][i]=maximum[j][i];
            temp_need[j][i]=need[j][i];
        }
    }
    for(int i=0;i<NUMBER_OF_RESOURCES;i++) { 
		// pretend to give the resource to the thread
        banker_available[i]=banker_available[i]-request[i];
        temp_allow[customer_number][i]=temp_allow[customer_number][i]+request[i];
        temp_need[customer_number][i]=temp_need[customer_number][i]-request[i];
    }
//safety Algorithm
    int count=0;
    while(1){
        int I=-1;
        for(int i=0;i<NUMBER_OF_CUSTOMERS;i++){ 
			// to find a thread that its need is less than or equal to available.
            int nLessThanA=1;
            for(int j=0; j<NUMBER_OF_RESOURCES;j++){
                if(temp_need[i][j]>banker_available[j] || finished[i]==1){
                    nLessThanA=0;
                    break;
                }
            }
            if(nLessThanA){ 
				// if the thread is found, record its thread number
                I=i;
                break;
            }
            
        }
        if(I!=-1){
			// record the sequence
            safeSequence[count]=I; 
            count++;
			// mark the thread "finish"
            finished[I]=1; 
			// pretend to give the reaource to thread
            for(int k=0;k<NUMBER_OF_RESOURCES;k++){  
                banker_available[k]=banker_available[k]+temp_allow[I][k];
            }
        }
        else{ 
			// if can not find any thread that its need is less than or equal to available.
            for(int i=0;i<NUMBER_OF_CUSTOMERS;i++){
                // if there is any thread hasn't been found, that means it can't find a safe sequence
				if(finished[i]==0){ 
                    return -1;
                }
            }
			// all the threads have been found
            return 0;  

        }
    }
    
//safety Algorithm
}

// function to print the state of the process
void printState(){
	// print the resources that are currently allocated
    printf("\nCurrently allocated resources:\n");
    for(int i=0; i<NUMBER_OF_CUSTOMERS;i++){
        for(int j=0;j<NUMBER_OF_RESOURCES;j++){
            printf("%d ",allow[i][j]);
        }
        printf("\n");
    }
    // print the resources needed by the customers
    printf("\nNeeded resources:\n");
    for(int i=0; i<NUMBER_OF_CUSTOMERS;i++){
        for(int j=0;j<NUMBER_OF_RESOURCES;j++){
            printf("%d ",need[i][j]);
        }
        printf("\n");
    }
    // print the resources that are available to the customers
    printf("\nAvailable resources:\n");
    for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
        printf("%d ",available[i]);
    }
    printf("\n");
}

int main(int argc, const char * argv[]) {
  //initialize the matrices
    for(int i=0 ;i<argc-1;i++){
        available[i]=atoi(argv[i+1]);
        // in the begining available resources equal to total resources
		total[i]=available[i];      
    }
    for(int i=0;i< NUMBER_OF_RESOURCES;i++){
        for(int j=0;j<NUMBER_OF_CUSTOMERS;j++){
			 //maximum should less than total
            maximum[j][i]=rand()%(total[i]+1);
			// need=maximum-allow  (allow=0)    
            need[j][i]=maximum[j][i];    
        }
    }
    //initialize the matrices
    //print the total resources in the system
    printf("\nTotal system resources are:\n");
    for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
        printf("%d ",total[i]);
    }
    // print the maximum resources in the system
    printf("\n\nMaximum resources:\n");
    for(int i=0; i<NUMBER_OF_CUSTOMERS;i++){
        for(int j=0;j<NUMBER_OF_RESOURCES;j++){
            printf("%d ",maximum[i][j]);
        }
        printf("\n");
    }
    // call the printState function to print the state of the process
    printState();
   //initialize the mutex
   pthread_mutex_init(&mutex, NULL);    
   pthread_t p1,p2,p3,p4,p5;
   int a=0,b=1,c=2,d=3,e=4;
    //create 5 threads for the number of customers
   pthread_create(&p1,NULL,thread_func,&a);
   pthread_create(&p2,NULL,thread_func,&b);
   pthread_create(&p3,NULL,thread_func,&c);
   pthread_create(&p4,NULL,thread_func,&d);
   pthread_create(&p5,NULL,thread_func,&e);    
   char *returnV;
	// wait for all the 5 threads to terminate
   pthread_join(p1,(void**)&returnV);
   pthread_join(p2,(void**)&returnV);
   pthread_join(p3,(void**)&returnV);
   pthread_join(p4,(void**)&returnV);
   pthread_join(p5,(void**)&returnV); 
    return 0;
}
