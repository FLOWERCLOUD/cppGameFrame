
--pb.import("test.proto");
--pb.import("addressbook.proto");

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

function tickme()
	Log.log(LOG_INFO, "TICK ME==============");
end

function toHex(str)
	return string.gsub(str, ".", function (b) return string.format("%02X ", string.byte(b)) end )
end

local redis = sRedis;
local mysql = sMySql;
function onLuaMessage(conId, msg)
	Log.log(LOG_INFO, "999999999999999===========onLuaMessage");
	Log.log(LOG_INFO, "999999999999999===========uid: " .. msg.uid);
	Log.log(LOG_INFO,"argback: " .. msg.argback);
	--collectgarbage("collect");	
	do
		local sendmsg = pb.new("lm.test");
		sendmsg.uid = 10;
		sendmsg.param = 1000;
		wk.send(conId, sendmsg);
	end
	collectgarbage("collect");
	
	-- test redis set
	local state, result = Redis.command(redis, "SET testuser:1111 6666666666666688881111111");
	Log.log(LOG_INFO,"set result: " .. state);
	if state == REDIS_REPLY_STATUS then
		Log.log(LOG_INFO,"set result: " .. result);
	end
	-- test redis get
	state, result = Redis.command(redis, "GET testuser:1111");
	Log.log(LOG_INFO, "get result: " .. state);
	if state == REDIS_REPLY_STRING then
		Log.log(LOG_INFO, "get result:" .. result);
	end
	
	-- test redis hset
	state, result  = Redis.command(redis, "hset myhash field1 helloworld");
	Log.log(LOG_INFO, "hset result: " .. state);
	if state == REDIS_REPLY_STATUS then
		Log.log(LOG_INFO,"hset result: " .. result);
	end
	
	state, result = Redis.command(redis, "hset myhash field2 \"hello world\"");
	Log.log(LOG_INFO, "hset result: " .. state);
	if state == REDIS_REPLY_STATUS then
		Log.log(LOG_INFO,"hset result: " .. result);
	end
	
	-- test redis hget
	state, result = Redis.command(redis, "hgetall myhash");
	Log.log(LOG_INFO, "========hget result: " .. state);
	if state == REDIS_REPLY_ARRAY then
		Log.log(LOG_INFO, "TEST ===== ");
		local t = result;
		for k, v in ipairs(t) do
			Log.log(LOG_INFO, "k: " .. k .. " v: " .. v);
		end	
	end
	
	-- test redis error
	state, result = Redis.command(redis, "hget ");
	if state == REDIS_REPLY_ERROR then
		Log.log(LOG_INFO, "=========== test redis error, result: " .. result);
	end
	-- test mysql query
	local t = MySql.query(mysql, "select uid, table_bin, update_time from user");
	if t == nil then
		Log.log(LOG_INFO, "not result");
	else
		--for _, v1 in ipairs(t) do
		--	for _, v2 in ipairs(v1) do
		--		Log.log(LOG_INFO, "========= v2:" .. toHex(v2));
		--	end
		--end
		local rowCount = #t;
		Log.log(LOG_INFO, " row count: " .. rowCount);
		for i = 1, #t do
			Log.log(LOG_INFO, "{ uid: " .. t[i][1] .. " table_bin:'" .. toHex(t[i][2]) .. "' update_time: " .. t[i][3] .. "}");
		end	
	end
	
	-- test mysql execute
	local uid=200;
	local table_bin = "2432543546546765766dfsafasfsaffafds";
	local update_time = 35567785;
	local updatesql = string.format("update user set table_bin='%s', update_time=%d where uid=%d", table_bin, update_time, uid);
	local ret = MySql.execute(mysql, updatesql);
	if ret then
		Log.log(LOG_INFO, "update sql, success!!! ");
	end
	
	-- test start time
	--wk.startTime(3, "tickme", WK_TIME_ATTIME);
	
	-- test repeate pb
	do
		Log.log(LOG_INFO, "START TEST PB=========");
		local addressmsg = pb.new("lm.AddressBook");
		local person1 = addressmsg.person.add();
		person1.name = "zjx";
		person1.id = 11111;
		person1.email = "zjx@126.com";
		
		addressmsg.param.add(1);
		addressmsg.param.add(2);
		addressmsg.param.add(3);
		addressmsg.param.add(4);
		addressmsg.param[1] = 1000;
		
		local person2 = addressmsg.person.add();
		person2.name = "hlz";
		person2.id = 22222;
		person2.email = "hlz@126.com";
		
		-- error write style
		--addressmsg.person.add = person1;
		
		local person3 = addressmsg.person.add();
		person3.name = "666";
		person3.id = 77777;
		--  error type
		-- person3.xxx = 99999;
		
		if person3.email == nil then
			Log.log(LOG_INFO, "person3 email is null");
		end
		
		Log.log(LOG_INFO, "person3 email: " .. person3.email);
		Log.log(LOG_INFO, "person num: " .. addressmsg.person.size());

		Log.log(LOG_INFO, "old addressmsg: " .. pb.tostring(addressmsg) );
		local size = addressmsg.person.size();
		for i = 1, size do
			addressmsg.person[i].id = 9999;
			addressmsg.person[i].name = "8888888";
		end
		
		size = addressmsg.param.size();
		for i = 1, size do
			addressmsg.param[i] = 90000 + i;
		end
		
		Log.log(LOG_INFO, "new addressmsg: " .. pb.tostring(addressmsg) );
		Log.log(LOG_INFO, "END TEST PB=========");	
	end
end

