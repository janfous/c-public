#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#define MAX_SIZE_LOG 1000000
#define MAX_SIZE_IDS 100000
#define MAX_INPUT_LEN 20

#define ID_INTERVAL_BEG 0
#define ID_INTERVAL_END 99999

// add: + (int)x
// result: add new access with ID=x
#define ADD_IN_CNT 2
#define ADD_OPERATION '+'

// query: ? (int)x (int)y
// result: number of unique IDs in accesses n. x-y
// ex.: + 1; + 2; + 2; ? 0 2 -> 2 / 3  (2 unique IDs out of 3 total)
#define QUERY_IN_CNT 3
#define QUERY_OPERATION '?'

#define FALSE 0
#define TRUE  1

// single line, starts with '?' or '+' followed by a number (up to 5 digits), and optionally another number (up to 5 digits)
// ie  "+ 1", "? 2", "+ 1 2", "? 1 2"
#define PATTERN "^\\s*([+?])\\s+([[:digit:]]{1,5})(\\s+([[:digit:]]{1,5}))?\\s*$"
#define MAX_GROUPS 4
#define G_LINE 0
#define G_OPERATION 1
#define G_NUMBER_1 2
#define G_NUMBER_2 3

/*
 * __access_log tracks the user ids in the order in which they accessed the server, index = __access_log_index
 * __access_ids tracks how many times each id has accessed the server, index = id
 */
int __access_log_index = 0;
int __access_log[MAX_SIZE_LOG];
int __access_ids[MAX_SIZE_IDS] = { 0 };


regex_t __regex;

/*
 * @param char* operation : return parameter
 * @param int* number_1  : return parameter
 * @param int* number_2  : return parameter
 * @return int (FALSE, TRUE)
 *
 * read from stdin using fgets
 * return FALSE if reading fails, otherwise return result of parse_and_validate()
 */
int read_input(char* operation, int* number_1, int* number_2);

/*
 * @param char* operation : return parameter
 * @param int* number_1  : return parameter
 * @param int* number_2  : return parameter
 * @param char* line     : string to be parsed, size = MAX_INPUT_LEN
 * @return int (FALSE, TRUE)
 *
 * attempts to parse operation, number_1, number_2 from line using regex (matching with PATTERN)
 * return FALSE if string does not match pattern, or if any parsed value is invalid
 * otherwise return TRUE
 */
int parse_and_validate(char* operation, int* number_1, int* number_2, char* line);

/*
 * @param int id : user id
 * @return int (FALSE, TRUE)
 *
 * return FALSE if __access_log_index >= MAX_SIZE_LOG - 1
 * otherwise add access to __access_log and increment __access_ids[id] and return TRUE
 */
int add_access(int id);

/*
 * @param int from
 * @param int to
 * @return int (FALSE, TRUE)
 *
 * return FALSE if to >= __access_log_index
 * otherwise copy __access_log[from]-__access_log[to] to a temporary array,
 * sort this array and count unique ids, print result and return TRUE
 */
int query(int from, int to);

/*
 * @param const void* a
 * @param const void* b
 * @return int
 *
 * compare function for qsort
 * cast void* a,b to int*, dereference it and return a - b
 */
int compare(const void* a, const void* b);

int main() {

  char operation;

  //if +: number_1 = id, number 2 unused;
  //if ?: number_1 = from, number_2 = to
  int number_1, number_2 = 0;

  int valid_result = TRUE;

  printf("Pozadavky:\n");

  regcomp(&__regex, PATTERN, REG_EXTENDED);

  //read until invalid input or EOL is reached
  while(read_input(&operation, &number_1, &number_2)) {
    switch (operation) {
      case ADD_OPERATION:
        valid_result = add_access(number_1);
      break;
      default:
        valid_result = query(number_1, number_2);
      break;
    }

    if (valid_result == FALSE) {
      printf("Nespravny vstup.\n");
      break;
    }
  }

  regfree(&__regex);
  return 1;
}

int read_input(char* operation, int* number_1, int* number_2) {
  char line[MAX_INPUT_LEN];

  if (fgets(line, MAX_INPUT_LEN, stdin) != NULL) {
    return parse_and_validate(operation, number_1, number_2, line);
  }

  return FALSE;
}

int parse_and_validate (char* operation, int* number_1, int* number_2, char* line) {

  int has_two_numbers = FALSE;
  regmatch_t group_array[MAX_GROUPS];

  //attempt to match line to PATTERN
  if (regexec(&__regex, line, MAX_GROUPS, group_array, 0) == 0) {
    for (int i = 0; i < MAX_GROUPS; i++) {
      if (group_array[i].rm_so == -1) break;

      char* line_cpy = (char*) malloc(strlen(line) + 1);
      strcpy(line_cpy, line);
      line_cpy[group_array[i].rm_eo] = 0;

      switch (i) {
        case G_OPERATION:
          *operation = (line_cpy + group_array[i].rm_so)[0];
          break;
        case G_NUMBER_1: //convert to int and validate
          *number_1 = (int) strtol(line_cpy + group_array[i].rm_so, (char **)NULL, 10);
          switch (*operation) {
            case ADD_OPERATION:
              if (*number_1 < ID_INTERVAL_BEG || *number_1 > ID_INTERVAL_END) {
                printf("Nespravny vstup.\n"); //todo remove all error message printing from outside of main while loop
                return FALSE;
              }
            break;
            default:
              if (*number_1 < ID_INTERVAL_BEG) {
                printf("Nespravny vstup.\n"); //todo remove all error message printing from outside of main while loop
                return FALSE;
              }
            break;
          }
        break;
        case G_NUMBER_2: //convert to int and validate
          *number_2 = (int) strtol(line_cpy + group_array[i].rm_so, (char **)NULL, 10);
          if ((*operation == ADD_OPERATION && *number_2 != 0) || (*number_2 < *number_1)) {
            printf("Nespravny vstup.\n"); //todo remove all error message printing from outside of main while loop
            return FALSE;
          }
          has_two_numbers = TRUE;
        break;
        default: break;
      }

      free(line_cpy);
    }
  } else {
    //line does not match regex pattern, input is invalid
    printf("Nespravny vstup.\n"); //todo remove all error message printing from outside of main while loop
    return FALSE;
  }


  if (*operation == QUERY_OPERATION && has_two_numbers == FALSE) {
    printf("Nespravny vstup.\n"); //todo remove all error message printing from outside of main while loop
    return FALSE;
  }


  return TRUE;
}

int add_access(int id) {
  if (__access_log_index >= (MAX_SIZE_LOG - 1)) {

    return FALSE;
  } else {
    __access_ids[id]++;
    switch (__access_ids[id]) {
      case 1:
        printf("> prvni navsteva\n");
      break;
      default:
        printf("> navsteva #%d\n", __access_ids[id]);
      break;
    }

    __access_log[__access_log_index] = id;
    __access_log_index++;
  }

  return TRUE;
}

int query(int from, int to) {
  if (to >= __access_log_index) {
    return FALSE;
  }

  //create a temprary subset of __access_log with required values <from, to>
  //sort this subset for potentially faster counting of unique ids
  int total_length = to - from + 1;
  int total_unique = 0;
  int* access_log_subset = (int*) malloc(total_length * sizeof(int*));

  int access_log_subset_index = 0;
  for (int i = from; i <= to; i++) {
    access_log_subset[access_log_subset_index] = __access_log[i];
    access_log_subset_index++;
  }

  qsort(access_log_subset, total_length, sizeof(int), compare);

  //count unique ids (access_log_subset has to be sorted)
  for (int i = 0; i < total_length; i++) {
    while(i < (total_length - 1) && access_log_subset[i] == access_log_subset[i + 1]) {
      i++;
    }

    total_unique++;
  }

  printf("> %d / %d\n", total_unique, total_length);

  free(access_log_subset);
  return TRUE;
}

int compare (const void * a, const void * b) {
  return (*(int*)a - *(int*)b);
}
