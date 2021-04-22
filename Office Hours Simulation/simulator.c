/*
* Author: Dakota Kallas
* Date:   12/12/2020
* Description: Program to simulate office hours.
*/
#include <string.h>
#include <ctype.h>
#include "stu_list.h"

/* globals */

int num_students;
professor prof;         /* struct containing information about the professor */
student * all_students; /* dynamically allocated array of all of the students */
sched_t priority_mode;
int rr_quantum;
/* counters for final stats */
int tot_waiting = 0;
int tot_done = 0;
int tot_visit_min = 0;

/* function prototypes */
void read_data(FILE * infile);
stu_list * init_day(days_t today);
void cleanup_list(stu_list * today);
void simulate(stu_list * today, days_t day);
int is_all_done();

/* added methods for implementation */
void prepend(stu_list * the_list, student * stu);

/* Given print and helper functions */
void print_final_stats();
void print_init_stats();
void print_state();
void print_daily_stats(days_t d, int visits, int v_mins, int waiting, int wait_mins);
void correct_usage_message();
char * day_to_string(days_t d);
char * type_to_string(visit_t t);



int main(int argc, char** argv){
  /* init lists and other data structures */
  FILE * infile = NULL;  /* file descriptor for file from user */

  stu_list * daily = NULL; /* list of students that are visiting today */
  days_t day; /* current day */

  /* parse command-line arguments
   *    -f : FIFO (no priority)
   *    -a : advising takes priority
   *    -q : questions have priority
   *    -d : demos have priority
   *    file : the file to read in
   */
  /* Check to see if enough arguments are passed in */
  if(argc != 3) {
    fprintf(stdout, "Incorrect number of arguments.\n");
    correct_usage_message();
    return -1;
  }

  /* Set the queue type */
  if(strcmp(argv[1], "-f") == 0) {
    priority_mode = FIFO;
  }
  else if(strcmp(argv[1], "-a") == 0) {
    priority_mode = PRI_A;
  }
  else if(strcmp(argv[1], "-d") == 0) {
    priority_mode = PRI_D;
  }
  else if(strcmp(argv[1], "-q") == 0) {
    priority_mode = PRI_Q;
  }
  else {
    fprintf(stdout, "Incorrect flag for algorithm.\n");
    correct_usage_message();
    return -1;
  }

  infile = fopen(argv[2], "r");
  /* Check to see if the file was read in correctly */
  if(NULL == infile) {
    fprintf(stdout, "Unable to open file.\n");
    correct_usage_message();
    return -1;
  }

  /* read in the data */
  read_data(infile);

  /* print initial stats */
  print_init_stats();

  /******* Deliverable 1 ends here *******/


  /******* Deliverable 2: *******/

  /* Initialize the day to Monday */
  day = MONDAY;

  /* while there are still students who want to visit... */
  while(is_all_done() != 0){
    /*   for each day, init day with the students who can visit today */
    daily = init_day(day);


    /* simulate day */
    simulate(daily, day);

    /* get ready for the next day */
    cleanup_list(daily);
    free(daily);
    daily = NULL;
    day = (day + 1) % 5;
  }

  /* print final stats */
  print_final_stats();

  /* free all data structures */
  free(all_students);


  /*********** End Deliverable 2 *********/

  return 0;
}



/*
* Read in the data from the file infile.  Assume that infile has been opened
* for reading and is not NULL.
*/
void read_data(FILE * infile){
  /* TODO */
  if(infile == NULL) {
    printf("Error: file pointer is null");
    return;
  }

  /* Gather the Professor's Information */
  int max_line_length = 30, line = 1;
  char *fgets_rtn = NULL;  // variable to capture return code
  char buffer[ max_line_length ];  // where we are reading the data into

  /* Loop to go through the first 7 lines in the file to fill in professors info. */
  do {
    /* Read data (up to LINELEN chars) into buffer from stdin */
    fgets_rtn = fgets( buffer, max_line_length, infile );
    if( fgets_rtn != NULL ) {
      /* fgets was successful! */
      if( '\n' == buffer[ strlen(buffer)-1 ] ) {
	buffer[ strlen(buffer)-1 ] = '\0';
      }

      if(line == 1) {
       prof.name = strdup(buffer);
       prof.id = 0;
      }
      else if(line > 1 && line < 7) {
        prof.schedule[line-2] = strtol(buffer, NULL, 10);
      }
      else {
        num_students = strtol(buffer, NULL, 10);
      }

      line++;
    }
  } while( fgets_rtn != NULL && line < 8);

  /* Gather each Student's Information */
  all_students = (student*) malloc( sizeof(student) * num_students );
  if(all_students == NULL) {
    fprintf(stderr, "Error: Size was not correctly set for all_students");
    return;
  }

  /* Variable used to represent the current student it is looking at */
  int cur_student = -1;

  do {
    /* Read data (up to LINELEN chars) into buffer from stdin */
    fgets_rtn = fgets( buffer, max_line_length, infile );
    if( fgets_rtn != NULL ) {
      /* fgets was successful! */
      if( '\n' == buffer[ strlen(buffer)-1 ] ) {
	buffer[ strlen(buffer)-1 ] = '\0';
      }
      /* Find out the current student's name */
      if(((line - 8) % 4) == 0) {
        cur_student++;
        all_students[cur_student].name = strdup(buffer);
        all_students[cur_student].id = cur_student;
        all_students[cur_student].curr_state = INIT;
      }
      /* Find out how what type of visit the student requires */
      else if(((line - 9) % 4) == 0) {
      	char * visit_temp = strdup(buffer);
      	if(strcmp(visit_temp, "Q") == 0) {
      	  all_students[cur_student].visit_type = QUESTION;
      	}
      	else if(strcmp(visit_temp, "A") == 0) {
      	  all_students[cur_student].visit_type = ADVISING;
      	}
      	else if(strcmp(visit_temp, "D") == 0) {
      	  all_students[cur_student].visit_type = DEMO;
      	}
      }
      /* Find out how long the student needs to meet */
      else if(((line - 10) % 4) == 0) {
      	all_students[cur_student].visit = strtol(buffer, NULL, 10);
      }
      /* Determine what day the student is available */
      else if(((line - 11) % 4) == 0) {
      	char * temp = strdup(buffer);
      	int temp_length = strlen(temp);
      	int i;
      	for(i = 0; i < temp_length; i++) {
      	  if(temp[i] == 'M') {
      	    all_students[cur_student].available[0] = 1;
      	  }
      	  else if(temp[i] == 'T') {
      	    all_students[cur_student].available[1] = 1;
      	  }
      	  else if(temp[i] == 'W') {
      	    all_students[cur_student].available[2] = 1;
      	  }
      	  else if(temp[i] == 'R') {
      	    all_students[cur_student].available[3] = 1;
      	  }
      	  else if(temp[i] == 'F') {
      	    all_students[cur_student].available[4] = 1;
      	  }
      	}
      }

      line++;
    }
  } while( fgets_rtn != NULL);

  /* free up space and close the file */
  fclose(infile);
}

/*
* Simulate the day
*/
void simulate(stu_list * today, days_t day){
  int visits = 0;
  int visit_minutes = 0;
  int waiting = today->size;
  int waiting_student_minutes = 0;

  /* Find out how many long office hours are that day */
  int office_minutes = 0;
  if(day == MONDAY) {
    office_minutes = prof.schedule[0];
  }
  else if(day == TUESDAY) {
    office_minutes = prof.schedule[1];
  }
  else if(day == WEDNESDAY) {
    office_minutes = prof.schedule[2];
  }
  else if(day == THURSDAY) {
    office_minutes = prof.schedule[3];
  }
  else {
    office_minutes = prof.schedule[4];
  }

  /* Go through the list of students and set them all to WAITING to start */
  stu_node * cur_pos = today->head->next;
  while(cur_pos != today->tail) {
    cur_pos->data->curr_state = WAITING;
    cur_pos = cur_pos->next;
  }

  cur_pos = today->head->next;
  /* Go through the list of students that day and meet with them */
  while(cur_pos != today->tail) {
    /* If the current student still needs to meet with the professor, meet with them */
    if(cur_pos->data->curr_state != DONE) {
      /* Update the visitation and waiting variables */
      visits++;
      waiting--;
      waiting_student_minutes += visit_minutes;
      cur_pos->data->waiting += visit_minutes;
      cur_pos->data->curr_state = VISITING;

      /* Determine what the professor is doing for the current student */
      if(cur_pos->data->visit_type == QUESTION) {
        printf("Professor \%s is answering a question from \%s\n", prof.name, cur_pos->data->name);
      }
      else if(cur_pos->data->visit_type == DEMO) {
        printf("Professor \%s is viewing a demo from \%s\n", prof.name, cur_pos->data->name);
      }
      else if(cur_pos->data->visit_type == ADVISING) {
        printf("Professor \%s is advising \%s\n", prof.name, cur_pos->data->name);
      }

      visit_minutes += cur_pos->data->visit;

      /* Once the office hours are over, clean up the visitation and waiting minutes */
      if(visit_minutes >= office_minutes) {
        cur_pos->data->visit = visit_minutes - office_minutes;
        visit_minutes -= cur_pos->data->visit;
        break;
      }

      cur_pos->data->visit = 0;
      cur_pos->data->curr_state = DONE;

    }
    cur_pos = cur_pos->next;
  }

  /* Check to see if there are people that never met with the professor */
  cur_pos = today->head->next;
  while(cur_pos != today->tail) {
    if(cur_pos->data->curr_state == WAITING) {
      cur_pos->data->waiting += visit_minutes;
      waiting_student_minutes += visit_minutes;
    }
    cur_pos = cur_pos->next;
  }

  free(cur_pos);
  /* Print out the daily stats */
  print_daily_stats(day, visits, visit_minutes, waiting, waiting_student_minutes);
}

/*
* Create a new list and add all of the students who will be waiting that day
* to the list according to the priority mode.
*/
stu_list * init_day(days_t today){
  stu_list * a_list = NULL;
  int i;
  int day;

  /* Find out what day it is in terms of int */
  if(today == MONDAY) {
    day = 0;
  }
  else if(today == TUESDAY) {
    day = 1;
  }
  else if(today == WEDNESDAY) {
    day = 2;
  }
  else if(today == THURSDAY) {
    day = 3;
  }
  else {
    day = 4;
  }

  a_list = (struct stu_list *)malloc(sizeof(stu_list));
  /* Initiate the list for the day */
  init_list(a_list);

  /* Check for the priority type */
  if(priority_mode == PRI_A) {
    for(i = 0; i < num_students; i++) {
      /* If the student is available that day and is in for ADVISING... */
      if(all_students[i].available[day] != 0 && all_students[i].visit_type == ADVISING && all_students[i].curr_state != DONE) {
        add(a_list, &all_students[i]);
      }
    }
    for(i = 0; i < num_students; i++) {
      /* If the student is available that day and is not in for ADVISING... */
      if(all_students[i].available[day] != 0 && all_students[i].visit_type != ADVISING && all_students[i].curr_state != DONE) {
        add(a_list, &all_students[i]);
      }
    }
  }
  else if(priority_mode == PRI_Q) {
    for(i = 0; i < num_students; i++) {
      /* If the student is available that day and is in for QUESTION... */
      if(all_students[i].available[day] != 0 && all_students[i].visit_type == QUESTION && all_students[i].curr_state != DONE) {
        add(a_list, &all_students[i]);
      }
    }
    for(i = 0; i < num_students; i++) {
      /* If the student is available that day and is not in for QUESTION... */
      if(all_students[i].available[day] != 0 && all_students[i].visit_type != QUESTION && all_students[i].curr_state != DONE) {
        add(a_list, &all_students[i]);
      }
    }
  }
  else if(priority_mode == PRI_D) {
    for(i = 0; i < num_students; i++) {
      /* If the student is available that day and is in for DEMO... */
      if(all_students[i].available[day] != 0 && all_students[i].visit_type == DEMO && all_students[i].curr_state != DONE) {
        add(a_list, &all_students[i]);
      }
    }
    for(i = 0; i < num_students; i++) {
      /* If the student is available that day and is not in for DEMO... */
      if(all_students[i].available[day] != 0 && all_students[i].visit_type != DEMO && all_students[i].curr_state != DONE) {
        add(a_list, &all_students[i]);
      }
    }
  }
  else {
    for(i = 0; i < num_students; i++) {
      /* If the student is available that day add them to the waiting list */
      if(all_students[i].available[day] != 0 && all_students[i].curr_state != DONE) {
        add(a_list, &all_students[i]);
      }
    }
  }

  return a_list;
}

/*
 * Method used to make sure all items in the list are ready for use the next
 * day and free up space.
 */
void cleanup_list(stu_list * a_list){
  stu_node * cur_pos = a_list->head->next;
  while(cur_pos != a_list->tail) {
    if(cur_pos->data->visit <= 0) {
      cur_pos->data->curr_state = DONE;
    }
    else if(cur_pos->data->curr_state == VISITING) {
      cur_pos->data->curr_state = WAITING;
    }
    cur_pos = cur_pos->next;
    free(cur_pos->prev);
  }

  free(cur_pos);
}


/*
* Return 0 when all of the students have visited the professor.
*/
int is_all_done(){
  int i;
  /* Check the state of each student */
  for(i = 0; i < num_students; i++) {
    if(all_students[i].curr_state != DONE) {
      return -1;
    }
  }
  return 0;
}


/*******************************************************************************
 * Helpers and print functions                                                 *
 ******************************************************************************/

/*
* Print the initial statistics about the simulation
*/
void print_init_stats(){
  printf("Simulating Office Hours for Professor %s\n", prof.name);
  printf("-----------------------------------------------------------\n");
  switch(priority_mode){
    case FIFO:
      printf("Algorithm: FIFO\n");
      break;
    case PRI_A:
      printf("Algorithm: Priority (Advising)\n");
      break;
    case PRI_D:
      printf("Algorithm: Priority (Demos)\n");
      break;
    case PRI_Q:
      printf("Algorithm: Priority (Questions)\n");
      break;
    default:
      printf("Algorithm: Undefined\n");
      break;
  }

  printf("Schedule:\n");
  printf("%10s %10s %10s %10s %10s\n", "M", "T", "W", "R", "F");
  printf("-----------------------------------------------------------\n");
  printf("%10d %10d %10d %10d %10d\n", prof.schedule[MONDAY],
                                       prof.schedule[TUESDAY],
                                       prof.schedule[WEDNESDAY],
                                       prof.schedule[THURSDAY],
                                       prof.schedule[FRIDAY]);
  printf("===========================================================\n");
  printf("Initial list of students:\n");
  print_state();
  printf("===========================================================\n");
}
/*
* Print the final statistics about the simulation
*/
void print_final_stats(){
  printf("-----------------------------------------------------------\n");
  printf("Professor %s met with %d students for %d minutes\nwith a total waiting time of %d.\n",
              prof.name, num_students, tot_visit_min, tot_waiting);
  printf("===========================================================\n");

}

/*
* Print the state of the simulation - used for debugging
*/
void print_state(){
  int i;
  for(i = 0; i < num_students; i++){
    printf("%d %s %d %d ", all_students[i].id, all_students[i].name, all_students[i].visit, all_students[i].waiting);
    switch(all_students[i].visit_type){
      case QUESTION:
        printf("Q ");
        break;
      case ADVISING:
        printf("A ");
        break;
      case DEMO:
        printf("D ");
        break;
      default:
        printf("uh oh! ");
        break;
    }
    switch(all_students[i].curr_state){
      case INIT:
        printf("-- init\n");
        break;
      case WAITING:
        printf("-- waiting\n");
        break;
      case VISITING:
        printf("-- visiting\n");
        break;
      case DONE:
        printf("-- done\n");
        break;
      default:
        printf("-- uh oh!\n");
        break;
    }
  }
}


void print_daily_stats(days_t d, int visits, int v_mins, int waiting, int wait_mins){
  printf("Stats for %s:\n", day_to_string(d));
  printf("\t Visits: %10d   |   Visit Minutes: %10d\n", visits, v_mins);
  printf("\t Waiting: %9d   |   Waiting Minutes: %8d\n", waiting, wait_mins);
  printf("-----------------------------------------------------------\n");
  tot_waiting += wait_mins;
  tot_visit_min += v_mins;
}

char * day_to_string(days_t d){
  switch(d){
    case MONDAY:
      return strdup("Monday");
    case TUESDAY:
      return strdup("Tuesday");
    case WEDNESDAY:
      return strdup("Wednesday");
    case THURSDAY:
      return strdup("Thursday");
    case FRIDAY:
      return strdup("Friday");
    default:
      return strdup("Uh oh!");
  }
}

char * type_to_string(visit_t t){
  switch(t){
    case QUESTION:
      return strdup("answering a question from");
    case ADVISING:
      return strdup("advising");
    case DEMO:
      return strdup("viewing a demo from");
    default:
      return strdup("Uh oh!");
  }
}

void correct_usage_message(){
  printf("Please use -f to indicate FIFO or -a, -d, or -q to prioritize a visit type, and provide the name of the file for configuring the simulation.\n");
}
