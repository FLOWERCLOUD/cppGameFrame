
local redis = sRedis;
local mysql = sMySql;

function onLuaMessage(conId, msg)
	print("onLuaMessage");
	local state, data = Redis.command(redis, "get user2:2222");
	print(data);
					local tmsg = pb.new("lm.user2.msgt");
					--print(tmsg);
					--local bin = retmsg.tables[i].table_bin
					pb.parseFromString(tmsg, data);
end
