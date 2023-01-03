
/*
    This is a glue c file for importing delta client c functions into Lua workflow.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>

/// @brief
/// @return a new Lua state. 
static int l_luaL_newstate (lua_State* L) {

    lua_State* s = luaL_newstate ();

    lua_pushlightuserdata (L, s);

    return 1;
}

/// @brief 
/// @param L the state we want to push the integer on.
/// @param i the integer we want to push.
/// @return 
static int l_lua_pushinteger (lua_State* L) {

    lua_State* s = (lua_State*) lua_touserdata (L, -2);
    int i = lua_tointeger (L, -1);

    lua_pushinteger (s, i);

    return 0;
}

/// @brief 
/// @param L the state we want to read the integer from.
/// @param i the index of the integer.
/// @return 
static int l_lua_tointeger (lua_State* L) {

    lua_State* s = (lua_State*) lua_touserdata (L, -2);
    int i = lua_tointeger (L, -1);

    lua_Integer j = lua_tointeger (s, i);

    lua_pushinteger (L, j);

    return 1;
}

static int l_call_with_current_state (lua_State* L) {

    int nargs = lua_gettop (L);

    luaL_argcheck (L, lua_isfunction (L, -1), 1, "Expected a function.");

    lua_pushvalue (L, -1);          // duplicate the given function.
    lua_pushlightuserdata (L, L);   // namely, the current state.

    lua_call (L, 1, LUA_MULTRET);

    return lua_gettop (L) - nargs;
}

static int l_lua_pushthread (lua_State* L) {

    lua_State* s = (lua_State*) lua_touserdata (L, -1);
    int ismain = lua_pushthread (s);
    lua_pushboolean (L, ismain);
    
    return 1;
}

static int l_lua_gettop (lua_State* L) {

    lua_State* s = (lua_State*) lua_touserdata (L, -1);

    int gt = lua_gettop (s);

    lua_pushinteger (L, gt);

    return 1;
}

static const struct luaL_Reg liblualua[] = {
    {"lua_gettop", l_lua_gettop},
    {"lua_pushinteger", l_lua_pushinteger},
    {"lua_tointeger", l_lua_tointeger},
    {"lua_pushthread", l_lua_pushthread},
    {"luaL_newstate", l_luaL_newstate},
    {"call_with_current_state", l_call_with_current_state},
    {NULL, NULL} /* sentinel */
};

static void constants(lua_State *L)
{
    lua_pushinteger(L, LUA_OK);
    lua_setfield(L, -2, "LUA_OK");
}

extern int luaopen_liblualua(lua_State *L)
{
    luaL_newlib(L, liblualua);

    constants(L);

    return 1;
}
