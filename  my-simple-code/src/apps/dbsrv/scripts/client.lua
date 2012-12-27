
pb.import("msg.proto");

local usermanage = {};

function onMessage(conId, msg)
	--print("onMessage");
	
	waitMessage(conId, msg);
	return 0;
end

function onConnection(conId)
	--print("onConnection");
	-- 
	usermanage[conId] = {};
	runAllTest(conId);
	return 0;
end

function onDisConnection(conId)
	--print("onDisConnection");
	usermanage[conId] = null;
	return 0;
end

function runAllTest(conId)
	usermanage[conId].co = coroutine.create(function (conId)
														runTest(conId)
													end);	
	waitMessage(conId, nil);																																									
end

function waitMessage(conId, msg)
	 local status, assert_function = coroutine.resume(usermanage[conId].co, conId); 
	 if msg ~= nil  and usermanage[conId].assert_function ~= nil then
	 	if usermanage[conId].assert_function(msg) then
	 		--print("test ok");
	 	else
	 		print("test error");
	 		os.exit();
	 	end
	 end
	 
	 usermanage[conId].assert_function = assert_function;	 
	 
	 if assert_function == nil then
	 	print("test end, no error!!!!\n");
	 	os.exit();
	 end
end

local testUnitTable = {};
function runTest(conId)
	--print("start runTest========= table size:" .. #testUnitTable);
	
	for _, fun in pairs(testUnitTable) do		
		local msg, assert_function = fun();
		client.send(conId, msg);		
		coroutine.yield(assert_function);
	end
	
	--coroutine.yield(nil);
	--print("end runTest=========");
	--print("test end, no error!!!!\n");
	--os.exit();
end

function addTestUnit(fun)
	testUnitTable[#testUnitTable + 1] = fun;
end

function toHex(str)
	return string.gsub(str, ".", function (b) return string.format("%02X ", string.byte(b)) end )
end

package.path = package.path .. ";" .. os.getenv("MYLUA_PATH") .. "/?.lua";
require("test1");
require("test2");
require("test3");
require("test5");
require("test4");
require("test6");
require("test7");
--local t = require("mod_test");
--t.test();
--t.testlocal();

