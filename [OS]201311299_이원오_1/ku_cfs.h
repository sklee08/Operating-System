#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <signal.h>




char abc[26][2] = {"A", "B", "C", "D","E", "F", "G", "H", "I", "J","K", "L","M","N","O","P","Q","R","S","T","U", "W","X","Y", "Z"};

float niceVal[5] = {0.64, 0.8, 1, 1.25, 1.5625};


typedef struct node{
	int pidNum;
	float vruntime;
	char* ch;
	int nice;
	struct node* next;
	struct node* prev;
}Node;


typedef struct list{
	Node* head;
}List;

List* list = NULL;

int *pidA;

int pro_num = 0;

struct sigaction sigac;
struct itimerval timer;

List* init();

void timer_handler(int signum);

int getSize(List* list);

void setVruntime(float vruntime, int pid, List* list);

void sort(int n, List* list);

void insertLastNode(int pid, float vruntime, List* list, char* ch, int nice);

int deleteNode(int pid, List *list);

void printList(List *list);

void organize(List* list);

void setTimer();

