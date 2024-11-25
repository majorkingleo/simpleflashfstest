/*
 * test_static_open.cc
 *
 *  Created on: 26.08.2024
 *      Author: martin.oberzalek
 */
#include "test_static_open.h"
#include "../src_2face/H7TwoFaceConfig.h"
#include "../src_2face/SimpleFlashFsNoDel.h"
#include <stderr_exception.h>
#include <format.h>
#include <memory>
#include <filesystem>
#include "../src/sim_pc/SimFlashMemoryPc.h"

using namespace Tools;
using namespace SimpleFlashFs::static_memory;
using namespace ::SimpleFlashFs::SimPc;

namespace {

class TestCaseOpenBase : public TestCaseBase<bool>
{
protected:
	std::optional<SimpleFsNoDel<ConfigH7>> fs;
	std::optional<::SimpleFlashFs::SimPc::SimFlashFsFlashMemory> mem;
	std::string file_name;

public:
	TestCaseOpenBase( const std::string & name_,
			bool expected_result_ = true,
			bool exception_ = false,
			const std::string_view & file_name_ = std::string_view() )
	: TestCaseBase<bool>( name_, expected_result_, exception_ ),
	  file_name( file_name_ )
	{
		if( file_name.empty() ) {
			file_name = "." + name + ".bin";
		}
	}


	void init()
	{
		if( std::filesystem::exists( file_name ) ) {
			if( !std::filesystem::remove( file_name ) ) {
				throw STDERR_EXCEPTION( format( "cannot delete %s", file_name ) );
			}
		}

		mem.emplace(file_name,SFF_MAX_SIZE);
		fs.emplace(&mem.value());

		if( !fs->init() ) {
			CPPDEBUG( format( "recreating fs %s", file_name ) );
			if( !fs->create() ) {
				throw STDERR_EXCEPTION( format( "cannot create %s", file_name ) );
			}
		}
	}

	void deinit() {
		fs.reset();
		mem.reset();
	}
};

template <class RetType>
class TestCaseFunc : public TestCaseOpenBase
{
	std::function<RetType(SimpleFsNoDel<ConfigH7> & fs)> func;

public:
	TestCaseFunc( const std::string & name_, std::function<RetType(SimpleFsNoDel<ConfigH7> & fs)>  func_ )
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


std::shared_ptr<TestCaseBase<bool>> test_case_static_open1()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {

		auto f = fs.open("test.open1", std::ios_base::in );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return true;
		}

		return false;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_open2()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {

		auto f = fs.open("test.open2", std::ios_base::in | std::ios_base::out );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}

		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_open3()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {

		auto f = fs.open("test.open3", std::ios_base::in | std::ios_base::app );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}

		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_open4()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {

		{
			auto f = fs.open("test.open4", std::ios_base::out | std::ios_base::trunc );

			if( !f ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}

			std::string_view sv( "test1" );

			if( f.write( reinterpret_cast<const std::byte*>(sv.data()), sv.size() ) != sv.size() ) {
				CPPDEBUG( "cannot write data to file" );
				return false;
			}
		}

		{
			auto f = fs.open("test.open4", std::ios_base::out | std::ios_base::trunc );

			if( !f ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}

			if( f.file_size() != 0 ) {
				CPPDEBUG( "file size is not 0" );
				return false;
			}
		}

		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_open5()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {

		{
			auto f = fs.open("test.open5", std::ios_base::out | std::ios_base::trunc );
			f.inode.attributes |= static_cast<decltype(f.inode.attributes)>(::SimpleFlashFs::base::InodeAttribute::SPECIAL);

			if( !f ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}
		}
		{
			auto f = fs.open("test.open5", std::ios_base::in );

			if( !f ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}

			CPPDEBUG( Tools::format( "attr: %d",
					static_cast<decltype(f.inode.attributes)>(::SimpleFlashFs::base::InodeAttribute::SPECIAL) ) );

			if( !(f.inode.attributes & static_cast<decltype(f.inode.attributes)>(::SimpleFlashFs::base::InodeAttribute::SPECIAL) ) ) {
				return false;
			}
		}

		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_write1()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {
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

std::shared_ptr<TestCaseBase<bool>> test_case_static_write2()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {
		auto f = fs.open("test.write2", std::ios_base::out | std::ios_base::trunc );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}

		unsigned const block_a_size = 400;
		std::vector<std::byte> v_inside_the_inode( block_a_size, std::byte('X') );

		{
			std::size_t bytes_written = f.write( v_inside_the_inode.data(), v_inside_the_inode.size() );
			if( bytes_written != v_inside_the_inode.size() ) {
				CPPDEBUG( format( "%d bytes written", bytes_written ) );
				return false;
			}
		}

		f.flush();

		std::vector<std::byte> v_on_data_page( block_a_size, std::byte('Y') );

		{
			std::size_t bytes_written = f.write( v_on_data_page.data(), v_on_data_page.size() );
			if( bytes_written != v_on_data_page.size() ) {
				CPPDEBUG( format( "%d bytes written", bytes_written ) );
				return false;
			}
		}

		f.flush();

		if( f.file_size() != block_a_size * 2 ) {
			CPPDEBUG( format( "invalid file size: %d", f.file_size() ));
			return false;
		}

		f.seek(0);

		{
			std::vector<std::byte> v_read1( block_a_size, std::byte(0) );
			std::size_t read_len = f.read( v_read1.data(), v_read1.size() );

			if( read_len != v_read1.size() ) {
				CPPDEBUG( format( "%d bytes read", read_len ) );
				return false;
			}

			if( v_read1 != v_inside_the_inode ) {
				CPPDEBUG( "invalid data" );
				return false;
			}
		}

		{
			std::vector<std::byte> v_read2( block_a_size, std::byte(0) );
			std::size_t read_len = f.read( v_read2.data(), v_read2.size() );

			if( read_len != v_read2.size() ) {
				CPPDEBUG( format( "%d bytes read", read_len ) );
				return false;
			}

			if( v_read2 != v_on_data_page ) {
				CPPDEBUG( "invalid data" );
				return false;
			}
		}

		return true;
	});
}

/**
 * Testcase for seeking to a pos without writing the data. So the data between should be filled with
 * zeros.
 */
std::shared_ptr<TestCaseBase<bool>> test_case_static_write3()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {
		auto f = fs.open("test.write3", std::ios_base::out | std::ios_base::trunc );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}

		unsigned const block_a_size = 400;
		std::vector<std::byte> v_data( block_a_size, std::byte('X') );

		f.seek(600);

		{
			std::size_t bytes_written = f.write( v_data.data(), v_data.size() );
			if( bytes_written != v_data.size() ) {
				CPPDEBUG( format( "%d bytes written", bytes_written ) );
				return false;
			}
		}

		f.flush();

		if( f.file_size() != block_a_size + 600 ) {
			CPPDEBUG( format( "invalid file size: %d", f.file_size() ));
			return false;
		}

		f.seek(0);

		{
			std::vector<std::byte> v_read1( 600, std::byte('Y') );
			std::vector<std::byte> v_read_cmp( 600, std::byte(0) );

			std::size_t read_len = f.read( v_read1.data(), v_read1.size() );

			if( read_len != v_read1.size() ) {
				CPPDEBUG( format( "%d bytes read", read_len ) );
				return false;
			}

			if( v_read1 != v_read_cmp ) {
				CPPDEBUG( "invalid data" );
				return false;
			}
		}

		{
			std::vector<std::byte> v_read2( block_a_size, std::byte(0) );
			std::size_t read_len = f.read( v_read2.data(), v_read2.size() );

			if( read_len != v_read2.size() ) {
				CPPDEBUG( format( "%d bytes read", read_len ) );
				return false;
			}

			if( v_read2 != v_data ) {
				CPPDEBUG( "invalid data" );
				return false;
			}
		}

		return true;
	});
}


std::shared_ptr<TestCaseBase<bool>> test_case_static_write4()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {
		auto f = fs.open("test.write4", std::ios_base::out | std::ios_base::trunc );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}

		unsigned const block_a_size = 200;
		std::vector<std::byte> v_data( block_a_size, std::byte('X') );

		f.seek(200);

		{
			std::size_t bytes_written = f.write( v_data.data(), v_data.size() );
			if( bytes_written != v_data.size() ) {
				CPPDEBUG( format( "%d bytes written", bytes_written ) );
				return false;
			}
		}

		f.flush();

		if( f.file_size() != block_a_size + 200 ) {
			CPPDEBUG( format( "invalid file size: %d", f.file_size() ));
			return false;
		}

		f.seek(0);

		{
			std::vector<std::byte> v_read1( 200, std::byte('Y') );
			std::vector<std::byte> v_read_cmp( 200, std::byte(0) );

			std::size_t read_len = f.read( v_read1.data(), v_read1.size() );

			if( read_len != v_read1.size() ) {
				CPPDEBUG( format( "%d bytes read", read_len ) );
				return false;
			}

			if( v_read1 != v_read_cmp ) {
				CPPDEBUG( "invalid data" );
				return false;
			}
		}

		{
			std::vector<std::byte> v_read2( block_a_size, std::byte(0) );
			std::size_t read_len = f.read( v_read2.data(), v_read2.size() );

			if( read_len != v_read2.size() ) {
				CPPDEBUG( format( "%d bytes read", read_len ) );
				return false;
			}

			if( v_read2 != v_data ) {
				CPPDEBUG( "invalid data" );
				return false;
			}
		}

		return true;
	});
}

// same as test_case_static_write4 but no flushing meanwhile
std::shared_ptr<TestCaseBase<bool>> test_case_static_read1()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {
		auto f = fs.open("test.write4", std::ios_base::out | std::ios_base::trunc );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}

		unsigned const block_a_size = 200;
		std::vector<std::byte> v_data( block_a_size, std::byte('X') );

		f.seek(200);

		{
			std::size_t bytes_written = f.write( v_data.data(), v_data.size() );
			if( bytes_written != v_data.size() ) {
				CPPDEBUG( format( "%d bytes written", bytes_written ) );
				return false;
			}
		}

		if( f.file_size() != block_a_size + 200 ) {
			CPPDEBUG( format( "invalid file size: %d", f.file_size() ));
			return false;
		}

		f.seek(0);

		{
			std::vector<std::byte> v_read1( 200, std::byte('Y') );
			std::vector<std::byte> v_read_cmp( 200, std::byte(0) );

			std::size_t read_len = f.read( v_read1.data(), v_read1.size() );

			if( read_len != v_read1.size() ) {
				CPPDEBUG( format( "%d bytes read", read_len ) );
				return false;
			}

			if( v_read1 != v_read_cmp ) {
				CPPDEBUG( "invalid data" );
				return false;
			}
		}

		{
			std::vector<std::byte> v_read2( block_a_size, std::byte(0) );
			std::size_t read_len = f.read( v_read2.data(), v_read2.size() );

			if( read_len != v_read2.size() ) {
				CPPDEBUG( format( "%d bytes read", read_len ) );
				return false;
			}

			if( v_read2 != v_data ) {
				CPPDEBUG( "invalid data" );
				return false;
			}
		}

		return true;
	});
}

// same as test_case_static_write4 but no flushing meanwhile
std::shared_ptr<TestCaseBase<bool>> test_case_static_read2()
{
	return std::make_shared<TestCaseFunc<bool>>(__FUNCTION__, [](SimpleFsNoDel<ConfigH7> & fs) {
		auto f = fs.open("test.write3", std::ios_base::out | std::ios_base::trunc );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}

		unsigned const block_a_size = 400;
		std::vector<std::byte> v_data( block_a_size, std::byte('X') );

		f.seek(600);

		{
			std::size_t bytes_written = f.write( v_data.data(), v_data.size() );
			if( bytes_written != v_data.size() ) {
				CPPDEBUG( format( "%d bytes written", bytes_written ) );
				return false;
			}
		}

		if( f.file_size() != block_a_size + 600 ) {
			CPPDEBUG( format( "invalid file size: %d", f.file_size() ));
			return false;
		}

		f.seek(0);

		{
			std::vector<std::byte> v_read1( 600, std::byte('Y') );
			std::vector<std::byte> v_read_cmp( 600, std::byte(0) );

			std::size_t read_len = f.read( v_read1.data(), v_read1.size() );

			if( read_len != v_read1.size() ) {
				CPPDEBUG( format( "%d bytes read", read_len ) );
				return false;
			}

			if( v_read1 != v_read_cmp ) {
				CPPDEBUG( format( "invalid data: 0x%X", (unsigned)v_read1[0] ) );
				return false;
			}
		}

		{
			std::vector<std::byte> v_read2( block_a_size, std::byte(0) );
			std::size_t read_len = f.read( v_read2.data(), v_read2.size() );

			if( read_len != v_read2.size() ) {
				CPPDEBUG( format( "%d bytes read", read_len ) );
				return false;
			}

			if( v_read2 != v_data ) {
				CPPDEBUG( "invalid data" );
				return false;
			}
		}

		return true;
	});
}
