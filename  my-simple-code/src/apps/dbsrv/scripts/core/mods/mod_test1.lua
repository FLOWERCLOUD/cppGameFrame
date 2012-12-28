
module(..., package.seeall);

local data = 1;

--function load()
--	print("load");
--	data = 999;
--end

--function unload()
--	print("unload");
--end

--function reload()
--	print("reload");
--end

--function save()
--	print("save");
--	return data;
--end

--function restore(d)
--	print("restore ");
--	print(d);
--	data = d;
--end

----------------
function setdata(d)
	data = d;
end

function getdata()
	return data;
end

function test()
	print("hello world");
end
