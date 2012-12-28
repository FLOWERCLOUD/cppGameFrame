
module(..., package.seeall);

function test3()
	local msg = pb.new("db_srv.get");
	msg.uid = 1111;
	msg.argback = "test3";
	msg.table_name.add("user");
	msg.table_name.add("user2");
	msg.table_name.add("pet");
	
	local function assert_function(retmsg)
		print("\nstart check test3============");
		if retmsg.uid == 1111 then
			local size = retmsg.tables.size();
			for i = 1, size  do 
				local tablename = retmsg.tables[i].table_name;
				if retmsg.tables[i].table_status == 0 then
						print("test error, table name: " .. tablename);
				else
						print("test ok, table name: " ..  tablename);
				end			
			end	
		end
	end
	
	return msg, assert_function;
end

addTestUnit(test3);
