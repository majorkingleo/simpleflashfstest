#include "test_dynamic_init.h"
#include "test_dynamic_wrapper.h"

#include <SimpleFlashFsDynamic.h>
#include <SimpleFlashFsDynamicInstanceHandler.h>
#include <stderr_exception.h>
#include <format.h>
#include <SimpleFlashFsDynamicWrapper.h>
#include <CpputilsDebug.h>
#include <SimFlashMemoryPc.h>

#define fopen( path, mode ) SimpleFlashFs_dynamic_fopen( path, mode )
#define fclose( file ) SimpleFlashFs_dynamic_fclose( file)
#define fwrite( ptr, size, nmemb, stream ) SimpleFlashFs_dynamic_fwrite( ptr, size, nmemb, stream )
#define fread( ptr, size, nmemb, stream ) SimpleFlashFs_dynamic_fread( ptr, size, nmemb, stream )
#define fgets( s, size, stream ) SimpleFlashFs_dynamic_fgets( s, size, stream )
#define FILE SIMPLE_FLASH_FS_DYNAMIC_FILE

using namespace Tools;
using namespace SimpleFlashFs;
using namespace SimpleFlashFs::dynamic;
using namespace SimpleFlashFs::SimPc;

unsigned instance_count = 0;

namespace {

class TestCaseWrapperBase : public TestCaseBase<bool>
{
	std::size_t page_size;
	std::size_t size;
	std::shared_ptr<SimFlashFsFlashMemory> mem;
	std::shared_ptr<SimpleFlashFs::dynamic::SimpleFlashFs> fs;
	const std::string instance_name;

public:
	TestCaseWrapperBase( const std::string & name_,
			std::size_t page_size_ = 528,
			std::size_t size_ = 100*1024,
			bool expected_result_ = true,
			bool exception_ = false )
	: TestCaseBase<bool>( name_, expected_result_, exception_ ),
	  page_size( page_size_ ),
	  size( size_ ),
	  instance_name( format( ".test%d.bin", instance_count++ ) )
	{

	}

	~TestCaseWrapperBase()
	{
		InstanceHandler::instance().deregister_instance(instance_name);
	}

	bool init()
	{
		auto & instance_handler = InstanceHandler::instance();

		const std::string file = instance_name;

		mem = std::make_shared<SimFlashFsFlashMemory>(file,size);
		fs = std::make_shared<SimpleFlashFs::dynamic::SimpleFlashFs>(mem.get());

		instance_handler.register_instance(instance_name, fs );

		if( !fs->create(fs->create_default_header(page_size, size/page_size)) ) {
			throw STDERR_EXCEPTION( Tools::format( "cannot create %s", file ) );
		}

		//CPPDEBUG( format( "registering instance: '%s'", instance_name));
		SimpleFlashFs_dynamic_wrapper_register_default_instance_name(instance_name.c_str());

		return true;
	}

	void deinit()
	{
		SimpleFlashFs_dynamic_wrapper_unregister_default_instance_name(instance_name.c_str());
	}
/*
	bool run() override
	{
		if( !init() ) {
			return false;
		}

		return true;
	}
*/
};


class TestCaseWrapperFunc : public TestCaseWrapperBase
{
	std::function<bool()> func;

public:
	TestCaseWrapperFunc( const std::string & name_, std::function<bool()>  func_ )
	: TestCaseWrapperBase( name_ ),
	  func( func_ )
	{

	}

	bool run() override
	{
		if( !init() ) {
			CPPDEBUG( "init failed" );
			return false;
		}

		try {
			bool ret = func();

			deinit();

			return ret;
		} catch( std::exception & error ) {
			deinit();
			throw error;
		}
	}
};

} // namespace

std::shared_ptr<TestCaseBase<bool>> test_case_wrapper_fopen1()
{
	return std::make_shared<TestCaseWrapperFunc>("fopen1", []() {
		FILE *f = fopen( "test", "r" );
		if( f == nullptr ) {
			return true;
		}

		fclose( f );
		return false;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_wrapper_fopen2()
{
	return std::make_shared<TestCaseWrapperFunc>("fopen2", []() {
		FILE *f = fopen( "test", "r+" );
		if( f == nullptr ) {
			return true;
		}
		fclose( f );
		return false;
	});
}


std::shared_ptr<TestCaseBase<bool>> test_case_wrapper_fopen3()
{
	return std::make_shared<TestCaseWrapperFunc>("fopen3", []() {
		FILE *f = fopen( "test", "rw+" );
		if( f == nullptr ) {
			return false;
		}
		fclose( f );
		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_wrapper_fopen4()
{
	return std::make_shared<TestCaseWrapperFunc>("fopen4", []() {
		FILE *f = fopen( "test", "a" );
		if( f == nullptr ) {
			return false;
		}
		fclose( f );
		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_wrapper_fopen5()
{
	return std::make_shared<TestCaseWrapperFunc>("fopen5", []() {
		FILE *f = fopen( "test", "a+" );
		if( f == nullptr ) {
			return false;
		}
		fclose( f );
		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_wrapper_fwrite1()
{
	return std::make_shared<TestCaseWrapperFunc>("fwrite1", []() {
		FILE *f = fopen( "test", "w+" );
		if( f == nullptr ) {
			return false;
		}

		char buffer[1000] = { "Hello" };

		std::size_t bytes_written = fwrite( buffer, 1, sizeof(buffer), f );
		if( bytes_written != sizeof(buffer) ) {
			CPPDEBUG( Tools::format( "%d bytes written", bytes_written ) );
			return false;
		}

		fclose( f );
		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_wrapper_fwrite2()
{
	return std::make_shared<TestCaseWrapperFunc>("fwrite2", []() {
		FILE *f = fopen( "test", "w+" );
		if( f == nullptr ) {
			return false;
		}

		char buffer[10000] = { "Hello" };

		std::size_t bytes_written = fwrite( buffer, 1, sizeof(buffer), f );
		if( bytes_written != sizeof(buffer) ) {
			CPPDEBUG( Tools::format( "%d bytes written", bytes_written ) );
			return false;
		}

		fclose( f );
		return true;
	});
}

// AI generated by GitHub Copilot GPT-5.3-Codex START
// write numbers 1 to 74, each on its own line, using repeated open/append/close
// verifies that each number is stored with a trailing newline (cat output must not merge lines)
std::shared_ptr<TestCaseBase<bool>> test_case_wrapper_fwrite4()
{
	return std::make_shared<TestCaseWrapperFunc>("fwrite4", []() {
		const char *filename = "count";
		const uint32_t max_val = 74;

		for( uint32_t i = 1; i <= max_val; i++ ) {
			std::string content = std::to_string(i) + "\n";

			FILE *f = fopen( filename, "a" );
			if( f == nullptr ) {
				CPPDEBUG( Tools::format( "cannot open '%s' for append at i=%d", filename, i ) );
				return false;
			}

			std::size_t bytes_written = fwrite( content.c_str(), 1, content.size(), f );
			if( bytes_written != content.size() ) {
				CPPDEBUG( Tools::format( "fwrite returned %d, expected %d at i=%d", bytes_written, content.size(), i ) );
				fclose( f );
				return false;
			}

			if( fclose( f ) != 0 ) {
				CPPDEBUG( Tools::format( "fclose failed at i=%d", i ) );
				return false;
			}
		}

		// Now read back and verify every line
		FILE *f = fopen( filename, "r+" );
		if( f == nullptr ) {
			CPPDEBUG( "cannot open file for reading" );
			return false;
		}

		for( uint32_t i = 1; i <= max_val; i++ ) {
			char buf[32] = {};
			char *line = fgets( buf, sizeof(buf), f );
			if( line == nullptr ) {
				CPPDEBUG( Tools::format( "fgets returned nullptr at i=%d", i ) );
				fclose( f );
				return false;
			}

			std::string expected = std::to_string(i) + "\n";
			if( std::string(buf) != expected ) {
				CPPDEBUG( Tools::format( "line mismatch at i=%d: got '%s' expected '%s'",
					i, std::string(buf).c_str(), expected.c_str() ) );
				fclose( f );
				return false;
			}
		}

		fclose( f );
		return true;
	});
}
// AI generated by GitHub Copilot GPT-5.3-Codex END

// no space left on device
std::shared_ptr<TestCaseBase<bool>> test_case_wrapper_fwrite3()
{
	return std::make_shared<TestCaseWrapperFunc>("fwrite3", []() {
		FILE *f = fopen( "test", "w+" );
		if( f == nullptr ) {
			return false;
		}

		std::vector<char> buffer;
		buffer.resize(200 * 1042 * sizeof(char));
		snprintf( buffer.data(), buffer.size(), "%s", "Hello"  );

		std::size_t bytes_written = fwrite( buffer.data(), 1, buffer.size(), f );
		if( bytes_written != buffer.size() ) {
			CPPDEBUG( Tools::format( "%d bytes written", bytes_written ) );
			return true;
		}

		fclose( f );
		return false;
	});
}
