/*
  input:
    add:
      format: ([0-9]+): ([\S]+) ([a-zA-Z]{3} [0-9]{1,2} [0-9]{1,2}:[0-9]{1,2})
        $1 = gate number
        $2 = registration plate
        $3 = date in format %b %d %H:%M

      input starts with '{', ends with '}', each line is separated by ',', newline optional
    query:
      format: ([\S]+) ([a-zA-Z]{3} [0-9]{1,2} [0-9]{1,2}:[0-9]{1,2})
        $1 = registration plate
        $2 date in format %b %d %H:%M

  output for each query:
    > Predchazejici: (%b %d %H:%M, $x [$y]) | (N/A)  #N/A if no prior/latter match was found,
    > Pozdejsi: (%b %d %H:%M, $x [$y]) | (N/A)       #Predchazejici and Pozdejsi not printed if BOTH result in N/A
    > Presne: %b %d %H:%M, $x [$y]  #not printed if N/A

    $x = how many times the queried plate was seen (1x, 2x, 3x,...)
    $y = at which gate(s) it was seen ([12], [15,60],...)

    if no match for queried plate was found print "Automobil nenalezen." instead

    ex. input:
      { 1: A Mar 7 21:32, 1: B Jan 7 21:32, 2: B Jul 1 16:06, 3: C Mar 7 21:32, 4: E Jul 1 16:06, 5: F Mar 7 21:32, 6: G Jul 1 16:06, 2: B Mar 6 16:10, 2: B Mar 6 16:10, 2: B Mar 6 16:10, 2: B Mar 6 16:10 }
      B Mar 6 16:06

    ex. output
      Data z kamer:
      Hledani:
      > Predchazejici: Jan 7 21:32, 1x [1]
      > Pozdejsi: Mar 6 16:10, 4x [2, 2, 2, 2]

    ex. input 2:
      {10: ABC-12-34 Oct 1 7:30, 289: XYZ-98-76 Oct 10 15:40}
      XYZ-98-76 Oct 10 15:39 XYZ-98-76 Oct 10 15:41

    ex. output 2:
      Data z kamer:
      Hledani:
      > Predchazejici: N/A
      > Pozdejsi: Oct 10 15:40, 1x [289]
      > Predchazejici: Oct 10 15:40, 1x [289]
      > Pozdejsi: N/A
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 1
#define MONTH_ARR_LEN 12
#define MONTH_LEN 4
#define MONTH_ERR -1
#define EXACT 0
#define AFTER 1
#define BEFORE -1
#define VALIDATE_ERROR -1
#define VALIDATE_SUCCESS 1

typedef struct {
    int camera_id;
    int day;
    int month;
    int hour;
    int minute;
    char registration[1001];
} RECORD;

int size = SIZE;

void recordsPrint(RECORD record) {
    printf("camera_id[%d]; registration[%s]; month[%d]; day[%d]; hour[%d]; minute[%d]\n",
           record.camera_id, record.registration, record.month, record.day, record.hour, record.minute);
}

/**
 * returns n. of month, MONTH_ERR if invalid month (program exits)
 *
 * @param month string
 * @return month as number, -1 if invalid
 */
int getMonthToInt(char *month) {

    char m_arr[MONTH_ARR_LEN][MONTH_LEN] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov",
                                            "Dec"};

    // cycle index + 1 = month number
    for (int i = 0; i < MONTH_ARR_LEN; ++i) {
        if (strcmp(month, m_arr[i]) == 0) {
            return i + 1;
        }
    }

    return MONTH_ERR;
}

/**
 * returns month as string, "ERR" if invalid index (program exits)
 *
 * @param month string
 * @return month as number, -1 if invalid
 */
void getIntToMonth(int index, char* month) {

    char m_arr[MONTH_ARR_LEN][MONTH_LEN] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov",
                                            "Dec"};

    if (index <= 12 && index >= 1) {
        strcpy(month, m_arr[index -1 ]);
    } else {
        strcpy(month, "ERR");
    }

}

/**
 * validates record, if any error is found return VALIDATE_ERROR (program exits), otherwise return VALIDATE_SUCCESS
 *
 * @param record
 * @return
 */
int validateRecord(RECORD record) {


    //minutes, hours and days
    if (record.minute >= 60 || record.minute < 0 || record.hour >= 24 || record.hour < 0 || record.day > 31 ||
        record.day < 1) {
        return VALIDATE_ERROR;

    }

    //Feb days
    if (record.month == 2 && record.day > 28) {
        return VALIDATE_ERROR;
    }

    //short month days
    if (record.month == 4 || record.month == 6 || record.month == 9 || record.month == 11) {
        if (record.day > 30) {
            return VALIDATE_ERROR;
        }
    }

    return VALIDATE_SUCCESS;
}

/**
 * reads input and returns it as RECORD array, size of the array is stored in global variable $size
 *
 * @return RECORD*
 */
RECORD *recordsRead() {
    RECORD *records = (RECORD *) malloc(size * sizeof(RECORD));
    RECORD tmp;
    char pattern_check[2];
    int req = 8;
    int index = 0;
    int first = 1;
    int die = 0;
    int input;

    //read until } is found
    while (1) {
        if (first == 1 && index > 0) {
            first = 0;
        }
        char month[4];
        //read input and store it into a temporary variable
        if (first == 1) {
            input = scanf(" %c %d : %s %s %d %d : %d %c",
                          &pattern_check[0],
                          &tmp.camera_id,
                          tmp.registration,
                          month,
                          &tmp.day,
                          &tmp.hour,
                          &tmp.minute,
                          &pattern_check[1]);
        } else {
            //input pattern changes after the first input
            input = scanf("%d: %s %s %d %d:%d %c",
                          &tmp.camera_id,
                          tmp.registration,
                          month,
                          &tmp.day,
                          &tmp.hour,
                          &tmp.minute,
                          &pattern_check[1]);
            req = 7;
        }

        tmp.month = getMonthToInt(month);
        if (tmp.month == MONTH_ERR) {
            die = 1;
        }

        //basic input validation
        if ((first == 1 && pattern_check[0] != '{') || (pattern_check[1] != '}' && pattern_check[1] != ',')) {
            die = 1;
        }

        //records array needs to be expanded
        if (index >= size) {
            RECORD *tmp_realloc;

            size += 1;
            tmp_realloc = (RECORD *) realloc(records, size * sizeof(RECORD));

            //out of memory, program exits
            if (tmp_realloc == NULL) {
                free(records);
                printf("Nedostatek pameti.\n");
                exit(0);
            }

            records = tmp_realloc;
        }

        //add values to array, return if end of input ( '}' )
        records[index] = tmp;


        //input is invalid ? exit
        if (input != req || die == 1 || validateRecord(records[index]) == VALIDATE_ERROR || strcmp(records[index].registration, "\0") == 0) {
            printf("Nespravny vstup.\n");
            free(records);
            exit(0);
        }

        index++;

        // } = end of input
        if (pattern_check[1] == '}') {
            return records;
        }
    }
}

/**
 * a > b => 1 = AFTER
 * a < b => -1 = BEFORE
 * a == b => 0 = EXACT
 *
 * @param a
 * @param b
 * @return int
 */
int compareRecords(const void *a, const void *b) {
    RECORD rec_a = *(const RECORD *) a;
    RECORD rec_b = *(const RECORD *) b;

    if (rec_a.camera_id > rec_b.camera_id) {
        return AFTER;
    } else if (rec_b.camera_id > rec_a.camera_id) {
        return BEFORE;
    } else {
        if (rec_a.month > rec_b.month) {
            return AFTER;
        } else if (rec_b.month > rec_a.month) {
            return BEFORE;
        } else {
            if (rec_a.day > rec_b.day) {
                return AFTER;
            } else if (rec_b.day > rec_a.day) {
                return BEFORE;
            } else {
                if (rec_a.hour > rec_b.hour) {
                    return AFTER;
                } else if (rec_b.hour > rec_a.hour) {
                    return BEFORE;
                } else {
                    if (rec_a.minute > rec_b.minute) {
                        return AFTER;
                    } else if (rec_b.minute > rec_a.minute) {
                        return BEFORE;
                    }
                }
            }
        }
    }

    return EXACT;
}


/**
 * @see compareRecord() plus ignore camera_id
 */
int compareRecordsIgnoreId(const void *a, const void *b) {
    RECORD rec_a = *(const RECORD *) a;
    RECORD rec_b = *(const RECORD *) b;

    if (rec_a.month > rec_b.month) {
        return AFTER;
    } else if (rec_b.month > rec_a.month) {
        return BEFORE;
    } else {
        if (rec_a.day > rec_b.day) {
            return AFTER;
        } else if (rec_b.day > rec_a.day) {
            return BEFORE;
        } else {
            if (rec_a.hour > rec_b.hour) {
                return AFTER;
            } else if (rec_b.hour > rec_a.hour) {
                return BEFORE;
            } else {
                if (rec_a.minute > rec_b.minute) {
                    return AFTER;
                } else if (rec_b.minute > rec_a.minute) {
                    return BEFORE;
                }
            }
        }
    }

    return EXACT;
}

/**
 * try to find $registration in $records_array, creates new $found_records array and returns it, size of $found_records is returned in $found_size
 *
 * @param records_array array of records in which the desired registration number is being found
 * @param registration
 * @param found_size size of the newly created array
 * @return array of records with matching registration number
 */
RECORD *getRecordsRegistration(RECORD *records_array, char *registration, int *found_size) {
    int j = 0; //index in found_records array
    RECORD *found_records = (RECORD *) malloc(*found_size * sizeof(RECORD));

    //iterate over records_array, if registrations are matching add it to found_records
    for (int i = 0; i < size; ++i) {
        if (strcmp(records_array[i].registration, registration) == 0) {
            if (j >= *found_size) {
                *found_size += 1;
                RECORD *tmp_realloc = (RECORD *) realloc(found_records, *found_size * sizeof(RECORD));

                if (tmp_realloc == NULL) {
                    free(found_records);
                    free(records_array);
                    printf("Nedostatek pameti.\n");
                    exit(0);
                }

                found_records = tmp_realloc;
            }

            found_records[j] = records_array[i];
            j++;
        }
    }

    return found_records;

}

/**
 * returns all sightings of a registration number that is stored in $record_query
 *
 * cmp_val values:
 *      EXACT  => query == records_array[item]
 *      AFTER  => query >  records_array[item]
 *      BEFORE => query <  records_array[item]
 *
 * @param records_array
 * @param records_array_size
 * @param record_query
 * @param cnt
 * @param cmp_val
 * @return RECORD array and its size as cnt
 */
RECORD *getSightings(RECORD *records_array, int records_array_size, RECORD record_query, int *cnt, int cmp_val) {

    int res_size = 1;
    int compare;

    RECORD *query_results = (RECORD *) malloc(res_size * sizeof(RECORD));



    //iterate over records_array and look for matching registration numbers
    for (int i = 0; i < records_array_size; ++i) {
        record_query.camera_id = records_array[i].camera_id;
        compare = compareRecords(&records_array[i], &record_query);

        if (compare == cmp_val) {
            *cnt += 1;

            if ((*cnt - 1) >= res_size) {
                res_size += 1;
                RECORD *tmp = (RECORD *) realloc(query_results, res_size * sizeof(RECORD));

                if (tmp == NULL) {
                    free(records_array);
                    printf("Nedostatek pameti.\n");
                    exit(0);
                }

                query_results = tmp;
            }

            //use $cnt as index by subtracting 1
            query_results[*cnt - 1] = records_array[i];
        }
    }

    return query_results;
}

/**
 * returns a new (usually smaller) array of records where:
 * A) the times of all records match
 * B) the times are the highest ones in original array
 *
 * @param records_array
 * @param records_array_size
 * @param cnt
 * @return
 */
RECORD *getMaximums(RECORD *records_array, int records_array_size, int *cnt) {
    RECORD max = records_array[records_array_size - 1];
    *cnt = 0;
    int ids_arr_size = 1;
    RECORD *max_ids = (RECORD *) malloc(ids_arr_size * sizeof(RECORD));

    for (int i = 0; i < records_array_size; ++i) {
        RECORD pseudo_record = records_array[i];

        if (compareRecordsIgnoreId(&pseudo_record, &max) == EXACT) {
            *cnt += 1;

            if ((*cnt - 1) >= ids_arr_size) {
                ids_arr_size += 1;
                RECORD *tmp = (RECORD *) realloc(max_ids, ids_arr_size * sizeof(RECORD));

                if (tmp == NULL) {
                    free(records_array);
                    printf("Nedostatek pameti.\n");
                    exit(0);
                }

                max_ids = tmp;
            }
            max_ids[*cnt - 1] = pseudo_record;
        }
    }

    return max_ids;
}

/**
 * @see getMaximums(), except minimums
 *
 * @param records_array
 * @param records_array_size
 * @param cnt
 * @return
 */
RECORD *getMinimums(RECORD *records_array, int records_array_size, int *cnt) {
    qsort(records_array, records_array_size, sizeof(RECORD), compareRecordsIgnoreId);
    RECORD min = records_array[0];
    *cnt = 0;
    int ids_arr_size = 1;
    RECORD *min_ids = (RECORD *) malloc(ids_arr_size * sizeof(RECORD));

    for (int i = 0; i < records_array_size; ++i) {
        RECORD pseudo_record = records_array[i];
        min.camera_id = pseudo_record.camera_id;
        if (compareRecords(&pseudo_record, &min) == EXACT) {
            *cnt += 1;

            if ((*cnt - 1) >= ids_arr_size) {
                ids_arr_size += 1;
                RECORD *tmp = (RECORD *) realloc(min_ids, ids_arr_size * sizeof(RECORD));

                if (tmp == NULL) {
                    free(records_array);
                    printf("Nedostatek pameti.\n");
                    exit(0);
                }

                min_ids = tmp;
            }
            min_ids[*cnt - 1] = pseudo_record;
        }
    }

    return min_ids;
}

/**
 * main function for finding registration records in the $records_array
 * reads registration numbers and dates from stdin, tries to find records in $records_array
 *
 * @param records_array
 */
void query(RECORD *records_array) {
    int find_month, find_day, find_hour, find_minute;
    char month_str[4], find_registration[1001];
    char month_print[4];

    while (1) {
        int found_size = 0;
        int input = scanf("%s %s %d %d:%d", find_registration, month_str, &find_day, &find_hour, &find_minute);
        if (input != 5) {
            break;
        }



        find_month = getMonthToInt(month_str);
        RECORD *found_records = getRecordsRegistration(records_array, find_registration, &found_size);

        //create a pseudo record with the query params
        RECORD pseudo_record;
        strcpy(pseudo_record.registration, records_array[0].registration);
        pseudo_record.month = find_month;
        pseudo_record.day = find_day;
        pseudo_record.hour = find_hour;
        pseudo_record.minute = find_minute;

        //query params invalid
        if (validateRecord(pseudo_record) == VALIDATE_ERROR || find_month == MONTH_ERR) {
            printf("Nespravny vstup.\n");
            free(records_array);
            exit(0);
        }

        int exact_cnt = 0;

        if (found_size > 0) {
            //attempt to find exact time matches
            RECORD *exact_cam_id_array = getSightings(found_records, found_size, pseudo_record, &exact_cnt, EXACT);

            if (exact_cnt >= 1) {
                //exact matches were found, print and continue
                printf("> Presne: %s %d %02d:%02d, %dx [", month_str, find_day, find_hour, find_minute, exact_cnt);
                for (int i = 0; i < exact_cnt - 1; ++i) {
                    printf("%d, ", exact_cam_id_array[i].camera_id);
                }
                printf("%d]\n", exact_cam_id_array[exact_cnt - 1].camera_id);
                continue;
            }

            qsort(found_records, found_size, sizeof(RECORD), compareRecordsIgnoreId);

            //attempt to find time matches before and after query times
            int before_cnt = 0;
            RECORD *before_cam_id_array = getSightings(found_records, found_size, pseudo_record, &before_cnt, BEFORE);

            int after_cnt = 0;
            RECORD *after_cam_id_array = getSightings(found_records, found_size, pseudo_record, &after_cnt, AFTER);

            if (after_cnt >= 1 || before_cnt >= 1) {
                //after or before was found, print each non-empty result and continue

                if (before_cnt >= 1) {
                    //print before
                    int max_cnt;
                    RECORD *maximums = getMaximums(before_cam_id_array, before_cnt, &max_cnt);

                    getIntToMonth(maximums[max_cnt - 1].month, month_print);
                    if (strcmp(month_print, "ERR") == 0) {
                        exit(10);
                    }

                    printf("> Predchazejici: %s %d %02d:%02d, %dx [", month_print, maximums[max_cnt - 1].day,
                           maximums[max_cnt - 1].hour, maximums[max_cnt - 1].minute, max_cnt);
                    for (int i = 0; i < max_cnt - 1; ++i) {
                        printf("%d, ", maximums[i].camera_id);
                    }
                    printf("%d]\n", maximums[max_cnt - 1].camera_id);

                } else {
                    printf("> Predchazejici: N/A\n");
                }

                if (after_cnt >= 1) {
                    //print after
                    int min_cnt;
                    RECORD *minimums = getMinimums(after_cam_id_array, after_cnt, &min_cnt);


                    getIntToMonth(minimums[min_cnt - 1].month, month_print);
                    if (strcmp(month_print, "ERR") == 0) {
                        exit(10);
                    }

                    printf("> Pozdejsi: %s %d %02d:%02d, %dx [", month_print, minimums[min_cnt - 1].day,
                           minimums[min_cnt - 1].hour, minimums[min_cnt - 1].minute, min_cnt);
                    for (int i = 0; i < min_cnt - 1; ++i) {
                        printf("%d, ", minimums[i].camera_id);
                    }
                    printf("%d]\n", minimums[min_cnt - 1].camera_id);

                } else {
                    printf("> Pozdejsi: N/A\n");
                }
                free(found_records);
                continue;
            } else {
                //registration not found
                printf("> Automobil nenalezen.\n");
            }

        } else {
            //registration not found
            printf("> Automobil nenalezen.\n");
        }

        free(found_records);
    }
}

int main() {

    /*
     * algorithm:
     *
     * 1) read input
     *  1.1) validate, if invalid exit
     * 2) sort by id, time
     * 3) read query
     *  3.1) validate, if invalid exit
     *  3.2) try finding matching registration plates, goto 3) if empty
     *  3.3) try finding exact time matches, print results and goto 3) if not empty
     *  3.4) try finding matches before/after query time, print results if not empty
     * 4) goto 3)
     */

    printf("Data z kamer:\n");
    RECORD *records = recordsRead();

    //sort input by id, month, day, hour, minute
    qsort(records, size, sizeof(RECORD), compareRecords);

    printf("Hledani:\n");
    query(records);

    free(records);

    return 0;
}
