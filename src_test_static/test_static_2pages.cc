/*
 * test_static_open.cc
 *
 *  Created on: 26.08.2024
 *      Author: martin.oberzalek
 */
#include "test_static_open.h"
#include "../src_2face/H7TwoFaceConfig.h"
#include "../src_2face/SimpleFlashFs2FlashPages.h"
#include "../src_2face/H7TwoFace.h"
#include <stderr_exception.h>
#include <format.h>
#include "../src/sim_pc/SimFlashMemoryPc.h"

using namespace Tools;
using namespace SimpleFlashFs;
using namespace SimpleFlashFs::static_memory;
using namespace ::SimpleFlashFs::SimPc;

namespace {

class TestCaseOpenBase : public TestCaseBase<bool>
{
protected:
	std::optional<SimpleFs2FlashPages<ConfigH7>> fs;
	std::optional<::SimpleFlashFs::SimPc::SimFlashFsFlashMemory> mem1;
	std::optional<::SimpleFlashFs::SimPc::SimFlashFsFlashMemory> mem2;

public:
	TestCaseOpenBase( const std::string & name_,
			bool expected_result_ = true,
			bool exception_ = false )
	: TestCaseBase<bool>( name_, expected_result_, exception_ )
	{

	}


	void init()
	{
		const std::string file1 = "test_h7_page1.bin";
		const std::string file2 = "test_h7_page2.bin";
		mem1.emplace(file1,SFF_MAX_SIZE);
		mem2.emplace(file2,SFF_MAX_SIZE);
		fs.emplace(&mem1.value(),&mem2.value());

		if( !fs->init() ) {
			throw STDERR_EXCEPTION( Tools::format("cannot create fs (%s,%s)", file1, file2 ) );
		}
	}

	void deinit() {
		fs.reset();
		mem1.reset();
		mem2.reset();
	}
};

template <class RetType>
class TestCaseFunc : public TestCaseOpenBase
{
	std::function<RetType(SimpleFs2FlashPages<ConfigH7> & fs)> func;

public:
	TestCaseFunc( const std::string & name_, std::function<RetType(SimpleFs2FlashPages<ConfigH7> & fs)>  func_ )
	: TestCaseOpenBase( name_ ),
	  func( func_ )
	{

	}

	bool run() override
	{
		init();

		try {
			RetType ret = func(*fs);
			deinit();

			return ret;
		} catch( std::exception & error ) {
			deinit();
			throw error;
		}
	}
};


} // namespace


std::shared_ptr<TestCaseBase<bool>> test_case_static_2pages_write1()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFs2FlashPages<ConfigH7> & fs) {
		auto f = fs.open("test.write1", std::ios_base::out | std::ios_base::trunc );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}

		char buffer[1000] = { "Hello" };

		std::size_t bytes_written = f.write( reinterpret_cast<std::byte*>(buffer), sizeof(buffer) );
		if( bytes_written != sizeof(buffer) ) {
			CPPDEBUG( format( "%d bytes written", bytes_written ) );
			return false;
		}

		return true;
	});
}



