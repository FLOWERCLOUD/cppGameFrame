
module(..., package.seeall);

function test2()
	local msg = pb.new("db_srv.set");
	msg.uid = 1111;
	msg.argback = "test2";
	
	local table1 = msg.tables.add();
	table1.table_name = "user";
	table1.table_bin = "2343254354354354654455454fksjdkfjdskfjkd";
	
	local table2 = msg.tables.add();
	table2.table_name = "pet";
	table2.table_bin = "dsfjsdkafjkjfksajfkjskdfjklasjfkdljafjalj";
	
	local function assert_function(retmsg)
		print("start test2 result ============================== ");
		if retmsg.uid == 1111 and retmsg.argback== "test2" then
			local size = retmsg.table_statuses.size();
			for i = 1, size do
				if retmsg.table_statuses[i].status == "OK" then
					print(" test success table " .. retmsg.table_statuses[i].table_name);
				end
			end
			
			print("test2 success");
			print("end test2 result ==============================\n ");
			--os.exit();
			return true;
		end
		
		print("test 2 error");
		print("end test2 result ==============================\n ");
		return false;
	end
	
	return msg, assert_function;
end

addTestUnit(test2);
