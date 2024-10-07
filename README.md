# Dijkstra演算法，以priority queue實作

*此為[github patat0717/Dijkstra_PQ repository](https://github.com/patata0717/Dijkstra_PQ)的README file*

*2024/5/17寫完fibonacci heap，未來可能更新binomial heap和binary heap*
*2027/10/7增加測試方法、testcase generator(python script)*

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

簡單介紹一下三種heap，binary heap就是heap sort裡的用到的heap、binomial heap改善了binary heap不易union的特性，犧牲FindMin()的complexity去trade off Union()。Fibonacci heap是一種在數學上取巧的heap，ExtractMin實際上是O(n)complexity，但因為前面已經有n次Insert()，它就把O(n)平攤給前面的Insert()，所以得到O(1)。Fibonacci heap的精神就是「不做heapify」，只要遇到需要heapify的地方，就直接建一顆新的樹，樹會越來越多直到遇到下一個ExtractMin()，ExtractMin()會把forest塌縮成binomial heap。

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
    - If visited, update dist, DecreasKey() to update FH key
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

FHNode只存了key，要注意key是dist from src，不是weight of edge。

首先，第一個資料的連結，就是FHNode和edge。FHNode本身只存了key，我們希望讓priority queue吐出某條edge，所以必須把key和edge做連結。我們選擇在FHNode上掛一個link連到edge。

再來，DecreaseKey()需要得到FHNode的位址，我們是從adjacentVertex要得到FHNode，因為FHNode是連到Edge，所以我們要有辦法從GNode連到edge，再從edge連到FHNode，所以我們分別還需要建兩個連結。

以上就是所有資料結構之間的連結。

因為FHNode是在dijkstra才建的，而edge是在ReadInput時就建了，所以我們選擇在FHNode上直接同時建立兩邊的連結。

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

    // Create connectness for edge and FH node
    node->link = edge;  // from FHNode to edge
    edge->link = node;  // from edge to FHNode

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

### Print Path

如果我們存下所有predecessor，就溯源print出path。所以dijkstra演算法既可以算出shortest path的值，也能實際畫出路徑。

## Validation

用ref2的例子，手動AddEdge輸入input，結果如下：

|V| = 8, |E| = 11

![image](https://imgur.com/KF9zfPf.jpg)

對比ref2的結果。

![image](https://i.imgur.com/GHZ9pEB.jpg)

跑一下valgrind抓memory leak。

![image](https://imgur.com/CPDgSiF.jpg)

證明沒有memory leak問題。

## Complexity analysis

我同時也製作了read from file的版本(dijkstra_FB2)，以方便進行complexity的測試。

使用python進行測試檔案的生成，我們輸入`|V|`, `|E|`，程式自動生成測試檔案`g_{V}_{E}.dat`，原理如下：

1. 建立spanning tree：將所有vertex編號並隨機排序，以此建立spanning tree。
2. 計算出剩餘的edge數，隨機挑選兩個原本沒有相連的點生成edge。
3. Weight是介於1~10的隨機整數。

下面我們要證明dijkstra演算法使用fibonacci heap的效率是`O(E+VlogV)`，證明方法如下：

1. `|V|` 和 `|E|` 相同增長速率`f(n)`，如果CPU time的增長速率對V和E是`O(f(n)*log(f(n)))`，代表V項或|E|項至少有一項是 `O(n*logn)` speed。
2. `|V|` 的增長速率是`f(n)`，`|E|` 的增長速率是`f(n*log(n))`，如果CPU time的增長速率對E是`O(n)`，對V是`O(n*log(n))`，代表V項比E項快，**V項是`O(n*logn)`**，E項 <= `O(n)`。
3. `|V|` 的增長速率是`f(n)`，`|E|` 的增長速率是`f(n2)`，如果CPU time的增長速率對E是`O(n)`，**則證明E項是`O(n)`**。

設`f(x) = n2`，用python script生成測試資料。

![Imgur](https://imgur.com/6xPzzSf.jpg)

![Imgur](https://imgur.com/Tr3dHFG.jpg)

![Imgur](https://imgur.com/KJNd2h1.jpg)

![Imgur](https://imgur.com/Z9tlxKA.jpg)

![Imgur](https://imgur.com/XUgbyxe.jpg)

![Imgur](https://imgur.com/wyWs7NE.jpg)

![Imgur](https://imgur.com/SFmwAls.jpg)

通過圖表，可以確認與預期結果沒有明顯錯誤，然而我採用的製圖方式，再加上數據誤差，沒有辦法很明顯的區分出`O(n)`和`O(n*log(n))`，有關這一點，我未來會再思考如何改進。

## Reference

演算法楓葉本(cormen)3rd edition, chapter 19.
