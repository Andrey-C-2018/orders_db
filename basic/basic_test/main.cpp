#include <basic/Exception.h>
#include <basic/tstream.h>
#include <basic/Process.h>

int main() {
	//try {
	//	XException e(0, "const char *");

	//	e << L" and wchar_t";
	//	throw e;
	//}
	//catch (XException &e) {
	//	Tcout << e.what()<< std::endl;
	//}

	//try {
	//	XException e(0, L"");

	//	e << " and char";
	//	e << 1234;
	//	throw e;
	//}
	//catch (XException &e) {
	//	Tcout << e.what() << std::endl;
	//}

	std::string cmd;
	exec("c:\\Windows\\System32\\cmd.exe", "/c echo OK && pause", "c:\\Windows\\System32");

	Tcin.get();
	return 0;
}