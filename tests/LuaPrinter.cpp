#include <lua.hpp>
#include <LuaBinder.hpp>
#include <lualib.hpp>
#include <LuaPrinter.hpp>
#include <StringUtil.hpp>

bool LuaPrinter::loaded = LuaPrinter::staticInit();

bool LuaPrinter::staticInit()
{
  using MyBinder = LuaBinder<LuaPrinter>;

  MyBinder::registerMetaMethod("__index", &LuaPrinter::luaIndex);
  MyBinder::registerMetaMethod("__newindex", &LuaPrinter::luaNewIndex);

  MyBinder::registerMethod("print", &LuaPrinter::luaPrint);

  //MyBinder::registerRead("wrte", &LuaPrinter::luaRead);
  //MyBinder::registerWrite("read", &LuaPrinter::luaWrite);

  return true;
}

LuaPrinter::LuaPrinter()
  : L(luaL_newstate())
{
  if (!this->L)
    throw std::runtime_error("Failed to create Lua state.");

  // Load standard libraries
  luaL_openlibs(L);

  // Register self into the Lua state
  lua_newtable(L);
  LuaBinder<LuaPrinter>::registerObject(L, this, -1);
  lua_setglobal(L, "printer");
}

LuaPrinter::~LuaPrinter()
{
  lua_close(this->L);
  this->L = nullptr;
}

int32_t LuaPrinter::luaIndex(lua_State* L)
{
  return LuaBinder<LuaPrinter>::processCommonLuaIndex(L, this);
}

int32_t LuaPrinter::luaNewIndex(lua_State* L)
{
  return LuaBinder<LuaPrinter>::processCommonLuaNewIndex(L, this);
}

int32_t LuaPrinter::luaPrint(lua_State* L)
{
  this->lines.emplace_back(luaL_checkstring(L, -1));
  return 0;
}

const std::string& LuaPrinter::getLastLine() const
{
  if (this->lines.empty())
    throw std::runtime_error("Lines are empty.");
  return this->lines.back();
}

std::string LuaPrinter::runCommand(const std::string& command)
{
  const size_t before = this->lines.size();

  this->doString("printer.print(tostring(" + command + "))");

  if (this->lines.size() > before)
    return this->lines.back();
  return std::string();
}

std::string LuaPrinter::scriptCommand(const std::string& command)
{
  const size_t before = this->lines.size();

  this->doString(command);

  if (this->lines.size() > before)
    return this->lines.back();
  return std::string();
}

void LuaPrinter::doString(const std::string& string)
{
  if (luaL_dostring(this->L, string.c_str()))
  {
    std::string error;
    switch (lua_type(this->L, -1))
    {
      case LUA_TNIL:
        error = "nil";
        break;
      case LUA_TNUMBER:
        error = StringUtil::ssprintf(LUA_NUMBER_FMT, luaL_checknumber(this->L, -1));
        break;
      case LUA_TBOOLEAN:
        error = lua_toboolean(this->L, -1) == 0 ? "false" : "true";
        break;
      case LUA_TSTRING:
        error = luaL_checkstring(this->L, -1);
        break;
      default:
        error = "Unknown type";
    }

    throw std::runtime_error("Error: " + error);
  }
}