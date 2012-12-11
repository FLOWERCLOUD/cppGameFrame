
pb.import("test.proto");
function test(param)
	print("hello test " .. param);
	do
	local msg = pb.new("lm.test");
	--print("msg: " .. getmetatable(msg));
	local tmp = getmetatable(msg);
	--print(getmetatable(msg));

	for k in pairs(tmp) do
		print("k: " .. k);
	end

	msg.uid = 1;  --(msg, "uid", 1);
	--pb.get(msg, "uid");

	print("=== uid: " .. msg.uid);	
	--pb.delete(msg);
	--msg = {};
	end
	--collectgarbage("collect");
	print("end ");
	return 5;
end

function onLuaMessage(conId, msg, thread)
	print("===========onLuaMessage");
	print("===========uid: " .. msg.uid);
	print("argback: " .. msg.argback);
	--collectgarbage("collect");	
	do
		local sendmsg = pb.new("lm.test");
		sendmsg.uid = 10;
		sendmsg.param = 1000;
		wk.send(thread, conId, sendmsg);
	end
	collectgarbage("collect");
end

