
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>

/// @brief
/// @return a new Lua state, as a *light userdata* Lua object.
static int l_luaL_newstate(lua_State *L)
{
    lua_State *s = luaL_newstate();

    int ismain = lua_pushthread(s);
    assert(ismain == 1);

    lua_pushlightuserdata(L, s); //  to let the client close it.
    lua_xmove(s, L, 1);
    
    return 2;
}

static int l_lua_close(lua_State *L)
{
    lua_State *s = (lua_State *)lua_touserdata(L, 1);

    lua_close(s);

    return 0;
}

/// @brief
/// @param L the state we want to push the integer on.
/// @param i the integer we want to push.
/// @return
static int l_lua_pushinteger(lua_State *L)
{
    lua_State *s = lua_tothread(L, 1);
    lua_Integer i = lua_tointeger(L, 2);

    lua_pushinteger(s, i);

    return 0;
}

/// @brief
/// @param L the state we want to read the integer from.
/// @param i the index of the integer.
/// @return
static int l_lua_tointeger(lua_State *L)
{

    lua_State *s = lua_tothread(L, 1);
    int i = lua_tointeger(L, 2);

    lua_Integer j = lua_tointeger(s, i);

    lua_pushinteger(L, j);

    return 1;
}

static int l_current_thread(lua_State *L)
{
    int ismain = lua_pushthread(L);

    lua_pushboolean(L, ismain);

    return 2;
}

static int l_call_with_current_state(lua_State *L)
{
    int nargs = lua_gettop(L);

    luaL_argcheck(L, lua_isfunction(L, 1), 1, "Expected a function.");

    lua_pushvalue(L, 1); // duplicate the given function.

    int nres = l_current_thread(L);

    lua_call(L, nres, LUA_MULTRET);

    return lua_gettop(L) - nargs;
}

static int l_lua_pushthread(lua_State *L)
{

    lua_State *s = lua_tothread(L, 1);
    int ismain = lua_pushthread(s);
    lua_pushboolean(L, ismain);

    return 1;
}

static int l_lua_tothread(lua_State *L)
{

    lua_State *s = lua_tothread(L, 1);
    int i = lua_tointeger(L, 2);

    lua_State *t = lua_tothread(s, i);

    lua_pushlightuserdata(L, t);

    return 1;
}

static int l_lua_newthread(lua_State *L)
{
    lua_State *s = lua_isnone(L, 1) ? L : lua_tothread(L, 1);

    lua_State *t = lua_newthread(s);

    if (s != L)
        lua_pushlightuserdata(L, t);

    return 1;
}

static int l_push(lua_State *L)
{
    lua_State *s = lua_tothread(L, 1);

    int nargs = lua_gettop(L) - 1;

    lua_xmove(L, s, nargs);

    return 0;
}

static int l_lua_resume(lua_State *L)
{

    lua_State *s = lua_tothread(L, 1);
    lua_State *f = lua_tothread(L, 2);
    int nargs = lua_tointeger(L, 3);

    int res;

    int retcode = lua_resume(s, f, nargs, &res);

    lua_pushinteger(L, retcode);
    lua_pushinteger(L, res);

    return 2;
}

static int l_lua_call(lua_State *L)
{
    lua_State *s = lua_tothread(L, 1);
    int nargs = lua_tointeger(L, 2);
    int nres = lua_tointeger(L, 3);

    lua_call(s, nargs, nres);

    return 0;
}

static int l_lua_gettop(lua_State *L)
{

    lua_State *s = lua_tothread(L, 1);

    int gt = lua_gettop(s);

    lua_pushinteger(L, gt);

    return 1;
}

static const struct luaL_Reg liblualua[] = {
    {"lua_gettop", l_lua_gettop},
    {"lua_pushinteger", l_lua_pushinteger},
    {"lua_tointeger", l_lua_tointeger},
    {"lua_pushthread", l_lua_pushthread},
    {"lua_newthread", l_lua_newthread},
    {"lua_tothread", l_lua_tothread},
    {"lua_resume", l_lua_resume},
    {"lua_call", l_lua_call},
    {"lua_close", l_lua_close},
    {"luaL_newstate", l_luaL_newstate},
    {"push", l_push},
    {"current_thread", l_current_thread},
    {"call_with_current_state", l_call_with_current_state},
    {NULL, NULL} /* sentinel */
};

static void constants(lua_State *L)
{
    lua_pushinteger(L, LUA_OK);
    lua_setfield(L, -2, "LUA_OK");

    lua_pushinteger(L, LUA_YIELD);
    lua_setfield(L, -2, "LUA_YIELD");

    lua_pushinteger(L, LUA_MULTRET);
    lua_setfield(L, -2, "LUA_MULTRET");
}

extern int luaopen_liblualua(lua_State *L)
{
    luaL_newlib(L, liblualua);

    constants(L);

    return 1;
}
