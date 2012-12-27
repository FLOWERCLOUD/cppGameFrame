
module(..., package.seeall)

local dir_sep, path_sep = package.config:match("^(%S+)%s(%S+)");
local mod_dir = {};
for path in (os.getenv("LUA_MOD_PATH") or "./mods/"):gsub("[/\\]", dir_sep):gmatch("[^"..path_sep.."]+") do
	path = path..dir_sep; -- add path separator to path end
	path = path:gsub(dir_sep..dir_sep.."+", dir_sep); -- coalesce multiple separaters
	mod_dir[#mod_dir + 1] = path;
end

local io_open, os_time = io.open, os.time;
local loadstring, pairs = loadstring, pairs;

local function load_file(names)
	local file, err, path;
	for i=1,#mod_dir do
		for j=1,#names do
			path = mod_dir[i]..names[j];
			file, err = io_open(path);
			if file then
				local content = file:read("*a");
				file:close();
				return content, path;
			end
		end
	end
	return file, err;
end

function load_resource(mod, resource)
	resource = resource or "mod_"..mod..".lua";

	local names = {
		"mod_"..mod.."/"..mod.."/"..resource; -- mod_hello/hello/mod_hello.lua
		"mod_"..mod.."/"..resource;              -- mod_hello/mod_hello.lua
		mod.."/"..resource;                      -- hello/mod_hello.lua
		resource;                                   -- mod_hello.lua
	};

	return load_file(names);
end

function load_code(mod, resource)
	local content, err = load_resource(mod, resource);
	if not content then return content, err; end
	local path = err;
	local f, err = loadstring(content, "@"..path);
	if not f then return f, err; end
	return f, path;
end
