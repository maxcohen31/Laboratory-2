/*
    Implementation of a BST
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _Node
{
    int val;
    struct _Node *left;
    struct _Node *right;

} Node;


void preorder(Node *root)
{
    if (root == NULL)
    {
        return;
    }

    printf("%d ", root->val);
    preorder(root->left);
    preorder(root->right);
    
}


void inorder(Node *root)
{
    if (root == NULL)
    {
        return;
    }

    inorder(root->left);
    printf("%d ", root->val);
    inorder(root->right);
}


Node *create_node(int val)
{
    Node *new_node = malloc(sizeof(Node));
    new_node->val = val;
    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

Node *insert_node(Node *root, int val)
{
    if (root == NULL)
    {
        root = create_node(val);
        return root;
    }

    if (root->val < val)
    {
        root->right = insert_node(root->right, val);
    }
    else 
    {
        root->left = insert_node(root->left, val);
    }

    return root;
}

Node *find_min(Node *root)
{ 
    while (root->left != NULL)
    {
        root = root->left;
    }
    return root;
}

Node *delete_node(Node *root, int val)
{
    if (root == NULL)
    {
        return NULL;
    }

    // searching the left subtree 
    if (val < root->val)
    {
        root->left = delete_node(root->left, val);
    }

    else if (val > root->val)
    {
        root->right = delete_node(root->right, val);
    }
    else 
    {
        // no children
        if (root->left == NULL && root->right == NULL) 
        {
            free(root);
            root = NULL;
        }

        // 1 child
        if (root->left == NULL)
        {
            Node *tmp = root;
            root = root->right;
            free(tmp);
        }
        else if (root->right == NULL)
        {
            Node *tmp = root;
            root = root->left;
            free(tmp);
        }
        // 2 children
        else 
        {
            // find minimum in right subtree
            Node *min_node = find_min(root->right);
            root->val = min_node->val;
            root->right = delete_node(root->right, min_node->val); 
        }
    }

    return root;
}

void free_tree(Node* root)
{
    if (root == NULL)
    {
        return;
    }
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main(int argc, char **argv)
{
    Node *root = NULL;
    root = insert_node(root, 10);
    root = insert_node(root, 7);
    root = insert_node(root, 72);
    root = insert_node(root, 2);
    

    preorder(root);
    root = delete_node(root, 7);
    puts("\n===============");
    inorder(root);
    free_tree(root);

    return 0;
}




