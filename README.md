# Dijkstra演算法，以priority queue實作

*5/17寫完fibonacci heap，未來可能更新binomial heap和binary heap*

## 前言

Dijkstra演算法是一個single source、all destination的shortest path algorithm，得知從單一一個點到所有其它點的最短矩離。

可以在directed, cylic, weighted的graph上跑，唯一的限制是不能有negative edge。

Dijkstra的步驟：

1. Set source's distant from source(dist) 0.
2. Start from source, traverse all connected vertices, updata dist if (dist of current vertex + edge < original dist).
3. Find a unvisited node that has min dist, go to step 2, loop til all node are visited.

裡面最耗時的步驟是3，find the min dist，在一般的資料結構，我們必須traverse all unvisit node才能找到，這樣complexity是O(V)。Step 2, Step 3的loop總共會操作V次，所以complexity就是O(V2)。

如果我們能利用priority queue的特性，就可以達到更好的complexity。

![image](https://imgur.com/h55JHNl.jpg)

[ref1](https://helios2.mi.parisdescartes.fr/~lomn/Cours/AV/Complements/Heap.pdf)

在這份project，我們會盡力做到最好的time complexity和space complexity，以fibonacci heap和adjacency list來實作。

Fibonaccie heap參考cormen的psuedo code。

Example參考[ref2](https://www.youtube.com/watch?v=CerlT7tTZfY)。

Time complexity: O(E+VlogV) (amortized)

Space complexity: O(V+E) AdjLists—O(2V), EdgeList—O(E), Fibonacciheap—O(E)

## Fibonacci heap

請參考[ref1](https://helios2.mi.parisdescartes.fr/~lomn/Cours/AV/Complements/Heap.pdf)和[ref3](https://cihcih.medium.com/%E5%9C%96%E8%AB%96%E6%BC%94%E7%AE%97%E6%B3%95ch19-2-fibonacci-heaps-3abde385b88c)。

我們會用到Insert(), DecreaseKey(), ExtractMin()。

簡單介紹一下三種heap，binary heap就是heap sort裡的heap，binomial heap改善了binary heap不易union的特性，犧牲FindMin()去trade off Union()。Fibonacci heap是一種在數學上取巧的heap，ExtractMin實際上是O(n)complexity，但因為前面已經有n次Insert()，它就把O(n)平攤給前面的Insert()，所以得到O(1)。Fibonacci heap的精神就是「不做heapify」，只要遇到需要heapify的地方，就直接建一顆新的樹，樹會越來越多直到遇到下一個ExtractMin()，ExtractMin()會把forest塌縮成binomial heap。

## Dijkstra和priority queue的整合

請參考[ref1](https://helios2.mi.parisdescartes.fr/~lomn/Cours/AV/Complements/Heap.pdf)，在第8頁有提供psuedo code。

![image](https://imgur.com/4bxhNYk.jpg)

ref1和ref2用的方法稍有差異，ref1是把所有的node都設成無限大，然後用decrease key來update，fibonacci heap裡操作的是graph node。但這是一個weighted graph，我們需要存edge的值，所以應該用edge來操作會比較適合，也是ref2影片的方法。

重新整理Dijkstra的步驟，

Input: AdjLists, EdgeList, Priority Queue(empty)

Output: Dist array, predecessor array

1. Visit src, set dist of src = 0
2. Using adjLists to traverse all connected vertices:
    - If unvisited, calculate dist from src, Insert() to priority queue
    - If visited, update dist, DecreasKey() to update FB key
3. ExtractMin() to find the current unvisit vertex closest to src, update dist array(don't do DecreaseKey()), go to step 2, terminate when all vertices are visited

所以我們會用到Insert()、ExtractMin()、DecreaseKey()三種運算，不管是用哪一種priority queue都是用這一個流程。

### AdjLists, EdgeList, Fibonacci heap的整合

看Psuedo code的步驟雖然不長，但實際情形沒有這麼容易，因為目前不同資料結構之間的資料是不能交流的。我覺得這個project最重要的部分，就是三種資料結構的連結。

AdjMatrix, AdjList的差別：

AdjMatrix清楚的連結了src, dest, connectness，甚至可以直接在adjmatrix裡存edge的weight，但complexity是O(V2)。

AdjLists雖然complexity是O(V)，但一無法存weight(可以其實存在GNode裡，但有點詞不達義，還是存在EdgeList裡比較好)，二無法從任一GNode就推知src是誰。

使用AdjMatrix明顯比AdjLists簡單很多，但因為complexity的關係，我們希望使用ajdLists完成。

我們先來看看三種資料結構各存了什麼：

AdjLists存了connectness，而且只能透過source去查destination，查的過程還要traverse，我們不能容許O(1)以外的資料連結方式！

EdgeList存了source、destination、以及weight。

FBNode只存了key，要注意key是dist from src，不是weight of edge。

首先，第一個資料的連結，就是FBNode和edge。FBNode本身只存了key，我們希望讓priority queue吐出某條edge，所以必須把key和edge做連結。我們選擇在FBNode上掛一個link連到edge。

再來，DecreaseKey()需要得到FBNode的位址，我們是從adjacentVertex要得到FBNode，因為FBNode是連到Edge，所以我們要有辦法從GNode連到edge，再從edge連到FBNode，所以我們分別還需要建兩個連結。

以上就是所有資料結構之間的連結。

因為FBNode是在dijkstra才建的，而edge是在ReadInput時就建了，所以我們選擇在FBNode上直接同時建立兩邊的連結。

```
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
    node->link = edge;  // from FBNode to edge
    edge->link = node;  // from edge to FBNode

    return node;
}
```

至於GNode到edge的部分，因為edgeList是array，我們只要存它的index就可以了。

```
// Graph Node
typedef struct sGNode {
    int vertex;
    struct sGNode* next;
    int edgeIndex; // Index for the respective edge
} GNode;
```

處理完資料結構，就只剩一些小問題而已

### Path

如果我們存下所有predecessor，就溯源print出path。所以dijkstra演算法既可以算出shortest path的值，也能實際畫出路徑。

## Validation

用ref2的例子，手動AddEdge輸入input，結果如下：

|V| = 8, |E| = 11

![image](https://imgur.com/VHka79k.jpg)

跑一下valgrind抓memory leak。

![image](https://imgur.com/CPDgSiF.jpg)

我同時也製作了read from file的版本。
