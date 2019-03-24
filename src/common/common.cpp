#include "common.hpp"


std::string timestamp_now_as_string(cppdb::session& database) {
	cppdb::statement st = database.prepare("SELECT now()");
	cppdb::result res = st.query();
	if(res.next()) {
		std::string value;
		res >> value;
		return value;
	}
	return "";
}
