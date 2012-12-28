module(..., package.seeall)

local debug_traceback = debug.traceback;
local unpack, select = unpack, select;
pcall = function(f, ...)
	local n = select("#", ...);
	local params = {...};
	return xpcall(function() return f(unpack(params, 1, n)) end, function(e) return tostring(e).."\n"..debug_traceback(); end);
end

local module_map = {};
function load(modname)
	if not module_map[modname] then 
		local mod = require(modname);
		if not mod then
			return nil, "mod load fail";
		end
		-- 初始化一下模块哦
		call_module_method(mod, "load");
		
		module_map[modname] = mod;
		return mod, "module load success";
	else
		return nil, "module has load";
	end
end

function unload(modname)
	if not module_map[modname] then
		return nil, "module not load";
	end
	-- 卸载模块的时候 清理一下模块吧
	call_module_method(module_map[modname], "unload");
	
	_G[modname] = nil;
	package.loaded[modname] = nil;
	module_map[modname] = nil;
	return true, "module unload success";
end

function reload(modname)
	if not module_map[modname] then 
		return nil, "module-not-loaded"; 
	end
	-- reload模块的时候 是不是有数据要保存起来的
	local ok, data = call_module_method(module_map[modname], "save");

	_G[modname] = nil;
	package.loaded[modname] = nil;
	require(modname);
	call_module_method(module_map[modname], "unload");
	module_map[modname] = nil;
	
	load(modname);
	
	-- 是否有模块数据 要恢复的
	if ok then
		call_module_method(module_map[modname], "restore", data);
	end
	
	return module_map[modname], "module reload success";
end

function module_has_method(module, method)
	return type(module[method]) == "function";
end

function call_module_method(module, method, ...)
	if module_has_method(module, method) then
		local f = module[method];
		return pcall(f, ...);
	else
		return false, "no-such-method";
	end
end

function get_module(modname)
	return module_map[modname];
end
