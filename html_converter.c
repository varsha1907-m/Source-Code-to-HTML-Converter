
#include <stdio.h>
#include "html_converter.h"

/* Convert special characters into HTML entities */
static void write_escaped(FILE *fp, const char *str)
{
    while(*str)
    {
        switch(*str)
        {
            case '&':
                fputs("&amp;", fp);
                break;

            case '<':
                fputs("&lt;", fp);
                break;

            case '>':
                fputs("&gt;", fp);
                break;

            case '"':
                fputs("&quot;", fp);
                break;

            case '\'':
                fputs("&#39;", fp);
                break;

            default:
                fputc(*str, fp);
        }

        str++;
    }
}

/* Start HTML document */
void html_begin(FILE *fp)
{
    fprintf(fp,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "<title>Source Code to HTML Converter</title>\n"
        "<link rel=\"stylesheet\" href=\"style.css\">\n"
        "</head>\n"
        "<body>\n"
        "<h2>Source Code to HTML Converter</h2>\n"
        "<pre>\n");
}

/* End HTML document */
void html_end(FILE *fp)
{
    fprintf(fp,
        "</pre>\n"
        "</body>\n"
        "</html>\n");
}

/* Convert tokens into HTML */
void convert_to_html(FILE *fp, Token *token)
{
    const char *class_name = NULL;

    if(token == NULL)
        return;

    switch(token->type)
    {
        case TOKEN_KEYWORD:
            if(token->property == DATA_KEYWORD)
                class_name = "reserved_key1";
            else if(token->property == OTHER_KEYWORD)
                class_name = "reserved_key2";
            break;

        case TOKEN_NUMBER:
            class_name = "numeric_constant";
            break;

        case TOKEN_STRING:
            class_name = "string";
            break;

        case TOKEN_CHARACTER:
            class_name = "ascii_char";
            break;

        case TOKEN_HEADER:
            class_name = "header_file";
            break;

        case TOKEN_PREPROCESSOR:
            class_name = "preprocess_dir";
            break;

        case TOKEN_COMMENT:
            class_name = "comment";
            break;

        case TOKEN_OPERATOR:
            class_name = "operator";
            break;

        case TOKEN_NORMAL:
        case TOKEN_EOF:
            break;
    }

    if(class_name != NULL)
    {
        fprintf(fp, "<span class=\"%s\">", class_name);
        write_escaped(fp, token->data);
        fprintf(fp, "</span>");
    }
    else
    {
        write_escaped(fp, token->data);
    }
}
