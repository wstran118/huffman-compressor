#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 256 // ASCII characters

//huffman tree node
typedef struct Node {
    char data; // character (leaf nodes)
    unsigned freq; //frequency for characters
    struct Node *left, *right; // child pointers
} Node;

// min-heap for priority queue
typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    Node **array;
} MinHeap;

//create a new node
Node* newNode(char data, unsigned freq) {
    Node* node = (Node*)malloc(sizeof(Node));
    if(!node) {
        perror("Memory allocation for new node failed!")
        exit(1);
    }
    node->data = data;
    node->freq = freq;
    node->left = node->right = NULL;
    return node;
}

//create a new min heap
MinHeap* createMin(unsigned capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    if(!heap) {
        perror("Memory allocation for heap failed!")
        exit(1);
    }
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = (Node**)malloc(heap->capacity * sizeof(Node*));
    if(!heap->array)  {
        perror("Memory allocation for array failed!");
        exit(1);
    }
    return heap;
}

//swap 2 nodes
void swapNodes(Node **a, Node **b) {
    Node *temp = *a;
    *a = *b;
    *b = temp;
}

//heapify at given index
void minHeapify(MinHeap **heap, int index) {
    int smallest = index;
    int left = 2*index + 1;
    int right = 2*index + 2;
    if(left < heap->size && heap->array[left]->freq < heap->array[smallest]->freq)
        smallest = left;
    if(right  < heap->size && heap->array[right]->freq < heap->array[smallest]->freq)
        smallest = right;
    if(smallest!= index) {
        swapNodes(&heap->array[index], &heap->array[smallest]);
        minHeapify(heap, smallest);
    }
}

//insert new node into min-heap
void insertMinHeap(MinHeap *heap, Node *node){
    if(heap->size == heap->capacity){
        fprintf(stderr, "Heap overflow.\n");
        exit(1);
    }
    heap->size++;
    int i = heap->size-1;
    heap->array[i] = node;

    while(i>0 && heap->array[(i-1)/2]->freq > heap->array[i]->freq) {
        swapNodes(&heap->array[i], &heap->array[(i-1)/2]);
        i = (i-1)/2;
    }
}

//extract the min node
Nodd* extractMin(MinHeap *heap) {
    if(heap->size == 0) return NULL;
    Node *min = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;
    minHeapify(heap,0);
    return min;
}
// Build frequency table from input file
void buildFrequencyTable(const char *filename, unsigned *freq) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening input file");
        exit(1);
    }
    int c;
    while ((c = fgetc(file)) != EOF) {
        freq[c]++;
    }
    fclose(file);
}

// Build Huffman tree
Node* buildHuffmanTree(unsigned *freq) {
    MinHeap *heap = createMinHeap(MAX_SIZE);
    for (int i = 0; i < MAX_SIZE; i++) {
        if (freq[i] > 0) {
            insertMinHeap(heap, newNode((char)i, freq[i]));
        }
    }
    while (heap->size > 1) {
        Node *left = extractMin(heap);
        Node *right = extractMin(heap);
        Node *parent = newNode('$', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        insertMinHeap(heap, parent);
    }
    Node *root = extractMin(heap);
    free(heap->array);
    free(heap);
    return root;
}

// Generate Huffman codes
void generateCodes(Node *root, char *code, int top, char *codes[]) {
    if (!root) return;
    char *leftCode = (char*)malloc(top + 2);
    char *rightCode = (char*)malloc(top + 2);
    if (!leftCode || !rightCode) {
        perror("Memory allocation failed");
        exit(1);
    }
    strcpy(leftCode, code);
    strcpy(rightCode, code);
    strcat(leftCode, "0");
    strcat(rightCode, "1");

    if (!root->left && !root->right) {
        codes[(unsigned char)root->data] = strdup(code[0] ? code : "0");
    }
    generateCodes(root->left, leftCode, top + 1, codes);
    generateCodes(root->right, rightCode, top + 1, codes);
    free(leftCode);
    free(rightCode);
}

// Free Huffman tree
void freeTree(Node *root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// Free Huffman codes
void freeCodes(char *codes[]) {
    for (int i = 0; i < MAX_SIZE; i++) {
        if (codes[i]) free(codes[i]);
    }
}

// Compress file
void compressFile(const char *inputFile, const char *outputFile) {
    unsigned freq[MAX_SIZE] = {0};
    buildFrequencyTable(inputFile, freq);
    Node *root = buildHuffmanTree(freq);
    char *codes[MAX_SIZE] = {0};
    generateCodes(root, "", 0, codes);

    FILE *in = fopen(inputFile, "r");
    FILE *out = fopen(outputFile, "wb");
    if (!in || !out) {
        perror("Error opening files");
        freeTree(root);
        freeCodes(codes);
        exit(1);
    }

    // Write frequency table
    fwrite(freq, sizeof(unsigned), MAX_SIZE, out);

    // Encode file
    int c, bitCount = 0;
    unsigned char buffer = 0;
    while ((c = fgetc(in)) != EOF) {
        char *code = codes[c];
        for (int i = 0; code[i]; i++) {
            buffer = (buffer << 1) | (code[i] - '0');
            bitCount++;
            if (bitCount == 8) {
                fputc(buffer, out);
                bitCount = 0;
                buffer = 0;
            }
        }
    }
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        fputc(buffer, out);
    }

    fclose(in);
    fclose(out);
    freeTree(root);
    freeCodes(codes);
}

// Decompress file
void decompressFile(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "rb");
    FILE *out = fopen(outputFile, "w");
    if (!in || !out) {
        perror("Error opening files");
        exit(1);
    }

    // Read frequency table
    unsigned freq[MAX_SIZE];
    if (fread(freq, sizeof(unsigned), MAX_SIZE, in) != MAX_SIZE) {
        fprintf(stderr, "Invalid compressed file\n");
        fclose(in);
        fclose(out);
        exit(1);
    }

    Node *root = buildHuffmanTree(freq);
    Node *current = root;
    int c, bit;
    while ((c = fgetc(in)) != EOF) {
        for (int i = 7; i >= 0; i--) {
            bit = (c >> i) & 1;
            current = bit ? current->right : current->left;
            if (!current->left && !current->right) {
                fputc(current->data, out);
                current = root;
            }
        }
    }

    fclose(in);
    fclose(out);
    freeTree(root);
}

// Main function with command-line interface
int main(int argc, char *argv[]) {
    if (argc != 4 || (strcmp(argv[1], "-c") != 0 && strcmp(argv[1], "-d") != 0)) {
        printf("Usage: %s [-c|-d] input_file output_file\n", argv[0]);
        printf("  -c: Compress input_file to output_file\n");
        printf("  -d: Decompress input_file to output_file\n");
        return 1;
    }

    if (strcmp(argv[1], "-c") == 0) {
        compressFile(argv[2], argv[3]);
        printf("Compressed %s to %s\n", argv[2], argv[3]);
    } else {
        decompressFile(argv[2], argv[3]);
        printf("Decompressed %s to %s\n", argv[2], argv[3]);
    }
    return 0;
}
