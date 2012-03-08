/*
 * list.c - Simple doubly linked list
 */

#include <list.h>
int list_init(struct list *l)
{
	if (l){
		l->first = 0; //Inicialitzem els paràmetres
		l->last = 0;
		return 0;
	}
	else{
		return -1; //Si la llista no existeix, retornem error
	}
}

int list_add(struct list_head *new, struct list *l)
{

	if (!l || !new) return -1;
	
	if ((l->first == 0) && (l->last == 0)) { //si la llista està buida
		l->first = new;
		l->last = new;
		new->prev = new;
		new->next = new;
		return 0;
	}

	l->first->prev = new;
	l->last->next = new;
	new->prev = l->last;
	new->next = l->first;
	l->last = new;
	return 0;

}

int list_del(struct list_head *entry, struct list *l)
{
	if (!entry || !l) return -1;

	if (list_empty(l) == 1) return -1;

	if ((l->first == l->last) && (l->first == entry)){ //Si la llista només té un element, i l'element es el que volem borrar
		l->first = 0;
		l->last = 0;
		entry->next = 0;
		entry->prev = 0;
		return 0;
	}

	if (l->first == entry){ //Si el primer element de la llista és el que busquem, i la llista té més d'un element
		l->last->next = l->first->next;
		l->first->next->prev = l->last;
		l->first = l->first->next;
		entry->next = 0;
		entry->prev = 0;
		return 0;
	}

	if (l->last == entry){ //Si l'element és l'últim de la llista, i la llista té més d'un element
		l->first->prev = l->last->prev;
		l->last->prev->next = l->first;
		l->last = l->last->prev;
		entry->next = 0;
		entry->prev = 0;
		return 0;
		}

	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
	entry->next = 0;
	entry->prev = 0;
	return 0;

}
struct list_head * list_first(struct list *l)
{
	if (l){
		return l->first;
	}
	else {
		return 0;
	}
}

struct list_head * list_next(struct list_head *entry)
{
	if (entry){
		return entry->next;
	}
	else{
		return 0;
	}
}

int list_empty(struct list *l)
{
	if (!l){
		return -1; //Si la llista no existeix, donem error
	}
	
	if (l->first){
		return 0; //Si la llista té algun contingut, retornem 0
	}

	return 1; //Si la llista no té res, retornem 1
}

