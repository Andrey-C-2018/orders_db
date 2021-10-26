#include <UnitTest++/UnitTest++.h>

TEST(Sanity) {

	CHECK_EQUAL(1, 1);
}

int main(int argc, char **argv){

    return UnitTest::RunAllTests();
}