#include "jobs.h"
#include <stdio.h>

int get_active_job(int job_cnt,int job_status[])
{
    int current_index = -1;
    for(int i = (job_cnt - 1);i >= 0;i--)
    {
        if((job_status[i] != 2) && (job_status[i] !=3))
        {
            current_index = i;
        } else 
        {
            continue;
        }
    }
    return current_index;
}


void printJobs(int job_cnt,int job_status[],int current_job,char* job_commands[])
{
   int i = 0;   
   for(i = 0; i < job_cnt; i +=1)
   {
       if(job_status[i] == 3)
       {
           continue;
       }else
       {
           //CURRENT JOB(+) NOT CURRENT JOB(-)
           printf("[%d]", (i + 1));
           if(i == current_job)
           {
                printf(" + ");
           } else
           {
               printf(" - ");
           }

           //JOB STATE
           if(job_status[i] == 0)
           {
               printf("Stopped\t");
           } else if(job_status[i] == 1)
           {
              printf("Running\t");
           } else if(job_status[i] == 2)
           {
               job_status[i] = 3;
               printf("Done\t");
           }
           printf("%s\n",job_commands[i]);
       }
    }
}

