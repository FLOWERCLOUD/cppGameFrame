
module(..., package.seeall);

function test1()
	local msg = pb.new("db_srv.get");
	msg.uid = 1111;
	msg.argback = "test1";
	msg.table_name.add("user");
	msg.table_name.add("pet");
	
	local function assert_function(retmsg)
		print("start test1 result ============================== ");
		--print(retmsg);
		if retmsg.uid == 1111 then
			local size = retmsg.tables.size();
			--print("size: " .. size);
			for i = 1, size  do 
				local tablename = retmsg.tables[i].table_name;
				if retmsg.tables[i].table_status == 0 then
					--if tablename ~= nil then
						print("test error, table name: " .. tablename);
					--end
				else
					--if tablename ~= nil then
						print("test ok, table name: " ..  tablename);
					--end
				end			
			end
			
			print("test 1 ok");
			print("end test1 result ============================== \n");
			return true;
		end
		print("test 1 error");
		print("end test1 result ============================== \n");
		return false;
	end
	
	return msg, assert_function;
end
-- 添加test1的测试用例
addTestUnit(test1);
