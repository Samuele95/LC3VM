typedef struct node {
    char* token;
    struct node* left;
    struct node* right;
} node_t;

node_t* new_node(char* token);
node_t* insert(node_t* rootnode, char* token);
int deletenode(node_t* node);
void printtree(node_t *tree);
