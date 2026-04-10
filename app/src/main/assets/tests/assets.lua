
function tressa(v, message)
	if(v ~= nil and v ~= false) then
		return assert(v, message or "tressatation")
	end
end

print('testing asset i/o')

local a,b,c = io.open('/Assets/xuxu_nao_existe')
assert(not a and type(b) == "string" and type(c) == "number")

a,b,c = io.open('/Assets/a/b/c/d', 'w')
assert(not a and type(b) == "string" and type(c) == "number")

local empty = "/Assets/tests/empty"
local nonexist = "/Assets/xuxu_nao_existe"
local file = "/Assets/tests/_file1"
local file2 = "/Assets/tests/_file2"
local file3 = "/Assets/tests/_file3"
local file4 = "/Assets/tests/_file4"
local file5 = "/Assets/tests/_file5"
local file6 = "/Assets/tests/_file6"
local file7 = "/Assets/tests/_file7"

assert(os.setlocale('C', 'all'))

io.input(io.stdin); io.output(io.stdout);

tressa(os.remove(file))
tressa(os.remove(nonexist))
assert(loadfile(nonexist) == nil)
assert(io.open(nonexist) == nil)

tressa(pcall(function() io.output(file) end), "ensure opening an asset for writing will fail.")

print('+')

-- test GC for files
collectgarbage()
for i=1,120 do
  for i=1,5 do
    io.input(file)
    assert(io.open(file, 'r'))
    io.lines(file)
  end
  collectgarbage()
end

tressa(pcall(function() os.rename(file, file2) end))
tressa(pcall(function() io.output(io.open(file2, "a")) end))

-- test reading past end
local f = io.lines(file)
while f() do end;
assert(not pcall(f))  -- read lines after EOF
assert(not pcall(f))  -- read lines after EOF

local f = assert(io.open(file))
assert(io.type(f) == "file")
tressa(pcall(function() io.output(file) end))

for l in f:lines() do end
assert(f:close());
assert(not pcall(io.close, f))   -- error trying to close again
assert(tostring(f) == "file (closed)")
assert(io.type(f) == "closed file")

io.input(file)
f = io.open(file):lines()
for l in io.lines() do
	local a = f();
	print(l, a)
	assert(l == a, tostring(l) .. " ...... " .. tostring(a))
end

tressa(os.remove(file2))

io.input(file)
do  -- test error returns
  local a,b,c = io.input():write("xuxu")
  assert(not a and type(b) == "string" and type(c) == "number")
end
assert(io.read(0) == "")   -- not eof
assert(io.read(5, '*l') == '"\225lo"')
assert(io.read(0) == "")
assert(io.read() == "second line")
local x = io.input():seek()
assert(io.read() == "third line ")
assert(io.input():seek("set", x))
assert(io.read('*l') == "third line ")
assert(io.read(1) == "\231")
assert(io.read(string.len"fourth_line") == "fourth_line")
assert(io.input():seek("cur", -string.len"fourth_line"))
assert(io.read() == "fourth_line")
assert(io.read() == "")  -- empty line
assert(io.read('*n') == 3450)
assert(io.read(1) == '\n')
assert(io.read(0) == nil)  -- end of file
assert(io.read(1) == nil)  -- end of file
assert(({io.read(1)})[2] == nil)
assert(io.read() == nil)  -- end of file
assert(({io.read()})[2] == nil)
assert(io.read('*n') == nil)  -- end of file
assert(({io.read('*n')})[2] == nil)
assert(io.read('*a') == '')  -- end of file (OK for `*a')
assert(io.read('*a') == '')  -- end of file (OK for `*a')
collectgarbage()
print('+')
io.close(io.input())
assert(not pcall(io.read))

local t = '0123456789'
for i=1,12 do t = t..t; end
assert(string.len(t) == 10*2^12)
collectgarbage()
print('+')

io.input(file2)
assert(io.read() == "alo")
assert(io.read(1) == ' ')
assert(io.read(string.len(t)) == t)
assert(io.read(1) == ' ')
assert(io.read(0))
assert(io.read('*a') == ';end of file\n')
assert(io.read(0) == nil)
assert(io.close(io.input()))

print('+')

local x1 = "string\n\n\\com \"\"''coisas [[estranhas]] ]]'"
assert(loadfile(file3))()
assert(x1 == x2)
print('+')

local filehandle = assert(io.open(file4, 'r'))
local file2handle = assert(io.open(file5, 'rb'))
assert(filehandle ~= file2handle)
assert(type(filehandle) == "userdata")
assert(filehandle:read('*l') == "qualquer coisa")
io.input(file2handle)
assert(io.read(string.len"outra coisa") == "outra coisa")
assert(filehandle:read('*l') == "mais qualquer coisa")
filehandle:close();
assert(type(filehandle) == "userdata")
io.input(file2handle)
assert(io.read(4) == "\0\1\3\0")
assert(io.read(3) == "\0\0\0")
assert(io.read(0) == "")        -- 255 is not eof
assert(io.read(1) == "\255")
assert(io.read('*a') == "\0")
assert(not io.read(0))
assert(file2handle == io.input())
file2handle:close()
collectgarbage()

io.input(file6)
local _,a,b,c,d,e,h,__ = io.read(1, '*n', '*n', '*l', '*l', '*l', '*a', 10)
assert(io.close(io.input()))
assert(_ == ' ' and __ == nil)
assert(type(a) == 'number' and a==123.4 and b==-56e-2)
assert(d=='second line' and e=='third line')
assert(h==[[

and the rest of the file
]])
collectgarbage()

-- testing large files (> BUFSIZ)
io.input(file7)
local x = io.read('*a')
io.input():seek('set', 0)
local y = io.read(30001)..io.read(1005)..io.read(0)..io.read(1)..io.read(100003)
assert(x == y and string.len(x) == 5001*13 + 6)
io.input():seek('set', 0)
y = io.read()  -- huge line
assert(x == y..'\n'..io.read())
assert(io.read() == nil)
io.close(io.input())
x = nil; y = nil

print'+'

io.output(io.stdout)
local d = os.date('%d')
local m = os.date('%m')
local a = os.date('%Y')
local ds = os.date('%w') + 1
local h = os.date('%H')
local min = os.date('%M')
local s = os.date('%S')
print(string.format('test done on %2.2d/%2.2d/%d', d, m, a))
print(string.format(', at %2.2d:%2.2d:%2.2d\n', h, min, s))
print(string.format('%s\n', _VERSION))
