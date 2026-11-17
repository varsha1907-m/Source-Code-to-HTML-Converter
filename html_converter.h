
#ifndef HTML_CONVERTER_H
#define HTML_CONVERTER_H

#include <stdio.h>
#include "lexer.h"

void html_begin(FILE *fp);
void html_end(FILE *fp);
void convert_to_html(FILE *fp, Token *token);

#endif
