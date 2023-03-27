#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "json.h"

JsonList_t *mallocJsonList(int len) {
	return (JsonList_t*)malloc(sizeof(JsonList_t) + sizeof(Json_t)*len);
}

Json_t *newJsonNode(Json_t *next, Json_Typename_t type, char *key, void *data, int data_size) {
	Json_t *new_node = (Json_t*)malloc(sizeof(Json_t));
	new_node->type = type;
	new_node->key = (char*)malloc(strlen(key)+1);
	strcpy(new_node->key, key);
	new_node->data = (void*)malloc(data_size);
	memcpy(new_node->data, data, data_size);
	return new_node;
}

Json_t *newStringJsonNode(Json_t *next, char *key, char *str) {
	return newJsonNode(next, string, key, str, strlen(str)+1);
}

void freeJsonNode(Json_t *node) {
	free(node->key);
	free(node->data);
	free(node);
	return;
}

Json_t *searchJson(Json_t *top, char *key) {
	Json_t *p = top;
	while(p != NULL) {
		if(strcmp(p->key, key) == 0) {
			return p;
		}
		p = p->next;
	}
	return NULL;
}

Token_t *newToken(Token_t *next, Token_type_t type, int size, char *str) {
	Token_t *new_token = (Token_t*)malloc(sizeof(Token_t));
	new_token->next = next;
	new_token->type = type;
	new_token->size = size;
	new_token->str = (char*)malloc(strlen(str)+1);
	strcpy(new_token->str, str);
	return new_token;
}

Token_t *tokenize(char* json) {
	char *p = json;
	char *q = NULL;
	int i = 0, num = 0;
	Token_t *top = NULL;
	Token_t *tok_ptr = NULL;

	if(json[0] != '{') {
		fprintf(stderr, "invalid json.\n");
		return NULL;
	}
	top = tok_ptr = newToken(NULL, TK_BRACKET, 1, p);
	p++;
	while(*p != '\0') {
		if(*p == '{' || *p == '}' || *p == '[' || *p == ']') {
			tok_ptr->next = newToken(NULL, TK_BRACKET, 1, p);
			tok_ptr = tok_ptr->next;
			p++;
		} else if(*p == ':') {
			tok_ptr->next = newToken(NULL, TK_COLON, 1, p);
			tok_ptr = tok_ptr->next;
			p++;
		} else if(*p == ',') {
			tok_ptr->next = newToken(NULL, TK_COMMA, 1, p);
			tok_ptr = tok_ptr->next;
			p++;
		} else if(*p == '"') {
			i = 1;
			while(p[i] != '"') i++;
			i--;
			tok_ptr->next = newToken(NULL, TK_STR, i, p+1);
			tok_ptr = tok_ptr->next;
			p += i+2;
		} else if(strncmp(p, "true", 4) == 0 || strncmp(p, "null", 4) == 0) {
			tok_ptr->next = newToken(NULL, TK_BOOLEAN, 4, p);
			tok_ptr = tok_ptr->next;
			p += 4;
		} else if(strncmp(p, "false", 5) == 0) {
			tok_ptr->next = newToken(NULL, TK_BOOLEAN, 5, p);
			tok_ptr = tok_ptr->next;
			p += 5;
		} else {
			errno = 0;
			num = strtol(p, &q, 10);
			if(errno == 0 && p != q) {
				tok_ptr->next = newToken(NULL, TK_NUM, q-p, p);
				tok_ptr = tok_ptr->next;
				p = q;
			} else if(*p == ' ' || *p == '\t' || *p == '\n'){
				p++;
			} else {
				fprintf(stderr, "tokenize failed.\n");
				return NULL;
			}
		}
	}
	return top;
}

bool isToken(Token_t *tok, Token_type_t type, char *str) {
	return 0;
}

Json_t *parseJson(Token_t *tok_top) {	
	Json_t *json_top;
	Token_t *tok_p = tok_top;
	if(!isToken(tok_p, TK_BRACKET, "{")) {
		fprintf(stderr, "parse failed.\n");
		return NULL;
	}
	while(tok_p != NULL) {
		if(tok_p->next == NULL) { //次のトークンがないのでこのトークンは必ず終端の}になるはず。
			if(isToken(tok_p, TK_BRACKET, "}")) {
				return json_top;
			} else {
				fprintf(stderr, "parse failed.\nmaybe you forget }\n");
				return NULL;
			}
		} else if(!isToken(token_p->next, TK_COLON, ":")) { 
			//終端でなければ キー:要素 が来るはずなので、次のトークンは必ず:のはず
			fprintf(stderr, "parse failed.\n");
			return NULL;
		} else if(token_p->next->next == NULL) {
			//キー:まであるのに要素が無いのでダメーーー！！！
			fprintf(stderr, "parse failed.\n");
			return NULL;
		} else {
		}
	}

Json_t *analyzeJson(char *json) {
	Token_t *t = tokenize(json);
	if(t == NULL) {
		fprintf(stderr, "analyze failed.\n");
		return NULL;
	}
	return parseJson(t);
}