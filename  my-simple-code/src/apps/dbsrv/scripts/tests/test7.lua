
module(..., package.seeall);

function test()
	local msg = pb.new("db_srv.get");
	msg.uid = 2222;
	msg.argback = "test7";
	msg.table_name.add("user2");
	
	local function assert_function(retmsg)
		print("\nstart check test7============");
		if retmsg.uid == 2222  then
			local size = retmsg.tables.size();
			for i = 1, size do
				if retmsg.tables[i].table_name == "user2" then
					local tmsg = pb.new("lm.user2");
					local bin = retmsg.tables[i].table_bin;
					pb.parseFromString(tmsg, bin);
					--print("======== param1:" .. tmsg.param1);
					--print("======== uid:" .. tmsg.uid);
					--print(tmsg);
				end
			end	
		end
	end
	
	return msg, assert_function;
end

addTestUnit(test);
