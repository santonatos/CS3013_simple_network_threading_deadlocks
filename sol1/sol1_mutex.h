#ifndef SOL1_MUTEX_C
#define SOL1_MUTEX_C

//author: spiros antonatos

#define max_num_messages_node 1000000
#define max_num_nodes 10000

//linked list of messages
typedef struct message{
	//char text[50]; //= create_text();
	char* text;
	int mid; //= pick_id();
	int is_seen; //boolean
	struct message* next;
	struct message* previous;
}Message;

typedef struct regular_node{
	int id;
	int x;
	int y;
	int blocked;
	struct message* message;
	int current_number_messages;
	int current_neighboring_nodes;
	int total_seen_number_messages;
	char* file;
	//array(s) of messages to see/send
	pthread_mutex_t starting_lock;
	pthread_mutex_t message_lock;
	pthread_mutex_t sending_lock;
	pthread_mutex_t lock1;
	pthread_mutex_t lock6;
	pthread_mutex_t lock11;
	struct regular_node* neighboring_nodes[max_num_nodes];
	//lock 1,lock 2 3
	int current_frequency;
	int seen_messages[max_num_messages_node];
	pthread_t thread;
}node,*pNode;

typedef struct noisemaker_node{
	int id;
	int x;
	int y;
	char* file;
	pthread_t thread;
	pthread_mutex_t starting_lock;
	int current_frequency;
	struct regular_node* neighboring_regular_nodes[max_num_nodes];
	int current_neighboring_nodes;
	//int currently_blocked[max_num_nodes];

}noisemaker,*pNoisemaker;



char* create_text();
int pick_id();
//void (*functionPtr_node)(pNode);

/*
struct message * go_through(struct regular_node * start, int which);
void delete_message(struct regular_node * start, int which);
void add_message(struct regular_node * start, struct message * new);
*/

void add_message(struct regular_node* pnode,struct message* pmessage_add);
struct message* go_through(struct regular_node* pnode,int index);
void delete_message(struct regular_node* pnode,int index);

void print_messages(struct regular_node* pnode);
int is_not_seen(struct regular_node* receiver,struct message* candidate_message);

void place_node(pNode p_node,int * i,int pos_x,int pos_y);
void determine_neighbors();
void initialize_node_thread(pNode p_node);
void change_frequency(pNode p_node);
void broadcast(pNode p_node,struct message* message_to_send,int random_number);
void* node_routine(void* p_node);
void send_message(pNode p_node);
void try_to_send(pNode p_node);
void create_file_node(struct regular_node* pnode);
void create_and_add(pNode pnode);

void place_noisemaker(pNoisemaker p_noisemaker,int * i,int pos_x,int pos_y);
void determine_noisemaker_neighbors();
void initialize_noisemaker_thread(pNoisemaker p_noisemaker);
void change_noisemaker_frequency(pNoisemaker p_noisemaker);
void* noisemaker_routine(void* p_noisemaker);
void block_channel(pNoisemaker p_noisemaker);
void unblock_channel(pNoisemaker p_noisemaker);
void create_file_noisemaker(struct noisemaker_node* pnoisemaker);

int interfere_or_not();
int send_message_or_not();

#endif
