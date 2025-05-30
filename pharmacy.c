#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BATCH_SIZE 50
#define NAME_SIZE 100
#define CONTACT_SIZE 12

typedef struct ExpiryDate {
    int day;
    int month;
    int year;
} ExpiryDate;

typedef struct SupplierData {
    unsigned long Supplier_ID;
    char Supplier_Name[NAME_SIZE];
    unsigned int Quantity_of_stock_bysupplier;
    char Contact[CONTACT_SIZE];
} SupplierData;

typedef struct SupplierInternalNode {
    unsigned long *keys;                       // Array of supplier IDs
    int order;                                 // Maximum number of keys
    int cursize;                               // Current number of keys
    struct SupplierNode **children;            // Pointers to children nodes
    struct SupplierInternalNode *parent;       // Pointer to parent
} SupplierInternalNode;

typedef struct SupplierLeafNode {
    unsigned long *keys;                       // Array of supplier IDs 
    SupplierData *values;                      // Array of supplier data
    int order;                                 // Maximum number of keys
    int cursize;                               // Current number of keys
    struct SupplierLeafNode *next;             // Pointer to next leaf node for sequential access
    struct SupplierLeafNode *prev;             // Pointer to previous leaf node
    struct SupplierInternalNode *parent;       // Pointer to parent
} SupplierLeafNode;

typedef struct SupplierNode {
    bool isLeaf;                               // Flag to identify leaf vs internal node
    union {
        SupplierInternalNode internal;
        SupplierLeafNode leaf;
    };
} SupplierNode;

typedef struct SupplierBPlusTree {
    SupplierNode *root;                        // Root node
    int order;                                 // Order of the tree
    SupplierLeafNode *leftmost_leaf;           // Pointer to leftmost leaf for range queries
} SupplierBPlusTree;

typedef struct MedicationData {
    unsigned long Medication_ID;
    char Medicine_Name[NAME_SIZE];
    unsigned int Quantity_in_stock;
    unsigned int Price_per_Unit;
    int Reorderlevel;

    struct {
        char Batch[BATCH_SIZE];
        ExpiryDate Expiration_Date;
        int Total_sales;
    } Batch_details;

    struct SupplierBPlusTree *Suppliers; 
} MedicationData;

typedef struct MedicationInternalNode {
    unsigned long *keys;                       // Array of medication IDs
    int order;                                 // Maximum number of keys
    int cursize;                               // Current number of keys
    struct MedicationNode **children;          // Pointers to children nodes
    struct MedicationInternalNode *parent;     // Pointer to parent
} MedicationInternalNode;

typedef struct MedicationLeafNode {
    unsigned long *keys;                       // Array of medication IDs
    MedicationData *values;                    // Array of medication data
    int order;                                 // Maximum number of keys
    int cursize;                               // Current number of keys
    struct MedicationLeafNode *next;           // Pointer to next leaf node for sequential access
    struct MedicationLeafNode *prev;           // Pointer to previous leaf node
    struct MedicationInternalNode *parent;     // Pointer to parent
} MedicationLeafNode;

typedef struct MedicationNode {
    bool isLeaf;                               // Flag to identify leaf vs internal node
    union {
        MedicationInternalNode internal;
        MedicationLeafNode leaf;
    };
} MedicationNode;

typedef struct MedicationBPlusTree {
    MedicationNode *root;                      // Root node
    int order;                                 // Order of the tree
    MedicationLeafNode *leftmost_leaf;         // Pointer to leftmost leaf for range queries
} MedicationBPlusTree;

typedef struct UniqueSupplierData {
    unsigned long Supplier_ID;
    char Supplier_Name[NAME_SIZE];
    unsigned int noOfUniqueMedicines; // Number of unique medicines supplied
    unsigned long turnoverProduced;  // Total turnover produced by the supplier
} UniqueSupplierData;

typedef struct UniqueSupplierLeafNode {
    unsigned long *keys;                       // Array of supplier IDs
    UniqueSupplierData *values;                // Array of unique supplier data
    int order;                                 // Maximum number of keys
    int cursize;                               // Current number of keys
    struct UniqueSupplierLeafNode *next;       // Pointer to next leaf node
    struct UniqueSupplierLeafNode *prev;       // Pointer to previous leaf node
    struct UniqueSupplierInternalNode *parent; // Pointer to parent
} UniqueSupplierLeafNode;

typedef struct UniqueSupplierInternalNode {
    unsigned long *keys;                       // Array of supplier IDs
    int order;                                 // Maximum number of keys
    int cursize;                               // Current number of keys
    struct UniqueSupplierNode **children;      // Pointers to children nodes
    struct UniqueSupplierInternalNode *parent; // Pointer to parent
} UniqueSupplierInternalNode;

typedef struct UniqueSupplierNode {
    bool isLeaf; // Flag to identify leaf vs internal node
    union {
        UniqueSupplierInternalNode internal;
        UniqueSupplierLeafNode leaf;
    };
} UniqueSupplierNode;

typedef struct UniqueSupplierBPlusTree {
    UniqueSupplierNode *root;                  // Root node
    int order;                                 // Order of the tree
    UniqueSupplierLeafNode *leftmost_leaf;     // Pointer to leftmost leaf
} UniqueSupplierBPlusTree;

typedef struct HeapNode {
    UniqueSupplierData data; 
} HeapNode;

typedef struct ExpirationIndexData {
    unsigned long long int expirationKey; // Expiration date in YYYYMMDD format
    unsigned long medicationID; // Medication ID
    char medicineName[NAME_SIZE]; // Medication name
} ExpirationIndexData;

typedef struct ExpirationLeafNode {
    unsigned long long int* keys; // Array of expiration keys
    ExpirationIndexData* values; // Array of pointers to medication records
    int order; // Maximum number of keys
    int cursize; // Current number of keys
    struct ExpirationLeafNode* next; // Pointer to the next leaf node
    struct ExpirationLeafNode* prev; // Pointer to the previous leaf node
    struct ExpirationInternalNode* parent; // Pointer to the parent node
} ExpirationLeafNode;

typedef struct ExpirationInternalNode {
    unsigned long long int* keys; // Array of expiration keys
    int order; // Maximum number of keys
    int cursize; // Current number of keys
    struct ExpirationNode** children; // Pointers to child nodes
    struct ExpirationInternalNode* parent; // Pointer to the parent node
} ExpirationInternalNode;

typedef struct ExpirationNode {
    bool isLeaf; // Flag to identify leaf vs internal node
    union {
        ExpirationInternalNode internal;
        ExpirationLeafNode leaf;
    };
} ExpirationNode;

typedef struct ExpirationBPlusTree {
    ExpirationNode* root; // Root node
    int order; // Order of the tree
    ExpirationLeafNode* leftmost_leaf; // Pointer to the leftmost leaf node
} ExpirationBPlusTree;

//=====================================================================================================================



MedicationNode* findLeafNode(MedicationBPlusTree *tree, unsigned long key);
MedicationNode* splitLeafNode(MedicationLeafNode *leaf, unsigned long *midKey);
MedicationNode* splitInternalNode(MedicationInternalNode *node, unsigned long *midKey);
bool insertIntoLeaf(MedicationLeafNode *leaf, unsigned long key, MedicationData data);
void insertIntoInternalNode(MedicationInternalNode *node, unsigned long key, MedicationNode *left, MedicationNode *right);
bool insertMedication(MedicationBPlusTree *tree, MedicationData data) ;
bool CheckMedicIdExist(unsigned long newID, MedicationBPlusTree *tree) ;
MedicationData createMedicationData(int order, MedicationBPlusTree *tree) ;
MedicationNode* createMedicationNode(int order, bool isLeaf) ;
MedicationBPlusTree* createMedicationBPlusTree(int order);


void printTreeStructure(MedicationNode *node, int level);
void printBPlusTree(MedicationBPlusTree* tree);

SupplierData initSupplier(SupplierBPlusTree *supplierTree, MedicationData *medicine);
bool checkSuppID(unsigned long id, SupplierBPlusTree *tree);
SupplierNode* splitSupplierLeafNode(SupplierLeafNode *leaf,unsigned long *midKey) ;
SupplierNode* splitSupplierInternalNode( SupplierInternalNode *node,unsigned long *midKey) ;
SupplierNode* createSupplierNode(int order, bool isLeaf);
SupplierBPlusTree* createSupplierBPlusTree(int order);
bool insertSupplier(SupplierBPlusTree *tree, SupplierData data) ;
SupplierNode* findSupplierLeafNode(SupplierBPlusTree *tree, unsigned long key); 


ExpirationNode* createExpirationNode(int order, bool isLeaf);
ExpirationBPlusTree* createExpirationBPlusTree(int order);
ExpirationNode* splitInternalNodeForExpiry(ExpirationInternalNode* node, int* midKey);
void insertIntoInternalNodeForExpiry(ExpirationInternalNode* node, int key, ExpirationNode* left, ExpirationNode* right);
void insertIntoLeafForExpiry(ExpirationLeafNode* leaf, int expirationKey, unsigned long medicationID, const char* medicineName);
ExpirationNode* findLeafNodeForExpiry(ExpirationBPlusTree* tree, int expirationKey);
ExpirationNode* splitLeafNodeForExpiry(ExpirationLeafNode* leaf, int* midKey);
void insertIntoParentForExpiry(ExpirationBPlusTree* tree, ExpirationNode* leftNode, int midKey, ExpirationNode* rightNode);
void insertIntoExpirationTree(ExpirationBPlusTree* tree, unsigned long long int expirationKey, unsigned long medicationID, const char* medicineName);


bool checkUniqueSupplierID(unsigned long id, UniqueSupplierBPlusTree* tree);
UniqueSupplierNode* findUniqueSupplierLeafNode(UniqueSupplierBPlusTree* tree, unsigned long supplierID);
void deleteUniqueSupplier(UniqueSupplierBPlusTree* tree, unsigned long supplierID);
void updateUniqueSupplierTreeAfterInsert(unsigned long supplierID, const char *supplierName, unsigned int quantity, unsigned long turnover);
void updateUniqueSupplierTreeAfterDelete(unsigned long supplierID, unsigned int quantity, unsigned long turnover);
bool checkUniqueSupplier(unsigned long id);
void printUniqueSuppliers(UniqueSupplierBPlusTree* tree);



UniqueSupplierBPlusTree *uniqueSupplierTree = NULL;
ExpirationBPlusTree* expirationTree = NULL;
int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//==============================================================================

void printExpirationTreeStructure(ExpirationNode *node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");

    if (node->isLeaf) {
        printf("Leaf Node (Level %d): ", level);
        for (int i = 0; i < node->leaf.cursize; i++) {
            printf("%d ", node->leaf.keys[i]);
        }
        printf("\n");
    } else {
        printf("Internal Node (Level %d): ", level);
        for (int i = 0; i < node->internal.cursize; i++) {
            printf("%d ", node->internal.keys[i]);
        }
        printf("\n");
        for (int i = 0; i <= node->internal.cursize; i++) {
            printExpirationTreeStructure(node->internal.children[i], level + 1);
        }
    }
}

void printExpirationBPlusTree(ExpirationBPlusTree* tree) {
    if (!tree || !tree->root) {
        printf("The expiration B+ tree is empty.\n");
        return;
    }

    printf("\n======================================================\n");
    printf("           EXPIRATION B+ TREE (Sorted by Expiration Date) \n");
    printf("======================================================\n");

    // Start at the leftmost leaf
    ExpirationLeafNode* current = tree->leftmost_leaf;

    // Traverse all leaf nodes
    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            printf("\nExpiration Key: %d\n", current->keys[i]);
            printf("Medication ID: %lu\n", current->values[i].medicationID);
            printf("Name: %s\n", current->values[i].medicineName);
            printf("----------------------------------------\n");
        }
        current = current->next; // Move to the next leaf node
    }

    printf("======================================================\n");
}

//--====================================================================================================================================================

ExpirationNode* splitInternalNodeForExpiry(ExpirationInternalNode* node, int* midKey) {
    int median = node->order / 2;
    *midKey = node->keys[median];

    ExpirationNode* newNode = createExpirationNode(node->order, false);
    ExpirationInternalNode* newInternal = &(newNode->internal);

    // Copy the second half of keys and children to the new node
    for (int i = median + 1; i < node->cursize; i++) {
        newInternal->keys[i - (median + 1)] = node->keys[i];
        newInternal->children[i - (median + 1)] = node->children[i];
        if (node->children[i]->isLeaf) {
            node->children[i]->leaf.parent = newInternal;
        } else {
            node->children[i]->internal.parent = newInternal;
        }
        newInternal->cursize++;
    }

    // Copy the rightmost child
    newInternal->children[newInternal->cursize] = node->children[node->cursize];
    if (node->children[node->cursize]->isLeaf) {
        node->children[node->cursize]->leaf.parent = newInternal;
    } else {
        node->children[node->cursize]->internal.parent = newInternal;
    }

    // Update the current size of the original node
    node->cursize = median;

    // Set parent pointer
    newInternal->parent = node->parent;



    return newNode;
}

void insertIntoInternalNodeForExpiry(ExpirationInternalNode* node, int key, ExpirationNode* left, ExpirationNode* right) {
    // Find the position to insert
    int pos = 0;
    while (pos < node->cursize && node->keys[pos] < key) {
        pos++;
    }

    // Shift existing keys and children
    for (int i = node->cursize; i > pos; i--) {
        node->keys[i] = node->keys[i - 1];
        node->children[i + 1] = node->children[i];
    }

    // Insert the new key and children
    node->keys[pos] = key;
    node->children[pos] = left;
    node->children[pos + 1] = right;
    node->cursize++;

    if (left->isLeaf) {
        left->leaf.parent = node;
    } else {
        left->internal.parent = node;
    }

    if (right->isLeaf) {
        right->leaf.parent = node;
    } else {
        right->internal.parent = node;
    }

}

void insertIntoLeafForExpiry(ExpirationLeafNode* leaf, int expirationKey, unsigned long medicationID, const char* medicineName) {
    // Find the position to insert
    int pos = 0;
    while (pos < leaf->cursize && leaf->keys[pos] < expirationKey) {
        pos++;
    }

    // Shift existing keys and values
    for (int i = leaf->cursize; i > pos; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->values[i] = leaf->values[i - 1];
    }

    // Insert the new key and value
    leaf->keys[pos] = expirationKey;
    leaf->values[pos].expirationKey = expirationKey;
    leaf->values[pos].medicationID = medicationID;
    strcpy(leaf->values[pos].medicineName, medicineName);
    leaf->cursize++;
}

ExpirationNode* findLeafNodeForExpiry(ExpirationBPlusTree* tree, int expirationKey) {
    if (!tree || !tree->root) {
        return NULL; // Tree is empty
    }

    ExpirationNode* current = tree->root;

    // Traverse down to the leaf node
    while (!current->isLeaf) {
        ExpirationInternalNode* internal = &(current->internal);
        int i;

        // Find the child to traverse
        for (i = 0; i < internal->cursize; i++) {
            if (expirationKey < internal->keys[i]) {
                break;
            }
        }

        current = internal->children[i];
    }

    // Return the leaf node
    return current;
}

ExpirationNode* createExpirationNode(int order, bool isLeaf) {

    ExpirationNode* node = (ExpirationNode*)malloc(sizeof(ExpirationNode));
    if (!node) return NULL;

    node->isLeaf = isLeaf;

    if (isLeaf) {
        node->leaf.keys = (unsigned long long int*)calloc(order ,sizeof(unsigned long long int));
        node->leaf.values = (ExpirationIndexData*)calloc(order,sizeof(ExpirationIndexData));
        node->leaf.order = order;
        node->leaf.cursize = 0;
        node->leaf.next = NULL;
        node->leaf.prev = NULL;
        node->leaf.parent = NULL;
    } else {
        node->internal.keys = (unsigned long long int*)calloc(order ,sizeof(unsigned long long int));
        node->internal.children = (ExpirationNode**)calloc(order+1 , sizeof(ExpirationNode*));
        node->internal.order = order;
        node->internal.cursize = 0;
        node->internal.parent = NULL;
    }

    return node;
}

ExpirationNode* splitLeafNodeForExpiry(ExpirationLeafNode* leaf, int* midKey) {
    int median = leaf->order / 2;

    // Create a new leaf node
    ExpirationNode* newNode = createExpirationNode(leaf->order, true);
    ExpirationLeafNode* newLeaf = &(newNode->leaf);

    // Copy the second half of keys and values to the new leaf
    for (int i = median; i < leaf->cursize; i++) {
        newLeaf->keys[i - median] = leaf->keys[i];
        newLeaf->values[i - median] = leaf->values[i];
        newLeaf->cursize++;
    }

    leaf->cursize = median;
    newLeaf->next = leaf->next;

    if (newLeaf->next != NULL) {
        newLeaf->next->prev = newLeaf;
    }
    leaf->next = newLeaf;
    newLeaf->prev = leaf;

    *midKey = newLeaf->keys[0];
    newLeaf->parent = leaf->parent;

    return newNode;
}

void insertIntoParentForExpiry(ExpirationBPlusTree* tree, ExpirationNode* leftNode, int midKey, ExpirationNode* rightNode) {
    // Case 1: leaf is the root (has no parent)
    if ((leftNode->isLeaf && !leftNode->leaf.parent) || (!leftNode->isLeaf && !leftNode->internal.parent)) {
        // Create a new root
        ExpirationNode* newRoot = createExpirationNode(tree->order, false);
        if (!newRoot) return;

        tree->root = newRoot;

        newRoot->internal.keys[0] = midKey;
        newRoot->internal.children[0] = leftNode;
        newRoot->internal.children[1] = rightNode;
        newRoot->internal.cursize = 1;

        // Update parent pointers of children
        if (leftNode->isLeaf) {
            leftNode->leaf.parent = &(newRoot->internal);
        } else {
            leftNode->internal.parent = &(newRoot->internal);
        }

        if (rightNode->isLeaf) {
            rightNode->leaf.parent = &(newRoot->internal);
        } else {
            rightNode->internal.parent = &(newRoot->internal);
        }

        printf("Created new root with key: %d\n", midKey);
        return;
    }

    // Get the parent internal node
    ExpirationInternalNode* parent;
    if (leftNode->isLeaf) {
        parent = leftNode->leaf.parent;
    } else {
        parent = leftNode->internal.parent;
    }

    // Create a container node for the parent (for easier handling)
    ExpirationNode parentContainer;
    parentContainer.isLeaf = false;
    parentContainer.internal = *parent;

    // Check if parent has space for a new key
    if (parent->cursize < parent->order - 1) {
        // Insert the new key and child into parent
        int i = parent->cursize - 1;
        while (i >= 0 && parent->keys[i] > midKey) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children[i + 2] = parent->children[i + 1];
            i--;
        }

        parent->keys[i + 1] = midKey;
        parent->children[i + 2] = rightNode;
        parent->cursize++;

        // Update parent pointer of the right child
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = parent;
        } else {
            rightNode->internal.parent = parent;
        }
        printf("Inserted key %d into existing parent\n", midKey);
        return;
    }

    // Parent is full, need to split
    int newMidKey;
    ExpirationNode* newParentNode = splitInternalNodeForExpiry(parent, &newMidKey);

    // Decide which parent node should contain our key
    if (midKey > newMidKey) {
        // Insert into right parent
        ExpirationInternalNode* rightParent = &(newParentNode->internal);
        int i = rightParent->cursize - 1;
        while (i >= 0 && rightParent->keys[i] > midKey) {
            rightParent->keys[i + 1] = rightParent->keys[i];
            rightParent->children[i + 2] = rightParent->children[i + 1];
            i--;
        }

        rightParent->keys[i + 1] = midKey;
        rightParent->children[i + 2] = rightNode;
        rightParent->cursize++;

        // Update parent pointer
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = rightParent;
        } else {
            rightNode->internal.parent = rightParent;
        }
    } else {
        // Insert into original parent
        int i = parent->cursize - 1;
        while (i >= 0 && parent->keys[i] > midKey) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children[i + 2] = parent->children[i + 1];
            i--;
        }

        parent->keys[i + 1] = midKey;
        parent->children[i + 2] = rightNode;
        parent->cursize++;

        // Update parent pointer
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = parent;
        } else {
            rightNode->internal.parent = parent;
        }
    }
    ExpirationNode* parentContainer2 = (ExpirationNode*)malloc(sizeof(ExpirationNode));
    
    parentContainer2->isLeaf = false;
    parentContainer2->internal = *parent;
    insertIntoParentForExpiry(tree, parentContainer2, newMidKey, newParentNode);

}
    
void insertIntoExpirationTree(ExpirationBPlusTree* tree, unsigned long long int expirationKey, unsigned long medicationID, const char* medicineName) {
    if (!tree) {
        printf("Error: ExpirationBPlusTree is not initialized.\n");
        return;
    }

    // If the tree is empty, create the first leaf node
    if (!tree->root) {
        ExpirationNode* firstNode = createExpirationNode(tree->order, true);
        firstNode->leaf.keys[0] = expirationKey;
        firstNode->leaf.values[0].expirationKey = expirationKey;
        firstNode->leaf.values[0].medicationID = medicationID;
        strcpy(firstNode->leaf.values[0].medicineName, medicineName);
        firstNode->leaf.cursize = 1;

        tree->root = firstNode;
        tree->leftmost_leaf = &(firstNode->leaf);
        return;
    }

    // Find the appropriate leaf node
    ExpirationNode* leaf2 = findLeafNodeForExpiry(tree, expirationKey);
    ExpirationLeafNode* leaf = &(leaf2->leaf);

    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == expirationKey) {
            // Update the existing value
            leaf->values[i].medicationID = medicationID;
            strcpy(leaf->values[i].medicineName, medicineName);
            printf("Updated existing key: %lu\n", expirationKey);
            return;
        }
    }

    if (leaf->cursize < leaf->order - 1) {
        insertIntoLeafForExpiry(leaf, expirationKey, medicationID, medicineName);
        return;
    }

    // If the leaf is full, split it
    int midKey;
    ExpirationNode* newLeaf = splitLeafNodeForExpiry(leaf, &midKey);

    // Determine which leaf to insert into
    if (expirationKey >= midKey) {
        insertIntoLeafForExpiry(&(newLeaf->leaf), expirationKey, medicationID, medicineName);
    } else {
        insertIntoLeafForExpiry(leaf, expirationKey, medicationID, medicineName);
    }

   insertIntoParentForExpiry(tree, leaf2, midKey, newLeaf);
   return;
}

ExpirationBPlusTree* createExpirationBPlusTree(int order) {
    ExpirationBPlusTree* tree = (ExpirationBPlusTree*)malloc(sizeof(ExpirationBPlusTree));
    if (!tree){ 
        printf("Error: Memory allocation failed for ExpirationBPlusTree.\n");
        return NULL;
    }

    tree->order = order;
    tree->root = NULL;
    tree->leftmost_leaf = NULL;
    return tree;
}

//=========================================================================================

void printUniqueSuppliers(UniqueSupplierBPlusTree* tree) {
    if (!tree || !tree->root) {
        printf("The Unique Supplier B+ Tree is empty.\n");
        return;
    }

    printf("\n======================================================\n");
    printf("           UNIQUE SUPPLIERS B+ TREE                   \n");
    printf("======================================================\n");

    // Start at the leftmost leaf
    UniqueSupplierLeafNode* current = tree->leftmost_leaf;

    // Traverse all leaf nodes
    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            printf("Supplier ID: %lu\n", current->keys[i]);
            printf("  Name: %s\n", current->values[i].Supplier_Name);
            printf("  Number of Unique Medicines: %u\n", current->values[i].noOfUniqueMedicines);
            printf("  Total Turnover: %lu\n", current->values[i].turnoverProduced);
            printf("----------------------------------------\n");
        }
        current = current->next; // Move to the next leaf node
    }

    printf("======================================================\n");
}



bool checkUniqueSupplier(unsigned long id) {
    if (!uniqueSupplierTree || !uniqueSupplierTree->root){ 
        printf("Error: UniqueSupplierBPlusTree is not initialized.\n");
        return true; 
    }

    UniqueSupplierNode* leaf2 = findUniqueSupplierLeafNode(uniqueSupplierTree, id);
    
    UniqueSupplierLeafNode* leaf = &(leaf2->leaf);

    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == id) {
            return false; 
        }
    }
    return true; 
}

UniqueSupplierBPlusTree* createUniqueSupplierBPlusTree(int order) {
    UniqueSupplierBPlusTree* tree = (UniqueSupplierBPlusTree*)malloc(sizeof(UniqueSupplierBPlusTree));
    if (!tree) return NULL;

    tree->order = order;
    tree->root = NULL;
    tree->leftmost_leaf = NULL;
    return tree;
}

UniqueSupplierNode* createUniqueSupplierNode(int order, bool isLeaf) {
    UniqueSupplierNode* node = (UniqueSupplierNode*)malloc(sizeof(UniqueSupplierNode));
    if (!node) return NULL;

    node->isLeaf = isLeaf;

    if (isLeaf) {
        node->leaf.keys = (unsigned long*)malloc(sizeof(unsigned long) * order);
        node->leaf.values = (UniqueSupplierData*)malloc(sizeof(UniqueSupplierData) * order);
        node->leaf.order = order;
        node->leaf.cursize = 0;
        node->leaf.next = NULL;
        node->leaf.prev = NULL;
        node->leaf.parent = NULL;
    } else {
        node->internal.keys = (unsigned long*)malloc(sizeof(unsigned long) * order);
        node->internal.children = (UniqueSupplierNode**)malloc(sizeof(UniqueSupplierNode*) * (order + 1));
        node->internal.order = order;
        node->internal.cursize = 0;
        node->internal.parent = NULL;
    }

    return node;
}
bool insertIntoUniqueSupplierLeaf(UniqueSupplierLeafNode* leaf, unsigned long key, UniqueSupplierData data) {
    if (!leaf ) return false;

    // Find the position to insert
    int pos = 0;
    while (pos < (leaf)->cursize && (leaf)->keys[pos] < key) {
        pos++;
    }

    // Shift existing keys and values
    for (int i = (leaf)->cursize; i > pos; i--) {
        (leaf)->keys[i] = (leaf)->keys[i - 1];
        (leaf)->values[i] = (leaf)->values[i - 1];
    }

    // Insert the new key and value
    (leaf)->keys[pos] = key;
    (leaf)->values[pos] = data;
    (leaf)->cursize++;

    return true;
}

UniqueSupplierNode* splitUniqueSupplierLeafNode(UniqueSupplierLeafNode *leaf, unsigned long *midKey) {
    int median = leaf->order / 2;

    // Create a new leaf node
    UniqueSupplierNode* newNode = createUniqueSupplierNode(leaf->order, true);
    UniqueSupplierLeafNode *newLeaf = &(newNode->leaf);

    // Copy the second half of keys and values to the new leaf
    for (int i = median; i < leaf->cursize; i++) {
        newLeaf->keys[i - median] = leaf->keys[i];
        newLeaf->values[i - median] = leaf->values[i];
        newLeaf->cursize++;
    }

    // Update the current size of the original leaf
    leaf->cursize = median;

    // Connect the new leaf to the linked list
    newLeaf->next = leaf->next;
    if (newLeaf->next != NULL) {
        newLeaf->next->prev = newLeaf;
    }
    leaf->next = newLeaf;
    newLeaf->prev = leaf;

    // Get the first key in the new leaf for the parent
    *midKey = newLeaf->keys[0];

    // Set parent pointer
    newLeaf->parent = leaf->parent;

    return newNode;
}

UniqueSupplierNode* splitUniqueSupplierInternalNode(UniqueSupplierInternalNode* node, unsigned long* midKey) {
    int median = node->order / 2;
    *midKey = node->keys[median];

    // Create a new internal node
    UniqueSupplierNode* newNode = createUniqueSupplierNode(node->order, false);
    UniqueSupplierInternalNode* newInternal = &(newNode->internal);

    for (int i = median + 1; i < node->cursize; i++) {
        newInternal->keys[i - (median + 1)] = node->keys[i];
        newInternal->children[i - (median + 1)] = node->children[i];
        newInternal->cursize++;

        // Update parent pointers in children
        if (node->children[i]->isLeaf) {
            node->children[i]->leaf.parent = newInternal;
        } else {
            node->children[i]->internal.parent = newInternal;
        }
    }

    // Copy the rightmost child
    newInternal->children[newInternal->cursize] = node->children[node->cursize];
    if (node->children[node->cursize]->isLeaf) {
        node->children[node->cursize]->leaf.parent = newInternal;
    } else {
        node->children[node->cursize]->internal.parent = newInternal;
    }

    // Update the current size of the original node
    node->cursize = median;

    newInternal->parent = node->parent;

    return newNode;
}

void insertIntoUniqueSupplierParent(UniqueSupplierBPlusTree* tree, UniqueSupplierNode* leftNode, unsigned long midKey, UniqueSupplierNode* rightNode) {
    if ((leftNode->isLeaf && !leftNode->leaf.parent) || (!leftNode->isLeaf && !leftNode->internal.parent)) {
        // printf("[DEBUG] Creating a new root for the UniqueSupplierBPlusTree.\n");

        UniqueSupplierNode* newRoot = createUniqueSupplierNode(tree->order, false);
        if (!newRoot) {
            printf("[ERROR] Memory allocation failed for new root.\n");
            return;
        }

        tree->root = newRoot;

        newRoot->internal.keys[0] = midKey;
        newRoot->internal.children[0] = leftNode;
        newRoot->internal.children[1] = rightNode;
        newRoot->internal.cursize = 1;

        if (leftNode->isLeaf) {
            leftNode->leaf.parent = &(newRoot->internal);
        } else {
            leftNode->internal.parent = &(newRoot->internal);
        }
        
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = &(newRoot->internal);
        } else {
            rightNode->internal.parent = &(newRoot->internal);
        }

        return;
    }

    UniqueSupplierInternalNode* parent ; 
    if (leftNode->isLeaf) {
        parent = leftNode->leaf.parent;
    } else {
        parent = leftNode->internal.parent;
    }

    UniqueSupplierNode parentNode;
    parentNode.isLeaf = false;
    parentNode.internal = *parent;

    if (parent->cursize < parent->order - 1) {
        int i = parent->cursize - 1;
        while (i >= 0 && parent->keys[i] > midKey) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children[i + 2] = parent->children[i + 1];
            i--;
        }

        parent->keys[i + 1] = midKey;
        parent->children[i + 2] = rightNode;
        parent->cursize++;

        if (rightNode->isLeaf) {
            rightNode->leaf.parent = parent;
        } else {
            rightNode->internal.parent = parent;
        }
        return;
    }

    unsigned long newMidKey;
    UniqueSupplierNode* newParentNode = splitUniqueSupplierInternalNode(parent, &newMidKey);

    if (midKey > newMidKey) {
        UniqueSupplierInternalNode* rightParent = &(newParentNode->internal);
        int i = rightParent->cursize - 1;
        while (i >= 0 && rightParent->keys[i] > midKey) {
            rightParent->keys[i + 1] = rightParent->keys[i];
            rightParent->children[i + 2] = rightParent->children[i + 1];
            i--;
        }

        rightParent->keys[i + 1] = midKey;
        rightParent->children[i + 2] = rightNode;
        rightParent->cursize++;

        if (rightNode->isLeaf) {
            rightNode->leaf.parent = rightParent;
        } else {
            rightNode->internal.parent = rightParent;
        }
    } else {
        int i = parent->cursize - 1;
        while (i >= 0 && parent->keys[i] > midKey) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children[i + 2] = parent->children[i + 1];
            i--;
        }

        parent->keys[i + 1] = midKey;
        parent->children[i + 2] = rightNode;
        parent->cursize++;

        if (rightNode->isLeaf) {
            rightNode->leaf.parent = parent;
        } else {
            rightNode->internal.parent = parent;
        }
    }

    UniqueSupplierNode* parentContainer2 = (UniqueSupplierNode*)malloc(sizeof(UniqueSupplierNode));
    parentContainer2->isLeaf = false;
    parentContainer2->internal = *parent;

    insertIntoUniqueSupplierParent(tree, parentContainer2, newMidKey, newParentNode);
}

UniqueSupplierNode* findUniqueSupplierLeafNode(UniqueSupplierBPlusTree* tree, unsigned long supplierID) {
    if (!tree || !tree->root) return NULL;

    UniqueSupplierNode* current = tree->root;

    // Traverse down to the leaf node
    while (!current->isLeaf) {
        int i;
        for (i = 0; i < current->internal.cursize; i++) {
            if (supplierID < current->internal.keys[i]) break;
        }
        current = current->internal.children[i];
    }

    return current;
}

bool insertUniqueSupplier(UniqueSupplierBPlusTree* tree, unsigned long supplierID, const char* supplierName, unsigned int noOfUniqueMedicines, unsigned long turnover) {

    if (!tree) return false;

    unsigned long id = supplierID;

    UniqueSupplierData data;
    data.Supplier_ID = supplierID;
    strcpy(data.Supplier_Name, supplierName);
    data.noOfUniqueMedicines = noOfUniqueMedicines;
    data.turnoverProduced = turnover;


    if (!tree->root) {
        UniqueSupplierNode *firstNode = createUniqueSupplierNode(tree->order, true);
        
        // Insert the key and value
        firstNode->leaf.keys[0] = id;
        firstNode->leaf.values[0] = data;
        firstNode->leaf.cursize = 1;

        tree->root = firstNode;
        tree->leftmost_leaf = &(firstNode->leaf);
        return true;
    }
    

    UniqueSupplierNode *leaf2 = findUniqueSupplierLeafNode(tree, id);
    UniqueSupplierLeafNode *leaf = &(leaf2->leaf);
    if (!leaf) return false;


    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == id) {
            leaf->values[i].noOfUniqueMedicines += 1;
            leaf->values[i].turnoverProduced += turnover;
            return true;
        }
    }

    if (leaf->cursize < leaf->order - 1) {
        bool result = insertIntoUniqueSupplierLeaf(leaf, id, data);
        return result;
    }

    unsigned long midKey;
    UniqueSupplierNode* newLeafNode = splitUniqueSupplierLeafNode(leaf, &midKey);
    if(!newLeafNode) return false;

    if(id >= midKey) {
        UniqueSupplierLeafNode* newLeaf = &(newLeafNode->leaf);
        bool result = insertIntoUniqueSupplierLeaf(newLeaf, id, data);
        if(!result) return false;
    } else {
        // Insert into the original leaf node
        bool result = insertIntoUniqueSupplierLeaf(leaf, id, data);
        if(!result) return false;
    }

    insertIntoUniqueSupplierParent(tree, leaf2, midKey, newLeafNode);
    return true;
}


// Function to delete a supplier from the UniqueSupplierBPlusTree
void deleteUniqueSupplier(UniqueSupplierBPlusTree* tree, unsigned long supplierID) {
    if (!tree || !tree->root) return;

    // Find the appropriate leaf node
    UniqueSupplierNode* leaf2 = findUniqueSupplierLeafNode(tree, supplierID);
    UniqueSupplierLeafNode* leaf = &(leaf2->leaf);
    if (!leaf) return;

    // Find the supplier in the leaf node
    int pos = -1;
    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == supplierID) {
            pos = i;
            break;
        }
    }

    if (pos == -1) return; // Supplier not found

    // Shift keys and values to remove the supplier
    for (int i = pos; i < leaf->cursize - 1; i++) {
        leaf->keys[i] = leaf->keys[i + 1];
        leaf->values[i] = leaf->values[i + 1];
    }
    leaf->cursize--;

    // If the leaf is empty and is the root, free the root
    if (leaf->cursize == 0 && leaf->parent == NULL) {
        free(leaf->keys);
        free(leaf->values);
        free(tree->root);
        tree->root = NULL;
        tree->leftmost_leaf = NULL;
    }
}

void updateUniqueSupplierTreeAfterInsert(unsigned long supplierID, const char *supplierName, unsigned int quantity, unsigned long turnover) {
    if (!uniqueSupplierTree) {
        printf("Error: UniqueSupplierBPlusTree is not initialized.\n");
        return;
    }

    // Insert or update the unique supplier in the tree
    insertUniqueSupplier(uniqueSupplierTree, supplierID, supplierName, 1, turnover);
}

// Function to update the UniqueSupplierBPlusTree after deleting a supplier
void updateUniqueSupplierTreeAfterDelete(unsigned long supplierID, unsigned int quantity, unsigned long turnover) {
    if (!uniqueSupplierTree) {
        printf("Error: UniqueSupplierBPlusTree is not initialized.\n");
        return;
    }

    // Find the supplier in the unique supplier tree
    UniqueSupplierNode *leaf2 = findUniqueSupplierLeafNode(uniqueSupplierTree, supplierID);
    UniqueSupplierLeafNode *leaf = &(leaf2->leaf);
    if (!leaf) return;

    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == supplierID) {
            // Decrease the turnover and unique medicine count
            leaf->values[i].turnoverProduced -= turnover;
            leaf->values[i].noOfUniqueMedicines--;

            // If the supplier no longer supplies any medicines, remove it from the tree
            if (leaf->values[i].noOfUniqueMedicines == 0) {
                deleteUniqueSupplier(uniqueSupplierTree, supplierID);
            }
            return;
        }
    }
}


//===========================================================================================

SupplierData initSupplier(SupplierBPlusTree *supplierTree, MedicationData *medicine) {
    SupplierData supplier;
    unsigned long SupID;
    
    do {
        printf("Enter Supplier-ID : \n");
        scanf("%lu", &SupID);
        
        if (!checkSuppID(SupID, supplierTree)) {
            printf("ID %lu already exists. Please enter a different ID.\n", SupID);
        }
    } while (!checkSuppID(SupID, supplierTree));

    supplier.Supplier_ID = SupID;
    
    printf("Enter Supplier name : \n");
    scanf("%s", supplier.Supplier_Name);

    printf("Enter Quantity of medication by supplier : \n");
    scanf("%u", &supplier.Quantity_of_stock_bysupplier);

    medicine->Quantity_in_stock += supplier.Quantity_of_stock_bysupplier;

    printf("Enter Contact details for Supplier (10 digits) : \n");
    scanf("%s", supplier.Contact);
    printf("\n");

    return supplier;
}

SupplierNode* createSupplierNode(int order, bool isLeaf) {
    SupplierNode* node = (SupplierNode*)malloc(sizeof(SupplierNode));
    if (!node) {
        return NULL; // Memory allocation failed
    }
    
    node->isLeaf = isLeaf;
    
    if (isLeaf) {
        // Leaf node setup
        node->leaf.keys = (unsigned long*)malloc(sizeof(unsigned long) * (order - 1));
        node->leaf.values = (SupplierData*)malloc(sizeof(SupplierData) * order);
        node->leaf.order = order;
        node->leaf.cursize = 0;
        node->leaf.next = NULL;
        node->leaf.prev = NULL;
        node->leaf.parent = NULL;
    } else {
        // Internal node setup
        node->internal.keys = (unsigned long*)malloc(sizeof(unsigned long) *( order -1));
        node->internal.children = (SupplierNode**)malloc(sizeof(SupplierNode*) * (order));
        node->internal.order = order;
        node->internal.cursize = 0;
        node->internal.parent = NULL;
    }
    
    return node;
}

SupplierBPlusTree* createSupplierBPlusTree(int order) {
    SupplierBPlusTree* tree = (SupplierBPlusTree*)malloc(sizeof(SupplierBPlusTree));
    if (!tree) return NULL;

    tree->order = order;
    tree->root = NULL;
    tree->leftmost_leaf = NULL;
    return tree;    
}

bool checkSuppID(unsigned long id, SupplierBPlusTree *tree) {
    if (!tree || !tree->root) {
        return true; // ID doesn't exist in an empty tree
    }
    
    SupplierNode *leaf2 = findSupplierLeafNode(tree, id);
    SupplierLeafNode *leaf = &(leaf2->leaf);
    
    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == id) {
            return false; // ID already exists
        }
    }
    
    return true; // ID doesn't exist
}

bool insertIntoSupplierLeaf(SupplierLeafNode *leaf, unsigned long key, SupplierData data) {
    int pos = 0;
    while (pos < leaf->cursize && leaf->keys[pos] < key) {
        pos++;
    }
    
    // Check if key already exists
    if (pos < leaf->cursize && leaf->keys[pos] == key) {
        // Update existing value
        leaf->values[pos] = data;
        return true;
    }
    
    // Shift existing keys and values
    for (int i = leaf->cursize; i > pos; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->values[i] = leaf->values[i - 1];
    }
    
    // Insert new key and value
    leaf->keys[pos] = key;
    leaf->values[pos] = data;
    leaf->cursize++;
    
    return true;
}

void insertIntoSupplierParent(SupplierBPlusTree *tree, SupplierNode *leftNode, unsigned long midKey, SupplierNode *rightNode) {
    // Case 1: leaf is the root (has no parent)
    if ((leftNode->isLeaf && !leftNode->leaf.parent) || (!leftNode->isLeaf && !leftNode->internal.parent)) {
        // Create a new root
        SupplierNode* newRoot = createSupplierNode(tree->order, false);
        if (!newRoot) return;
        
        tree->root = newRoot;
        
        newRoot->internal.keys[0] = midKey;
        newRoot->internal.children[0] = leftNode;
        newRoot->internal.children[1] = rightNode;
        newRoot->internal.cursize = 1;
        
        // Update parent pointers of children
        if (leftNode->isLeaf) {
            leftNode->leaf.parent = &(newRoot->internal);
        } else {
            leftNode->internal.parent = &(newRoot->internal);
        }
        
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = &(newRoot->internal);
        } else {
            rightNode->internal.parent = &(newRoot->internal);
        }
        
        printf("Created new root with key: %lu\n", midKey);
        return;
    }

    // Get the parent internal node
    SupplierInternalNode *parent;
    if (leftNode->isLeaf) {
        parent = leftNode->leaf.parent;
    } else {
        parent = leftNode->internal.parent;
    }
    
    // Create a container node for the parent (for easier handling)
    SupplierNode parentNode;
    parentNode.isLeaf = false;
    parentNode.internal = *parent;
    
    // Check if parent has space for a new key
    if (parent->cursize < parent->order - 1) {
        // Insert the new key and child into parent
        int i = parent->cursize - 1;
        while (i >= 0 && parent->keys[i] > midKey) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children[i + 2] = parent->children[i + 1];
            i--;
        }
        
        parent->keys[i + 1] = midKey;
        parent->children[i + 2] = rightNode;
        parent->cursize++;
        
        // Update parent pointer of the right child
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = parent;
        } else {
            rightNode->internal.parent = parent;
        }
        
        printf("Inserted key %lu into existing parent\n", midKey);
        return;
    }
    
    // Parent is full, need to split
    unsigned long newMidKey;
    SupplierNode *newParentNode = splitSupplierInternalNode(parent, &newMidKey);
    
    // Decide which parent node should contain our key
    if (midKey > newMidKey) {
        // Insert into right parent
        SupplierInternalNode *rightParent = &(newParentNode->internal);
        int i = rightParent->cursize - 1;
        while (i >= 0 && rightParent->keys[i] > midKey) {
            rightParent->keys[i + 1] = rightParent->keys[i];
            rightParent->children[i + 2] = rightParent->children[i + 1];
            i--;
        }
        
        rightParent->keys[i + 1] = midKey;
        rightParent->children[i + 2] = rightNode;
        rightParent->cursize++;
        
        // Update parent pointer
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = rightParent;
        } else {
            rightNode->internal.parent = rightParent;
        }
    } else {
        // Insert into original parent
        int i = parent->cursize - 1;
        while (i >= 0 && parent->keys[i] > midKey) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children[i + 2] = parent->children[i + 1];
            i--;
        }
        
        parent->keys[i + 1] = midKey;
        parent->children[i + 2] = rightNode;
        parent->cursize++;
        
        // Update parent pointer
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = parent;
        } else {
            rightNode->internal.parent = parent;
        }
    }
    
    // Now we need to insert the split parent into its parent
    // Create a container node for the original parent (for recursive call)
    SupplierNode *parentContainer = (SupplierNode*)malloc(sizeof(SupplierNode));
    if (!parentContainer) return;
    
    parentContainer->isLeaf = false;
    parentContainer->internal = *parent;
    
    // Recursive call to insert the split parent
    insertIntoSupplierParent(tree, parentContainer, newMidKey, newParentNode);

}

bool insertSupplier(SupplierBPlusTree *tree, SupplierData data) {
    if (!tree){ 
        printf("Tree is NULL\n");
        return false; // Tree is NULL
    }
    unsigned long id = data.Supplier_ID;
    
    // If tree is empty, create the first leaf node
    if (!tree->root) {
        SupplierNode *firstNode = createSupplierNode(tree->order, true);
        
        // Insert the key and value
        firstNode->leaf.keys[0] = id;
        firstNode->leaf.values[0] = data;
        firstNode->leaf.cursize = 1;

        tree->root = firstNode;
        tree->leftmost_leaf = &(firstNode->leaf);

        return true;
    }
    
    // Find the appropriate leaf node
    SupplierNode *leaf2 = findSupplierLeafNode(tree, id);
    SupplierLeafNode *leaf = &(leaf2->leaf);
    if (!leaf) return false;



    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == id) {
            // Update existing value
            leaf->values[i] = data;
            return true;
        }
    }

    // If leaf is not full, simply insert
    if (leaf->cursize < leaf->order - 1) {
        bool result = insertIntoSupplierLeaf(leaf, id, data);
        return result;
    }

    unsigned long midKey;
    SupplierNode *newLeafNode = splitSupplierLeafNode(leaf, &midKey);
    if (!newLeafNode) return false;

    if (id >= midKey) {
        insertIntoSupplierLeaf(&(newLeafNode->leaf), id, data);
    } else {
        insertIntoSupplierLeaf(leaf, id, data);
    }

    insertIntoSupplierParent(tree, leaf2, midKey, newLeafNode);  
    return true;
}
 
SupplierNode* splitSupplierInternalNode( SupplierInternalNode *node,unsigned long *midKey) {
    int median = node->order / 2;
    *midKey = node->keys[median];
    
    // Create a new internal node
    SupplierNode *newNode = createSupplierNode(node->order, false);
    SupplierInternalNode *newInternal = &(newNode->internal);
    
    // Copy the second half of keys and children to the new node
    for (int i = median + 1; i < node->cursize; i++) {
        newInternal->keys[i - (median + 1)] = node->keys[i];
        newInternal->children[i - (median + 1)] = node->children[i];
        newInternal->cursize++;
        
        // Update parent pointers in children
        if (node->children[i]->isLeaf) {
            node->children[i]->leaf.parent = newInternal;
        } else {
            node->children[i]->internal.parent = newInternal;
        }
    }
    
    // Copy the rightmost child
    newInternal->children[newInternal->cursize] = node->children[node->cursize];
    if (node->children[node->cursize]->isLeaf) {
        node->children[node->cursize]->leaf.parent = newInternal;
    } else {
        node->children[node->cursize]->internal.parent = newInternal;
    }
    
    // Update current node size (excluding the median)
    node->cursize = median;

    newInternal->parent = node->parent;

    return newNode;

}

SupplierNode* splitSupplierLeafNode(SupplierLeafNode *leaf,unsigned long *midKey) {
    int median = leaf->order / 2;
    
    // Create a new leaf node
    SupplierNode *newNode = createSupplierNode(leaf->order, true);

    SupplierLeafNode *newLeaf = &(newNode->leaf);
    
    // Copy the second half of keys and values to the new leaf
    for (int i = median; i < leaf->cursize; i++) {
        newLeaf->keys[i - median] = leaf->keys[i];
        newLeaf->values[i - median] = leaf->values[i];
        newLeaf->cursize++;
    }
    
    // Update the current size of the original leaf
    leaf->cursize = median;
    
    // Connect the new leaf to the linked list
    newLeaf->next = leaf->next;
    if (newLeaf->next != NULL) {
        newLeaf->next->prev = newLeaf;
    }
    leaf->next = newLeaf;
    newLeaf->prev = leaf;

    *midKey = newLeaf->keys[0];
    
    newLeaf->parent = leaf->parent;
    
    return newNode;
}

SupplierNode* findSupplierLeafNode(SupplierBPlusTree *tree, unsigned long key) {
    if (!tree || !tree->root) {
        return NULL;
    }
    
    SupplierNode *current = tree->root;
    
    // Traverse down to leaf node
    while (!current->isLeaf) {
        int i;
        for (i = 0; i < current->internal.cursize; i++) {
            if (key < current->internal.keys[i]) {
                break;
            }
        }
        current = current->internal.children[i];
    }
    
    return current;
}

// ==============================================================================================================

bool CheckMedicIdExist(unsigned long newID, MedicationBPlusTree *tree) {
    if (!tree || !tree->root) {
        return false; // ID doesn't exist in an empty tree
    }
    
    MedicationNode *leaf2 = findLeafNode(tree, newID);
    MedicationLeafNode *leaf = &(leaf2->leaf);
    
    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == newID) {
            return true; // ID already exists
        }
    }
    
    return false; // ID doesn't exist
}

MedicationData createMedicationData(int order, MedicationBPlusTree *tree) {
    MedicationData newMed;
    unsigned long newID;
    
    bool idExists;
    do {
        printf("Enter Medication ID: ");
        scanf("%lu", &newID);

        idExists = CheckMedicIdExist(newID, tree); // Store the result of the check
        if (idExists) {
            printf("ID %lu already exists. Please enter a different ID.\n", newID);
        }
    } while (idExists);

    newMed.Medication_ID = newID;
    
    printf("Enter Medication Name: ");
    scanf("%s", newMed.Medicine_Name);

    printf("Enter Quantity in Stock: ");
    scanf("%u", &newMed.Quantity_in_stock);

    printf("Enter Price per Unit: ");
    scanf("%u", &newMed.Price_per_Unit);

    printf("Enter the Batch Name: ");
    scanf("%s", newMed.Batch_details.Batch);

    printf("Enter Reorder Level: ");
    scanf("%d", &newMed.Reorderlevel);

    printf("Enter Expiration Date in DD-MM-YYYY \n");
    printf("Enter Day: ");
    scanf("%d", &newMed.Batch_details.Expiration_Date.day);
    printf("Enter Month: ");
    scanf("%d", &newMed.Batch_details.Expiration_Date.month);
    printf("Enter Year: ");
    scanf("%d", &newMed.Batch_details.Expiration_Date.year);

    // Initialize total sales
    newMed.Batch_details.Total_sales = 0;
    
    // Create a B+ tree for suppliers
    newMed.Suppliers = createSupplierBPlusTree(order);
    
    int nS;
    printf("Enter the number of suppliers: ");
    scanf("%d", &nS);

    for (int i = 0; i < nS; i++) {
        printf("Enter the supplier %d details\n\n", i+1);
        SupplierData suppl = initSupplier(newMed.Suppliers, &newMed);
        insertSupplier(newMed.Suppliers, suppl);
    }

    unsigned long long int expirationKey = newMed.Batch_details.Expiration_Date.year * 10000 +
    newMed.Batch_details.Expiration_Date.month * 100 +
    newMed.Batch_details.Expiration_Date.day + 100000000*newID;

    insertIntoExpirationTree(expirationTree, expirationKey, newMed.Medication_ID, newMed.Medicine_Name);

    return newMed;
}

MedicationNode* createMedicationNode(int order, bool isLeaf) {

    MedicationNode* node = (MedicationNode*)malloc(sizeof(MedicationNode));
    if (!node) return NULL;
    
    node->isLeaf = isLeaf;
    
    if (isLeaf) {
        // Leaf node setup
        node->leaf.keys = (unsigned long*)malloc(sizeof(unsigned long) * order);
        node->leaf.values = (MedicationData*)malloc(sizeof(MedicationData) * order);
        node->leaf.order = order;
        node->leaf.cursize = 0;
        node->leaf.next = NULL;
        node->leaf.prev = NULL;
        node->leaf.parent = NULL;
    } else {
        // Internal node setup
        node->internal.keys = (unsigned long*)malloc(sizeof(unsigned long) * order);
        node->internal.children = (MedicationNode**)malloc(sizeof(MedicationNode*) * (order + 1));
        node->internal.order = order;
        node->internal.cursize = 0;
        node->internal.parent = NULL;
    }
    
    return node;    
}

MedicationBPlusTree* createMedicationBPlusTree(int order) {
    MedicationBPlusTree* tree = (MedicationBPlusTree*)malloc(sizeof(MedicationBPlusTree));
    if (!tree) return NULL;

    tree->order = order;
    tree->root = NULL;
    tree->leftmost_leaf = NULL;
    return tree;
}

MedicationNode* findLeafNode(MedicationBPlusTree *tree, unsigned long key) {
    if (!tree || !tree->root) {
        printf("Tree is empty.\n");
        return NULL;
    }
    
    MedicationNode *current = tree->root;
    
    // Traverse down to leaf node
    while (!current->isLeaf) {
        int i;
        for (i = 0; i < current->internal.cursize; i++) {
            if (key < current->internal.keys[i]) {
                break;
            }
        }
        current = current->internal.children[i];
    }
    
    return current;
}

bool insertIntoLeaf(MedicationLeafNode *leaf, unsigned long key, MedicationData data) {
    // Find position to insert
    int pos = 0;
    while (pos < leaf->cursize && leaf->keys[pos] < key) {
        pos++;
    }
    
    // Check if key already exists
    if (pos < leaf->cursize && leaf->keys[pos] == key) {
        // Update existing value
        leaf->values[pos] = data;
        return true;
    }
    
    // Shift existing keys and values
    for (int i = leaf->cursize; i > pos; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->values[i] = leaf->values[i - 1];
    }
    
    // Insert new key and value
    leaf->keys[pos] = key;
    leaf->values[pos] = data;
    leaf->cursize++;
    
    return true;
}

void insertIntoInternalNode(MedicationInternalNode *node, unsigned long key, MedicationNode *left, MedicationNode *right) {
    // Find position to insert
    int pos = 0;
    while (pos < node->cursize && node->keys[pos] < key) {
        pos++;
    }
    
    // Shift existing keys and children
    for (int i = node->cursize; i > pos; i--) {
        node->keys[i] = node->keys[i - 1];
        node->children[i + 1] = node->children[i];
    }
    
    // Insert new key and child
    node->keys[pos] = key;
    node->children[pos] = left;
    node->children[pos + 1] = right;
    node->cursize++;
    
    // Update parent pointers
    if (left->isLeaf) {
        left->leaf.parent = node;
    } else {
        left->internal.parent = node;
    }
    
    if (right->isLeaf) {
        right->leaf.parent = node;
    } else {
        right->internal.parent = node;
    }
}

MedicationNode* splitLeafNode(MedicationLeafNode *leaf, unsigned long *midKey) {
    int median = leaf->order / 2;
    
    // Create a new leaf node
    MedicationNode *newNode = createMedicationNode(leaf->order, true);
    if (!newNode) return NULL;
    
    MedicationLeafNode *newLeaf = &(newNode->leaf);
    
    // Copy the second half of keys and values to the new leaf
    for (int i = median; i < leaf->cursize; i++) {
        newLeaf->keys[i - median] = leaf->keys[i];
        newLeaf->values[i - median] = leaf->values[i];
        newLeaf->cursize++;
    }
    
    // Update the current size of the original leaf
    leaf->cursize = median;
    
    // Connect the new leaf to the linked list
    newLeaf->next = leaf->next;
    if (newLeaf->next != NULL) {
        newLeaf->next->prev = newLeaf;
    }
    leaf->next = newLeaf;
    newLeaf->prev = leaf;
    
    // Get the first key in the new leaf for the parent
    *midKey = newLeaf->keys[0];
    
    // Set parent pointer
    newLeaf->parent = leaf->parent;
    
    return newNode;
}

MedicationNode* splitInternalNode(MedicationInternalNode *node, unsigned long *midKey) {
    int median = node->order / 2;
    *midKey = node->keys[median];
    
    // Create a new internal node
    MedicationNode *newNode = createMedicationNode(node->order, false);
    if (!newNode) return NULL;
    
    MedicationInternalNode *newInternal = &(newNode->internal);
    
    // Copy the second half of keys and children to the new node (excluding the median)
    for (int i = median + 1; i < node->cursize; i++) {
        newInternal->keys[i - (median + 1)] = node->keys[i];
        newInternal->children[i - (median + 1)] = node->children[i];
        
        // Update parent pointers in children
        if (node->children[i]->isLeaf) {
            node->children[i]->leaf.parent = newInternal;
        } else {
            node->children[i]->internal.parent = newInternal;
        }
        
        newInternal->cursize++;
    }
    
    // Copy the rightmost child
    newInternal->children[newInternal->cursize] = node->children[node->cursize];
    if (node->children[node->cursize]->isLeaf) {
        node->children[node->cursize]->leaf.parent = newInternal;
    } else {
        node->children[node->cursize]->internal.parent = newInternal;
    }
    
    // Update current node size (excluding the median key)
    node->cursize = median;
    
    // Set parent pointer
    newInternal->parent = node->parent;
    
    printf("Internal node split completed. MidKey: %lu\n", *midKey);
    printf("Original internal node keys: ");
    for (int i = 0; i < node->cursize; i++) {
        printf("%lu ", node->keys[i]);
    }
    printf("\nNew internal node keys: ");
    for (int i = 0; i < newInternal->cursize; i++) {
        printf("%lu ", newInternal->keys[i]);
    }
    printf("\n");
    
    return newNode;
}

void insertIntoParent(MedicationBPlusTree *tree, MedicationNode *leftNode, unsigned long midKey, MedicationNode *rightNode) {
    // Case 1: leftNode is the root (has no parent)
    if ((leftNode->isLeaf && !leftNode->leaf.parent) || (!leftNode->isLeaf && !leftNode->internal.parent)) {
        // Create a new root
        MedicationNode* newRoot = createMedicationNode(tree->order, false);
        if (!newRoot) return;
        
        tree->root = newRoot;
        
        newRoot->internal.keys[0] = midKey;
        newRoot->internal.children[0] = leftNode;
        newRoot->internal.children[1] = rightNode;
        newRoot->internal.cursize = 1;
        
        // Update parent pointers of children
        if (leftNode->isLeaf) {
            leftNode->leaf.parent = &(newRoot->internal);
        } else {
            leftNode->internal.parent = &(newRoot->internal);
        }
        
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = &(newRoot->internal);
        } else {
            rightNode->internal.parent = &(newRoot->internal);
        }
        
        return;
    }

    // Get the parent internal node
    MedicationInternalNode *parent;
    if (leftNode->isLeaf) {
        parent = leftNode->leaf.parent;
    } else {
        parent = leftNode->internal.parent;
    }
    
    // Create a container node for the parent (for easier handling)
    MedicationNode parentNode;
    parentNode.isLeaf = false;
    parentNode.internal = *parent;
    
    // Check if parent has space for a new key
    if (parent->cursize < parent->order - 1) {
        // Insert the new key and child into parent
        int i = parent->cursize - 1;
        while (i >= 0 && parent->keys[i] > midKey) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children[i + 2] = parent->children[i + 1];
            i--;
        }
        
        parent->keys[i + 1] = midKey;
        parent->children[i + 2] = rightNode;
        parent->cursize++;
        
        // Update parent pointer of the right child
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = parent;
        } else {
            rightNode->internal.parent = parent;
        }

        return;
    }
    // Parent is full, need to split
    unsigned long newMidKey;
    MedicationNode *newParentNode = splitInternalNode(parent, &newMidKey);
    
    if (midKey > newMidKey) {
        MedicationInternalNode *rightParent = &(newParentNode->internal);
        int i = rightParent->cursize - 1;
        while (i >= 0 && rightParent->keys[i] > midKey) {
            rightParent->keys[i + 1] = rightParent->keys[i];
            rightParent->children[i + 2] = rightParent->children[i + 1];
            i--;
        }
        
        rightParent->keys[i + 1] = midKey;
        rightParent->children[i + 2] = rightNode;
        rightParent->cursize++;
        
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = rightParent;
        } else {
            rightNode->internal.parent = rightParent;
        }
    } else {
        int i = parent->cursize - 1;
        while (i >= 0 && parent->keys[i] > midKey) {
            parent->keys[i + 1] = parent->keys[i];
            parent->children[i + 2] = parent->children[i + 1];
            i--;
        }
        
        parent->keys[i + 1] = midKey;
        parent->children[i + 2] = rightNode;
        parent->cursize++;
        
        if (rightNode->isLeaf) {
            rightNode->leaf.parent = parent;
        } else {
            rightNode->internal.parent = parent;
        }
    }
    

    MedicationNode *parentContainer = (MedicationNode*)malloc(sizeof(MedicationNode));
    if (!parentContainer) return;
    
    parentContainer->isLeaf = false;
    parentContainer->internal = *parent;
    
    insertIntoParent(tree, parentContainer, newMidKey, newParentNode);
}

bool insertMedication(MedicationBPlusTree *tree, MedicationData data) {
    if (!tree) return false;
    
    unsigned long key = data.Medication_ID;

    // Case 1: Empty tree
    if (!tree->root) {
        MedicationNode *newNode = createMedicationNode(tree->order, true);
        if (!newNode) return false;
        
        newNode->leaf.keys[0] = key;
        newNode->leaf.values[0] = data;
        newNode->leaf.cursize = 1;
        
        tree->root = newNode;
        tree->leftmost_leaf = &(newNode->leaf);
        return true;
    }
    
    // Find the leaf node where this key should be inserted
    MedicationNode *leafNode = findLeafNode(tree, key);
    if (!leafNode) return false;
    
    MedicationLeafNode *leaf = &(leafNode->leaf);
    
    // Check if key already exists
    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == key) {
            // Update the existing value
            leaf->values[i] = data;
            return true;
        }
    }
    
    // If leaf is not full, simply insert
    if (leaf->cursize < leaf->order - 1) {
        bool result = insertIntoLeaf(leaf, key, data);
        return result;
    }
    
    // Leaf is full, need to split
    unsigned long midKey;
    MedicationNode *newLeafNode = splitLeafNode(leaf, &midKey);
    if (!newLeafNode) return false;
    
    // After split, determine which leaf should contain our new key
    if (key >= midKey) {
        insertIntoLeaf(&(newLeafNode->leaf), key, data);
    } else {
        insertIntoLeaf(leaf, key, data);
    }
    
    // Update the tree by inserting the separator key into the parent
    insertIntoParent(tree, leafNode, midKey, newLeafNode);
    
    return true;
}

// =================================================================================================================================

void checkStockAlerts(MedicationBPlusTree* pharmacy){
    if (!pharmacy || !pharmacy->root) {
        printf("The medication B+ tree is empty.\n");
        return;
    }

    MedicationLeafNode* current = pharmacy->leftmost_leaf;
    printf("\nStock Alerts:\n");

    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            if (current->values[i].Quantity_in_stock <= current->values[i].Reorderlevel && current->values[i].Quantity_in_stock > 0) {
                printf("Medication ID: %lu, Name: %s, Stock: %u\n",
                       current->keys[i],
                       current->values[i].Medicine_Name,
                       current->values[i].Quantity_in_stock);
            }
            else if(current->values[i].Quantity_in_stock < 0){
                printf("NO STOCK LEFT FOR MEDICATION ID: %lu, Name: %s\n",
                       current->keys[i],
                       current->values[i].Medicine_Name);
            }
        }
        current = current->next;
    }
}

//====================================================================================================================
int Check_leap_year(int year) {
    //FINDING GIVEN YEAR IS LEAP YEAR OR NOT
    if((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)){
        return 1;
    }
    return 0;
}

int DaysCompleted_InYear(int day, int month, int year) {
    int total_days = 0;
    //Till before month we are adding all days as we have an array storing days of 12 months
    for (int i = 1; i < month; i++) {
        total_days += days_in_month[i - 1];
        if (i == 2 && Check_leap_year(year)) {
            total_days += 1;
        }
    }
    total_days += day;
    return total_days;
}

int daysDifference(int day1, int month1, int year1, int day2, int month2, int year2) {
    int days_Complemented_year1 = DaysCompleted_InYear(day1, month1, year1);
    int days_Complemented_year2 = DaysCompleted_InYear(day2, month2, year2);

    if (year1 == year2) {
        return days_Complemented_year2 - days_Complemented_year1;
    } 
    else {
        if(year1>year2) return 10000000;
        int days_remaining_in_year1;
        if(Check_leap_year(year1)){
            days_remaining_in_year1 =  366  - days_Complemented_year1;
        }
        else{
            days_remaining_in_year1 =  365  - days_Complemented_year1;
        }
        
        int days_bw_years = 0;
        for (int i = year1 + 1; i < year2; i++) {
            days_bw_years += Check_leap_year(i) ? 366 : 365;
        }

        int x = days_remaining_in_year1 + days_Complemented_year2 + days_bw_years;

        return x;
    }
}

void checkExpirationDates(int day, int month, int year, MedicationBPlusTree* pharmacy) {
    if (!pharmacy || !pharmacy->root) {
        printf("The medication B+ tree is empty.\n");
        return;
    }

    // Reset February to 28 days for non-leap years
    days_in_month[1] = 28;

    // Update February to 29 days if the current year is a leap year
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        days_in_month[1] = 29;
    }

    MedicationLeafNode* current = pharmacy->leftmost_leaf;
    printf("\nMedicines that are expired or will expire soon:\n");

    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            int eday = current->values[i].Batch_details.Expiration_Date.day;
            int emonth = current->values[i].Batch_details.Expiration_Date.month;
            int eyear = current->values[i].Batch_details.Expiration_Date.year;

            // Calculate the difference in days
            int result = daysDifference(day, month, year, eday, emonth, eyear);

            if (result <= 30) { // Expired or will expire within 30 days
                printf("\n==================== ALERT ====================\n");
                printf("Medication ID: %lu\n", current->keys[i]);
                printf("Name: %s\n", current->values[i].Medicine_Name);
                printf("Expiration Date: %02d/%02d/%04d\n", eday, emonth, eyear);

                if (result <= 0) {
                    printf("Status: Medication Already Expired\n");
                } else {
                    printf("Status: %d Days Remaining Until Expiration\n", result);
                }
                printf("===============================================\n");
            }
            else if(result == 10000000){
                printf("Medication ID: %lu\n", current->keys[i]);
                printf("Name: %s\n", current->values[i].Medicine_Name);
                printf("Status: Medication Expired\n");
            }
        }
        current = current->next;
    }
}


//====================================================================================================================


void heapifyDown(HeapNode* heap, int size, int index, bool sortByTurnover) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    // Compare with left child
    if (left < size && 
        ((sortByTurnover && heap[left].data.turnoverProduced > heap[largest].data.turnoverProduced) ||
         (!sortByTurnover && heap[left].data.noOfUniqueMedicines > heap[largest].data.noOfUniqueMedicines))) {
        largest = left;
    }

    // Compare with right child
    if (right < size && 
        ((sortByTurnover && heap[right].data.turnoverProduced > heap[largest].data.turnoverProduced) ||
         (!sortByTurnover && heap[right].data.noOfUniqueMedicines > heap[largest].data.noOfUniqueMedicines))) {
        largest = right;
    }

    // If the largest is not the current index, swap and continue heapifying
    if (largest != index) {
        HeapNode temp = heap[index];
        heap[index] = heap[largest];
        heap[largest] = temp;

        heapifyDown(heap, size, largest, sortByTurnover);
    }
}

void buildMaxHeap(HeapNode* heap, int size, bool sortByTurnover) {
    for (int i = size / 2 - 1; i >= 0; i--) {
        heapifyDown(heap, size, i, sortByTurnover);
    }
}

HeapNode* populateHeapFromTree(UniqueSupplierBPlusTree* tree, int* size) {
    if (!tree || !tree->leftmost_leaf) {
        printf("The Unique Supplier B+ Tree is empty.\n");
        *size = 0;
        return NULL;
    }

    // Count the total number of suppliers
    int count = 0;
    UniqueSupplierLeafNode* current = tree->leftmost_leaf;
    while (current) {
        count += current->cursize;
        current = current->next;
    }

    // Allocate memory for the heap
    HeapNode* heap = (HeapNode*)malloc(sizeof(HeapNode) * count);
    if (!heap) {
        printf("Memory allocation failed.\n");
        *size = 0;
        return NULL;
    }

    // Populate the heap with supplier data
    int index = 0;
    current = tree->leftmost_leaf;
    while (current) {
        for (int i = 0; i < current->cursize; i++) {
            heap[index].data = current->values[i];
            index++;
        }
        current = current->next;
    }

    *size = count;
    return heap;
}

void extractTop10(HeapNode* heap, int size, bool sortByTurnover) {
    printf("\nTop 10 Suppliers:\n");
    printf("======================================================\n");

    int count = 0;
    while (size > 0 && count < 10) {
        // Print the root (largest element)
        printf("Supplier ID: %lu\n", heap[0].data.Supplier_ID);
        printf("  Name: %s\n", heap[0].data.Supplier_Name);
        printf("  Number of Unique Medicines: %u\n", heap[0].data.noOfUniqueMedicines);
        printf("  Total Turnover: %lu\n", heap[0].data.turnoverProduced);
        printf("----------------------------------------\n");

        // Replace the root with the last element and reduce the heap size
        heap[0] = heap[size - 1];
        size--;

        // Heapify down to maintain the max-heap property
        heapifyDown(heap, size, 0, sortByTurnover);

        count++;
    }

    printf("======================================================\n");
}

//=====================================================================================================================

bool deleteMedication(MedicationBPlusTree **Btree) {
    unsigned long id;
    printf("Enter the Medication ID to delete: ");
    scanf("%lu", &id);

    MedicationBPlusTree *tree = *Btree;
    if (!tree || !tree->root) {
        printf("Leaf node not found for ID %lu.\n", id);
        return false; // Tree is empty
    }

    // Find the leaf node containing the key
    MedicationNode *current = tree->root;
    MedicationInternalNode *parent = NULL;
    int parentIndex = 0;

    // Track the path to the leaf node (for possible rebalancing)
    MedicationInternalNode *path[256];  // Assumes tree height < 256
    int pathIndices[256];
    int pathLen = 0;

    // Navigate to the leaf node
    while (!current->isLeaf) {
        parent = &(current->internal);
        path[pathLen] = parent;
        
        int i;
        for (i = 0; i < parent->cursize; i++) {
            if (id < parent->keys[i]) break;
        }
        
        pathIndices[pathLen] = i;
        pathLen++;
        current = parent->children[i];  
    }

    // Now we're at the leaf node
    MedicationLeafNode *leaf = &(current->leaf);
    int keyIndex = -1;

    // Find the key in the leaf node
    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == id) {
            keyIndex = i;
            break;
        }
    }

    if (keyIndex == -1) {
        printf("Medication ID %lu not found in the tree.\n", id);
        return false; // Key not found
    }

    // Delete the key from the leaf node
    for (int i = keyIndex; i < leaf->cursize - 1; i++) {
        leaf->keys[i] = leaf->keys[i + 1];
        leaf->values[i] = leaf->values[i + 1];
    }
    leaf->cursize--;

    // Case 1: Leaf node has enough keys after deletion (at least order/2)
    int minKeys = (leaf->order - 1) / 2;
    if (leaf->cursize >= minKeys) {
        return true; // No further adjustment needed
    }

    // Case 2: Leaf node is the root
    if (pathLen == 0) {
        if (leaf->cursize == 0) {
            // Tree becomes empty
            free(leaf->keys);
            free(leaf->values);
            free(tree->root);
            tree->root = NULL;
            tree->leftmost_leaf = NULL;
        }
        return true;
    }

    // Case 3:If the left sibling has more than the minimum number of keys, the function borrows the rightmost key from the left sibling.
    // Updates the parent key to reflect the new key in the leaf node.
    while (pathLen > 0) {
        pathLen--;
        parent = path[pathLen];
        int childIndex = pathIndices[pathLen];
        
        // Try to borrow from left sibling
        if (childIndex > 0) {
            MedicationNode *leftSibling = parent->children[childIndex - 1];
            
            if (leftSibling->isLeaf) {
                MedicationLeafNode *leftLeaf = &(leftSibling->leaf);
                
                if (leftLeaf->cursize > minKeys) {
                    // Borrow the rightmost key from left sibling
                    for (int i = leaf->cursize; i > 0; i--) {
                        leaf->keys[i] = leaf->keys[i - 1];
                        leaf->values[i] = leaf->values[i - 1];
                    }
                    
                    leaf->keys[0] = leftLeaf->keys[leftLeaf->cursize - 1];
                    leaf->values[0] = leftLeaf->values[leftLeaf->cursize - 1];
                    
                    leftLeaf->cursize--;
                    leaf->cursize++;
                    
                    // Update parent key
                    parent->keys[childIndex - 1] = leaf->keys[0];
                    
                    return true;
                }
            }
        }

        //If the right sibling has more than the minimum number of keys, the function borrows the leftmost key from the right sibling.
        // Updates the parent key to reflect the new key in the right sibling.
        // Try to borrow from right sibling

        if (childIndex < parent->cursize) {
            MedicationNode *rightSibling = parent->children[childIndex + 1];
            
            if (rightSibling->isLeaf) {
                MedicationLeafNode *rightLeaf = &(rightSibling->leaf);
                
                if (rightLeaf->cursize > minKeys) {
                    // Borrow the leftmost key from right sibling
                    leaf->keys[leaf->cursize] = rightLeaf->keys[0];
                    leaf->values[leaf->cursize] = rightLeaf->values[0];
                    leaf->cursize++;
                    
                    // Shift keys in right sibling
                    for (int i = 0; i < rightLeaf->cursize - 1; i++) {
                        rightLeaf->keys[i] = rightLeaf->keys[i + 1];
                        rightLeaf->values[i] = rightLeaf->values[i + 1];
                    }
                    rightLeaf->cursize--;
                    
                    // Update parent key
                    parent->keys[childIndex] = rightLeaf->keys[0];
                    
                    return true;
                }
            }
        }
        
        // Merge with a sibling
        //If borrowing is not possible, the function merges the leaf node with its left sibling.
        // Updates the parent node by removing the key corresponding to the merged node.
        if (childIndex > 0) {
            // Merge with left sibling
            MedicationNode *leftSibling = parent->children[childIndex - 1];
            
            if (leftSibling->isLeaf) {
                MedicationLeafNode *leftLeaf = &(leftSibling->leaf);
                
                // Copy keys and values from current leaf to left leaf
                for (int i = 0; i < leaf->cursize; i++) {
                    leftLeaf->keys[leftLeaf->cursize + i] = leaf->keys[i];
                    leftLeaf->values[leftLeaf->cursize + i] = leaf->values[i];
                }
                
                leftLeaf->cursize += leaf->cursize;
                leftLeaf->next = leaf->next;
                
                if (leaf->next) {
                    leaf->next->prev = leftLeaf;
                }
                
                // Remove the parent key and adjust parent's children
                for (int i = childIndex - 1; i < parent->cursize - 1; i++) {
                    parent->keys[i] = parent->keys[i + 1];
                    parent->children[i + 1] = parent->children[i + 2];
                }
                parent->cursize--;
                
                // Free the merged node
                free(leaf->keys);
                free(leaf->values);
                free(current);
                
                // If parent is now empty and is the root, update root
                if (parent->cursize == 0 && pathLen == 0) {
                    tree->root = parent->children[0];
                    if(tree->root->isLeaf) {
                        tree->root->leaf.parent = NULL;
                    } 
                    else {
                        tree->root->internal.parent = NULL; 
                    }
                    
                    free(parent->keys);
                    free(parent->children);
                    free(path[0]);
                }
                
                return true;
            }
        } else {
            // Merge with right sibling
            //If merging with the left sibling is not possible, the function merges the leaf node with its right sibling.
            // Updates the parent node by removing the key corresponding to the merged node.
            MedicationNode *rightSibling = parent->children[childIndex + 1];
            
            if (rightSibling->isLeaf) {
                MedicationLeafNode *rightLeaf = &(rightSibling->leaf);
                
                // Copy keys and values from right leaf to current leaf
                for (int i = 0; i < rightLeaf->cursize; i++) {
                    leaf->keys[leaf->cursize + i] = rightLeaf->keys[i];
                    leaf->values[leaf->cursize + i] = rightLeaf->values[i];
                }
                
                leaf->cursize += rightLeaf->cursize;
                leaf->next = rightLeaf->next;
                
                if (rightLeaf->next) {
                    rightLeaf->next->prev = leaf;
                }
                
                // Remove the parent key and adjust parent's children
                for (int i = childIndex; i < parent->cursize - 1; i++) {
                    parent->keys[i] = parent->keys[i + 1];
                    parent->children[i + 1] = parent->children[i + 2];
                }
                parent->cursize--;
                
                // Free the merged node
                free(rightLeaf->keys);
                free(rightLeaf->values);
                free(rightSibling);
                
                // If parent is now empty and is the root, update root
                if (parent->cursize == 0 && pathLen == 0) {
                    tree->root = parent->children[0];
                    // Update the root's parent pointer
                    if(tree->root->isLeaf) {
                        tree->root->leaf.parent = NULL;
                    } 
                    else {
                        tree->root->internal.parent = NULL; 
                    }
                    
                    free(parent->keys);
                    free(parent->children);
                    free(path[0]);
                }
                
                return true;
            }
        }
        
        // If we get here, we need to rebalance the parent
        if (parent->cursize < minKeys && pathLen > 0) {
            // Continue up the tree and balance the parent
            current = path[pathLen - 1]->children[pathIndices[pathLen - 1]];
        } else {
            break; // Parent has enough keys
        }
    }
    
    return true;
}

void printMedicationDetails(MedicationData* medication) {
    printf("\n=== MEDICATION DETAILS ===\n");
    printf("ID: %lu\n", medication->Medication_ID);
    printf("Name: %s\n", medication->Medicine_Name);
    printf("Quantity in stock: %u\n", medication->Quantity_in_stock);
    printf("Price per unit: $%u\n", medication->Price_per_Unit);
    printf("Reorder level: %d\n", medication->Reorderlevel);
    printf("Batch: %s\n", medication->Batch_details.Batch);
    printf("Expiration date: %d-%d-%d\n", 
           medication->Batch_details.Expiration_Date.day,
           medication->Batch_details.Expiration_Date.month,
           medication->Batch_details.Expiration_Date.year);
    printf("Total sales: %d\n", medication->Batch_details.Total_sales);
}

void printSupplier(SupplierData* supplier) {
    printf("  - Supplier ID: %lu\n", supplier->Supplier_ID);
    printf("    Name: %s\n", supplier->Supplier_Name);
    printf("    Quantity: %u\n", supplier->Quantity_of_stock_bysupplier);
    printf("    Contact: %s\n", supplier->Contact);
}

void printAllSuppliers(SupplierBPlusTree* tree) {
    if (!tree || !tree->root) {
        printf("  No suppliers for this medication.\n");
        return;
    }
    
    printf("\n  --- SUPPLIERS ---\n");
    
    // Start at the leftmost leaf
    SupplierLeafNode* current = tree->leftmost_leaf;
    
    // Traverse all leaf nodes
    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            printSupplier(&current->values[i]);
            printf("\n");
        }
        current = current->next;
    }
}


void printMedicationBPlusTree(MedicationBPlusTree* tree) {
    if (!tree || !tree->root) {
        printf("The medication B+ tree is empty.\n");
        return;
    }
    
    printf("\n======================================================\n");
    printf("           MEDICATION B+ TREE (Order: %d)              \n", tree->order);
    printf("======================================================\n");
    
    // Start from the leftmost leaf
    MedicationLeafNode* current = tree->leftmost_leaf;

    // Traverse all leaf nodes
    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            printf("\nMedication %d:\n", i + 1);
            printMedicationDetails(&current->values[i]);
            printAllSuppliers(current->values[i].Suppliers);
        }
        current = current->next; // Move to the next leaf node
    }

    printf("\n======================================================\n");
}

bool deleteSupplier(SupplierBPlusTree **Btree) {
    unsigned long id;
    printf("Enter the Supplier ID to delete: ");
    scanf("%lu", &id);

    SupplierBPlusTree *tree = *Btree;
    if (!tree || !tree->root) {
        return false; // Tree is empty
    }

    // Find the leaf node containing the key
    SupplierNode *current = tree->root;
    SupplierInternalNode *parent = NULL;
    int parentIndex = 0;

    // Track the path to the leaf node (for possible rebalancing)
    SupplierInternalNode *path[256];  // Assumes tree height < 256
    int pathIndices[256];
    int pathLen = 0;

    // Navigate to the leaf node
    while (!current->isLeaf) {
        parent = &(current->internal);
        path[pathLen] = parent;
        
        int i;
        for (i = 0; i < parent->cursize; i++) {
            if (id < parent->keys[i]) break;
        }
        
        pathIndices[pathLen] = i;
        pathLen++;
        current = parent->children[i];
    }

    // Now we're at the leaf node
    SupplierLeafNode *leaf = &(current->leaf);
    int keyIndex = -1;

    // Find the key in the leaf node
    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == id) {
            keyIndex = i;
            break;
        }
    }

    if (keyIndex == -1) {
        return false; // Key not found
    }

    // Delete the key from the leaf node
    for (int i = keyIndex; i < leaf->cursize - 1; i++) {
        leaf->keys[i] = leaf->keys[i + 1];
        leaf->values[i] = leaf->values[i + 1];
    }
    leaf->cursize--;

    // Case 1: Leaf node has enough keys after deletion
    int minKeys = (leaf->order - 1) / 2;
    if (leaf->cursize >= minKeys) {
        return true;
    }

    // Case 2: Leaf node is the root
    if (pathLen == 0) {
        if (leaf->cursize == 0) {
            free(leaf->keys);
            free(leaf->values);
            free(tree->root);
            tree->root = NULL;
            tree->leftmost_leaf = NULL;
        }
        return true;
    }

    // Case 3: Leaf node has fewer than minimum keys - need to borrow or merge
    while (pathLen > 0) {
        pathLen--;
        parent = path[pathLen];
        int childIndex = pathIndices[pathLen];

        // Try to borrow from left sibling
        if (childIndex > 0) {
            SupplierNode *leftSibling = parent->children[childIndex - 1];
            
            if (leftSibling->isLeaf) {
                SupplierLeafNode *leftLeaf = &(leftSibling->leaf);
                
                if (leftLeaf->cursize > minKeys) {
                    // Borrow from left sibling
                    for (int i = leaf->cursize; i > 0; i--) {
                        leaf->keys[i] = leaf->keys[i - 1];
                        leaf->values[i] = leaf->values[i - 1];
                    }
                    leaf->keys[0] = leftLeaf->keys[leftLeaf->cursize - 1];
                    leaf->values[0] = leftLeaf->values[leftLeaf->cursize - 1];
                    
                    leftLeaf->cursize--;
                    leaf->cursize++;
                    
                    parent->keys[childIndex - 1] = leaf->keys[0];
                    return true;
                }
            }
        }

        // Try to borrow from right sibling
        if (childIndex < parent->cursize) {
            SupplierNode *rightSibling = parent->children[childIndex + 1];
            
            if (rightSibling->isLeaf) {
                SupplierLeafNode *rightLeaf = &(rightSibling->leaf);
                
                if (rightLeaf->cursize > minKeys) {
                    // Borrow from right sibling
                    leaf->keys[leaf->cursize] = rightLeaf->keys[0];
                    leaf->values[leaf->cursize] = rightLeaf->values[0];
                    leaf->cursize++;
                    
                    for (int i = 0; i < rightLeaf->cursize - 1; i++) {
                        rightLeaf->keys[i] = rightLeaf->keys[i + 1];
                        rightLeaf->values[i] = rightLeaf->values[i + 1];
                    }
                    rightLeaf->cursize--;
                    
                    parent->keys[childIndex] = rightLeaf->keys[0];
                    return true;
                }
            }
        }

        // Merge with a sibling
        if (childIndex > 0) {
            SupplierNode *leftSibling = parent->children[childIndex - 1];
            
            if (leftSibling->isLeaf) {
                SupplierLeafNode *leftLeaf = &(leftSibling->leaf);
                
                for (int i = 0; i < leaf->cursize; i++) {
                    leftLeaf->keys[leftLeaf->cursize + i] = leaf->keys[i];
                    leftLeaf->values[leftLeaf->cursize + i] = leaf->values[i];
                }
                leftLeaf->cursize += leaf->cursize;
                leftLeaf->next = leaf->next;
                
                free(leaf->keys);
                free(leaf->values);
                free(current);
                
                for (int i = childIndex - 1; i < parent->cursize - 1; i++) {
                    parent->keys[i] = parent->keys[i + 1];
                    parent->children[i + 1] = parent->children[i + 2];
                }
                parent->cursize--;
                return true;
            }
        }
    }
    return true;
}

bool searchSupplier(SupplierBPlusTree *Btree, unsigned long supplierID) {
    if (!Btree || !Btree->root) {
        return false; // Tree is empty
    }

    SupplierNode *current = Btree->root;

    // Traverse to the leaf node where the supplier ID might be stored
    while (!current->isLeaf) {
        SupplierInternalNode *internal = &(current->internal);
        int i;
        for (i = 0; i < internal->cursize; i++) {
            if (supplierID < internal->keys[i]) {  
                break;
            }
        }
        current = internal->children[i];
    }

    // We are now at the leaf node
    SupplierLeafNode *leaf = &(current->leaf);

    // Search for the supplierID in the leaf node
    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == supplierID) {  // Corrected comparison
            return true; // Supplier found
        }
    }

    return false; // Supplier not found
}

void updateDetails(MedicationData *medication) {
    printf("Enter the details you want to update:\n");
    printf("1. Update Price\n2. Update Stock\n3. Update Supplier Information\n");
    int ch;
    scanf("%d", &ch);

    switch (ch) {
        case 1: {
            printf("Enter the new price: \n");
            scanf("%u", &medication->Price_per_Unit);
            printf("Price updated successfully.\n");
            break;
        }
        case 2: {
            printf("Enter the new stock: \n");
            scanf("%u", &medication->Quantity_in_stock);
            printf("Stock updated successfully.\n");
            break;
        }
        case 3: {
            printf("Enter the details you want to update for suppliers:\n");
            printf("1. Add New Supplier\n2. Update Supplier Details\n3. Delete Supplier\n4. Search Supplier\n5. Print All Suppliers\n");
            int ch1;
            scanf("%d", &ch1);
            unsigned long id;

            switch (ch1) {
                case 1: {
                    SupplierData suppl = initSupplier(medication->Suppliers, medication);
                    insertSupplier(medication->Suppliers, suppl);
                    printf("New supplier added successfully.\n");
                    break;
                }
                case 2: {
                    printf("Enter the Supplier ID to update: \n");
                    scanf("%lu", &id);
                    SupplierNode* leaf2 = findSupplierLeafNode(medication->Suppliers, id);
                    SupplierLeafNode* leaf = &(leaf2->leaf);
                    if (leaf) {
                        for (int i = 0; i < leaf->cursize; i++) {
                            if (leaf->keys[i] == id) {
                                printf("Enter details to update:\n");
                                printf("1. Update Supplier Name\n2. Update Quantity\n3. Update Contact\n");
                                int updateChoice;
                                scanf("%d", &updateChoice);
                                switch (updateChoice) {
                                    case 1:
                                        printf("Enter new Supplier Name: ");
                                        scanf("%s", leaf->values[i].Supplier_Name);
                                        break;
                                    case 2:
                                        printf("Enter new Quantity: ");
                                        scanf("%u", &leaf->values[i].Quantity_of_stock_bysupplier);
                                        break;
                                    case 3:
                                        printf("Enter new Contact: ");
                                        scanf("%s", leaf->values[i].Contact);
                                        break;
                                    default:
                                        printf("Invalid choice.\n");
                                }
                                printf("Supplier details updated successfully.\n");
                                break;
                            }
                        }
                    } else {
                        printf("Supplier ID %lu not found.\n", id);
                    }
                    break;
                }
                case 3: {
                    if (deleteSupplier(&medication->Suppliers)) {
                        printf("Supplier deleted successfully.\n");
                    } else {
                        printf("Supplier not found.\n");
                    }
                    break;
                }
                case 4: {
                    printf("Enter the Supplier ID to search: \n");
                    scanf("%lu", &id);
                    if (searchSupplier(medication->Suppliers, id)) {
                        printf("Supplier ID %lu found.\n", id);
                        printf("Details:\n");
                        SupplierNode* leaf2 = findSupplierLeafNode(medication->Suppliers, id);
                        SupplierLeafNode* leaf = &(leaf2->leaf);
                        for (int i = 0; i < leaf->cursize; i++) {
                            if (leaf->keys[i] == id) {
                                printSupplier(&leaf->values[i]);
                                break;
                            }
                        }
                    } else {
                        printf("Supplier ID %lu not found.\n", id);
                    }
                    break;
                }
                case 5: {
                    printAllSuppliers(medication->Suppliers);
                    break;
                }
                default:
                    printf("Invalid choice.\n");
            }
            break;
        }
        default:
            printf("Invalid choice. Please try again.\n");
    }
}

void ReadFileAndStoreData(const char* filename , MedicationBPlusTree *tree) {
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Unable to open file %s\n", filename);
        return;
    }

    // printf("reading from file...\n");
    char Buffer[256];

    while (fgets(Buffer, sizeof(Buffer), file)) {

        // printf("Entered into loop");
       
        if (Buffer[0] == '\n' || Buffer[0] == '\r') {
            continue;
        }

        MedicationData medication;
        unsigned long medicationID;
        char medicineName[NAME_SIZE];
        unsigned int quantityInStock, pricePerUnit, reorderLevel;
        char batch[BATCH_SIZE];
        int day, month, year, totalSales, numSuppliers;

        sscanf(Buffer, "%lu", &medicationID);
        fscanf(file, "\n");

        fgets(medicineName, sizeof(medicineName), file);
        medicineName[strcspn(medicineName, "\n")] = 0;  // Remove newline

        fscanf(file, "%u", &quantityInStock);
        fscanf(file, "%u", &pricePerUnit);
        fscanf(file, "%s", batch);

        fscanf(file, "%d %d %d", 
            &day,
            &month,
            &year
        );

        fscanf(file, "%d", &numSuppliers);

        medication.Suppliers = createSupplierBPlusTree(tree->order);

        for (int j = 0; j < numSuppliers; j++) {

            SupplierData supplier; // Allocate new supplier node
            unsigned long supplierID;
            char supplierName[NAME_SIZE];
            unsigned int quantityBySupplier;
            char contact[CONTACT_SIZE];

            fscanf(file, "%lu", &supplierID);
            fscanf(file, "%s", supplierName);
            fscanf(file, "%u", &quantityBySupplier);
            fscanf(file, "%s", contact);

            supplier.Supplier_ID = supplierID;
            strcpy(supplier.Supplier_Name, supplierName);
            supplier.Quantity_of_stock_bysupplier = quantityBySupplier;
            unsigned long z = quantityBySupplier;
            strcpy(supplier.Contact, contact);

            insertSupplier(medication.Suppliers, supplier);

            // if notfound fn return true else if found return false
            unsigned long turnov = quantityBySupplier * pricePerUnit;

            if(checkUniqueSupplier(supplierID) == false){
                // Update the existing supplier's quantity and turnover
                UniqueSupplierNode* leaf2 = findUniqueSupplierLeafNode(uniqueSupplierTree, supplierID);
                UniqueSupplierLeafNode* leaf = &(leaf2->leaf);
                if (leaf) {
                    for (int i = 0; i < leaf->cursize; i++) {
                        if (leaf->keys[i] == supplierID) {
                            leaf->values[i].turnoverProduced += turnov;
                            leaf->values[i].noOfUniqueMedicines += 1;
                            break;
                        }
                    }
                }
            }else{
                insertUniqueSupplier(uniqueSupplierTree,supplierID, supplierName, 1, turnov);
            }

            // printf("before insertion of supplier ...\n");

        }

        fscanf(file, "%d", &reorderLevel);

        medication.Medication_ID = medicationID;
        strcpy(medication.Medicine_Name, medicineName);
        medication.Quantity_in_stock = quantityInStock;
        medication.Price_per_Unit = pricePerUnit;
        strcpy(medication.Batch_details.Batch, batch);
        medication.Batch_details.Expiration_Date.day = day;
        medication.Batch_details.Expiration_Date.month = month;
        medication.Batch_details.Expiration_Date.year = year;
        medication.Batch_details.Total_sales = 0;
        medication.Reorderlevel = reorderLevel;

        printf("before insertion of medicatn ...\n");

        insertMedication(tree, medication);   

        printf("end of iteration...\n");

        int expirationKey = year * 10000 + month * 100 + day + 100000000*medicationID;

        insertIntoExpirationTree(expirationTree, expirationKey, medicationID, medicineName);

    }

    fclose(file);
}

void searchMedicationByID(MedicationBPlusTree *tree, unsigned long id) {
    printf("Searching for medication with ID %lu...\n", id);
    if (!tree || !tree->root) {
        printf("The medication B+ tree is empty.\n");
        return;
    }

    // Find the leaf node where the medication ID might be stored
    MedicationNode *leaf2 = findLeafNode(tree, id);
    MedicationLeafNode *leaf = &(leaf2->leaf);
    if(leaf != NULL) {
        printf("Leaf node found for ID %lu.\n", id);
    } else {
        printf("Leaf node not found for ID %lu.\n", id);
        return;
    }

    // Search for the ID in the leaf node
    for (int i = 0; i < leaf->cursize; i++) {
        if (leaf->keys[i] == id) {
            printf("Medication found:\n");
            printMedicationDetails(&(leaf->values[i]));
            return;
        }
    }

    // If the ID is not found in the leaf node
    printf("Medication ID %lu not found in the tree.\n", id);
}

void printTreeStructure(MedicationNode *node, int level) {
    if (!node) {
        printf("Empty tree\n");
        return;
    }
    
    printf("Level %d: ", level);
    if (node->isLeaf) {
        printf("Leaf Node Keys: ");
        for (int i = 0; i < node->leaf.cursize; i++) {
            printf("%lu ", node->leaf.keys[i]);
        }
        printf("\n");
    } else {
        printf("Internal Node Keys: ");
        for (int i = 0; i < node->internal.cursize; i++) {
            printf("%lu ", node->internal.keys[i]);
        }
        printf("\n");
        
        // Recursively print children
        for (int i = 0; i <= node->internal.cursize; i++) {
            printTreeStructure(node->internal.children[i], level + 1);
        }
    }
}

void SaveDataToFile(const char* filename, MedicationBPlusTree* tree) {
    if (!tree || !tree->root) {
        printf("The medication B+ tree is empty. Nothing to save.\n");
        return;
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Unable to open file %s for writing.\n", filename);
        return;
    }

    printf("Saving data to file: %s\n", filename);

    // Start from the leftmost leaf
    MedicationLeafNode* current = tree->leftmost_leaf;

    // Traverse all leaf nodes
    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            MedicationData* medication = &current->values[i];

            // Write medication details
            fprintf(file, "%lu\n", medication->Medication_ID);
            fprintf(file, "%s\n", medication->Medicine_Name);
            fprintf(file, "%u\n", medication->Quantity_in_stock);
            fprintf(file, "%u\n", medication->Price_per_Unit);
            fprintf(file, "%s\n", medication->Batch_details.Batch);
            fprintf(file, "%d %d %d\n", 
                medication->Batch_details.Expiration_Date.day,
                medication->Batch_details.Expiration_Date.month,
                medication->Batch_details.Expiration_Date.year);
            fprintf(file, "%d\n", medication->Reorderlevel);

            // Write supplier details
            SupplierBPlusTree* suppliers = medication->Suppliers;
            if (suppliers && suppliers->root) {
                SupplierLeafNode* supplierLeaf = suppliers->leftmost_leaf;
                int supplierCount = 0;

                // Count the number of suppliers
                while (supplierLeaf != NULL) {
                    supplierCount += supplierLeaf->cursize;
                    supplierLeaf = supplierLeaf->next;
                }

                fprintf(file, "%d\n", supplierCount);

                // Write supplier details
                supplierLeaf = suppliers->leftmost_leaf;
                while (supplierLeaf != NULL) {
                    for (int j = 0; j < supplierLeaf->cursize; j++) {
                        SupplierData* supplier = &supplierLeaf->values[j];
                        fprintf(file, "%lu\n", supplier->Supplier_ID);
                        fprintf(file, "%s\n", supplier->Supplier_Name);
                        fprintf(file, "%u\n", supplier->Quantity_of_stock_bysupplier);
                        fprintf(file, "%s\n", supplier->Contact);
                    }
                    supplierLeaf = supplierLeaf->next;
                }
            } else {
                fprintf(file, "0\n"); // No suppliers
            }
        }
        current = current->next; // Move to the next leaf node
    }

    fclose(file);
    printf("Data successfully saved to file: %s\n", filename);
}

void supplierManagement(MedicationData *medication){

    printf("Supplier Management...\n");
    unsigned long id;
    printf("Enter the Supplier ID to update: \n");
    scanf("%lu", &id);
    int choice;
    printf("1. Add Supplier\n2. Update Supplier\n3. Delete Supplier\n4. Search Supplier\n5. Print All Suppliers\n0. Exit\n");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            printf("Adding new supplier...\n");
            SupplierData suppl = initSupplier(medication->Suppliers, medication);
            insertSupplier(medication->Suppliers, suppl);
            printf("\nNew supplier added successfully.\n");
            break;
        case 2:
        {
            SupplierNode *leaf2 = findSupplierLeafNode(medication->Suppliers, id);
            SupplierLeafNode *leaf = &(leaf2->leaf);
            if (leaf) {
                for (int i = 0; i < leaf->cursize; i++) {
                    if (leaf->keys[i] == id) {
                        printf("Enter details to update:\n");
                        printf("1. Update Supplier Name\n2. Update Quantity\n3. Update Contact\n");
                        int updateChoice;
                        scanf("%d", &updateChoice);
                        switch (updateChoice) {
                            case 1:
                                printf("Enter new Supplier Name: ");
                                scanf("%s", leaf->values[i].Supplier_Name);
                                break;
                            case 2: {
                                unsigned int oldQuantity = leaf->values[i].Quantity_of_stock_bysupplier;
                                printf("Enter new Quantity: ");
                                scanf("%u", &leaf->values[i].Quantity_of_stock_bysupplier);

                                // Update the unique supplier tree
                                updateUniqueSupplierTreeAfterInsert(
                                    leaf->keys[i],
                                    leaf->values[i].Supplier_Name,
                                    leaf->values[i].Quantity_of_stock_bysupplier - oldQuantity,
                                    (leaf->values[i].Quantity_of_stock_bysupplier - oldQuantity) * medication->Price_per_Unit
                                );
                                
                                // Update the medication's stock
                                medication->Quantity_in_stock += (leaf->values[i].Quantity_of_stock_bysupplier - oldQuantity);
                                break;
                            }
                            case 3:
                                printf("Enter new Contact: ");
                                scanf("%s", leaf->values[i].Contact);
                                break;
                            default:
                                printf("Invalid choice.\n");
                        }
                        printf("Supplier details updated successfully.\n");
                        break;
                    }
                }
            } else {
                printf("Supplier ID %lu not found.\n", id);
            }
            break;
        }
        
    case 3: {
        unsigned long deleteId;
        printf("Enter the Supplier ID to delete: ");
        scanf("%lu", &deleteId);
        if (deleteSupplier(&medication->Suppliers)) {
            printf("Supplier deleted successfully.\n");
        } else {
            printf("Supplier not found.\n");
        }
        break;
    }
    case 4: {
        printf("Enter the Supplier ID to search: \n");
        scanf("%lu", &id);
        if (searchSupplier(medication->Suppliers, id)) {
            printf("Supplier ID %lu found.\n", id);
            printf("Details:\n");
            SupplierNode *leaf2 = findSupplierLeafNode(medication->Suppliers, id);
            SupplierLeafNode *leaf = &(leaf2->leaf);
            for (int i = 0; i < leaf->cursize; i++) {
                if (leaf->keys[i] == id) {
                    printSupplier(&leaf->values[i]);
                    break;
                }
            }
        } else {
            printf("Supplier ID %lu not found.\n", id);
        }
        break;
    }
    case 5: {
        printAllSuppliers(medication->Suppliers);
        break;
    }
    case 0:
        printf("Exiting supplier management.\n");
        break;
    default:
        printf("Invalid choice.\n");
    
    }
    return;
}

void salesTracking(MedicationBPlusTree* pharmacy){
    if (!pharmacy || !pharmacy->root) {
        printf("The medication B+ tree is empty.\n");
        return;
    }

    MedicationLeafNode* current = pharmacy->leftmost_leaf;
    printf("\nEnter the id of the medication to track sales: ");
    unsigned long id;
    scanf("%lu", &id);

    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            if(current->keys[i] == id){
                int sales;
                printf("\nEnter the number of sales for %s: ", current->values[i].Medicine_Name);
                scanf("%d", &sales);
                if(sales > current->values[i].Quantity_in_stock){
                    printf("\nNot enough stock available for this medication.");
                }
                else{
                    current->values[i].Batch_details.Total_sales += sales;
                    current->values[i].Quantity_in_stock -= sales;
                    printf("====Sales updated successfully===\n");
                }
            }
        }
        current = current->next;
    }
}

void searchPharmacyUsingName(MedicationBPlusTree* pharmacy) {
    if (!pharmacy || !pharmacy->root) {
        printf("The medication B+ tree is empty.\n");
        return;
    }
    char name[NAME_SIZE];
    printf("Enter the name of the medication to search: \n");
    scanf("%s", name);

    printf("\nSearching for medication with name: %s\n", name);

    MedicationLeafNode* current = pharmacy->leftmost_leaf;
    bool found = false;

    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            if (strcmp(current->values[i].Medicine_Name, name) == 0) { // Case-insensitive comparison
                printMedicationDetails(&current->values[i]);
                found = true;
            }
        }
        current = current->next;
    }

    if (!found) {
        printf("No medication found with the name: %s\n", name);
    }
}

void searchMedicationsUsingSupplier(MedicationBPlusTree* pharmacy, unsigned long supplierID) {
    if (!pharmacy || !pharmacy->root) {
        printf("The medication B+ tree is empty.\n");
        return;
    }

    printf("\nSearching for medications supplied by Supplier ID %lu...\n", supplierID);

    MedicationLeafNode* current = pharmacy->leftmost_leaf;

    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            if (current->values[i].Suppliers) {
                SupplierLeafNode* supplierLeaf = current->values[i].Suppliers->leftmost_leaf;
                while (supplierLeaf != NULL) {
                    for (int j = 0; j < supplierLeaf->cursize; j++) {
                        if (supplierLeaf->keys[j] == supplierID) {
                            printMedicationDetails(&current->values[i]);
                            break;
                        }
                    }
                    supplierLeaf = supplierLeaf->next;
                }
            }
        }
        current = current->next;
    }
}

void printUniqueSupplierBPlusTree(UniqueSupplierBPlusTree* tree) {
    if (!tree || !tree->root) {
        printf("The unique supplier B+ tree is empty.\n");
        return;
    }

    UniqueSupplierLeafNode* current = tree->leftmost_leaf;

    printf("\n======================================================\n");
    printf("           UNIQUE SUPPLIER B+ TREE (Order: %d)        \n", tree->order);
    printf("======================================================\n");

    while (current != NULL) {
        for (int i = 0; i < current->cursize; i++) {
            printf("Supplier ID: %lu\n   -> Name: %s\n   ->Turnover: %lu\n   ->Number of Unique Medicines: %d\n",
                current->keys[i],
                current->values[i].Supplier_Name,
                current->values[i].turnoverProduced,
                current->values[i].noOfUniqueMedicines);
        }
        current = current->next;
    }

    printf("======================================================\n");
}   

int main(){

    int order;
    printf("\nEnter the order of the B+ tree: ");
    scanf("%d", &order);

    MedicationBPlusTree* pharmacy = createMedicationBPlusTree(order);
    expirationTree = createExpirationBPlusTree(order);
    uniqueSupplierTree = createUniqueSupplierBPlusTree(order);

    if (pharmacy == NULL) {
        printf("Failed to create B+ tree.\n");
        return -1;
    }
    if (expirationTree == NULL) {
        printf("Failed to create Expiration B+ tree.\n");
        return -1;
    }
    if (!uniqueSupplierTree) {
        printf("Failed to create Unique Supplier B+ tree.\n");
        return -1;
    }

    ReadFileAndStoreData("medication.txt", pharmacy);

    printf(" \nWelcome to the India's Top Medical Store. \n\n");
    int flag = 1;

    printf("-------------------------------------------------------\n");
    while(flag){
        printf("\n1. Add New Medication\n2. Update Medication Details\n3. Delete Medication\n4. Search Medication");
        printf("\n5. Stock Alerts\n6. Check Expiration Dates\n7. Sort Medication By Expiration Dates");
        printf("\n8. Sales Tracking\n9. Supplier Management\n10. Find All-rounder Suppliers\n11. Find Suppliers with Largest Turn-over\n12. Print the Whole Data\n13.Print Unique Suppliers\n14. Print Tree Structure\n0. Exit\n");

        int ch;
        printf("\nEnter the Operation You want to monitor : ");
        scanf("%d",&ch);

        while (getchar() != '\n'); // Clear the input buffer
        

        switch(ch){
            case 0: 
                flag = 0;
                printf("You are now exitted from the process. re run to start ur process again !!!");
                printf("----------------------------------------------------------------------------------------------------------------------------\n");
                break;
            case 1:
                {
                    MedicationData data = createMedicationData(order , pharmacy);
                    insertMedication(pharmacy, data);
                    printf("Medication added successfully.\n");
                    break;
                }
            case 2: {
                unsigned long id;
                printf("Enter the Medication ID to update: ");
                scanf("%lu", &id);
            
                MedicationNode* leaf2 = findLeafNode(pharmacy, id);
                MedicationLeafNode* leaf = &(leaf2->leaf);

                if (leaf) {
                    for (int i = 0; i < leaf->cursize; i++) {
                        if (leaf->keys[i] == id) {
                            updateDetails(&(leaf->values[i]));
                            break;
                        }
                    }
                } else {
                    printf("Medication ID %lu not found.\n", id);
                }
                break;
            }
            case 3:
                {
                    if (deleteMedication(&pharmacy)) {
                        printf("Medication deleted successfully.\n");
                    } else {
                        printf("Medication not found.\n");
                    }
                    break;
                }
            case 4: {
                printf("Search medication using\n 1.ID\n 2.Name\n 3.Supplier:\n");
                printf("Enter your choice:");
                int searchChoice;
                scanf("%d", &searchChoice);
                if(searchChoice == 1) {
                    unsigned long id;
                    printf("Enter the Medication ID to search: \n");
                    scanf("%lu", &id);
                    searchMedicationByID(pharmacy, id);
                } else if (searchChoice == 2) {
                    searchPharmacyUsingName(pharmacy);
                } else if(searchChoice == 3){
                    unsigned long id;
                    printf("Enter the Supplier ID to search: \n");
                    scanf("%lu", &id);
                    searchMedicationsUsingSupplier(pharmacy, id);
                }
                else {
                    printf("Invalid choice.\n");
                }
                break;
            }
            case 5: {
                printf("Checking stock alerts...\n\n");
                checkStockAlerts(pharmacy);
                break;
            }
            case 6: {
                printf("Enter the Day, Month, and Year to check Expiration Dates: \n");
                int day, month, year;
                int tempday, tempmonth, tempyear;

                printf("Enter day, month, and year:\n");
                scanf("%d", &tempday);
                scanf("%d", &tempmonth);
                scanf("%d", &tempyear);

                year = tempyear;
                while (year < 1) {
                    printf("Enter a valid Year: ");
                    scanf("%d", &tempyear);
                    year = tempyear; // Update year
                }

                month = tempmonth;
                while (month < 1 || month > 12) {
                    printf("Enter a valid Month: ");
                    scanf("%d", &tempmonth);
                    month = tempmonth; // Update month
                }

                if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                    days_in_month[1] = 29; // February has 29 days in a leap year
                }

                // Ensure the day is valid for the given month and year
                day = tempday;
                while (day < 1 || day > days_in_month[month - 1]) {
                    printf("Enter a valid Day: ");
                    scanf("%d", &tempday);
                    day = tempday; // Update day
                }

                checkExpirationDates(day, month, year, pharmacy);
                printf("----------------------------------------------------------------------------------------------------------------------------\n");
                break;
            }
            case 7: {
                printf("Sorting medications by expiration dates...\n");
                printExpirationBPlusTree(expirationTree);
                break;
            }
            case 8:
                printf("\nSales Tracking...\n");
                salesTracking(pharmacy);
                break; 
            case 9:
                printf("\nSupplier Management...\n");
                unsigned long id;
                printf("Enter the Medication ID to update: ");
                scanf("%lu", &id);

                MedicationNode* leaf2 = findLeafNode(pharmacy, id);
                MedicationLeafNode* leaf = &(leaf2->leaf);
                if (leaf) {
                    for (int i = 0; i < leaf->cursize; i++) {
                        if (leaf->keys[i] == id) {
                            supplierManagement(&(leaf->values[i]));
                            break;
                        }
                    }
                } else {
                    printf("Medication ID %lu not found.\n", id);
                }
                break;
            case 10:
                printf("\nFinding All-rounder Suppliers...\n");
                {
                    int size;
                    HeapNode* heap = populateHeapFromTree(uniqueSupplierTree, &size);
                    if (heap) {
                        buildMaxHeap(heap, size, false); // Sort by noOfUniqueMedicines
                        extractTop10(heap, size, false);
                        free(heap); // Free the heap after use
                    }
                }
            break;
        
            case 11:
                printf("\nFinding Suppliers with Largest Turn-over...\n");
                {
                    int size;
                    HeapNode* heap = populateHeapFromTree(uniqueSupplierTree, &size);
                    if (heap) {
                        buildMaxHeap(heap, size, true); 
                        extractTop10(heap, size, true);
                        free(heap); // Free the heap after use
                    }
                }
                break;         
            case 12:
                printf("\nPrinting the entire pharmacy database...\n");
                printMedicationBPlusTree(pharmacy);
                break;
            case 13:
                printf("\nPrinting the entire unique supplier database...\n");
                printUniqueSupplierBPlusTree(uniqueSupplierTree);
                break;
            case 14:
                printf("\nPrinting the tree structure...\n");
                
                printTreeStructure(pharmacy->root, 0);
                break;
            case 15:
                SaveDataToFile("updated_medication.txt", pharmacy);
                printf("Data saved successfully to updated_medication.txt.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
                    

        }
    }

    return 0;
}
