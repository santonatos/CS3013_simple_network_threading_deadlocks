#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>

#include "sol2_sema.h"

#define grid_width 40
#define grid_length 40


#define max_num_noisemakers 10000

#define num_nodes 100
#define dwell_duration 1
#define dwell_probability 0.3
#define transmission_time 1
#define talk_window_time 1
#define talk_probability 1


#define num_noisemakers 20
#define dwell_noisemakers 1
#define dwell_probability_noisemakers 0.4
#define noisemaker_active_time 5 


//author: spiros antonatos
	
pNode array_nodes[max_num_nodes];
pNoisemaker array_noisemakers[max_num_noisemakers];
sem_t global_message_sem;
int global_message_counter;



void create_file_node(struct regular_node* pnode){
	char* firstPart = (char*)malloc(20*sizeof(char));
	char* unique = (char*)malloc(30*sizeof(char));
	char* filename = (char*)malloc(40*sizeof(char));
	char* type = (char*)malloc(10*sizeof(char));
	firstPart = "regular_node\0";
	int id = pnode->id;
	//unique = itoa(pnode->id);
	type = ".txt\0";

	snprintf(filename,sizeof("regular_node10000000.txt"),"%s%d%s",firstPart,id,type);
	pnode->file = filename;

	return;

	

}




void create_file_noisemaker(struct noisemaker_node* pnoisemaker){
	char* firstPart = (char*)malloc(20*sizeof(char));
	char* unique = (char*)malloc(30*sizeof(char));
	char* filename = (char*)malloc(40*sizeof(char));
	char* type = (char*)malloc(10*sizeof(char));
	firstPart = "noisemaker\0";
	int id = pnoisemaker->id;
	//unique = itoa(pnode->id);
	type = ".txt\0";

	snprintf(filename,sizeof("noisemaker10000000.txt"),"%s%d%s",firstPart,id,type);
	pnoisemaker->file = filename;

	return;

}

int main(){
	//fprintf(stdout,"before lock\n");
	srand(1);
	struct regular_node ** n1 = (struct regular_node**)malloc((num_nodes+5)*sizeof(struct regular_node*));//,n2,n3,n4,n5;
	struct noisemaker_node ** nm1 = (struct noisemaker_node**)malloc((num_nodes+5)*sizeof(struct noisemaker_node*));

	//noisemaker nm1,nm2;
	int current_number_nodes = 0;
	int current_number_noisemakers = 0;
	int i,j,k,l,x,y;
        //global_message_counter = 0;
	//fprintf(stdout,"before lock\n");
	pthread_mutex_t starting_lock1;
	pthread_mutex_t starting_lock2;
	pthread_mutex_t starting_lock3;
	sem_init(&global_message_sem,0,1);
	for(i = 0;i < num_nodes;i++){
		x = rand()%grid_width + 1;
		y = rand()%grid_length + 1;
		*n1 = (struct regular_node*)malloc(sizeof(struct regular_node)); 
		place_node(*n1,&current_number_nodes,x,y);
		(*n1)++;
	}

	for(j = 0;j < num_noisemakers;j++){
		x = rand()%grid_width + 1;
		y = rand()%grid_length + 1;
		*nm1 = (struct noisemaker_node*)malloc(sizeof(struct noisemaker_node)); 
		place_noisemaker(*nm1,&current_number_noisemakers,x,y);
		(*nm1)++;
	}

	//fill the neighbor array for each node 
	determine_neighbors();
	determine_noisemaker_neighbors();
	for(j = 0;j < num_noisemakers;j++){
		initialize_noisemaker_thread(array_noisemakers[j]);
	}

	for(i = 0;i < num_nodes;i++){
		initialize_node_thread(array_nodes[i]);
	}



	fprintf(stdout,"STARTING ALL THREADS\n");


	while(1);
}


void place_noisemaker(pNoisemaker p_noisemaker,int *i,int pos_x,int pos_y){
	if(*i > max_num_noisemakers){
		fprintf(stdout,"cannot add more noisemakers\n");
		return;
	}
	array_noisemakers[*i] = p_noisemaker;
	*i= *i + 1;
	p_noisemaker->id = *i;
	p_noisemaker->x = pos_x;
	p_noisemaker->y = pos_y;
	p_noisemaker->current_frequency = 1;
	p_noisemaker->current_neighboring_nodes = 0;
	create_file_noisemaker(p_noisemaker);

	pthread_mutex_init(&(p_noisemaker->starting_lock),NULL);
	//fprintf(stdout,"noisemaker placed at %d %d\n",p_noisemaker->x,p_noisemaker->y);
}

void initialize_noisemaker_thread(pNoisemaker p_noisemaker){
	//while(initialization_signal);
	if(pthread_create(&(p_noisemaker->thread),NULL,&noisemaker_routine,p_noisemaker)){
		fprintf(stdout,"error creating thead\n");
	}
	//fprintf(stdout,"initialized node and its thread\n");
}


void place_node(pNode p_node,int * i,int pos_x,int pos_y){
	if(*i >= max_num_nodes){
		fprintf(stdout,"cannot add more nodes\n");
		return;
	}
	array_nodes[*i] = p_node;
	*i= *i + 1;
	//fill node struct partially
	p_node->id = *i;
	p_node->blocked = -10;
	p_node->x = pos_x;
	p_node->y = pos_y;
	p_node->current_neighboring_nodes = 0;
	p_node->file = (char*)malloc(40*sizeof(char));
	create_file_node(p_node);
	p_node->message = (struct message*)malloc(sizeof(struct message));
	struct message* newm = (struct message*)malloc(sizeof(struct message));
	//printf("here3\n");
	newm->text = (char*)malloc(50*sizeof(char));//"message";
	newm->text = "message1\0";
	global_message_counter++;
	//printf("here3\n");
	newm->mid = global_message_counter;//rand() % 10;
	//printf("here3\n");
	newm->is_seen = 0;
	p_node->message = newm;

	struct message* newm2 = (struct message*)malloc(sizeof(struct message));
	newm2->text = (char*)malloc(50*sizeof(char));//"message";
	newm2->text = "message2\0";
	global_message_counter++;
	newm2->mid = global_message_counter;//rand() % 10;
	newm2->is_seen = 0;
	add_message(p_node,newm2);

	struct message* newm3 = (struct message*)malloc(sizeof(struct message));
	newm3->text = (char*)malloc(50*sizeof(char));//"message";
	newm3->text = "message3\0";
	global_message_counter++;
	newm3->mid = global_message_counter;//rand() % 10;
	newm3->is_seen = 0;
	add_message(p_node,newm3);

	

	p_node->current_number_messages = 3;
	p_node->current_frequency = 1;
	p_node->total_seen_number_messages = 3;
	p_node->seen_messages[0] = p_node->message->mid;
	p_node->seen_messages[1] = p_node->message->next->mid;
	p_node->seen_messages[2] = p_node->message->next->next->mid;
	sem_init(&(p_node->sending_sem),0,1);
	sem_init(&(p_node->sem1),0,1);
	sem_init(&(p_node->message_sem),0,1);

	//fprintf(stdout,"node placed %d %d %d\n",p_node->message->mid,p_node->message->next->mid,p_node->message->next->next->mid);
	return;
	
}

void initialize_node_thread(pNode p_node){
	if(pthread_create(&(p_node->thread),NULL,&node_routine,p_node)){
		fprintf(stdout,"error creating thead\n");
	}
}

void determine_neighbors(){
	int i,j;
	pNode pNode1,pNode2;	
	for(i = 0;i < num_nodes;i++){
		for(j = 0;j < num_nodes;j ++){
			pNode1 = array_nodes[i];
			pNode2 = array_nodes[j];
			if((pNode2->x <= (pNode1->x + 5)) && (pNode2->x >= (pNode1->x - 5)) && (pNode2->y <= (pNode1->y + 5)) && (pNode2->y >= (pNode1->y -5)) /*&& pNode2 != pNode1*/){
				pNode1->neighboring_nodes[pNode1->current_neighboring_nodes] = pNode2;
				pNode1->current_neighboring_nodes = pNode1->current_neighboring_nodes + 1;
				//printf("nhbs nodes %d %d,  %d %d\n",pNode1->x,pNode1->y,pNode2->x,pNode2->y);
				printf("neihgboring nodes %d, %d\n",pNode1->id,pNode2->id);
				//pNode1->neighboring_nodes
			}
		}		
	}



}

void determine_noisemaker_neighbors(){
	int i,j;
	pNode pNode1,pNode2;
	pNoisemaker pnm1;	
	for(i = 0;i < num_noisemakers;i++){
		for(j = 0;j < num_nodes;j ++){
			pnm1 = array_noisemakers[i];
			pNode2 = array_nodes[j];
			//fprintf(stdout,"noise %d %d,node %d %d,
			if((pNode2->x <= (pnm1->x + 5)) && (pNode2->x >= (pnm1->x - 5)) && (pNode2->y <= (pnm1->y + 5)) && (pNode2->y >= (pnm1->y -5))){
				pnm1->neighboring_regular_nodes[pnm1->current_neighboring_nodes] = pNode2;
				pnm1->current_neighboring_nodes = pnm1->current_neighboring_nodes + 1;
				//printf("nhbs node-noisemaker %d %d,  %d %d\n",pnm1->x,pnm1->y,pNode2->x,pNode2->y);
				printf("neihboring node-noisemaker %d, %d\n",pNode2->id,pnm1->id);
			}
		}		
	}



}

void* node_routine(void* p_node){
	//fprintf(stdout,"in routine %d %d\n",((pNode)p_node)->x,((pNode)p_node)->y);
	//fprintf(stdout,"in routine %d %d\n",array_nodes[0]->x,array_nodes[0]->y);
	((pNode)p_node)->current_frequency = 1;//rand() % 3;
	struct timeval start,end,start2,end2;
	while(1){
		//int dwell_time = 0;
		//fprintf(stdout,"in routine %d \n",((pNode)p_node)->id);
		int not_terminate = 1;
		gettimeofday(&start,NULL);
		while(not_terminate){
			
			gettimeofday(&end,NULL);
			//fprintf(stdout,"before dwell time\n");
			if(difftime(end.tv_sec,start.tv_sec) >= dwell_duration){
				not_terminate = 0;
			}
		}
		int not_terminate2 = 1;
		gettimeofday(&start2,NULL);
		while(not_terminate2){
			gettimeofday(&end2,NULL);
			//fprintf(stdout,"before dwell time\n");
			if(difftime(end2.tv_sec,start2.tv_sec) >= talk_window_time){
				not_terminate2 = 0;
			}

		}
		if(send_message_or_not()){
			try_to_send(p_node);
		}

		
	}

	return NULL;

}

void* noisemaker_routine(void* p_noisemaker){
	//pthread_mutex_lock(&(((pNode)p_noisemaker)->starting_lock));
	struct timeval start,end,start2,end2;
	int interfere = 1;
	int c = 1;
	int not_terminate,not_terminate1;
	while(1){
		//int dwell_time = 0;
		not_terminate = 1;
		gettimeofday(&start,NULL);
		c = 1;
		while(not_terminate){
			
			gettimeofday(&end,NULL);
			//fprintf(stdout,"in noise routine before check\n");
			if(difftime(end.tv_sec,start.tv_sec) >= dwell_noisemakers){
				not_terminate = 0;
			}
		}
		change_noisemaker_frequency(p_noisemaker);//using dwell_noisemaker_probability



		interfere = interfere_or_not();
		not_terminate1 = 1;
		if(interfere){
			block_channel(p_noisemaker);
		}
		int period = noisemaker_active_time;
		gettimeofday(&start2,NULL);
		while(not_terminate1){
		
			gettimeofday(&end2,NULL);
				//fprintf(stdout,"in noise routine before check\n");
			if(difftime(end2.tv_sec,start2.tv_sec) >= period){
				not_terminate1 = 0;
			}
		}
		if(interfere){
			unblock_channel(p_noisemaker);
		}

		//fprintf(stdout,"interfere = %d\n",interfere)		

		
	}

	return NULL;

}

int interfere_or_not(){
	//fprintf(stdout,"rand %d\n",rand());
	//fprintf(stdout,"randmac %d\n",RAND_MAX);
	double rand_value = ((double)rand())/((double)RAND_MAX);
	//fprintf(stdout,"interefere or not %f\n",rand_value);
	//fprintf(stdout,"in interfere or not,randvalue = %f\n",rand_value);
	if(rand_value > 0.5){
		return 1;
	}else{
		return 0;
	}

}
int send_message_or_not(){
	double rand_value = ((double)rand())/((double)RAND_MAX);
	//fprintf(stdout,"in send_message or not,randvalue = %f\n",rand_value);
	if(rand_value > 0.5){
		return 1;
	}else{
		return 0;
	}

}
void change_frequency(pNode p_node){
	//printf("change fre %d\n",getpid());
	FILE* f = fopen(p_node->file,"a+");
	double rand_value = ((double)rand())/((double)RAND_MAX);
	//fprintf(stdout,"randvaluenode %f\n",rand_value);
	struct timeval time;
	if(rand_value >= dwell_probability){
		double rand_value1 = ((double)rand())/((double)RAND_MAX);
		if(rand_value1 > 0.5){
			gettimeofday(&time,NULL);
			fprintf(f,"changed frequency to %d, time %f\n",11,(double)time.tv_sec);
			p_node->current_frequency = 11;
			fclose(f);
		}else{
			gettimeofday(&time,NULL);
			fprintf(f,"changed frequency to %d,time %f\n",6,(double)time.tv_sec);
			p_node->current_frequency = 6;
			fclose(f);
		}
	}else{
		gettimeofday(&time,NULL);
		fprintf(f,"changed frequency to %d, time %f\n",1,(double)time.tv_sec);
		fclose(f);
		return;
	}	

}

void change_noisemaker_frequency(pNoisemaker p_noisemaker){
	//printf("change fre %d\n",getpid());
	//fprintf(stdout,"change noise fre\n");
	FILE* f = fopen(p_noisemaker->file,"a+");
	struct timeval time;
	double rand_value = ((double)rand())/((double)RAND_MAX);
	if(rand_value >= dwell_probability_noisemakers){
		double rand_value1 = ((double)rand())/((double)RAND_MAX);
		if(rand_value1 > 0.5){
			gettimeofday(&time,NULL);
			fprintf(f,"changed frequency to %d, time %f\n",11,(double)time.tv_sec);
			p_noisemaker->current_frequency = 11;
			fclose(f);
		}else{
			gettimeofday(&time,NULL);
			fprintf(f,"changed frequency to %d, time %f\n",6,(double)time.tv_sec);
			p_noisemaker->current_frequency = 6;
			fclose(f);
		}
	}else{
		gettimeofday(&time,NULL);
		fprintf(f,"changed frequency to %d, time %f\n",1,(double)time.tv_sec);
		fclose(f);
		return;
	}	

}
void try_to_send(pNode p_node){
	create_and_add(p_node);
	sem_wait(&(p_node->sending_sem));
	send_message(p_node);
	sem_post(&(p_node->sending_sem));
	print_messages(p_node);
}


void send_message(pNode p_node){
	//fprintf(stdout,"sending message\n");
	if(p_node->current_number_messages <= 0){
	}else{
		for(int i = 0;i < p_node->current_neighboring_nodes;i++){
			sem_wait(&(p_node->neighboring_nodes[i]->sem1));
		}
		int j = p_node->current_number_messages;
		int random_message_number;
		if(j > 1){
			random_message_number = rand() % (j - 1)+1;
		}else{
			random_message_number = 1;
		}
		random_message_number = 1;
		struct message* message_to_send = go_through(p_node,random_message_number);
		broadcast(p_node,message_to_send,random_message_number);
		for(int i = 0;i < p_node->current_neighboring_nodes;i++){
			sem_post(&(p_node->neighboring_nodes[i]->sem1));
			//fprintf(stderr,"node %d %d released lock1 of node %d %d\n",p_node->x,p_node->y,p_node->neighboring_nodes[i]->x,p_node->neighboring_nodes[i]->y);
		}
		//fprintf(stdout,"before delete \n");
		delete_message(p_node,random_message_number);

	}


	struct timeval start,end;
		int not_terminate = 1;
		gettimeofday(&start,NULL);
		while(not_terminate){
			gettimeofday(&end,NULL);
			//fprintf(stdout,"before dwell time\n");
			if(difftime(end.tv_sec,start.tv_sec) >= transmission_time){
				not_terminate = 0;
			}
		}

}

void broadcast(pNode p_node,struct message* message_to_send,int random_number){
	struct timeval time;
	for(int i = 0;i < p_node->current_neighboring_nodes;i++){
		if(p_node->neighboring_nodes[i]->current_frequency == p_node->current_frequency && is_not_seen(p_node->neighboring_nodes[i],message_to_send)){

			sem_wait(&(p_node->neighboring_nodes[i]->message_sem));

			//fprintf(stderr,"node %d %d holds message lock of node %d %d\n",p_node->x,p_node->y,p_node->neighboring_nodes[i]->x,p_node->neighboring_nodes[i]->y);
			//fprintf(stdout,"before add id %d\n",p_node->neighboring_nodes[i]->message->mid);
			add_message(p_node->neighboring_nodes[i],message_to_send);

				
			FILE* f = fopen(p_node->file,"a+");
			gettimeofday(&time,NULL); 		
			fprintf(f,"sent messageid %d to nodeid %d at frequency %d time %f\n",message_to_send->mid,p_node->neighboring_nodes[i]->id,p_node->current_frequency,(double)time.tv_sec);
			fclose(f);
			f = fopen(p_node->neighboring_nodes[i]->file,"a+");
			fprintf(f,"received messageid %d from nodeid %d at frequency %d (other thread print) \n",message_to_send->mid,p_node->id,p_node->current_frequency);
			fclose(f);

			p_node->neighboring_nodes[i]->seen_messages[p_node->neighboring_nodes[i]->total_seen_number_messages] = message_to_send->mid;
			//fprintf(stdout,"added id %d to seen messages of node %d %d\n",message_to_send->mid,p_node->neighboring_nodes[i]->x,p_node->neighboring_nodes[i]->y);
			p_node->neighboring_nodes[i]->total_seen_number_messages = p_node->neighboring_nodes[i]->total_seen_number_messages + 1;
	
			//fprintf(stderr,"before unlock\n");

			sem_post(&(p_node->neighboring_nodes[i]->message_sem));


			//fprintf(stdout,"broadcasted message %d from %d %d to %d %d\n",message_to_send->mid,p_node->x,p_node->y,p_node->neighboring_nodes[i]->x,p_node->neighboring_nodes[i]->y);
		}
	}


}

int is_not_seen(struct regular_node* receiver,struct message* candidate_message){
	if(receiver == NULL || candidate_message == NULL){
		//fprintf(stdout,"weird error,receiving node or message is NULL-does not exist\n");
		return 0;
	}else{
		int not_seen = 1;
		int i;
		int id = candidate_message->mid;
		for(i = 0;i < receiver->total_seen_number_messages;i++){
			if(receiver->seen_messages[i] == id){
				not_seen = 0;
				//fprintf(stdout,"message %d is already seen by node %d %d\n",candidate_message->mid,receiver->x,receiver->y);
				return not_seen;
			}
		}
	//fprintf(stdout,"message %d is not  seen by node %d %d\n",candidate_message->mid,receiver->x,receiver->y);
				
		return not_seen;
	}
}
				 
		 	

void add_message(struct regular_node* pnode,struct message* pmessage_add){
	//fprintf(stdout,"in add message\n");
	
	struct message* last_message = (struct message*)malloc(sizeof(struct message));
	//fprintf(stdout,"id %d\n",pnode->message->mid);
	if(pnode->message == NULL){
		//fprintf(stdout,"in first if\n");
		pnode->message = (struct message*)malloc(sizeof(struct message));
		pnode->message->next = NULL;
		pnode->message->previous = NULL;
		pnode->message->text = (char*)malloc(50*sizeof(char));
		pnode->message->text = pmessage_add->text;
		pnode->message->mid = pmessage_add->mid;
		//fprintf(stdout,"added message here1\n");
	}else{
		last_message= pnode->message;
		//fprintf(stdout,"before while loop\n");
		while(last_message->next != NULL){
			//fprintf(stdout,"text of message : %s\n",last_message->text);
	                //fprintf(stdout,"in while loop\n");
			last_message = last_message->next;
		}
		last_message->next = (struct message*)malloc(sizeof(struct message));
		last_message->next->next = NULL;
		last_message->next->text = (char*)malloc(50*sizeof(char)); 
		last_message->next->text = pmessage_add->text;
		last_message->next->mid = pmessage_add->mid;
		last_message->next->previous = last_message;
	}
	//fprintf(stdout,"somehow added message\n");
	pnode->current_number_messages = pnode->current_number_messages + 1;
}

struct message* go_through(struct regular_node* pnode,int index){
	struct message* start = (struct message*)malloc(sizeof(struct message));
	struct message* found = (struct message*)malloc(sizeof(struct message));
	if(pnode->message == NULL){
		fprintf(stdout,"error, list empty\n");
		return NULL;
	}else if(pnode->current_number_messages < index){
		fprintf(stdout,"error, list index out of range\n");
		return NULL;
	}else{
		start = pnode->message;
		found = start;
		int i;
		for(i = 1;i < index;i++){
			if(found->next == NULL){
				fprintf(stdout,"requested node does not exist\n");
				return found;
			}
			found = found->next;
		}
		return found;
	}
}

void delete_message(struct regular_node* pnode,int index){
	struct message* to_delete = (struct message*)malloc(sizeof(struct message));
	to_delete = go_through(pnode,index);
	//fprintf(stdout,"message to delete %s\n",to_delete->text);
	if(index == 1 && pnode->current_number_messages == 1){
		pnode->message = NULL;
		to_delete->next = NULL;
		to_delete->previous = NULL;
	}else if(index == 1 && pnode->current_number_messages > index){
		pnode->message = pnode->message->next;
		pnode->message->previous = NULL;
		to_delete->next = NULL;
		to_delete->previous = NULL;
	}else if(index == pnode->current_number_messages){
		to_delete->previous->next = NULL;
	}else{
		to_delete->previous->next = to_delete->next;
		to_delete->next->previous = to_delete->previous;
		//printf("new nbrs %s %s\n",to_delete->previous->next->text,to_delete->next->previous->text);
		to_delete->next = NULL;
		to_delete->previous = NULL;
		
	}
	pnode->current_number_messages = pnode->current_number_messages - 1;
	FILE* f = fopen(pnode->file,"a+");
	if(to_delete != NULL){
		fprintf(f,"deleted messageid %d\n",to_delete->mid);
	}
	fclose(f);

}
void print_messages(struct regular_node* pnode){
	int index = pnode->current_number_messages;
	struct message* start = (struct message*)malloc(sizeof(struct message));
	struct message* found = (struct message*)malloc(sizeof(struct message));

	FILE* f = fopen(pnode->file,"a+");
	struct timeval time;
	gettimeofday(&time,NULL);
	sem_wait((&pnode->message_sem));
	fprintf(f,"will show messages, time %f\n",(double)time.tv_sec);
	if(pnode->message == NULL){
		fprintf(f,"no messages in node\n");
		//fclose(f);

	}else if(pnode->current_number_messages < index){
		fprintf(stdout,"error, list index out of range\n");
		//return;
	}else{
		start = pnode->message;
		found = start;
		int i;
		//fprintf(f,"id of message : %d\n",found->mid);
		for(i = 1;i <= index;i++){
			if(found == NULL)break;
			fprintf(f,"id of message : %d\n",found->mid);
			/*if(found->next == NULL){
				fprintf(stdout,"requested node does not exist\n");
				return;
			}*/
			if(found->next == NULL)break;
			found = found->next;
		}
	}
	sem_post((&pnode->message_sem));
	fclose(f);
	return;
}


void block_channel(pNoisemaker p_noisemaker){
	//fprintf(stdout,"BLOCKING CHANELL by %d %d\n",p_noisemaker->x,p_noisemaker->y);
	int i;
	int frequency = p_noisemaker->current_frequency;
	struct timeval time;
	for(i = 0;i < p_noisemaker->current_neighboring_nodes;i++){
		if(frequency == p_noisemaker->neighboring_regular_nodes[i]->current_frequency){
			sem_wait(&(p_noisemaker->neighboring_regular_nodes[i]->sending_sem));
			p_noisemaker->neighboring_regular_nodes[i]->blocked = p_noisemaker->id;

			gettimeofday(&time,NULL);
			FILE* f = fopen(p_noisemaker->file,"a+");
			fprintf(f,"BLOCKING CHANELL %d for nodeid %d, time %f\n",frequency,p_noisemaker->neighboring_regular_nodes[i]->id,(double)time.tv_sec);
			fclose(f);
			f = fopen(p_noisemaker->neighboring_regular_nodes[i]->file,"a+");
			fprintf(f,"BLOCKED IN CHANELL %d by noisemakereid %d (other thread print) \n",frequency,p_noisemaker->id);
			fclose(f);
		}
	}

	return;

}
void unblock_channel(pNoisemaker p_noisemaker){
	//fprintf(stdout,"UNBLOCKING CHANELL by %d %d\n",p_noisemaker->x,p_noisemaker->y);
	int i;
	int frequency = p_noisemaker->current_frequency;
	struct timeval time;
	for(i = 0;i < p_noisemaker->current_neighboring_nodes;i++){

			//fprintf(stdout,"UNBLOCKING CHANELL for %d %d\n",p_noisemaker->neighboring_regular_nodes[i]->x,p_noisemaker->neighboring_regular_nodes[i]->y);
		if(p_noisemaker->neighboring_regular_nodes[i]->blocked == p_noisemaker->id){
			gettimeofday(&time,NULL);
			FILE* f = fopen(p_noisemaker->file,"a+"); 
			fprintf(f,"UNBLOCKING CHANELL %d for nodeid %d time %f \n",frequency,p_noisemaker->neighboring_regular_nodes[i]->id,(double)(time.tv_sec));
			fclose(f);
			f = fopen(p_noisemaker->neighboring_regular_nodes[i]->file,"a+");
			fprintf(f,"UNBLOCKED IN CHANELL %d by noisemakereid %d (other thread print)\n",frequency,p_noisemaker->id);
			fclose(f);
			p_noisemaker->neighboring_regular_nodes[i]->blocked = -10;
			sem_post(&(p_noisemaker->neighboring_regular_nodes[i]->sending_sem));

		}
	}

	return;

}
void create_and_add(pNode pnode){
	struct message* newm = (struct message*)malloc(sizeof(struct message));
	newm->text = "dummy message";
	sem_wait(&global_message_sem);
	global_message_counter++;
	newm->mid = global_message_counter;
	sem_post(&global_message_sem);
	(pnode->current_number_messages)++;
	add_message(pnode,newm);
	FILE* f = fopen(pnode->file,"a+");
	fprintf(f,"created messageid %d\n",newm->mid);
	fclose(f);
	//fprintf(stdout,"created mess %d\n",newm->mid);
	return;
}

int mid(){
	return 1234;
}




