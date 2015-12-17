/* inih -- simple .INI file parser

inih is released under the New BSD license (see LICENSE.txt). Go to the project
home page for more info:

https://github.com/benhoyt/inih

dz 12.16.15 - this has been modified to read from SD card for arduino, the parser function
              has also been tweaked with a custom inline version of readLine that skips 
			  long comment lines, and parses partial buffers if the line was to long.

*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "ini.h"


#if !INI_USE_STACK
#include <stdlib.h>
#endif

#define MAX_SECTION 50
#define MAX_NAME 50

/* Strip whitespace chars off end of given string, in place. Return s. */
static char* rstrip(char* s)
{
    char* p = s + strlen(s);
    while (p > s && isspace((unsigned char)(*--p)))
        *p = '\0';
    return s;
}

/* Return pointer to first non-whitespace char in given string. */
static char* lskip(const char* s)
{
    while (*s && isspace((unsigned char)(*s)))
        s++;
    return (char*)s;
}

/* Return pointer to first char c or ';' comment in given string, or pointer to
   null at end of string if neither found. ';' must be prefixed by a whitespace
   character to register as a comment. */
static char* find_char_or_comment(const char* s, char c)
{
    int was_whitespace = 0;
    while (*s && *s != c && !(was_whitespace && *s == ';')) {
        was_whitespace = isspace((unsigned char)(*s));
        s++;
    }
    return (char*)s;
}

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
static char* strncpy0(char* dest, const char* src, size_t size)
{
    strncpy(dest, src, size);
    dest[size - 1] = '\0';
    return dest;
}

void readTillNewLine(File file){
	
	  char b=0;

	  while(1){
		  if(file.read(&b, 1)==0) break;
		  if(b=='\n') break;
	  }

}

/* See documentation in header file. */
int ini_parse_stream(File file, ini_handler handler,  void* user)
{
    /* Uses a fair bit of stack (use heap instead if you need to) */
#if INI_USE_STACK
    char line[INI_MAX_LINE];
#else
    char* line;
#endif
    char section[MAX_SECTION] = "";
    char prev_name[MAX_NAME] = "";

    char* start;
    char* end;
    char* name;
    char* value;
    int lineno = 0;
    int error = 0;

#if !INI_USE_STACK
    line = (char*)malloc(INI_MAX_LINE);
    if (!line) {
        return -2;
    }
#endif

    /* process file line by line */
	while( file.available() ){
		
freshLine:
		char b=0;
		int cnt=0;
		memset(line,0,INI_MAX_LINE);

		//load a line of text into internal buffer
		//this is my implementation of .readBytesUntil(\n) it has been customized so
		//that if first line character is a comment, then it will ignore the line
		//so comments can be longer than INI_MAX_LINE without error..
		while( file.available() ){
			
			if(cnt == INI_MAX_LINE){ //buffer full up, skip file pointer to end and process partial line? (visible error in output)
				readTillNewLine(file);
			    break;
			}

			size_t bytesRead = file.read(&b, 1);
			if (!bytesRead) return error;

			if(cnt==0 && (b=='#' || b==';') ){ //comment line, just skip it without impacting buffer length..
				readTillNewLine(file);
				goto freshLine;
			}else{
				line[cnt] = b;
				if (b == '\n') break;
				cnt++;
			}

		}

        lineno++;

        start = line;
        start = lskip(rstrip(start));

        if (*start == ';' || *start == '#') {
            /* Per Python ConfigParser, allow '#' comments at start of line */
        }
        else if (*start == '[') {
            /* A "[section]" line */
            end = find_char_or_comment(start + 1, ']');
            if (*end == ']') {
                *end = '\0';
                strncpy0(section, start + 1, sizeof(section));
                *prev_name = '\0';
            }
            else if (!error) {
                /* No ']' found on section line */
                error = lineno;
            }
        }
        else if (*start && *start != ';') {
            /* Not a comment, must be a name[=:]value pair */
            end = find_char_or_comment(start, '=');
            if (*end != '=') {
                end = find_char_or_comment(start, ':');
            }
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = rstrip(start);
                value = lskip(end + 1);
                end = find_char_or_comment(value, '\0');
                if (*end == ';')
                    *end = '\0';
                rstrip(value);

                /* Valid name[=:]value pair found, call handler */
                strncpy0(prev_name, name, sizeof(prev_name));
                if (!handler(user, section, name, value) && !error)
                    error = lineno;
            }
            else if (!error) {
                /* No '=' or ':' found on name[=:]value line */
                error = lineno;
            }
        }

	}

#if !INI_USE_STACK
    free(line);
#endif

    return error;
}

/* See documentation in header file. */
int ini_parse(const char* filename, ini_handler handler, void* user)
{
    File file;
    int error;

    file = SD.open(filename, FILE_READ);
    if (!file) return -1;
    error = ini_parse_stream(file, handler, user);
    file.close();
    return error;
}
