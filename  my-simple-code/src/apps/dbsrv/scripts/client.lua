
local mylua_path = os.getenv("MYLUA_PATH")
package.path = package.path .. ";" ..  mylua_path .. "/?.lua";

pb.import("msg.proto");

local events = require("utils/events").new();

local user = {};
function onMessage(conId, msg)
	waitMessage(conId, msg);
	return 0;
end

function onConnection(conId)
	user[conId] = {};
	runAllTest(conId);
	return 0;
end

function onDisConnection(conId)
	user[conId] = null;
	return 0;
end

function runAllTest(conId)
	user[conId].co = coroutine.create(function (conId)
														runTest(conId)
													end);	
	waitMessage(conId, nil);																																									
end

function waitMessage(conId, msg)
	if msg then
		local event = msg.argback;
		events.fire_event(event, msg);
	end
	
	coroutine.resume(user[conId].co, conId);  
end

local testUnitTable = {};
function runTest(conId)
	for _, fun in pairs(testUnitTable) do		
		local msg = fun();
		client.send(conId, msg);		
		coroutine.yield();
	end
end

function addTestUnit(fun)
	local msg, checkfun = fun();
	local event = msg.argback;
	if not msg or not checkfun or not event then
		error("add test unit error");
	end
	
	testUnitTable[#testUnitTable + 1] = fun;
	events.add_handler(event, checkfun);
end

function toHex(str)
	return string.gsub(str, ".", function (b) return string.format("%02X ", string.byte(b)) end )
end

require("tests/test");
