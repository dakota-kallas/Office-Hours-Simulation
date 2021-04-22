/*
 * Author: Dakota Kallas
 * Date:   12/12/2020
 *
 * Description: Helpers for Prof. Office Hour Problem
 */

 #include "stu_list.h"

 /*
  * Create a node with value val, next = NULL and tail = NULL.
  */
 stu_node * create(student * stu){
   stu_node * new_node = (stu_node *) malloc(sizeof(stu_node));
   new_node->data = stu;
   new_node->next = NULL;
   new_node->prev = NULL;
   return new_node;
 }

 /*
  * Initialize an empty list
  * Allocate space for sentinel nodes and set their links.
  * Initialize size to 0.
  */
 void init_list(stu_list * the_list){
   the_list->head = create(NULL);
   the_list->tail = create(NULL);
   the_list->head->next = the_list->tail;
   the_list->tail->prev = the_list->head;
   the_list->size = 0;
 }

 /*
  * Append
  * Adds to the end of the list
  */
 void add(stu_list * the_list, student * stu){
   stu_node * new_node = create(stu);

   new_node->next = the_list->tail;
   new_node->prev = the_list->tail->prev;
   the_list->tail->prev->next = new_node;
   the_list->tail->prev = new_node;
   
   // increment size
   the_list->size++;
 }

 /*
  * Return the student at the beginning of the list.
  * If list is empty, return NULL.
  * Free the node, but do not free the student struct.
  */
 student * pop(stu_list * the_list){
   student * retval = NULL;
   stu_node * to_pop = the_list->head->next;
   if(to_pop == the_list->tail){
     return retval;
   }
   // remove from list
   retval = to_pop->data;
   the_list->head->next = to_pop->next;
   to_pop->next->prev = the_list->head;
   free(to_pop);
   the_list->size--;
   return retval;
 }

void print_student(student * stu){
  printf("Student %d: | Waiting = %d | Visit = %d | State = %s (", stu->id, stu->waiting, stu->visit, stu->name);
  switch(stu->curr_state){
    case INIT:
      printf("INIT)");
      break;
    case WAITING:
      printf("WAITING)");
      break;
    case VISITING:
      printf("VISITING)");
      break;
    case DONE:
      printf("DONE)");
      break;
    default:
      printf("Uh oh!)");
      break;
  }
}

 /*
  * Print
  */
 void print_list(stu_list * the_list){
   stu_node * cur_pos = the_list->head->next;
   printf("{\n");
   while(cur_pos != the_list->tail){
     printf("\t");
     print_student(cur_pos->data);

     if(cur_pos->next != the_list->tail){
       printf(",\n");
     }
     else {
       printf("\n");
     }
     cur_pos = cur_pos->next;
   }
   printf("}\n");
 }
