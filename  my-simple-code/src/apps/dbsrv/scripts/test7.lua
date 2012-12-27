
module(..., package.seeall);

function test()
	local msg = pb.new("db_srv.get");
	msg.uid = 2222;
	msg.argback = "test7";
	msg.table_name.add("user2");
	
	local function assert_function(retmsg)
		print("start test7 result ============================== ");
		--print(retmsg);
		if retmsg.uid == 2222 and retmsg.argback== "test7" then
			local size = retmsg.tables.size();
			for i = 1, size do
				if retmsg.tables[i].table_name == "user2" then
					local tmsg = pb.new("lm.user2.msgt");
					pb.parseFromString(tmsg, retmsg.tables[i].table_bin);
					print(tmsg);
				end
			end
			
			print("test7 success");
			print("end test7 result ==============================\n ");
			--os.exit();
			return true;
		end
		
		print("test 7 error");
		print("end test7 result ==============================\n ");
		return false;
	end
	
	return msg, assert_function;
end

addTestUnit(test);
