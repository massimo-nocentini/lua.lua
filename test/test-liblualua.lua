
local lu = require 'luaunit'
local lua = require 'liblualua'
local lambda = require 'operator'

Test_lua = {}

function Test_lua:test_callCS_gettop ()
    local v = 1
    local w = 42

    local a, b = lua.call_with_current_state (
        function (L)
            lu.assertEquals (lua.lua_gettop (L), 1) -- namely this function.

            lua.lua_pushinteger (L, w)
            --lu.assertEquals (lua.lua_tointeger (L, -1), v) -- namely this function.

            lu.assertEquals (lua.lua_gettop (L), 1) -- namely this function.
            return v
        end)

    lu.assertEquals (a, v)
    lu.assertNil (b)
end

function Test_lua:test_luaL_newstate ()
    local L = lua.luaL_newstate ()
    local v = 42
    lu.assertEquals (lua.lua_gettop (L), 0) -- namely this function.
    lua.lua_pushinteger (L, v)
    lu.assertEquals (lua.lua_gettop (L), 1) -- namely this function.
    lu.assertEquals (lua.lua_tointeger (L, -1), v)
end

function Test_lua:test_current_thread ()
    local L = lua.luaL_newstate ()
    local ismain = lua.lua_pushthread (L)
    lu.assertTrue (ismain)
end

os.exit( lu.LuaUnit.run() )