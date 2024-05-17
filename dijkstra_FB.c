// dijkstra
// priority queue; fibonacci heap; (read from main)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Strcut declarations(to avoid cylic dependencies)
typedef struct sGNode GNode;
typedef struct sEdge Edge;
typedef struct sGraph Graph;
typedef struct sNode Node;

// Graph Node
typedef struct sGNode {
    int vertex;
    struct sGNode* next;
    int edgeIndex; // Index for the respective edge
} GNode;

// Graph Edge
typedef struct sEdge {
    int src, dest;
    double weight;
    Node* link;    // linked with FB node
} Edge;

// Graph dijk version
typedef struct sGraph {
    GNode **adjLists;   // adjacency lists
    int numVertices;    // number of vertices
    bool *visited;      // false: not been visited, true: visited
    int *p;             // predecessor
    int *dist;          // distance
    Edge* edgeList;     // edge list
} Graph;

// Graph operation
Graph* CreateGraph(int vertices);
    // constructor
void DeleteGraph(Graph* graph);
    // destructor
void AddWeightedEdge(Graph* graph, int src, int dest, double weight);
    // add a directed edge

/*---Fibonacci heap start---*/
// Node
typedef struct sNode {
    double key; // dist from source
    int degree; // number of child, LAZY(calculate while needed)
    bool mark;  // mark if has lost child
    struct sNode *leftSib;
    struct sNode *rightSib;
    struct sNode *parent;
    struct sNode *child;

    // loaded attribute
    Edge* link; // linked with an edge
} Node;

Node* _CreateNode(double key, Edge* link);
    // constructor
void _DeleteNode(Node* node);
    // destructor

// FibonacciHeap
typedef struct sFibonacciHeap {
    int n;    // number of nodes
    Node *min;
    Node *rootList;
} FibonacciHeap;

FibonacciHeap* CreateFibonacciHeap();
    // constructor
void _DeleteFibonacciHeap(FibonacciHeap* FH);
    // destructor
Node* FindMin(FibonacciHeap* FH);
    // find mininum node
void Insert(FibonacciHeap* FH, double key, Edge* link);
    // insert new node
void _MergeWithDoublyList(FibonacciHeap* FH, Node** root, Node* node);
    // throw a node to a circular doubly linked list
FibonacciHeap* UnionFibonacciHeap(FibonacciHeap* FH1, FibonacciHeap* FH2);
    // union 2 heaps
Node* ExtractMin(FibonacciHeap* FH);
    // find min and remove min
void _RemoveFromRootList(FibonacciHeap* FH, Node* node);
    // remove a node on root list
void _Consolidate(FibonacciHeap* FH);
    // build binomial heap and set true min
void _HeapLink(FibonacciHeap* FH, Node* y, Node* x);
    // make y child of x
void DecreaseKey(FibonacciHeap* FH, Node* x, int k);
    // make a key smaller
void _Cut(FibonacciHeap* FH, Node* x, Node* y);
    // throw a node from child to root list, reset mark
void _CascadingCut(FibonacciHeap* FH, Node* y);
    // keep cutting until find a unmarked node
Node* Delete(FibonacciHeap* FH, Node* x);
    // delete a node
/*---Fibonacci heap end---*/

// Single source shortest path algorithm
void Dijkstra(Graph* graph, int src, FibonacciHeap* FH);

// Global variables
int V = 8;
int E = 22;
int E_count = 0;

int main(void)
{
    int i, j;
    Graph *graph = CreateGraph(V);
    FibonacciHeap *FH = CreateFibonacciHeap();
    int src;

    AddWeightedEdge(graph, 0, 1, 4);   AddWeightedEdge(graph, 1, 0, 4);
    AddWeightedEdge(graph, 0, 2, 2);   AddWeightedEdge(graph, 2, 0, 2);
    AddWeightedEdge(graph, 0, 5, 7);   AddWeightedEdge(graph, 5, 0, 7);
    AddWeightedEdge(graph, 1, 3, 2);   AddWeightedEdge(graph, 3, 1, 2);
    AddWeightedEdge(graph, 2, 4, 8);   AddWeightedEdge(graph, 4, 2, 8);
    AddWeightedEdge(graph, 2, 5, 3);   AddWeightedEdge(graph, 5, 2, 3);
    AddWeightedEdge(graph, 3, 5, 5);   AddWeightedEdge(graph, 5, 3, 5);
    AddWeightedEdge(graph, 3, 6, 6);   AddWeightedEdge(graph, 6, 3, 6);
    AddWeightedEdge(graph, 4, 7, 3);   AddWeightedEdge(graph, 7, 4, 3);
    AddWeightedEdge(graph, 5, 7, 4);   AddWeightedEdge(graph, 7, 5, 4);
    AddWeightedEdge(graph, 6, 7, 2);   AddWeightedEdge(graph, 7, 6, 2);

    src = 0;
    Dijkstra(graph, src, FH);

    // Print shortest path
    for (i = 0; i < V; i++) {
        printf("The shortest path from 0 to %d is %d\n", i, graph->dist[i]);
        puts("Path: ");
        j = i;
        while (j != src) {
            printf("%d<-", j);
            j = graph->p[j];
        };
        printf("%d\n", src);
    }

    // Free memory
    DeleteGraph(graph);
    _DeleteFibonacciHeap(FH);

    return 0;
}

// Create graph
Graph* CreateGraph(int vertices)
{
    Graph* graph = (Graph*)malloc(sizeof(Graph));

    graph->adjLists = (GNode**)calloc(vertices, sizeof(GNode*));
    graph->numVertices = vertices;
    graph->visited = (bool*)calloc(vertices, sizeof(bool));
    graph->p = (int*)calloc(vertices, sizeof(int));
    graph->dist = (int*)calloc(vertices, sizeof(int));
    graph->edgeList = (Edge*)malloc(E * sizeof(Edge));

    return graph;
}

// Destructor
void DeleteGraph(Graph* graph)
{
    GNode *current, *next;
    int i;

    for (i = 0; i < graph->numVertices; i++) {
        current = graph->adjLists[i];
        while (current != NULL) {
            next = current->next;
            free(current);
            current = next;
        }
    }
    free(graph->adjLists);
    free(graph->visited);
    free(graph->p);
    free(graph->dist);
    free(graph->edgeList);
    free(graph);
}

// Add edge
void AddWeightedEdge(Graph* graph, int src, int dest, double weight)
{
    GNode* newNode = malloc(sizeof(GNode));

    // Add edge from src to dest
    newNode->vertex = dest;
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;

    // Make adjLists can link to edgeList
    newNode->edgeIndex = E_count;

    // Add edge list
    graph->edgeList[E_count].src = src;
    graph->edgeList[E_count].dest = dest;
    graph->edgeList[E_count].weight = weight;
    E_count++;
}

/*---Fibonacci heap start---*/
// Constructor
Node* _CreateNode(double key, Edge* edge) {
    Node *node = (Node*)malloc(sizeof(Node));

    node->key = key;
    node->degree = 0;
    node->mark = false;
    node->leftSib = node;
    node->rightSib = node;
    node->parent = NULL;
    node->child = NULL;

    // Create connectness for edge and FB node
    node->link = edge; // from edge to node
    edge->link = node; // from node to edge

    return node;
}

// Destructor
void _DeleteNode(Node* node) {
    free(node);
}

// Constructor
FibonacciHeap* CreateFibonacciHeap()
{
    FibonacciHeap *FH = (FibonacciHeap*)malloc(sizeof(FibonacciHeap));

    FH->n = 0;
    FH->min = NULL;
    FH->rootList = NULL;

    return FH;
}

// Destructor
void _DeleteFibonacciHeap(FibonacciHeap* FH)
{
    Node* N1;

    // Using ExtractMin to free all nodes
    while (FH->min != NULL) {
        N1 = ExtractMin(FH);
        _DeleteNode(N1);
    }
    free(FH);
}

// Find the minimum node
Node* FindMin(FibonacciHeap* FH)
{
    return FH->min;
}

// Insert new node
void Insert(FibonacciHeap* FH, double key, Edge* link)
{
    Node *node = _CreateNode(key, link);

    _MergeWithDoublyList(FH, &FH->rootList, node);

    if ((FH->min == NULL) || (node->key < FH->min->key))
        FH->min = node;

    FH->n++;
}

// Throw a node to a circular doubly linked list
void _MergeWithDoublyList(FibonacciHeap* FH, Node** root, Node* node)
{
    if (*root == NULL) {
        *root = node;
    } else {
        // Insert node in the left next to the root
        // Adjusting circular doubly linked list for both node and root
        node->leftSib = *root;
        node->rightSib = (*root)->rightSib;
        (*root)->rightSib->leftSib = node;
        (*root)->rightSib = node;
    }
}

// Union 2 heaps
FibonacciHeap* UnionFibonacciHeap(FibonacciHeap* FH1, FibonacciHeap* FH2)
{
    FibonacciHeap *FH3 = CreateFibonacciHeap();

    // Use FH1's rootList
    FH3->rootList = FH1->rootList;

    // Insert FH2 in FH1 root's left
    FH2->rootList->rightSib->leftSib = FH3->rootList->leftSib;
    FH3->rootList->leftSib->rightSib = FH2->rootList->rightSib;
    FH2->rootList->rightSib = FH3->rootList;
    FH3->rootList->leftSib = FH2->rootList;

    // Choose the smaller min between FH1 and FH2
    if (FH1->min->key < FH2->min->key) {
        FH3->min = FH1->min;
    } else {
        FH3->min = FH2->min;
    }

    // Sum up numbers of node
    FH3->n = FH1->n + FH2->n;

    return FH3;
}

// Find min + Remove min
// Consolidate the heap
// Need to free the node after using!!!
Node* ExtractMin(FibonacciHeap* FH)
{
    Node *node = FH->min;
    Node *temp;
    Node *next;
    Node *startPoint;

    if (node != NULL) {
        if (node->child != NULL) {
            // Throw all children to root list
            temp = node->child;
            startPoint = node->child;
            node->child = NULL;
            node->degree = 0;
            do {
                next = temp->rightSib;
                temp->parent = NULL;
                temp->rightSib = temp;
                temp->leftSib = temp;
                _MergeWithDoublyList(FH, &FH->rootList, temp);
                    // throw it to root list
                temp = next;
            } while (temp != startPoint);   // while traversed end
        }

        // Remove min node
        _RemoveFromRootList(FH, node);
        FH->n--;

        // Do consolidate
        if (node->rightSib == node) {   // node is the only node on root list
            FH->min = NULL;             // (and it disappear now)
            FH->rootList = NULL;
        } else {
            _Consolidate(FH);           // build binomial heap and set ture min
        }
    }

    return node;
}

// Remove node, noted that min is not correct
void _RemoveFromRootList(FibonacciHeap* FH, Node* node)
{
    // Last node
    if (node->rightSib == node) {
        FH->rootList = NULL;
        FH->min = NULL;
    } else {
        // Accidentally delete root
        if (node == FH->rootList)
            FH->rootList = node->rightSib;  // root right shift

        // Accidentally delete min
        if (node == FH->min)
            FH->min = node->rightSib;

        // Remove node
        node->leftSib->rightSib = node->rightSib;
        node->rightSib->leftSib = node->leftSib;
    }
}

// Build binomial heap and set true min
void _Consolidate(FibonacciHeap* FH)
{
    Node **A;       // the A[] array
    Node *x, *y;    // new tree(iterate), original tree in A[d]
    Node *temp;     // swapping
    Node *x_copy;
    Node *next;
    int num;
    int i;
    int d;          // degree
    int size = 0;   // size of A

    // Calculate log2N(size of A)
    num = FH->n;
    while (num > 0) {
        num = num / 2;
        size++;
    }

    // Initialize A
    A = (Node**)malloc(size * sizeof(Node*));
    for (i = 0; i < size; i++) {
        A[i] = NULL;
    }

    // x traverse circular list a cycle
    x = FH->rootList;
    next = x->rightSib;
    x_copy = NULL;
    while (x_copy != next) {
        x = next;
        x_copy = x;         // copy x for judging loop condition
        next = x->rightSib;
        d = x->degree;
        _RemoveFromRootList(FH, x);
        x->leftSib = x;     // cut the relationship with siblings
        x->rightSib = x;

        // Keep combining and incresing til find a empty A[d]
        while (A[d] != NULL) {  // A[d] is occupied
            y = A[d];           // y get the original tree link

            // Combine trees,
            // larger root become subtree of smaller root
            if (x->key > y->key) {
                temp = x; x = y; y = temp;      // exchange x, y
            }
            _HeapLink(FH, y, x);                // make y child of x
            A[d] = NULL;
            d++;            // increment degree
        }
        A[d] = x;           // find a vacant A[d], x landed
    };

    // Add subtrees in A to root list
    for (i = 0; i < size; i++) {
        if (A[i] != NULL) {
            _MergeWithDoublyList(FH, &FH->rootList, A[i]);
                // throw A[i] to new root list

            // Find min
            if ((FH->min == NULL) || (A[i]->key < FH->min->key)) {
                FH->min = A[i];
            }
        }
    }

    // Free A(A array is merged to root list now)
    free(A);
}

// Make y child of x
void _HeapLink(FibonacciHeap* FH, Node* y, Node* x)
{
    _MergeWithDoublyList(FH, &x->child, y);
        // merge y with x child's doubly list
    x->degree += 1;
    y->parent = x;
    y->mark = false;
}

// Make a key smaller
void DecreaseKey(FibonacciHeap* FH, Node* x, int k) {
    Node *y;

    if (k > x->key) {
        return;
    }
    x->key = k;
    y = x->parent;
    if (y != NULL && x->key < y->key) {
        _Cut(FH, x, y);
        _CascadingCut(FH, y);
    }
    if (x->key < FH->min->key) {
        FH->min = x;
    }
}

// Throw a node from child to root list, reset mark
// y is parent, x is child
void _Cut(FibonacciHeap* FH, Node* x, Node* y) {
    // Remove child x from y
    y->child = NULL;
    y->degree--;
    x->parent = NULL;
    x->rightSib = x;
    x->leftSib = x;
    _MergeWithDoublyList(FH, &FH->rootList, x);
    x->mark = false;
}

// Keep cutting until find a unmarked node
void _CascadingCut(FibonacciHeap* FH, Node* y) {
    Node *z = y->parent;

    if (z != NULL) {
        if (y->mark == false) {
            y->mark = true;
        } else {
            _Cut(FH, y, z);
            _CascadingCut(FH, z);
        }
    }
}

// Delete a node
Node* Delete(FibonacciHeap* FH, Node* x)
{
    DecreaseKey(FH, x, -__INT_MAX__);

    return ExtractMin(FH);
}
/*---Fibonacci heap end---*/

// Single source shortest path algorithm
void Dijkstra(Graph* graph, int src, FibonacciHeap* FH)
{
    GNode* temp;
    int currentVertex, adjVertex;
    int distFromSrc;
    Node *N1;
    bool flag1 = false; // N1 is null
    bool flag2 = false; // visited

    // Step 1: Visit src, set dist of src = 0
    graph->dist[src] = 0;
    graph->p[src] = -1;
    graph->visited[src] = true;
    currentVertex = src;

    do {
        // Step 2: using adjLists to traverse all connected vertices,
        //     if unvisited, calculate dist from src,
        //     insert to priority queue(don't touch dist array)
        //     else (visited), update dist array, decrese key
        //     set current vertex visited
        temp = graph->adjLists[currentVertex];
        while (temp != NULL) {
            adjVertex = temp->vertex;
            if (graph->visited[adjVertex] == false) {   // unvisited
                distFromSrc = graph->dist[currentVertex] +
                    graph->edgeList[temp->edgeIndex].weight;
                Insert(FH, distFromSrc, &graph->edgeList[temp->edgeIndex]);
                graph->p[adjVertex] = currentVertex;
            } else {    // visited
                distFromSrc = graph->dist[currentVertex] +
                    graph->edgeList[temp->edgeIndex].weight;
                if (distFromSrc < graph->dist[adjVertex]) {
                    graph->dist[adjVertex] = distFromSrc;
                    graph->p[adjVertex] = currentVertex;
                    DecreaseKey(FH, graph->edgeList[temp->edgeIndex].link,
                        distFromSrc);
                }
            }
            temp = temp->next;
        }
        graph->visited[currentVertex] = true;

        // Step 3: find the current unvisit vertex closest to src
        //     update dist array, go to step 2,
        //     terminate when all vertices are visited
        do {
            N1 = ExtractMin(FH);    // closest
            if (N1 == NULL) {
                flag1 = true;
            } else {
                if (graph->visited[N1->link->dest] == true) flag2 = true;
            }
            if (!flag1 && flag2) _DeleteNode(N1);
        } while (!flag1 && flag2); // keep find if visited
        if (!flag1) {
            currentVertex = N1->link->dest;
            graph->dist[currentVertex] = N1->key;
            graph->p[N1->link->dest] = N1->link->src;
            _DeleteNode(N1);
        }
    } while (!flag1); // empty queue
}