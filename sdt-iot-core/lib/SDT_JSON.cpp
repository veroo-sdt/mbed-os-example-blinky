#include "SDT_JSON.h"

SDT_JSON::SDT_JSON(/* args */)
{
}

SDT_JSON::~SDT_JSON()
{
    destroy();
}

void SDT_JSON::init()
{
    destroy();
}
void SDT_JSON::to_print()
{
    std::map<const char *, int>::iterator it = _tokens.begin();
    for (; it != _tokens.end(); it++)
    {
        int index = (*it).second;
        SDT_JSON::tokenvalue_t tokenvalue = _values[index];
        printf("[%d] |%s| : |%s|\n", tokenvalue.type, (*it).first, tokenvalue.value);
        // printf("==========================\n");
        // printf("KEY\t: |%s|\n", (*it).first);
        // printf("VALUE[%d]: |%s|\n", tokenvalue.type, tokenvalue.value);
    }
}

char *SDT_JSON::get_string(char *buffer, unsigned int start, unsigned int end)
{
    unsigned int size = end - start + 1;
    char *str = new char[size];
    memset(str, 0, size);
    memcpy(str, buffer + start, size - 1);
    // printf("%d ~ %d(%d) |%s|\n", start, end, size, str);
    return str;
}

unsigned int SDT_JSON::get_keyword(char *text, unsigned int pos)
{
    char c;
    int start = 0;
    int end = 0;
    // Key값이 오기전 다른 문자는 무시
    while (text[pos++] != '\"')
        ;
    //Key의 시작
    start = pos;
    while ((c = text[pos++]) != ':')
    {
        if (c == '\"')
        {
            // 문자열 중에 마지막 만나는 큰따옴표를 찾기 위해
            end = pos - 1;
            // 큰타옴표를 문자열로 넣기 위해 \를 넣어서 생기는 문제 해결
            if (text[end - 1] == '\\')
            {
                end--;
            }
        }
    }
    if (end != 0)
    {
        char *keyword = get_string(text, start, end);
        _tokens.insert(make_pair(keyword, _values.size()));
    }

    return pos - 1;
}

unsigned int SDT_JSON::get_number(char *text, unsigned int pos)
{
    SDT_JSON::tokenvalue_t tokenvalue = {INT, NULL};
    char c;
    unsigned start = pos;
    unsigned end = 0;
    while ((c = text[pos++]) != '\0')
    {
        if (c == ',' || c == '\r' || c == '\n' || c == '}')
        {
            tokenvalue.value = get_string(text, start, end);
            _values.push_back(tokenvalue);
            break;
        }
        else if (c == '.')
        {
            tokenvalue.type = FLOAT;
        }
        else if (c != 32)
        {
            end = pos;
        }
    }

    return pos;
}

unsigned int SDT_JSON::get_string(char *text, unsigned int pos)
{
    SDT_JSON::tokenvalue_t tokenvalue = {STRING, NULL};
    char c = text[pos];
    int start = pos;
    int innercount = 0;
    while ((c = text[++pos]) != '\0')
    {
        if (innercount % 2 != 0 && (c == ',' || c == '\r' || c == '\n' || c == '}'))
        {
            tokenvalue.value = get_string(text, start + 1, pos - 1);
            _values.push_back(tokenvalue);
            break;
        }
        else if (c == '\"')
        {
            innercount++;
        }
    }

    return pos;
}

unsigned int SDT_JSON::get_object(char *text, unsigned int pos)
{
    SDT_JSON::tokenvalue_t tokenvalue = {OBJECT, NULL};
    char c = text[pos];
    int start = pos;
    int innercount = 1;
    while ((c = text[++pos]) != '\0')
    {
        if (innercount == 0 && (c == ',' || c == '\r' || c == '\n' || c == '}'))
        {
            tokenvalue.value = get_string(text, start, pos);
            _values.push_back(tokenvalue);
            break;
        }
        else if (c == '{')
        {
            innercount++;
        }
        else if (c == '}')
        {
            innercount--;
        }
    }
    return pos;
}

unsigned int SDT_JSON::get_array(char *text, unsigned int pos)
{
    SDT_JSON::tokenvalue_t tokenvalue = {ARRAY, NULL};
    char c = text[pos];
    int start = pos;
    int innercount = 1;
    while ((c = text[++pos]) != '\0')
    {
        if (innercount == 0 && (c == ',' || c == '\r' || c == '\n' || c == '}'))
        {
            tokenvalue.value = get_string(text, start, pos);
            _values.push_back(tokenvalue);
            break;
        }
        else if (c == '[')
        {
            innercount++;
        }
        else if (c == ']')
        {
            innercount--;
        }
    }

    return pos;
}

unsigned int SDT_JSON::get_boolean(char *text, unsigned int pos)
{
    SDT_JSON::tokenvalue_t tokenvalue = {BOOL, NULL};
    char c = text[pos];
    int start = pos;
    while ((c = text[++pos]) != '\0')
    {
        if (c == ',' || c == '\r' || c == '\n' || c == '}')
        {
            tokenvalue.value = get_string(text, start, pos);
            _values.push_back(tokenvalue);
            break;
        }
    }

    return pos;
}

unsigned int SDT_JSON::get_value(char *text, unsigned int pos)
{
    char c = text[++pos];
    if ('0' <= c && c <= '9')
    {
        pos = get_number(text, pos);
    }
    else if ('f' == c || 'F' == c || 't' == c || 'T' == c)
    {
        pos = get_boolean(text, pos);
    }
    else if ('\"' == c)
    {
        pos = get_string(text, pos);
    }
    else if ('[' == c)
    {
        pos = get_array(text, pos);
    }
    else if ('{' == c)
    {
        pos = get_object(text, pos);
    }
    else
    {
        while (!isspace(text[++pos]))
            ;
    }
    return pos;
}

unsigned int SDT_JSON::parse(char *text, unsigned int len)
{
    init();
    // int count = 0;
    int pos = -1;
    char c;
    printf("=======original============\n[LENGTH : %d]\n\n%s\n\n\n", len, text);
    while (isspace(c = text[++pos]))
        ;
    if (c != '{')
    {
        return 0;
    }
    do
    {
        // 새로운 키워드나 다른 줄의 문장을 비교하기 위해 공백을 제외한 구간 표시
        while (isspace(c = text[++pos]))
            ;
        if (c == '}')
        {
            // Json 의 전체 종료시
            // Object 의 경우에도 } 표시가 종료 이지만, 해당 위치에서 체크되어질 가능성이 없다.
            break;
        }
        // printf("===K Start[%d] - CH : %c, CODE : %d\n", pos, c, c);
        pos = get_keyword(text, pos - 1);
        // printf("===K END  [%d] - CH : |%c|, CODE : %d\n", pos, text[pos], text[pos]);

        // Value 가 시작하기 전 공백 제거
        while (isspace(c = text[++pos]))
            ;
        // printf("===V Start[%d] - CH : |%c|, CODE : %d\n", pos, c, c);
        pos = get_value(text, pos - 1);
        // printf("===V END  [%d] CODE : %d\n", pos, text[pos]);
    } while (text[pos] != '\0' && text[pos] != '}');

    return _tokens.size();
}

SDT_JSON::tokenvalue_t *SDT_JSON::get_value(const char *keyword)
{
    auto search = _tokens.find(keyword);
    if (search != _tokens.end())
    {
        return &_values[search->second];
    }
    else
    {
        return NULL;
    }
}

void SDT_JSON::destroy()
{
    _tokens.clear();
    _values.clear();
}