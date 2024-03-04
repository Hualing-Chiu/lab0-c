#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));

    // if(!new)
    //     new = NULL;
    if (!new) {
        free(new);
        return NULL;
    }

    INIT_LIST_HEAD(new);  // use the function in list.h

    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (head) {
        element_t *entry, *safe;
        list_for_each_entry_safe (entry, safe, head, list) {
            q_release_element(entry);
        }
        free(head);
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    int slen = strlen(s) + 1;
    node->value = malloc(slen * sizeof(char));

    if (node->value) {
        strncpy(node->value, s, slen);
        list_add(&node->list, head);
        return true;
    } else {
        free(node);
        return false;
    }
    return false;
    // if(!(node->value = strdup(s))){
    //     q_release_element(node);
    //     return false;
    // }
    // list_add(&node->list, head);
    // return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;

    int slen = strlen(s) + 1;
    node->value = malloc(slen * sizeof(char));

    if (node->value) {
        strncpy(node->value, s, slen);
        list_add_tail(&node->list, head);
        return true;
    } else {
        free(node);
        return false;
    }
    return false;
    //     if(!(node->value = strdup(s))){
    //         q_release_element(node);
    //         return false;
    //     }
    //     list_add_tail(&node->list, head);
    //     return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = list_first_entry(head, element_t, list);
    list_del(&node->list);

    if (sp && bufsize) {
        strncpy(sp, node->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = list_last_entry(head, element_t, list);
    list_del(&node->list);

    if (sp && bufsize) {
        strncpy(sp, node->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)  // modified
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *left = head->prev;
    struct list_head *right = head->next;

    while (left != right && left->next != right) {
        left = left->prev;
        right = right->next;
    }

    list_del(right);
    element_t *element = list_entry(right, element_t, list);
    q_release_element(element);
    // struct list_head **indir = &head;
    // for(struct list_head *fast = head; head == fast->next; fast =
    // fast->next->next){
    //     // prev = *indir;
    //     indir = &(*indir)->next;
    // }
    // // struct list_head *next = (*indir)->next;
    // //free(*indir);
    // list_del(*indir);
    // element_t *element = list_entry(*indir, element_t, list);
    // q_release_element(element);
    // // prev->next = next;
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head)) {
        return false;
    }
    element_t *cur, *next;
    bool flag = false;
    list_for_each_entry_safe (cur, next, head, list) {
        if (&next->list != head && !strcmp(cur->value, next->value)) {
            list_del(&cur->list);
            q_release_element(cur);
            flag = true;
        } else if (flag) {
            list_del(&cur->list);
            q_release_element(cur);
            flag = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    int count = k;
    struct list_head *node, *safe, tmp, *tmp_head;
    tmp_head = head;
    INIT_LIST_HEAD(&tmp);
    list_for_each_safe (node, safe, head) {
        count--;
        if (count == 0) {
            count = k;
            list_cut_position(&tmp, tmp_head, node);
            q_reverse(&tmp);
            list_splice(&tmp, tmp_head);
            // safe->prev = node;
            tmp_head = safe->prev;
        }
    }
}

struct list_head *mergeTwoLists(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head, *ptr;
    // head should be placed
    if (strcmp(list_entry(L1, element_t, list)->value,
               list_entry(L2, element_t, list)->value) <= 0) {
        head = L1;
        ptr = head;
        L1 = L1->next;
    } else {
        head = L2;
        ptr = head;
        L2 = L2->next;
    }

    while (L1 && L2) {
        if (strcmp(list_entry(L1, element_t, list)->value,
                   list_entry(L2, element_t, list)->value) <= 0) {
            ptr->next = L1;
            L1->prev = ptr;
            L1 = L1->next;
        } else {
            ptr->next = L2;
            L2->prev = ptr;
            L2 = L2->next;
        }
        ptr = ptr->next;
    }

    if (L1) {
        ptr->next = L1;
        L1->prev = ptr;
    } else {
        ptr->next = L2;
        L2->prev = ptr;
    }
    head->prev = NULL;
    return head;
}

struct list_head *mergeSort(struct list_head *head)
{
    if (!head->next)
        return head;

    struct list_head *slow = head;
    for (struct list_head *fast = head; fast && fast->next;
         fast = fast->next->next)
        slow = slow->next;
    // while (fast && fast->next) {
    //     slow = slow->next;
    //     fast = fast->next->next;
    // }

    slow->prev->next = NULL;
    struct list_head *left, *right;
    left = mergeSort(head);
    right = mergeSort(slow);
    return mergeTwoLists(left, right);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    head->prev->next = NULL;
    head->next = mergeSort(head->next);
    struct list_head *last = head;
    while (last->next)
        last = last->next;

    head->prev = last;
    last->next = head;
    head->next->prev = head;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    // int size = 0;
    element_t *cur = list_first_entry(head, element_t, list);
    while (cur->list.next != head) {
        element_t *next = list_first_entry(&cur->list, element_t, list);
        if (strcmp(next->value, cur->value) < 0) {
            list_del(&next->list);
            q_release_element(next);
        } else {
            // size++;
            cur = next;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    element_t *cur = list_last_entry(head, element_t, list);
    while (cur->list.prev != head) {
        element_t *prev = list_last_entry(&cur->list, element_t, list);
        if (strcmp(prev->value, cur->value) < 0) {
            list_del(&prev->list);
            q_release_element(prev);
        } else {
            cur = prev;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);

    queue_contex_t *q1 = container_of(head->next, queue_contex_t, chain);
    for (struct list_head *cur = head->next->next; cur != head;
         cur = cur->next) {
        queue_contex_t *q = container_of(cur, queue_contex_t, chain);
        list_splice_init(q->q, q1->q);
        q->size = 0;
    }
    q_sort(q1->q, false);
    q1->size = q_size(q1->q);
    return q1->size;
}
