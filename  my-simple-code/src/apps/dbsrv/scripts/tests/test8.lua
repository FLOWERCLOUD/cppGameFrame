
module(..., package.seeall);

local uid = 9999999;
function test()
	local msg = pb.new("db_srv.get");
	msg.uid = uid;
	msg.argback = "test8";
	msg.table_name.add("user");
	
	local function assert_function(msg)
		print("\nstart check test8============");
		if msg.uid == uid  then
			local size = msg.tables.size();
			for i = 1, size do
				assert_equal(msg.tables[i].table_status, 0, "get table error", 1);
			end	
		end
	end
	
	return msg, assert_function;
end

addTestUnit(test);
