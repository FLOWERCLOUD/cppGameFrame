
module(..., package.seeall);

local uid = 1111;
function test()
	local msg = pb.new("db_srv.set");
	msg.uid = uid;
	msg.argback = "test2";
	
	local table1 = msg.tables.add();
	table1.table_name = "user";
	table1.table_bin = "2343254354354354654455454fksjdkfjdskfjkd";
	
	local table2 = msg.tables.add();
	table2.table_name = "pet";
	table2.table_bin = "dsfjsdkafjkjfksajfkjskdfjklasjfkdljafjalj";
	
	local function checktest(retmsg)
		print("\nstart check test2============");
		if retmsg.uid == uid  then
			local size = retmsg.table_statuses.size();
			for i = 1, size do
				if retmsg.table_statuses[i].status == "OK" then
					print(" test success table " .. retmsg.table_statuses[i].table_name);
				end
			end
		end
	end
	
	return msg, checktest;
end

-- 添加到测试用例中出
addTestUnit(test);
