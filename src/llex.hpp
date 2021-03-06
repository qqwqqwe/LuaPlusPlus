#pragma once
/*
** $Id: llex.h,v 1.79 2016/05/02 14:02:12 roberto Exp $
** Lexical Analyzer
** See Copyright Notice in lua.h
*/

#include <lobject.hpp>
#include <lzio.hpp>

#define FIRST_RESERVED  257

#if !defined(LUA_ENV)
#define LUA_ENV         "_ENV"
#endif

/*
 * WARNING: if you change the order of this enumeration,
 * grep "ORDER RESERVED"
 */
enum RESERVED : uint16_t
{
  /* terminal symbols denoted by reserved words */
  TK_AND = FIRST_RESERVED, TK_BREAK,
  TK_DO, TK_ELSE, TK_ELSEIF, TK_END, TK_FALSE, TK_FOR, TK_FUNCTION,
  TK_GOTO, TK_IF, TK_IN, TK_LOCAL, TK_NIL, TK_NOT, TK_OR, TK_REPEAT,
  TK_RETURN, TK_THEN, TK_TRUE, TK_UNTIL, TK_WHILE,
  /* other terminal symbols */
  TK_IDIV, TK_CONCAT, TK_DOTS, TK_EQ, TK_GE, TK_LE, TK_NE,
  TK_SHL, TK_SHR,
  TK_DBCOLON, TK_EOS,
  TK_FLT, TK_INT, TK_NAME, TK_STRING
};

/* number of reserved words */
#define NUM_RESERVED    (cast(int, TK_WHILE-FIRST_RESERVED+1))

union SemInfo
{
  lua_Number r;
  lua_Integer i;
  TString *ts;
};  /* semantics information */

struct Token
{
  int token;
  SemInfo seminfo;
};

/* state of the lexer plus state of the parser when shared by all
   functions */
struct LexState
{
  LexState(ZIO& z) : z(z) {}

  int current = 0;  /* current character (charint) */
  int linenumber = 0;  /* input line counter */
  int lastline = 0;  /* line of last token 'consumed' */
  Token t;  /* current token */
  Token lookahead;  /* look ahead token */
  struct FuncState* fs = nullptr;  /* current function (parser) */
  class lua_State* L = nullptr;
  ZIO& z;  /* input stream */
  Mbuffer* buff = nullptr;  /* buffer for tokens */
  Table* h = nullptr;  /* to avoid collection/reuse strings */
  struct Dyndata* dyd = nullptr;  /* dynamic structures used by the parser */
  TString* source = nullptr;  /* current source name */
  TString* envn = nullptr;  /* environment variable name */
};

LUAI_FUNC void luaX_init(lua_State *L);
LUAI_FUNC void luaX_setinput(lua_State *L, LexState *ls, TString *source, int firstchar);
LUAI_FUNC TString *luaX_newstring(LexState *ls, const char *str, size_t l);
LUAI_FUNC void luaX_next(LexState *ls);
LUAI_FUNC int luaX_lookahead(LexState *ls);
LUAI_FUNC void luaX_syntaxerror(LexState *ls, const char *s);
LUAI_FUNC const char *luaX_token2str(LexState *ls, int token);
