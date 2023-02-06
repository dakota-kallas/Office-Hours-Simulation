/*
 * Date:   12/12/2020
 *
 * Description: Helpers for Prof. Office Hour Problem
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Globals
 */


/*
 * First we need some data structures for our list.
 */

 /*
  * Priority Scheduling types
  */
 enum sched_t {FIFO, PRI_Q, PRI_A, PRI_D};
 typedef enum sched_t sched_t;

/*
 * Visit types
 */
enum visit_t {QUESTION, ADVISING, DEMO};
typedef enum visit_t visit_t;

/*
 * Days of the week enum for convenience
 */
enum days_t {MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY};
typedef enum days_t days_t;

/*
 * States for students
 */
enum state_t {INIT, WAITING, VISITING, DONE};
typedef enum state_t state_t;

/*
 * Student struct
 * Stores information about a student
 */
struct student {
  int id;
  char * name;
  int visit;
  int waiting;
  visit_t visit_type;
  // 0 for not available, non-zero for available
  int available[5];
  state_t curr_state;
};
typedef struct student student;

/*
 * Professor struct
 * Store information about a professor
 */
struct professor {
  int id;
  char * name;
  student * curr_student;
  // array of office hour lengths
  int schedule[5];
};
typedef struct professor professor;

/*
 * Node struct
 * Stores a value and a pointer to the next thing in the list.
 */
struct stu_node {
  // data
  student * data;
  // links
  struct stu_node * next;
  struct stu_node * prev;
};
typedef struct stu_node stu_node;

/*
 * List struct
 * Doubly-linked list with sentinel nodes for head and tail.
 */
struct stu_list {
  stu_node * head;
  stu_node * tail;
  int size;
};
typedef struct stu_list stu_list;

/******************************************************************/

/*
 * Now we need some operations that are performed on lists.
 */

/*
 * Create a node with value val, next = NULL and tail = NULL.
 */
stu_node * create(student * stu);

/*
 * Initialize an empty list
 * Allocate space for sentinel nodes and set their links.
 * Initialize size to 0.
 */
void init_list(stu_list * the_list);

/*
 * Append
 * Adds student to the end of the list.
 */
void add(stu_list * the_list, student * stu);

/*
 * Return the student at the beginning of the list.
 * If list is empty, return NULL.
 * Free the node, but do not free the student struct.
 */
student * pop(stu_list * the_list);

/*
 * Print a list
 */
void print_list(stu_list * the_list);

/*
 * Print a student's id, name and state
 */
void print_student(student * stu);
