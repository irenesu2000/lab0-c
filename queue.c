#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */
static inline void l_merge(struct list_head *head,
                           struct list_head *left,
                           struct list_head *right);

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *curr, *tmp;
    list_for_each_entry_safe (curr, tmp, l, list) {
        list_del(&curr->list);
        q_release_element(curr);
    }
    free(l);
}

/**
 * q_insert_head() - Insert an element in the head
 * @head: header of queue
 * @s: string would be inserted
 *
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 *
 * Return: true for success, false for allocation failed or queue is NULL
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *newnode = malloc(sizeof(element_t));
    if (!newnode)
        return false;
    newnode->value = strdup(s);
    if (!newnode->value) {
        free(newnode);
        return false;
    }
    list_add(&newnode->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *newnode = malloc(sizeof(element_t));
    if (!newnode)
        return false;
    newnode->value = strdup(s);
    if (!newnode->value) {
        free(newnode);
        return false;
    }
    list_add_tail(&newnode->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *target = list_first_entry(head, element_t, list);
    list_del_init(&target->list);

    if (sp) {
        memcpy(sp, target->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return target;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *target = list_last_entry(head, element_t, list);
    list_del_init(&target->list);

    if (sp) {
        memcpy(sp, target->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return target;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
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
    if (!head)
        return NULL;
    int size = q_size(head);
    int index;
    if (size % 2 == 1) {
        index = size / 2 + 1;
    } else {
        index = size / 2;
    }

    struct list_head *tmp = head;
    for (int i = 0; i <= index; i++) {
        if (i == index) {
            list_del(tmp);
            q_release_element(list_entry(tmp, element_t, list));
        }
        tmp = tmp->next;
    }
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return NULL;
    struct list_head *node = head->next;
    struct list_head *dup;
    while (node->next != head) {
        if (strcmp(list_entry(node, element_t, list)->value,
                   list_entry(node->next, element_t, list)->value) == 0) {
            dup = node->next;
            list_del(dup);
            q_release_element(list_entry(dup, element_t, list));
        } else {
            node = node->next;
        }
    }
    return true;
}

/**
 * list_swap - replace entry1 with entry2 and re-add entry1 at entry2's position
 * @entry1: the location to place entry2
 * @entry2: the location to place entry1
 */
/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *first = head->next;
    struct list_head *second = first->next;
    while (first != head && second != head) {
        first->prev->next = second;
        second->prev = first->prev;
        first->prev = second;
        first->next = second->next;
        second->next->prev = first;
        second->next = first;

        first = first->next;
        second = first->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    for (struct list_head *i = head; i->next != head->prev; i = i->next) {
        list_move(head->prev, i);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}


static inline void l_merge(struct list_head *head,
                           struct list_head *left,
                           struct list_head *right)
{
    while (!list_empty(left) && !list_empty(right)) {
        if (strcmp(list_entry(left->next, element_t, list)->value,
                   list_entry(right->next, element_t, list)->value) < 0) {
            list_move_tail(left->next, head);
        } else {
            list_move_tail(right->next, head);
        }
    }

    if (!list_empty(left)) {
        list_splice_tail_init(left, head);
    } else {
        list_splice_tail_init(right, head);
    }
}
/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *fast = head, *slow = head;
    do {
        fast = fast->next->next;
        slow = slow->next;
    } while (fast != head && fast->next != head);

    LIST_HEAD(left);
    LIST_HEAD(right);
    list_splice_tail_init(head, &right);
    list_cut_position(&left, &right, slow);
    q_sort(&left);
    q_sort(&right);
    l_merge(head, &left, &right);
}
/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return q_size(head);
    }
    char *max = list_entry(head->next, element_t, list)->value;
    for (struct list_head *i = head; i->next != head->prev; i = i->next) {
        if (strcmp(list_entry(i, element_t, list)->value, max) > 0) {
            max = list_entry(i, element_t, list)->value;
        }
    }
    struct list_head *check = head->prev;
    struct list_head *del;
    bool flag = 0;
    while (check != head) {
        if (check == head->prev) {
            if (strcmp(list_entry(check, element_t, list)->value, max) < 0) {
                check = check->prev;
            } else {
                del = check;
                check = check->prev;
                list_del(del);
            }
        } else {
            if (strcmp(list_entry(check, element_t, list)->value, max) == 0) {
                flag = 1;
                check = check->prev;
            }
            if (strcmp(list_entry(check, element_t, list)->value,
                       list_entry(check->next, element_t, list)->value) < 0 ||
                flag == 1) {
                del = check;
                check = check->prev;
                list_del(del);
            } else {
                check = check->prev;
            }
        }
    }
    return q_size(head);
}


/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
