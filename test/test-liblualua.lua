
local lu = require 'luaunit'
local lua = require 'liblualua'
local lambda = require 'operator'

Test_lua = {}


function Test_lua:test_luaL_newstate ()
    local L = lua.luaL_newstate ()
    local v = 42
    lu.assertEquals (lua.lua_gettop (L), 0)
    lua.lua_pushinteger (L, v)
    lu.assertEquals (lua.lua_gettop (L), 1)
    lu.assertEquals (lua.lua_tointeger (L, 1), v)
    lu.assertEquals (lua.lua_tointeger (L, -1), v)

    lua.lua_close (L)
end

function Test_lua:test_current_thread ()
    local L = lua.luaL_newstate ()
    local ismain = lua.lua_pushthread (L)
    lu.assertTrue (lua.lua_isthread (L, 1))
    lu.assertTrue (ismain)
    lua.lua_close (L)
end

function Test_lua:test_lua_call ()
   
    local L = lua.current_thread ()
    
    local T = lua.lua_newthread (L)
    
    local j = 42

    lua.push (T, 
        function (i)
            j = j + 1
            return i + 1, j 
        end, 
        42)

    lua.lua_call (T, 1, lua.LUA_MULTRET)

    local r, s = lua.lua_tointeger (T, 1), lua.lua_tointeger (T, 2)

    lu.assertEquals (r, 43)
    lu.assertEquals (s, j)

end


function Test_lua:test_lua_tocfunction ()
   
    local T, TT = lua.lua_newthread ()
 
    local j = 42

    lua.push (T, 
        function (i)
            j = j + 1
            return i + 1, j 
        end)

    lu.assertTrue (lua.lua_isfunction (T, 1))

    local f = lua.lua_tocfunction (T, 1)
    lu.assertNil (f)
end


function Test_lua:test_lua_resume ()
   
    local L = lua.current_thread ()

    local _, t = lua.lua_newthread (L)
    local T = lua.to_lightuserdata (t)
    local S = lua.lua_newthread (L)

    local j, init = 42, 0

    lua.push (T, function (i) j = j + 1; return i + 1, j end, init)

    local retcode, nres = lua.lua_resume (T, S, 1)

    lu.assertEquals (retcode, lua.LUA_OK)
    lu.assertEquals (nres, 2)
    lu.assertEquals (j, 43)
    lu.assertEquals (lua.lua_gettop(S), 0)
    lu.assertEquals (lua.lua_gettop(T), 2)
    lu.assertEquals (lua.lua_tointeger (T, 1), init + 1)
    lu.assertEquals (lua.lua_tointeger (T, 2), j)

end



function Test_lua:test_lua_resume_yield ()
   
    local L = lua.current_thread ()

    local T = lua.lua_newthread (L)
    local S = lua.lua_newthread (L)

    local init = 0

    lua.push (T, function (i) while true do i = coroutine.yield (i) end end, init)

    local retcode, nres = lua.lua_resume (T, S, 1)

    lu.assertEquals (retcode, lua.LUA_YIELD)
    lu.assertEquals (nres, 1)
    lu.assertEquals (lua.lua_gettop(S), 0)
    lu.assertEquals (lua.lua_gettop(T), 1)
    
    local j = lua.lua_tointeger (T, 1)
    lu.assertEquals (j, init)

    lua.push (T, j + 1);
    local retcode, nres = lua.lua_resume (T, S, 1)

    lu.assertEquals (retcode, lua.LUA_YIELD)
    lu.assertEquals (nres, 1)
    lu.assertEquals (lua.lua_gettop(S), 0)
    lu.assertEquals (lua.lua_gettop(T), 1)
    lu.assertEquals (lua.lua_tointeger (T, 1), init + 1)


end


function Test_lua:test_callCS_gettop ()
    local v = 1
    local w = 42

    local a, t, b = lua.call_with_current_state (
        function (L)
            lu.assertEquals (lua.lua_gettop (L), 1)

            lua.lua_pushinteger (L, w)
            lu.assertEquals (lua.lua_tointeger (L, -1), -1)

            lu.assertEquals (lua.lua_gettop (L), 1)
            
            return v, 3
        end)

    lu.assertEquals (a, v)
    lu.assertEquals (t, 3)
    lu.assertNil (b)
end


function Test_lua:test_lua_newthread ()
   
    lua.call_with_current_state (
        function (L)
            
            local T = lua.lua_newthread (L)
            local S = lua.lua_newthread (L)

            lu.assertNotEquals (T, S)
        end
    )

end

function Test_lua:test_multiple_current_thread ()
   
    local L = lua.current_thread ()

    lua.push (L, 3, 4)

    local function A (a, b)

        local L = lua.current_thread ()
        lu.assertEquals (lua.lua_gettop (L), 1)

        return 1, 2, 3

    end

    local a, b, c = A (3, 4)

    lua.push (L, a, b, c)

    lu.assertEquals (lua.lua_gettop(L), 1)


end

function Test_lua:test_same_current_thread_twice ()
           
    local T, t = lua.current_thread ()

    lua.push (T, 3)

    local S, s = lua.current_thread ()

    lua.push (S, 4)

    lu.assertEquals (T, S)
    lu.assertEquals (t, s)

    lu.assertEquals (lua.lua_gettop (T), lua.lua_gettop (S))

end


os.exit( lu.LuaUnit.run() )