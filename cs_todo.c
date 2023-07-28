#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVALID_PRIORITY -1

#define MAX_TASK_LENGTH 200
#define MAX_CATEGORY_LENGTH 40
#define MAX_STRING_LENGTH 1024

// You *should* #define each command
#define COMMAND_ADD_TASK 'a'
#define COMMAND_PRINT_TASK 'p'
#define COMMAND_UPDATE_PRIORITY 'i'
#define COMMAND_PRINT_NUMBER 'n'
#define COMMAND_TASK_COMPLETION 'c'

enum priority { LOW, MEDIUM, HIGH };

struct task {
    char task_name[MAX_TASK_LENGTH];
    char category[MAX_CATEGORY_LENGTH];
    enum priority priority;

    struct task *next;
};

struct completed_task {
    struct task *task;
    int start_time;
    int finish_time;
    struct completed_task *next;
};

struct todo_list {
    struct task *tasks;
    struct completed_task *completed_tasks;
};

////////////////////////////////////////////////////////////////////////////////
///////////////////// YOUR FUNCTION PROTOTYPES GO HERE /////////////////////////
////////////////////////////////////////////////////////////////////////////////



void command_loop(struct todo_list *todo);

void add_task(struct todo_list *todo, struct task *t);

int has_same_task(struct todo_list *todo, struct task *t);

struct task *get_task(struct todo_list *todo,
        char task_name[MAX_TASK_LENGTH],
        char task_category[MAX_CATEGORY_LENGTH]);

void print_all_tasks(struct todo_list *todo);

int number_tasks(struct todo_list *todo);

void remove_task(struct todo_list *todo, struct task *t);

void add_completed_task(struct todo_list *todo, struct completed_task *t_d);

void print_completed_tasks(struct todo_list *todo);

////////////////////////////////////////////////////////////////////////////////
//////////////////////// PROVIDED HELPER PROTOTYPES ////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void parse_add_task_line(
        char buffer[MAX_STRING_LENGTH], char task_name[MAX_TASK_LENGTH],
        char task_category[MAX_CATEGORY_LENGTH], enum priority *prio
);
void parse_task_category_line(
        char buffer[MAX_STRING_LENGTH], char task_name[MAX_TASK_LENGTH],
        char task_category[MAX_CATEGORY_LENGTH]
);
void parse_complete_task_line(
        char buffer[MAX_STRING_LENGTH], char task_name[MAX_TASK_LENGTH],
        char task_category[MAX_CATEGORY_LENGTH], int *start_time, int *finish_time
);

enum priority string_to_priority(char priority[MAX_STRING_LENGTH]);
void remove_newline(char input[MAX_STRING_LENGTH]);
void trim_whitespace(char input[MAX_STRING_LENGTH]);
void print_one_task(int task_num, struct task *task);
void print_completed_task(struct completed_task *completed_task);

int task_compare(struct task *t1, struct task *t2);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main(void) {
    // Stage 1.1
    // You should initialize the `todo` variable below. You will need
    // to use the malloc() function to allocate memory for it!
    struct todo_list *todo;

    todo = malloc(sizeof(struct todo_list));
    todo->tasks = NULL;
    todo->completed_tasks = NULL;

    puts("Welcome to CS ToDo!");

    command_loop(todo);

    puts("All done!");

    return 0;
}

/**
 * The central loop that executes commands until the program is completed.
 *
 * Parameters:
 *     todo - The todo list to execute commands on.
 *
 * Returns:
 *     Nothing
 */
void command_loop(struct todo_list *todo) {
    printf("Enter Command: ");
    char command;
    char buffer[MAX_STRING_LENGTH];
    int print_type = 0;
    while (scanf(" %c", &command) == 1) {
        // TODO: Add to this loop as you complete the assignment
        if (command == COMMAND_ADD_TASK) {
            // TODO: Add a t to the todo list here
            fgets(buffer, MAX_STRING_LENGTH, stdin);
            struct task *t = malloc(sizeof(struct task));
            t->next = NULL;
            parse_add_task_line(buffer, t->task_name, t->category, &t->priority);
            // add to list
            add_task(todo, t);
        }else if(command == COMMAND_PRINT_TASK){
            if(print_type == 0){
                print_all_tasks(todo);
            }else{
                print_completed_tasks(todo);
            }
            print_type = !print_type;
        }else if(command == COMMAND_UPDATE_PRIORITY){
            fgets(buffer, MAX_STRING_LENGTH, stdin);

            char task[MAX_TASK_LENGTH];
            char category[MAX_CATEGORY_LENGTH];
            parse_task_category_line(buffer, task, category);

            struct task *t = get_task(todo, task, category);

            if(t == NULL){
                printf("Could not find task '%s' in category '%s'.\n", task, category);
            }else{
                if(t->priority == LOW){
                    t->priority = MEDIUM;
                }else if(t->priority == MEDIUM){
                    t->priority = HIGH;
                }else if(t->priority == HIGH){
                    t->priority = LOW;
                }
            }
        }else if(command == COMMAND_PRINT_NUMBER){
            printf("There are %d items on your list!\n", number_tasks(todo));
        }else if(command == COMMAND_TASK_COMPLETION){
            fgets(buffer, MAX_STRING_LENGTH, stdin);

            char task[MAX_TASK_LENGTH];
            char category[MAX_CATEGORY_LENGTH];
            int start_time;
            int finish_time;
            parse_complete_task_line(buffer, task, category, &start_time, &finish_time);

            struct task *t = get_task(todo, task, category);
            if(t == NULL){
                printf("Could not find task '%s' in category '%s'.\n", task, category);
            }else{
                remove_task(todo, t);
                // create completed task
                struct completed_task *t_d = malloc(sizeof(struct completed_task));
                t_d->task = t;
                t_d->next = NULL;
                t_d->start_time = start_time;
                t_d->finish_time = finish_time;
                // add completed task
                add_completed_task(todo, t_d);
            }
        }

        // reset print type
        if(command != COMMAND_PRINT_TASK){
            print_type = 0;
        }

        printf("Enter Command: ");
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////// YOUR HELPER FUNCTIONS ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////


// You should add any helper functions you create here

void add_task(struct todo_list *todo, struct task *t) {
    // skip same task
    if(has_same_task(todo, t)){
        return;
    }
    if(todo->tasks == NULL){
        todo->tasks = t;
    }else{
        // add task to the last
        struct task *ct = todo->tasks;
        while(ct->next != NULL){
            ct = ct->next;
        }
        ct->next = t;
    }
}

int has_same_task(struct todo_list *todo, struct task *t) {
    struct task *ct = todo->tasks;
    while(ct != NULL){
        if(strcmp(ct->task_name, t->task_name) == 0 && strcmp(ct->category, t->category) == 0){
            return 1;
        }
        ct = ct->next;
    }
    return 0;
}

struct task *get_task(struct todo_list *todo,
        char task_name[MAX_TASK_LENGTH],
        char task_category[MAX_CATEGORY_LENGTH]){
    struct task *ct = todo->tasks;
    while(ct != NULL){
        if(strcmp(ct->task_name, task_name) == 0 && strcmp(ct->category, task_category) == 0){
            return ct;
        }
        ct = ct->next;
    }
    return NULL;
}

void print_all_tasks(struct todo_list *todo){
    struct task *ct = todo->tasks;
    puts("==== Your ToDo List ====");
    if(ct == NULL){
        puts("All tasks completed, you smashed it!");
    }else{
        int task_num = 1;
        while(ct != NULL){
            print_one_task(task_num, ct);
            task_num++;
            ct = ct->next;
        }
    }
    puts("====   That's it!   ====");
}

int number_tasks(struct todo_list *todo){
    int cnt = 0;
    struct task *ct = todo->tasks;
    while(ct != NULL){
        cnt += 1;
        ct = ct->next;
    }
    return cnt;
}

void remove_task(struct todo_list *todo, struct task *t){
    struct task *ct = todo->tasks;
    if(ct == t){
        todo->tasks = todo->tasks->next;
        return;
    }
    while(ct->next != NULL){
        if(ct->next == t){
            ct->next = t->next;
            break;
        }
        ct = ct->next;
    }
}

void add_completed_task(struct todo_list *todo, struct completed_task *t_d){
    if(todo->completed_tasks == NULL){
        todo->completed_tasks = t_d;
        return;
    }
    struct completed_task *ct_d = todo->completed_tasks;
    while(ct_d->next != NULL){
        ct_d = ct_d->next;
    }
    ct_d->next = t_d;
}

void print_completed_tasks(struct todo_list *todo){
    struct completed_task *ct_d = todo->completed_tasks;
    puts("==== Completed Tasks ====");
    if(ct_d == NULL){
        puts("No tasks have been completed today!");
    }else{
        while(ct_d != NULL){
            print_completed_task(ct_d);
            ct_d = ct_d->next;
        }
    }

    puts("=========================");
}


////////////////////////////////////////////////////////////////////////////////
/////////////////////// PROVIDED HELPER FUNCTIONS //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * Helper Function
 * You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
 *
 * Given a raw string in the format: [string1] [string2] [enum priority]
 * This function will extract the relevant values into the given variables.
 * The function will also remove any newline characters.
 *
 * For example, if given: "finish_assignment_2 assignment2 high"
 * The function will copy the string:
 *     "finish_assignment_2" into the `task_name` array
 * Then copy the string:
 *     "assignment2" into the `task_category` array
 * And finally, copy over the enum:
 *     "high" into the memory that `prio` is pointing at.
 *
 * Parameters:
 *     buffer        - A null terminated string in the following format
 *                     [string1] [string2] [enum priority]
 *     task_name     - A character array for the [string1] to be copied into
 *     task_category - A character array for the [string2] to be copied into
 *     prio          - A pointer to where [enum priority] should be stored
 *
 * Returns:
 *     None
 */
void parse_add_task_line(
        char buffer[MAX_STRING_LENGTH],
        char task_name[MAX_TASK_LENGTH],
        char task_category[MAX_CATEGORY_LENGTH],
        enum priority *prio
) {
    remove_newline(buffer);

    // Extract value 1 as string
    char *name_str = strtok(buffer, " ");
    if (name_str != NULL) {
        strcpy(task_name, name_str);
    }

    // Extract value 2 as string
    char *category_str = strtok(NULL, " ");
    if (category_str != NULL) {
        strcpy(task_category, category_str);
    }

    // Extract value 3 as string
    char *prio_str = strtok(NULL, " ");
    if (prio_str != NULL) {
        *prio = string_to_priority(prio_str);
    }

    if (
            name_str == NULL ||
            category_str == NULL ||
            prio_str == NULL ||
            *prio == INVALID_PRIORITY
            ) {
        // If any of these are null, there were not enough words.
        printf("Could not properly parse line: '%s'.\n", buffer);
    }
}

/*
 * Helper Function
 * You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
 *
 * See `parse_add_task_line` for explanation - This function is very similar,
 * with only the exclusion of an `enum priority`.
 */
void parse_task_category_line(
        char buffer[MAX_STRING_LENGTH],
        char task_name[MAX_TASK_LENGTH],
        char task_category[MAX_CATEGORY_LENGTH]
) {
    remove_newline(buffer);

    // Extract value 1 as string
    char *name_str = strtok(buffer, " ");
    if (name_str != NULL) {
        strcpy(task_name, name_str);
    }

    // Extract value 2 as string
    char *category_str = strtok(NULL, " ");
    if (category_str != NULL) {
        strcpy(task_category, category_str);
    }

    if (name_str == NULL || category_str == NULL) {
        // If any of these are null, there were not enough words.
        printf("Could not properly parse line: '%s'.\n", buffer);
    }
}

/*
 * Helper Function
 * You DO NOT NEED TO UNDERSTAND THIS FUNCTION, and will not need to change it.
 *
 * See `parse_add_task_line` for explanation - This function is very similar,
 * with only the exclusion of an `enum priority` and addition of start/end times
 */
void parse_complete_task_line(
        char buffer[MAX_STRING_LENGTH],
        char task_name[MAX_TASK_LENGTH],
        char task_category[MAX_CATEGORY_LENGTH],
        int *start_time,
        int *finish_time
) {
    remove_newline(buffer);

    // Extract value 1 as string
    char *name_str = strtok(buffer, " ");
    if (name_str != NULL) {
        strcpy(task_name, name_str);
    }

    // Extract value 2 as string
    char *category_str = strtok(NULL, " ");
    if (category_str != NULL) {
        strcpy(task_category, category_str);
    }

    // Extract value 2 as string
    char *start_str = strtok(NULL, " ");
    if (start_str != NULL) {
        *start_time = atoi(start_str);
    }

    // Extract value 2 as string
    char *finish_str = strtok(NULL, " ");
    if (finish_str != NULL) {
        *finish_time = atoi(finish_str);
    }

    if (
            name_str == NULL ||
            category_str == NULL ||
            start_str == NULL ||
            finish_str == NULL
            ) {
        // If any of these are null, there were not enough words.
        printf("Could not properly parse line: '%s'.\n", buffer);
    }
}

/**
 * Helper Function
 * You should not need to change this function.
 *
 * Given a raw string, will return the corresponding `enum priority`,
 * or INVALID_PRIORITY if the string doesn't correspond with the enums.
 *
 * Parameters:
 *     priority - string representing the corresponding `enum priority` value
 * Returns:
 *     enum priority
 */
enum priority string_to_priority(char priority[MAX_STRING_LENGTH]) {
    if (strcmp(priority, "low") == 0) {
        return LOW;
    } else if (strcmp(priority, "medium") == 0) {
        return MEDIUM;
    } else if (strcmp(priority, "high") == 0) {
        return HIGH;
    }

    return INVALID_PRIORITY;
}

/**
 * Helper Function
 * You should not need to change this function.
 *
 * Given an priority and a character array, fills the array with the
 * corresponding string version of the priority.
 *
 * Parameters:
 *     prio - the `enum priority` to convert from
 *     out  - the array to populate with the string version of `prio`.
 * Returns:
 *     Nothing
 */
void priority_to_string(enum priority prio, char out[MAX_STRING_LENGTH]) {
    if (prio == LOW) {
        strcpy(out, "LOW");
    } else if (prio == MEDIUM) {
        strcpy(out, "MEDIUM");
    } else if (prio == HIGH) {
        strcpy(out, "HIGH");
    } else {
        strcpy(out, "Provided priority was invalid");
    }
}

/*
 * Helper Function
 * You should not need to change this function.
 *
 * Given a raw string will remove and first newline it sees.
 * The newline character wil be replaced with a null terminator ('\0')
 *
 * Parameters:
 *     input - The string to remove the newline from
 *
 * Returns:
 *     Nothing
 */
void remove_newline(char input[MAX_STRING_LENGTH]) {
    // Find the newline or end of string
    int index = 0;
    while (input[index] != '\n' && input[index] != '\0') {
        index++;
    }
    // Goto the last position in the array and replace with '\0'
    // Note: will have no effect if already at null terminator
    input[index] = '\0';
}

/*
 * Helper Function
 * You likely do not need to change this function.
 *
 * Given a raw string, will remove any whitespace that appears at the start or
 * end of said string.
 *
 * Parameters:
 *     input - The string to trim whitespace from
 *
 * Returns:
 *     Nothing
 */
void trim_whitespace(char input[MAX_STRING_LENGTH]) {
    remove_newline(input);

    int lower;
    for (lower = 0; input[lower] == ' '; ++lower);

    int upper;
    for (upper = strlen(input) - 1; input[upper] == ' '; --upper);

    for (int base = lower; base <= upper; ++base) {
        input[base - lower] = input[base];
    }

    input[upper - lower + 1] = '\0';
}

/**
 * Helper Function
 * You SHOULD NOT change this function.
 *
 * Given a task, prints it out in the format specified in the assignment.
 *
 * Parameters:
 *     task_num - The position of the task within a todo list
 *     task     - The task in question to print
 *
 * Returns:
 *     Nothing
 */
void print_one_task(int task_num, struct task *task) {
    char prio_str[MAX_STRING_LENGTH];
    priority_to_string(task->priority, prio_str);

    printf(
            "  %02d. %-30.30s [ %s ] %s\n",
            task_num, task->task_name, task->category, prio_str
    );

    int i = 30;
    while (i < strlen(task->task_name)) {
        printf("      %.30s\n", task->task_name + i);
        i += 30;
    }
}

/**
 * Helper Function
 * You SHOULD NOT change this function.
 *
 * Given a completed task, prints it out in the format specified in the
 * assignment.
 *
 * Parameters:
 *     completed_task - The task in question to print
 *
 * Returns:
 *     Nothing
 */
void print_completed_task(struct completed_task *completed_task) {
    int start_hour = completed_task->start_time / 60;
    int start_minute = completed_task->start_time % 60;
    int finish_hour = completed_task->finish_time / 60;
    int finish_minute = completed_task->finish_time % 60;

    printf(
            "  %02d:%02d-%02d:%02d | %-30.30s [ %s ]\n",
            start_hour, start_minute, finish_hour, finish_minute,
            completed_task->task->task_name, completed_task->task->category
    );

    int i = 30;
    while (i < strlen(completed_task->task->task_name)) {
        printf("      %.30s\n", (completed_task->task->task_name + i));
        i += 30;
    }
}

/**
 * Compares two tasks by precedence of category -> priority -> name and returns
 * an integer referring to their relative ordering
 * 
 * Parameters:
 *     t1 - The first task to compare
 *     t2 - The second task to compare
 *
 * Returns:
 *     a negative integer if t1 belongs before t2
 *     a positive integer if t1 belongs after t2
 *     0 if the tasks are identical (This should never happen in your program)
 */
int task_compare(struct task *t1, struct task *t2) {
    int category_diff = strcmp(t1->category, t2->category);
    if (category_diff != 0) {
        return category_diff;
    }

    int priority_diff = t2->priority - t1->priority;
    if (priority_diff != 0) {
        return priority_diff;
    }

    return strcmp(t1->task_name, t2->task_name);
}
