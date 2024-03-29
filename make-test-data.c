/****************************************************************************
 *
 *  make-test-data
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>


/****************************************************************************
 *
 *  Usage
 *
 ***************************************************************************/
static void Usage(void)
{
    printf( "make-test-data\n"
            "   [-e|--empty-slot <missing_number>]\n"
            "   [-m|--max <max number>]\n"
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
    int empty_slot = -1;
    int max = -1;
    int i;
    int opt;

    struct option long_options[] = {
        {"empty-slot",  required_argument,  0,  'e'},
        {"max",         required_argument,  0,  'm'},
        {"help",        no_argument,        0,  'h'},
        {0,             0,                  0,  0}
    };
    //-----------------------------------------------------

    if (argc == 1)
        Usage();

    while ((opt = getopt_long_only( argc, argv, "e:m:h",
                                    long_options, NULL)) != -1) {
        switch(opt) {
            case 'e':
                empty_slot = atoi(optarg);
                break;

            case 'm':
                max = atoi(optarg);
                max++;
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


    for (i = 0; i < max; i++) {
        
        if (i == empty_slot)
            continue;

        printf("%d", i);

        if (i == (max - 1))
            break;

        if ((i % 16) || (i == 0))
            printf(", ");
        else
            printf(",\n");
    }

    printf("\n");

    return 0;
}

