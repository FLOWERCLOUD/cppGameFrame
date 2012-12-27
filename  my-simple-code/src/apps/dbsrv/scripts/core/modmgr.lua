
module(..., package.seeall)

local modloader = require("utils/modloader");
local module_map = {};

local debug_traceback = debug.traceback;
local unpack, select = unpack, select;
pcall = function(f, ...)
	local n = select("#", ...);
	local params = {...};
	return xpcall(function() return f(unpack(params, 1, n)) end, function(e) return tostring(e).."\n"..debug_traceback(); end);
end

function load(modname)
	if not module_map[modname] then 
		local mod, err = modloader.load_code(modname);
		if not mod then
			print("load mod error, result: " .. err);
			return;
		end
		local module = 
		{
			[module] = mod,
			[name] = modname,
			[path] = err,
		};
		module_map[modname] = module;
	else
		print("mod has load");
	end
end

function unload(modname)
	if not module_map[modname] then
		print("this mod not load");
	end
	module_map[modname] = nil;
	print("mod has unload");
end

function reload(modname)
end

function module_has_method(module, method)
	return type(module.module[method]) == "function";
end

function call_module_method(module, method, ...)
	if module_has_method(module, method) then
		local f = module.module[method];
		return pcall(f, ...);
	else
		return false, "no-such-method";
	end
end
