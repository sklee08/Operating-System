#include "ku_cfs.h"


List* init() // initailize the list

{
        List* newList = (List*)malloc(sizeof(List));
        Node* tmp = (Node*)malloc(sizeof(Node));

        tmp->prev = NULL;
        tmp->next = NULL;
        tmp->vruntime = 0.0;
        tmp->pidNum = 0;
        tmp->ch = "\0";
        tmp->nice = 0;

        newList->head = tmp;
        return newList;
}

void insertLastNode(int pid, float vruntime, List* list, char *ch, int nice) // inserting node to list
{
        Node* tmpNode = (Node*)malloc(sizeof(Node));
        Node* curr = list->head;


        if(tmpNode == NULL)
        {
                printf("Fail to Malloc!\n");
                exit(-1);
        }
        tmpNode->pidNum = pid;
        tmpNode->vruntime = vruntime;
        tmpNode->next = NULL;
        tmpNode->prev = NULL;
        tmpNode->ch = "\0";
        tmpNode->nice = nice;

        while(curr->next!= NULL)
        {
                curr = curr->next;
        }

	if(strcmp(curr->ch, "\0") == 0)
        {
                curr->pidNum = pid;
                curr->vruntime = vruntime;
                curr->next = NULL;
                curr->prev = NULL;
                curr->ch = ch;
                curr->nice = nice;
        }
        else
        {
                tmpNode->vruntime = vruntime;
                tmpNode->pidNum = pid;
                tmpNode->next = NULL;
                tmpNode->prev = NULL;
                tmpNode->ch = ch;
                tmpNode->nice = nice;

                tmpNode->prev = curr;
                curr->next = tmpNode;
        }


}

void printList(List *list)  // print all the char value of list
{
        Node *now;
        now = list->head;
        printf("<");
        while(now!= NULL)
        {
		printf("%s ", now->ch);
                now = now->next;
        }
        printf(">\n");
}
void sort(int n, List* list)  //sorting for list
{
        int i,j;
        Node *tmp, *curr, *nextone;
        for(i=0; i<n; i++)
        {
                curr = list->head;
                for(j=0; j<n-1-i; j++)
                {
                        if(curr->vruntime > curr->next->vruntime)
                        {
                                nextone = curr->next;
                                curr->next = nextone->next;
                                nextone->next = curr;
                                if(curr == list->head)
                                {
                                        list->head = nextone;
                                        curr = nextone;
                                }
                                else
                                {
                                        curr = nextone;
                                        tmp->next = nextone;
                                }
                        }
                        tmp = curr;
                        curr = curr->next;
                }
        }
}
int getSize(List* list) // get size of list
{
        int n = 0;
        Node *tmp = list->head;
        while(tmp!= NULL)
        {
                n+=1;
                tmp = tmp->next;
        }
        return n;
}

void execKu(char* ch)    // exec ku_app
{
        char *arg[3];
        arg[0] = "./ku_app";
        arg[1] = ch;
        arg[2] = NULL;
        int rc = execvp("./ku_app", arg);
        if(rc==-1) printf("Fail to exec!\n");
	fflush(stdout);
}

void setVruntime(float vruntime, int pid, List* list)// setting vruntime with each process'nice value
{
        Node* tmp = list->head;
        while(tmp != NULL)
        {
                if(tmp->pidNum == pid)
                {
                        float tmpVrt = tmp->vruntime;
                        float newVrt = vruntime;
			int current_nice = tmp->nice;
                        float result_vrt = tmpVrt + newVrt * niceVal[current_nice + 2];
                        tmp->vruntime = result_vrt;
                }
                tmp = tmp->next;
        }
}

void setTimer()   // setting the timer to 1 second
{
	memset(&sigac,0,sizeof(sigac));
        sigac.sa_handler = &timer_handler;
 	timer.it_value.tv_sec = 1;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = 1;
        timer.it_interval.tv_usec = 0;

        int setNum = setitimer(ITIMER_REAL, &timer, NULL);
        if(setNum) fprintf(stderr, "Fail to set timer");
		
}

void timer_handler(int signum) // sending signal to parent process and child process
{
	printf("\ttime handler child process pid: %d\n", list->head->pidNum);
        int current_childpid = list->head->pidNum;
        kill(current_childpid, SIGSTOP);  
        kill(getppid(), SIGCONT);
}

int main(int argc, char *argv[]) // main
{
	 if(argc != 7)
        {
                printf("Not the appropriate arguments!\n");
                exit(-1);
        }
        int ts = atoi(argv[6]); // time slice
        int n[5];
        int i,j = 0;
        int pid;
        int cnt = 0;
        int parentPid;
 	clock_t t1, t2, time_double;
        float time_float;    


        for(i=0; i<5; i++)
        {
                n[i] = atoi(argv[i+1]);
                pro_num += n[i];
        }

        pidA = (int *)malloc(sizeof(int) * (pro_num + 1));

	list = init(); // initialize the list

        int chVal = 0;
        for(i=0; i<5; i++)
        {
                int p = n[i];
                if(p < 0)
                {
                        printf("Wrong parameter!\n");
                        exit(0);
                }
                else{
                        for(j=0; j<p; j++)
                        {
				pid = fork();
                                if(pid == 0)
                                {
					fflush(stdout);
					execKu(abc[chVal]);
                                        break;
                                }
                                else
                                {
					printf("\t\t\tchild pid: %d // child char : %s\n\n", pid, abc[chVal]);
                                       	parentPid = getpid();
                                        if(cnt < pro_num)
                                        {
                                                pidA[cnt] = pid;
                                                cnt++;
                                        }
                                        pidA[pro_num] = parentPid;
                                        insertLastNode(pid,0.0,list,abc[chVal],i-2);
                                }
				chVal++;
                        }

                }
        }
	sleep(3); // synchronize
                
                setTimer(); // setting the timer
                sigaction(SIGALRM, &sigac, NULL); 
                
                 int RunPid = list->head->pidNum;
                 cnt = 0;
		printf("First List status\n");
                while(cnt != ts)
                {
			kill(list->head->pidNum, SIGCONT);
			t1 = clock();
			printList(list);
			pause();
		
                        if(cnt < ts)
                        {
				t2 = clock(); 
				time_double = t2-t1;
				double check_time = ((double)time_double) / (double)1000;
				time_float = (float)check_time;
        			setVruntime(time_float, list->head->pidNum, list);       
                        
				int n = getSize(list);
				sort(n, list);
                                RunPid = list->head->pidNum;
                                kill(RunPid, SIGCONT);
				cnt++;
                        }
                        else if(cnt == ts) return 0; // unreachable code
                }
  
        return 0;
}

