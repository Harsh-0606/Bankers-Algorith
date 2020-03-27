#ifndef BANKER_H // to prevent multiple includes in the main file
#define BANKER_H

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3
#include <stdbool.h>


/* these may be any values >= 0 */
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3
/* the available amount of each resource */
extern int available[NUMBER_OF_RESOURCES];
/*the maximum demand of each customer */
extern int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */
extern int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
extern int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* mutex lock */
//pthread_mutex_t lock;
pthread_mutex_t lock;



bool isSafe()
{


    bool finish[NUMBER_OF_CUSTOMERS];
    int work[NUMBER_OF_RESOURCES];
    //int finish_seq[NUMBER_OF_CUSTOMERS]; 
    bool process_can = true;
    int count = 0;
    int customer_count = 1;
    bool flag = true;

    for(int i=0; i<NUMBER_OF_RESOURCES; i++)
        work[i] = available[i];

    for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
        finish[i] = false;

    for(int k=0; k<NUMBER_OF_RESOURCES; k++)
        work[k] += allocation[customer_count][k];

    while(count<NUMBER_OF_CUSTOMERS)
    {

        if(finish[customer_count] == false)
        {
            /* check if the process can be granted the resources */
            for(int j=0; j<NUMBER_OF_RESOURCES; j++)
                if(need[customer_count][j]>work[j])  
                    process_can = false;

            /* process can finish */
            if(process_can)
            {
                for(int k=0; k<NUMBER_OF_RESOURCES; k++)
                    work[k] += allocation[customer_count][k];
                finish[customer_count] = true;
                count++;

                customer_count = (customer_count + 1) % NUMBER_OF_CUSTOMERS; 
                break;
            }
printf("STUCK HERE\n");

        }

        /* for all false states -if they cannot run- then we are in an unsafe state.*/

        for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
            if(finish[i]==false)
            {
                for(int j=0; j<NUMBER_OF_RESOURCES; j++)
                    if(work[j]>need[customer_count][j])
                        flag = false;
                break;
            }

        if(flag == true)
            return false;

    }

    return true;
}


bool release_resources(int release[],int customer_num)
{
    /*add resouces*/
    for(int i=0; i<  NUMBER_OF_RESOURCES; i++)
    {
        available[i] += release[i];
    }
    return true;
}

bool request_resources(int request[], int customer_num)
{

//    sleep(1);
    printf("\nCustomer %d is Requesting Resources:\n",customer_num);
    for(int i=0; i<NUMBER_OF_RESOURCES; i++)
    {
        printf("%d ", request[i]);
    }

    printf("\nAvailable Resources : \n");

    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        printf("%d ",available[i]);
    }

    printf("\nThe need : \n");
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++)
    {
        printf("%d ",need[customer_num][i]);
    }

    printf("\n");
    for(int i=0; i<NUMBER_OF_RESOURCES; i++)
    {
        if(request[i] <= need[customer_num][i]) 
        {
            if(request[i]>available[i])
            {
                printf("NOT SAFE with this request (request > available) \n");
                return false;
            }
            else  //taking resources//
            {
 
                if(isSafe())
                {
                    printf("The System is Safe \nResources Granted \n");

		       for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		        {
		            available[i] -= request[i];
		            allocation[customer_num][i] += request[i];
		            need[customer_num][i] -= request[i];
		        }	
		            return true;
		}
                else
                {
                    printf("NOT SAFE ! CAN'T GRANT RESOURCES \n");
                    return false;
                }
            }
        }

        else if(request[i] > need[customer_num][i])
        {
            printf("The request is more than the need. Abort! \n");
            return false;
        }
    }

}



void release_resources_highlevel(int customer_num){
        pthread_mutex_lock(&lock);


        release_resources(maximum[customer_num],customer_num);
        pthread_mutex_unlock(&lock);
	printf( "Thread %d finished execution \n" , customer_num);
}

bool request_resources_highlevel(int request[],int customer_num){
    //CRITICAL SECTION //
    bool released = false;
    pthread_mutex_lock(&lock);
    released=request_resources(request,customer_num);
    pthread_mutex_unlock(&lock);

    return released;
}

#endif 
