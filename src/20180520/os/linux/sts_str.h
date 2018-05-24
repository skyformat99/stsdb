
#ifndef _STS_STR_H
#define _STS_STR_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include <sts_types.h>
#include <sts_malloc.h>

#define sts_sprintf snprintf

int sts_strcpy(char *out_, size_t olen_, const char *in_);
int sts_strncpy(char *out_, size_t olen_, const char *in_, size_t ilen_);

// �Ե�һ���ַ���Ϊ���ȣ���ͷ��ʼ���бȽ�
int sts_strcase_match(const char *son_, const char *source_);

int sts_strcasecmp(const char *s1_, const char *s2_);
int sts_strncasecmp(const char *s1_, const char *s2_, size_t len_);

void sts_trim(char *s);

//STS_MALLOC
char *sts_strdup(const char *str_, size_t len_); 
//STS_MALLOC
char *sts_str_sprintf(size_t mlen_, const char *fmt_, ...);

const char *sts_str_split(const char *s, size_t *len_, char c);

int sts_str_substr_nums(const char *s, char c);
void sts_str_substr(char *out_, size_t olen_, const char *in_, char c, int idx_);
int sts_str_subcmp(const char *sub, const char *s, char c);  //-1û��ƥ���
int sts_str_subcmp_head(const char *sub, const char *s, char c);  //-1û��ƥ���,�Ƚ�ͷ�������ַ��Ƿ���ͬ

const char *sts_str_replace(const char *in, char ic_,char oc_); // ��in�е�ic�滻Ϊoc
void sts_str_to_lower(char *in_);
void sts_str_to_upper(char *in_);

// int sts_strlen_right(const char *str_, const char * right_,const char *ctf_);
// int sts_strlen_left(const char *str_, const char * left_, const char *ctf_);
const char *sts_str_getline(const char *e, int *len, const char *s, size_t size_);


#endif //_STS_STR_H