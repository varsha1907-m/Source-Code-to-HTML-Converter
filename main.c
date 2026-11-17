
#include <stdio.h>
#include "lexer.h"
#include "html_converter.h"

int main(int argc, char *argv[])
{
    FILE *src_fp;
    FILE *dest_fp;

    Token *token;

    char output_file[512];
    const char *name;
    int len;

    if(argc < 2 || argc > 3)
    {
        printf("Usage: ./html <source_file> [output_name]\n");
        return 1;
    }

    /* Open source file */
    src_fp = fopen(argv[1], "r");

    if(src_fp == NULL)
    {
        perror("Error opening source file");
        return 2;
    }

    /* Generate output filename */
    name = (argc == 3) ? argv[2] : argv[1];

    len = snprintf(output_file, sizeof(output_file),
                   "%s.html", name);

    if(len < 0 || (size_t)len >= sizeof(output_file))
    {
        printf("Output filename is too long\n");
        fclose(src_fp);
        return 3;
    }

    /* Create HTML file */
    dest_fp = fopen(output_file, "w");

    if(dest_fp == NULL)
    {
        perror("Error creating output file");
        fclose(src_fp);
        return 4;
    }

    /* Generate HTML */
    html_begin(dest_fp);

    while(1)
    {
        token = get_next_token(src_fp);

        if(token == NULL || token->type == TOKEN_EOF)
            break;

        convert_to_html(dest_fp, token);
    }

    html_end(dest_fp);

    fclose(src_fp);
    fclose(dest_fp);

    printf("HTML file generated successfully: %s\n", output_file);

    return 0;
}
