
--module(..., package.seeall);

function run_all_test()
	require("tests/test1");
	require("tests/test2");
	require("tests/test3");
	require("tests/test4");
	require("tests/test5");
	require("tests/test6");
	require("tests/test7");
	--require("tests/test8");
end

function assert_equal(a, b, message, level)
	if not (a == b) then
		error("\n   assert_equal failed: "..tostring(a).." ~= "..tostring(b)..(message and ("\n   Message: "..message) or ""), (level or 1) + 1);
	else
		print("assert_equal succeeded: "..tostring(a).." == "..tostring(b));
	end
end

function assert_not_equal(a, b, message, level)
	if  (a == b) then
		error("\n   assert_equal failed: "..tostring(a).." == "..tostring(b)..(message and ("\n   Message: "..message) or ""), (level or 1) + 1);
	else
		print("assert_equal succeeded: "..tostring(a).." ~= "..tostring(b));
	end
end

function assert_table(a, message, level)
	assert_equal(type(a), "table", message, (level or 1) + 1);
end
function assert_function(a, message, level)
	assert_equal(type(a), "function", message, (level or 1) + 1);
end
function assert_string(a, message, level)
	assert_equal(type(a), "string", message, (level or 1) + 1);
end
function assert_boolean(a, message)
	assert_equal(type(a), "boolean", message);
end
function assert_is(a, message)
	assert_equal(not not a, true, message);
end
function assert_is_not(a, message)
	assert_equal(not not a, false, message);
end


function end_all_test()
	local msg = pb.new("db_srv.get");
	msg.uid = uid;
	msg.argback = "end_all_test";

	local function checktest(msg)
		print("\nend all test=====================\n");
		os.exit();
	end
	return msg, checktest;
end

run_all_test();

addTestUnit(end_all_test);
