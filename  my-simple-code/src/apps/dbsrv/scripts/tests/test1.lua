
module(..., package.seeall);


local uid=1111;

function test()
	local msg = pb.new("db_srv.get");
	msg.uid = uid;
	msg.argback = "test1";
	msg.table_name.add("user");
	msg.table_name.add("pet");
	
	local function checktest(msg)
		print("\nstart check test1============");
		assert_equal(msg.uid,  uid, "test1 uid error", 1);
		local size = msg.tables.size();
		for i = 1, size do
			assert_not_equal(msg.tables[i].table_status, 0, "get table error", 1);
		end
	end
	return msg, checktest;
end

-- 添加到测试用例中出
addTestUnit(test);

