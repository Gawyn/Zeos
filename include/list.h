 /*
  * PROSO - Simple doubly linked list
  */
#ifndef LIST_H
#define LIST_H

 /* This type must be used on every new type using these lists */
 struct list_head {
         struct list_head *next, *prev;
 };

 struct list {
         struct list_head *first, *last;
 };
 
 /* Functions */

 /**
  * list_init - initializes list
  * @l: list to be initialized
  * returns: 0=OK, -1=ERROR
  */
 int list_init(struct list *l);
 
 /**
  * list_add - add a new entry at the end of the list
  * @new: new entry to be added
  * @l: list 
  * returns: 0=OK, -1=ERROR
  *
  * Insert a new entry after the specified head.
  * This is good for implementing stacks.
  */
 int list_add(struct list_head *new, struct list *l);

 /**
  * list_del - deletes entry from list.
  * @entry: the element to delete from the list.
  * @l: list 
  * returns: 0=OK, -1=ERROR
  */
 int list_del(struct list_head *entry, struct list *l);

 /**
  * list_first - Returns a pointer to the first element of the list 
  * @l: list 
  * returns: NULL =ERROR, not NULL =OK
  */
 struct list_head * list_first(struct list *l);

 /**
  * list_next - returns a pointer to the next element of the entry asked 
  * @entry: element to ask for the next 
  */
 struct list_head * list_next(struct list_head *entry);

 /**
  * list_empty - checks if the list is empty
  * @l: list 
  * returns: 1: empty / 0: not empty / -1: ERROR
  */
 int list_empty(struct list *l);

#endif
