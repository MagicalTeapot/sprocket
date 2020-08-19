function Class(base, init)
   local c = {}    -- a new class instance
   if not init and type(base) == 'function' then
      init = base
      base = nil
   elseif type(base) == 'table' then
    -- our new class is a shallow copy of the base class!
      for i,v in pairs(base) do
         c[i] = v
      end
      c._base = base
   end
   -- the class will be the metatable for all its objects,
   -- and they will look up their methods in it.
   c.__index = c
 
   -- expose a constructor which can be called by <classname>(<args>)
   local mt = {}
   mt.__call = function(class_tbl, ...)
   local obj = {}
   setmetatable(obj,c)
   if init then
      init(obj,...)
   else 
      -- make sure that any stuff from the base class is initialized!
      if base and base.init then
      base.init(obj, ...)
      end
   end
   return obj
   end
   c.init = init
   c.is_a = function(self, klass)
      local m = getmetatable(self)
      while m do 
         if m == klass then return true end
         m = m._base
      end
      return false
   end
   setmetatable(c, mt)
   return c
end

KEYBOARD_W = 87
KEYBOARD_A = 65
KEYBOARD_S = 83
KEYBOARD_D = 68
KEYBOARD_C = 67
KEYBOARD_Q = 81
KEYBOARD_E = 69
KEYBOARD_R = 82
KEYBOARD_F = 70
KEYBOARD_V = 86
KEYBOARD_X = 88
KEYBOARD_Y = 89
KEYBOARD_Z = 90

KEYBOARD_SPACE = 32
KEYBOARD_BACKSPACE = 259
KEYBOARD_LSHIFT = 340
KEYBOARD_ESC = 256
KEYBOARD_TAB = 258
KEYBOARD_ENTER = 257

KEYBOARD_KEYPAD_ENTER = 335

KEYBOARD_LEFT_ARROW = 263
KEYBOARD_RIGHT_ARROW = 262
KEYBOARD_UP_ARROW = 265
KEYBOARD_DOWN_ARROW = 264

KEYBOARD_PAGE_UP = 266
KEYBOARD_PAGE_DOWN = 267

KEYBOARD_HOME = 268
KEYBOARD_END = 269
KEYBOARD_INSERT = 260
KEYBOARD_DEL = 26