
package.path = package.path .. ";" .. os.getenv("MYLUA_PATH") .. "/?.lua";

local modmgr = require("modmgr");

local mod = modmgr.load("mods/mod_test1");
--mod.test();
--modmgr.unload("mods/mod_test1");
print("load before: " .. mod.getdata());
mod.setdata(100);
print("set data before: " .. mod.getdata());
mod = modmgr.reload("mods/mod_test1");
if mod then
	print("reload after:" .. mod.getdata());
else
	print("reload mod error");
end



