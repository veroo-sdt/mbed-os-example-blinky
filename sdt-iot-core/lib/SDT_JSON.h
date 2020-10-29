#ifndef __SDT_JSON__
#define __SDT_JSON__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>

using namespace std;

class SDT_JSON
{
public:
    typedef enum
    {
        UNDEFINED = 0,
        OBJECT,
        ARRAY,
        STRING,
        INT,
        FLOAT,
        DOUBLE,
        BOOL
    } valuetype_t;
    typedef struct tokenvalue
    {
        valuetype_t type;
        char *value;
    } tokenvalue_t;

private:
    struct char_cmp
    {
        bool operator()(const char *a, const char *b) const
        {
            return strcmp(a, b) < 0;
        }
    };

    std::map<const char *, int, char_cmp> _tokens;
    std::vector<tokenvalue_t> _values;

    void init();
    unsigned int get_keyword(char *text, unsigned int pos);
    unsigned int get_value(char *text, unsigned int pos);
    unsigned int get_number(char *text, unsigned int pos);
    unsigned int get_string(char *text, unsigned int pos);
    unsigned int get_object(char *text, unsigned int pos);
    unsigned int get_array(char *text, unsigned int pos);
    unsigned int get_boolean(char *text, unsigned int pos);
    char *get_string(char *buffer, unsigned int start, unsigned int size);

public:
    SDT_JSON(/* args */);
    ~SDT_JSON();

    unsigned int parse(char *text, unsigned int len);
    SDT_JSON::tokenvalue_t *get_value(const char *keyword);
    void destroy();
    void to_print();
};

#endif