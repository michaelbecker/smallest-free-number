/****************************************************************************
 *
 *  smallest-free-number
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>


int Debug = 0;


/****************************************************************************
 *
 *  ExitOnError
 *
 *  Prints out a formatted error message and exits the program.
 *
 ***************************************************************************/
static void ExitOnError(int line,
                        char *message,
                        ...)
{
    //------------------------------
    char buffer[300];
    int bytes_written;
    va_list arg_ptr;
    //------------------------------

    va_start(arg_ptr, message);

    bytes_written = vsnprintf(  &buffer[0],
                                sizeof(buffer),
                                message,
                                arg_ptr);

    va_end(arg_ptr);

    bytes_written += snprintf(  &buffer[bytes_written],
                                sizeof(buffer) - bytes_written,
                                " Line %d [%d: %s]",
                                line,
                                errno,
                                strerror(errno));

    printf("%s\n", buffer);

    exit(-1);
}


/****************************************************************************
 *
 *  PrintDataElements
 *
 *  For debug, printf out an array of integers.
 *
 ***************************************************************************/
static void PrintDataElements(int *elements, int num_elements)
{
    //----------
    int i;
    //----------

    for (i = 0; i < num_elements; i++) {
        printf("%d", *elements);
        elements++;

        if (i == (num_elements - 1))
            break;

        if ((i % 16) || (i == 0))
            printf(", ");
        else
            printf(",\n");
    }

    printf("\n");
}


/****************************************************************************
 *
 *  ReadNumbersFile
 *
 *  Takes in a filename then reads all integers from that file and returns
 *  an array of ints. The caller is responsible for freeing the array when
 *  they are done with it.
 *
 ***************************************************************************/
static int *ReadNumbersFile(char *filename, int *num_elements)
{
    //--------------------------------------------
    FILE *fp;
    char *token = NULL;
    const char TOKEN_DELIMITERS[] = " ,;|\t\n";
    char *line;
    const int line_length = 4096;
    char *success;
    int max_elements = 4096;
    int *elements;
    //--------------------------------------------

    *num_elements = 0;

    fp = fopen(filename, "r");
    if (!fp) {
        ExitOnError(__LINE__,
                    "Couldn't open %s",
                    filename);
    }

    line = malloc(line_length);
    if (!line) {
        ExitOnError(__LINE__,
                    "Failed malloc");
    }

    elements = malloc(max_elements * sizeof(int));
    if (!elements) {
        ExitOnError(__LINE__,
                    "Failed malloc");
    }

    success = fgets(line, line_length, fp);

    while (success) {

        token = strtok(line, TOKEN_DELIMITERS);

        while (token) {

            int value = atoi(token);
            elements[*num_elements] = value;
            (*num_elements)++;

            /*
             *  Handle growing our data array.
             */
            if (*num_elements >= max_elements) {

                max_elements += 4096;
                elements = realloc(elements, max_elements * sizeof(int));
                if (!elements) {
                    ExitOnError(__LINE__,
                                "Failed realloc");
                }
            }

            token = strtok(NULL, TOKEN_DELIMITERS);
        }

        success = fgets(line, line_length, fp);
    }

    free(line);
    fclose(fp);

    if (Debug) {
        PrintDataElements(elements, *num_elements);
    }

    return elements;
}


/****************************************************************************
 *
 *  PrintBitmap
 *
 *  For debug, printf out a bitmap.
 *
 ***************************************************************************/
static void PrintBitmap(char *bitmap, int num_bitmap_bytes)
{
    //----------
    int index;
    //----------

    for (index = 0; index < num_bitmap_bytes; index++) {
        printf("%02X", (unsigned char)bitmap[index]);

        if (index == (num_bitmap_bytes - 1))
            break;

        if ((index % 16) || (index == 0))
            printf(", ");
        else
            printf(",\n");
    }

    printf("\n");
}


/****************************************************************************
 *
 *  FindSmallestNotContained
 *
 *  Implements the actual algorithm.
 *
 *  [20, 10, 0, 30] => 1
 *  [3, 2, 1, 0] => 4
 *
 *  Algorithm complexity:
 *  2*O(n) + O(m) => Linear complexity (I think..)
 *
 ***************************************************************************/
static int FindSmallestNotContained(int *starting_element, int num_elements)
{
    //------------------------------------
    int min = INT_MAX;
    int max = INT_MIN;
    int i;
    int *elements = starting_element;
    char *bitmap;
    int num_bitmap_bytes;
    int bit_position;
    int index;
    int value;
    clock_t start;
    clock_t end;
    clock_t diff;
    //------------------------------------

    start = clock();

    /*
     *  Walk the list of ints and find max & min.
     *  O(n)
     */
    for (i = 0; i < num_elements; i++, elements++) {

        value = *elements;

        if (value > max)
            max = value;
        if (value < min)
            min = value;
    }

    /*
     *  Short-circut return
     */
    if (min > 0)
        return 0;

    /*
     *  Create a bitmap of integers that we have.
     *  We are trading time complexity for space complexity.
     *  O(n)
     */
    num_bitmap_bytes = (max / 8) + 1;

    bitmap = malloc(num_bitmap_bytes);
    if (!bitmap) {
        ExitOnError(__LINE__,
                    "Failed malloc");
    }

    memset(bitmap, 0, num_bitmap_bytes);

    elements = starting_element;

    for (i = 0; i < num_elements; i++, elements++) {

        value = *elements;

        index = value / 8;
        bit_position = value % 8;
        bitmap[index] |= (1 << bit_position);
    }

    if (Debug) {
        PrintBitmap(bitmap, num_bitmap_bytes);
    }

    /*
     *  Now search the bitmap for the first zero bit. That's
     *  the integer we are looking for.
     *  O(m) - Note, this depends on the max integer value in
     *  the data sent in, not directly on the list length.
     */
    value = max + 1;

    for (index = 0; index < num_bitmap_bytes; index++ ) {

        for (i = 0; i < 8; i++) {

            if ((bitmap[index] & (1 << i)) == 0) {
                value = (index * 8) + i;
                goto FOUND;
            }
        }
    }

FOUND:
    free(bitmap);

    end = clock();

    diff = end - start;

    printf( "Algorithm run time: %ld clocks / %f seconds\n",
            diff, (double)diff/CLOCKS_PER_SEC);

    return value;
}


/****************************************************************************
 *
 *  Usage
 *
 ***************************************************************************/
static void Usage(void)
{
    printf( "smallest-free-number\n"
            "   [-f|--file <filename>]\n"
            "   [-d|--debug]\n"
            "   [-h|--help]\n"
            );

    exit(-1);
}


/****************************************************************************
 *
 *  main
 *
 ***************************************************************************/
int main(int argc, char *argv[])
{
    //-----------------------------------------------------
    int *elements;
    int num_elements;
    int smallest;
    char *filename = NULL;
    int opt;

    struct option long_options[] = {
        {"file",    required_argument,  0,  'f'},
        {"debug",   no_argument,        0,  'd'},
        {"help",    no_argument,        0,  'h'},
        {0,         0,                  0,  0}
    };
    //-----------------------------------------------------

    if (argc == 1)
        Usage();

    while ((opt = getopt_long_only( argc, argv, "f:dh",
                                    long_options, NULL)) != -1) {
        switch(opt) {
            case 'f':
                filename = strdup(optarg);
                break;

            case 'd':
                Debug = 1;
                break;

            case 'h':
                Usage();
                break;

            default:
                printf("Unknown option: %c\n", opt);
                Usage();
                break;
        }
    }

    if (optind < argc) {
        printf("Unknown options: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
        Usage();
    }

    elements = ReadNumbersFile(filename, &num_elements);
    smallest = FindSmallestNotContained(elements, num_elements);

    printf("Smallest integer not contained = %d\n", smallest);

    free(elements);
    free(filename);

    return 0;
}


