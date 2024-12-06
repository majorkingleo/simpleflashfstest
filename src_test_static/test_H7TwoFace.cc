/*
 * test_H7TwoFace.cc
 *
 *  Created on: 29.08.2024
 *      Author: martin.oberzalek
 */
#include "test_H7TwoFace.h"
#include "../src_2face/H7TwoFace.h"
#include "../src_2face/H7TwoFaceConfig.h"
#include "../src_2face/SimpleFlashFs2FlashPages.h"
#include "../src_2face/SimpleIni.h"
#include <stderr_exception.h>
#include <format.h>
#include <filesystem>
#include "../src/sim_pc/SimSTM32InternalFlashPc.h"
#include <set>

using namespace Tools;

namespace {

class TestCaseH7Base : public TestCaseBase<bool>
{
protected:
	std::optional<::SimpleFlashFs::SimPc::SimSTM32InternalFlashPc> mem1;
	std::optional<::SimpleFlashFs::SimPc::SimSTM32InternalFlashPc> mem2;

	std::string file_name;
	std::string file1;
	std::string file2;

public:
	TestCaseH7Base( const std::string & name_,
			bool expected_result_ = true,
			bool exception_ = false,
			const std::string_view & file_name_ = std::string_view() )
	: TestCaseBase<bool>( name_, expected_result_, exception_ ),
	  file_name( file_name_ )
	{
		if( file_name.empty() ) {
			file_name = "." + name;
		}

		file1 = file_name + "_page1.bin";
		file2 = file_name + "_page2.bin";
	}


	void init()
	{
		clear( file1 );
		clear( file2 );

		mem1.emplace(file1,SFF_MAX_SIZE,false);
		mem2.emplace(file2,SFF_MAX_SIZE,false);
		H7TwoFace::set_memory_interface(&mem1.value(),&mem2.value());
	}

	void deinit() {
		H7TwoFace::set_memory_interface(nullptr,nullptr);
		mem1.reset();
		mem2.reset();
	}


	void init_mapped()
	{
		clear( file1 );
		clear( file2 );

		mem1.emplace(file1,SFF_MAX_SIZE,true);
		mem2.emplace(file2,SFF_MAX_SIZE,true);
		H7TwoFace::set_memory_interface(&mem1.value(),&mem2.value());
	}

	void deinit_mapped() {
		H7TwoFace::set_memory_interface(nullptr,nullptr);
		mem1.reset();
		mem2.reset();
	}

protected:
	void clear( const std::string_view & file ) {
		if( std::filesystem::exists( file ) ) {
			if( !std::filesystem::remove( file ) ) {
				throw STDERR_EXCEPTION( format( "cannot delete %s", file ) );
			}
		}
	};
};


class TestCaseH7FuncNoInp : public TestCaseH7Base
{
	typedef std::function<bool()> Func;
	Func func;

public:
	TestCaseH7FuncNoInp( const std::string & name,
			bool expected_result_,
			Func func_ )
	: TestCaseH7Base( name, expected_result_ ),
	  func( func_ )
	  {}

	bool run() override {
		try {
			init();
			bool ret1 = func();
			deinit();

			return ret1;

		} catch( const std::exception & error ) {
			deinit();
			throw error;

		} catch( ... ) {
			deinit();
			throw;
		}
	}
};

class TestCaseH7FuncNoInpMapped : public TestCaseH7Base
{
	typedef std::function<bool()> Func;
	Func func;

public:
	TestCaseH7FuncNoInpMapped( const std::string & name,
			bool expected_result_,
			Func func_ )
	: TestCaseH7Base( name, expected_result_ ),
	  func( func_ )
	  {}

	bool run() override {
		try {
			init_mapped();
			bool ret1 = func();
			deinit_mapped();
			return ret1;

		} catch( const std::exception & error ) {
			deinit_mapped();
			throw error;

		} catch( ... ) {
			deinit_mapped();
			throw;
		}
	}
};

} // namespace

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write1()
{
	return std::make_shared<TestCaseH7FuncNoInp>(__FUNCTION__, true, []() {

		auto f = H7TwoFace::open( "2Face.write1", std::ios_base::in | std::ios_base::out );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}

		char buffer[1000] = { "Hello" };

		std::size_t bytes_written = f->write( reinterpret_cast<std::byte*>(buffer), sizeof(buffer) );
		if( bytes_written != sizeof(buffer) ) {
			CPPDEBUG( format( "%d bytes written", bytes_written ) );
			return false;
		}

		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write2()
{
	return std::make_shared<TestCaseH7FuncNoInp>(__FUNCTION__, true, []() {

		// test opening another handle later
		{
			auto f = H7TwoFace::open( "2Face.write1", std::ios_base::in | std::ios_base::out );

			if( !f ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}
		}

		{
			auto f = H7TwoFace::open( "2Face.write2", std::ios_base::in | std::ios_base::out );

			if( !f ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}
		}

		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write3()
{
	return std::make_shared<TestCaseH7FuncNoInp>(__FUNCTION__, true, []() {

		{
			auto f = H7TwoFace::open( "2Face.write1", std::ios_base::in | std::ios_base::out );

			if( !f ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}
		}

		// test opening another handle later
		auto f1 = H7TwoFace::open( "2Face.write1", std::ios_base::in );

		if( !f1 ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}


		auto f2 = H7TwoFace::open( "2Face.write2", std::ios_base::in );

		// should not work, because instance f1 is still there
		if( !f2 ) {
			return true;
		}

		return true;
	});
}


std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_delete1()
{
	return std::make_shared<TestCaseH7FuncNoInp>(__FUNCTION__, true, []() {

		// test deleting a file
		{
			auto f1 = H7TwoFace::open( "2Face.delete1", std::ios_base::out );

			if( !f1 ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}
		}

		{
			auto f2 = H7TwoFace::open( "2Face.delete1", std::ios_base::in );

			// s	hould not work, because instance f1 is still there
			if( !f2 ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}

			if( !f2->delete_file() ) {
				return false;
			}

		}

		{
			auto f2 = H7TwoFace::open( "2Face.delete1", std::ios_base::in );

			if( !f2 ) {
				return true;
			}
		}

		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_rename1()
{
	return std::make_shared<TestCaseH7FuncNoInp>(__FUNCTION__, true, []() {

		// test renaming a file
		{
			auto f1 = H7TwoFace::open( "2Face.rename1", std::ios_base::out );

			if( !f1 ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}
		}

		{
			auto f2 = H7TwoFace::open( "2Face.rename1", std::ios_base::in );

			// should work, because instance "2Face.rename1" is still there
			if( !f2 ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}

			if( !f2->rename_file("2Face.rename1.hias") ) {
				CPPDEBUG( "rename failed" );
				return false;
			}
		}

		{
			auto f3 = H7TwoFace::open( "2Face.rename1", std::ios_base::in );

			if( !f3 ) {
				CPPDEBUG( "ok" );
			} else {
				CPPDEBUG( "file not renamed" );
				return false;
			}
		}

		{
			auto f2 = H7TwoFace::open( "2Face.rename1.hias", std::ios_base::in );

			if( !f2 ) {
				CPPDEBUG( "file not renamed" );
				return false;
			}
		}

		return true;
	});
}

static void clear_fs()
{
	auto file_names = H7TwoFace::list_files();

	for( auto & file_name : file_names ) {
		 CPPDEBUG( Tools::format( "deleting file '%s'", file_name ) );
		 auto file = H7TwoFace::open( file_name, std::ios_base::in );
		 file->delete_file();
	}
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_max_files1()
{
	return std::make_shared<TestCaseH7FuncNoInp>(__FUNCTION__, true, []() {

		auto stat = H7TwoFace::get_stat();

		for( unsigned i = 0; i < stat.max_number_of_files; ++i ) {
			std::string file_name = format( "file#%03d", i );
			auto file = H7TwoFace::open( file_name, std::ios_base::out );
			if( !file ) {
				CPPDEBUG( Tools::format( "cannot create file: '%s", file_name ) );
				return false;
			}
		}

		// should fail
		std::string file_name = "xx";
		auto file = H7TwoFace::open( file_name, std::ios_base::out );
		if( !file ) {
			return true;
		}

		CPPDEBUG( "could create more files than expected" );

		return false;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_max_files2()
{
	return std::make_shared<TestCaseH7FuncNoInp>(__FUNCTION__, true, []() {

		auto stat = H7TwoFace::get_stat();
		std::string last_file_name;

		for( unsigned i = 0; i < stat.max_number_of_files; ++i ) {
			std::string file_name = format( "file#%03d", i );
			last_file_name = file_name;
			auto file = H7TwoFace::open( file_name, std::ios_base::out );
			if( !file ) {
				CPPDEBUG( Tools::format( "cannot create file: '%s", file_name ) );
				return false;
			}
		}

		{
			// should fail
			std::string file_name = "xx";
			auto file = H7TwoFace::open( file_name, std::ios_base::out );
			if( !file ) {

			} else {
				CPPDEBUG( Tools::format("could open file '%s' unexpected", file_name ));
				return false;
			}
		}

		// delete something
		{
			auto file = H7TwoFace::open( last_file_name, std::ios_base::in );
			if( !file ) {
				CPPDEBUG( Tools::format( "opening file '%s' failed", last_file_name ));
				return false;
			}

			if( !file->delete_file() ) {
				CPPDEBUG( Tools::format( "delete file '%s' failed", last_file_name ));
				return false;
			}

			// CPPDEBUG( Tools::format( "deleting succeeded with file: '%s",  last_file_name ));
		}

		{
			// should succeed
			std::string file_name = "xx";
			auto file = H7TwoFace::open( file_name, std::ios_base::out );
			if( !file ) {
				CPPDEBUG( Tools::format( "opening file '%s' failed", file_name ));
				return false;
			}
		}


		return true;
	});
}


std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_list_files1()
{
	return std::make_shared<TestCaseH7FuncNoInpMapped>(__FUNCTION__, true, []() {
		clear_fs();

		std::set<std::string> file_names;

		for( unsigned i = 0; i < 10; i++ ) {
			file_names.insert( Tools::format( "file#%02d.txt", i ) );
		}

		for( auto & file_name : file_names ) {
			auto file = H7TwoFace::open( file_name, std::ios_base::out );
			if( !file ) {
				CPPDEBUG( Tools::format( "cannot create file '%s'", file_name ) );
				return false;
			}
		}

		auto file_names_in_fs = H7TwoFace::list_files();

		for( auto & file_name_in_fs : file_names_in_fs ) {
			if( !file_names.count( std::string(file_name_in_fs) ) ) {
				CPPDEBUG( Tools::format( "file '%s' not found", file_name_in_fs ) );
				return false;
			}
		}

		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_open_speacial1()
{
	return std::make_shared<TestCaseH7FuncNoInp>(__FUNCTION__, true, []() {

		for( const auto & reserved_name : SimpleFlashFs::static_memory::SimpleFs2FlashPages<ConfigH7>::RESERVED_NAMES ) {
			auto file = H7TwoFace::open( reserved_name, std::ios_base::out );

			if( !file ) {
				continue;
			}
			return false;
		}

		return true;
	});
}

std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write_ini1()
{
	return std::make_shared<TestCaseH7FuncNoInp>(__FUNCTION__, true, []() {

		{
			auto ini_file = H7TwoFace::open( "test.ini", std::ios_base::out | std::ios_base::in );

			if( !ini_file ) {
				CPPDEBUG( "cannot open ini file" );
				return false;
			}

			SimpleFlashFs::StaticFileBuffer<SFF_PAGE_SIZE> fbuffer( *ini_file );

			SimpleFlashFs::SimpleIni ini( fbuffer );

			if( !ini.write( "section1", "key1", "value1"  ) ) {
				CPPDEBUG( "writing key1 failed" );
				return false;
			}

			if( !ini.write( "section2", "key2", "value2"  ) ) {
				CPPDEBUG( "writing key2 failed" );
				return false;
			}

			if( !ini.write( "section1", "key3", "value3", "comment3"  ) ) {
				CPPDEBUG( "writing key3 failed" );
				return false;
			}

		}


		{
			auto ini_file = H7TwoFace::open( "test.ini", std::ios_base::in );

			if( !ini_file ) {
				CPPDEBUG( "cannot open ini file" );
				return false;
			}

			SimpleFlashFs::StaticFileBuffer<SFF_PAGE_SIZE> fbuffer( *ini_file );

			SimpleFlashFs::SimpleIni ini( fbuffer );

			{
				std::string_view sv_value;
				if( !ini.read( "section1", "key1", sv_value ) || sv_value != "value1" ) {
					CPPDEBUG( "reading key1 failed" );
					return false;
				}
			}

			{
				std::string_view sv_value;
				if( !ini.read( "section2", "key2", sv_value ) || sv_value != "value2" ) {
					CPPDEBUG( "reading key2 failed" );
					return false;
				}
			}

			{
				std::string_view sv_value;
				if( !ini.read( "section1", "key3", sv_value ) || sv_value != "value3" ) {
					CPPDEBUG( "reading key3 failed" );
					return false;
				}
			}
		}

		return true;
	});
}


std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write4()
{
	return std::make_shared<TestCaseH7FuncNoInpMapped>(__FUNCTION__, true, []() {

		auto func = []( char sign ){

			auto f = H7TwoFace::open( "2Face.write4", std::ios_base::in | std::ios_base::out );

			if( !f ) {
				CPPDEBUG( "cannot open file" );
				return false;
			}

			{
				char buffer[500];
				memset(buffer,sign,sizeof(buffer));

				std::size_t bytes_written = f->write( reinterpret_cast<std::byte*>(buffer), sizeof(buffer) );
				if( bytes_written != sizeof(buffer) ) {
					CPPDEBUG( format( "%d bytes written", bytes_written ) );
					return false;
				}
			}


			{
				char buffer[1000];
				memset(buffer,sign,sizeof(buffer));

				std::size_t bytes_written = f->write( reinterpret_cast<std::byte*>(buffer), sizeof(buffer) );
				if( bytes_written != sizeof(buffer) ) {
					CPPDEBUG( format( "%d bytes written", bytes_written ) );
					return false;
				}
			}

			return true;
		};

		if( !func('X') ) {
			return false;
		}

		if( !func('Y') ) {
			return true;
		}

		return true;
	});
}


std::shared_ptr<TestCaseBase<bool>> test_case_static_TwoFace_write5()
{
	return std::make_shared<TestCaseH7FuncNoInpMapped>(__FUNCTION__, true, []() {

		auto f = H7TwoFace::open( "2Face.write5", std::ios_base::in | std::ios_base::out );

		if( !f ) {
			CPPDEBUG( "cannot open file" );
			return false;
		}

		{
			char buffer[400];
			memset(buffer,'x',sizeof(buffer));

			std::size_t bytes_written = f->write( reinterpret_cast<std::byte*>(buffer), sizeof(buffer) );
			if( bytes_written != sizeof(buffer) ) {
				CPPDEBUG( format( "%d bytes written", bytes_written ) );
				return false;
			}
		}

		{
			char buffer[510];
			memset(buffer,'x',sizeof(buffer));

			std::size_t bytes_written = f->write( reinterpret_cast<std::byte*>(buffer), sizeof(buffer) );
			if( bytes_written != sizeof(buffer) ) {
				CPPDEBUG( format( "%d bytes written", bytes_written ) );
				return false;
			}
		}

		f->seek(0);

		{
			char buffer[1000];
			memset(buffer,'X',sizeof(buffer));

			std::size_t bytes_written = f->write( reinterpret_cast<std::byte*>(buffer), sizeof(buffer) );
			if( bytes_written != sizeof(buffer) ) {
				CPPDEBUG( format( "%d bytes written", bytes_written ) );
				return false;
			}
		}

		f->seek(0);

		{
			char buffer[1000];
			memset(buffer,'Y',sizeof(buffer));

			std::size_t bytes_written = f->write( reinterpret_cast<std::byte*>(buffer), sizeof(buffer) );
			if( bytes_written != sizeof(buffer) ) {
				CPPDEBUG( format( "%d bytes written", bytes_written ) );
				return false;
			}
		}

		return true;
	});
}
