/*
 * test_SimpleIni.cc
 *
 *  Created on: 06.09.2024
 *      Author: martin.oberzalek
 */
#include "test_SimpleIni.h"
#include <CpputilsDebug.h>
#include "FFile.h"
#include "../src_2face/SimpleIni.h"
#include <xml.h>
#include <fstream>

using namespace SimpleFlashFs;
using namespace Tools;

namespace {

class TestCaseFuncWriteIni : public TestCaseBase<bool>
{
	typedef std::function<bool( SimpleFlashFs::FileBuffer & file )> Func;
	Func func;
	std::size_t buffer_size;
	std::ios_base::openmode openmode;
	bool debug_file_buffer  = true;
	std::string expected_ini_text;

public:
	TestCaseFuncWriteIni( const std::string & name,
							Func func_,
							std::size_t buffer_size_,
							std::ios_base::openmode openmode_,
							bool debug_file_buffer_,
							std::string expected_ini_text_ )
	: TestCaseBase<bool>( name, true ),
	  func( func_ ),
	  buffer_size( buffer_size_ ),
	  openmode( openmode_ ),
	  debug_file_buffer( debug_file_buffer_ ),
	  expected_ini_text( expected_ini_text_ )
	  {}

	bool run() override {

		std::string file_name_buffered_fstream = Tools::format( ".%s.buffered_fstream.ini", name );
		std::vector<std::byte> buffer(buffer_size);
		std::span<std::byte> sbuffer(buffer);

		std::filesystem::remove(file_name_buffered_fstream);

		{
			FFile f_fstream_b( file_name_buffered_fstream, openmode, debug_file_buffer );

			if( !f_fstream_b ) {
				CPPDEBUG( Tools::format( "cannot open file: '%s'", f_fstream_b.get_file_name() ) );
				return false;
			}


			SimpleFlashFs::FileBuffer file_buffered_fstream( f_fstream_b, sbuffer );

			if( !func( file_buffered_fstream ) ) {
				return false;
			}
		}

		std::string data;
		if( !XML::read_file( file_name_buffered_fstream, data ) ) {
			return false;
		}

		if( data != expected_ini_text ) {
			CPPDEBUG( "output text differs ");
			std::string expected_out_file_name = file_name_buffered_fstream + ".expected.ini";
			std::ofstream out( expected_out_file_name, std::ios_base::trunc | std::ios_base::out );
			if( !out ) {
				CPPDEBUG( format( "cannot open file '%s'", expected_out_file_name ) );
				return false;
			}

			out << expected_ini_text;

			return false;
		}

		return true;
	}
};


bool write_default_ini1( SimpleFlashFs::FileBuffer & file )
{
	if( file.write(
			"[section1]\n" \
			"  key1=value1\n" \
			"  key2=value2\n" \
			"\n" \
			"[section2]\n" \
			"   \n"
			"  key1 = value1 \t\n" \
			" \t key2= value2 \n" \
			"[section3 ]\n" \
			"\" comment 1   \n"
			"  key3 = value3 \t\n" \
			"; comment = 2   \n"
			"  key4= value4 \n" \
			"[section4 ]\n" \
			"  key1=nolineend" ) <= 0 ) {
		return false;
	}

	return true;
}

} // namespace

std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_read_1()
{
	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		write_default_ini1( file );

		SimpleIni ini( file );

		std::string_view value;

		if( !ini.read("section1","key1", value ) || value != "value1" ) {
			CPPDEBUG( format( "value: %d", value ) );
			CPPDEBUG( "section1/key1 not found" );
			return false;
		}

		if( !ini.read("section1","key2", value ) || value != "value2" ) {
			CPPDEBUG( "section1/key2 not found" );
			return false;
		}

		if( ini.read("section1","key3", value ) ) {
			CPPDEBUG( "section1/key3 found" );
			return false;
		}

		if( !ini.read("section2","key1", value ) || value != "value1" ) {
			CPPDEBUG( format( "section2/key1 not found (value:'%s')", value ) );
			return false;
		}

		if( !ini.read("section2","key2", value ) || value != "value2" ) {
			CPPDEBUG( format( "section2/key2 not found (value:'%s')", value ) );
			return false;
		}

		if( !ini.read("section3","key3", value ) || value != "value3" ) {
			CPPDEBUG( "section3/key3 not found" );
			return false;
		}

		if( !ini.read("section3","key4", value ) || value != "value4" ) {
			CPPDEBUG( "section3/key4 not found" );
			return false;
		}

		if( !ini.read("section4","key1", value ) || value != "nolineend" ) {
			CPPDEBUG( "section4/key1 not found" );
			return false;
		}

		return true;
	};

	return std::make_shared<TestCaseFuncOneFileBuffer>(__FUNCTION__, test_func, 20,
			std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false );
}

namespace {

bool write_default_ini2( SimpleFlashFs::FileBuffer & file )
{
	if( file.write(
			"[section4 ]\n" \
			"  key1=nolineend" ) <= 0 ) {
		return false;
	}

	return true;
}

}

std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_read_2()
{
	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		write_default_ini2( file );

		SimpleIni ini( file );

		std::string_view value;

		if( !ini.read("section4","key1", value ) || value != "nolineend" ) {
			CPPDEBUG( format( "section4/key1 not found (value:'%s')", value ) );
			return false;
		}
		return true;
	};

	return std::make_shared<TestCaseFuncOneFileBuffer>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false );
}

namespace {

bool write_default_ini3( SimpleFlashFs::FileBuffer & file )
{
	if( file.write(
			"[section4 ]\n" \
			"  key1=" ) <= 0 ) {
		return false;
	}

	return true;
}

}


std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_read_3()
{
	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		write_default_ini3( file );

		SimpleIni ini( file );

		std::string_view value;

		if( !ini.read("section4","key1", value ) || value != "" ) {
			CPPDEBUG( format( "section4/key1 not found (value:'%s')", value ) );
			return false;
		}
		return true;
	};

	return std::make_shared<TestCaseFuncOneFileBuffer>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false );
}



namespace {

bool write_default_ini4( SimpleFlashFs::FileBuffer & file )
{
	if( file.write(
			"[section4 ]\n" \
			"  key1 " ) <= 0 ) {
		return false;
	}

	return true;
}

} // namespace

std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_read_4()
{
	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		write_default_ini4( file );

		SimpleIni ini( file );

		std::string_view value;

		if( ini.read("section4","key1", value ) ) {
			CPPDEBUG( format( "section4/key1 found (value:'%s')", value ) );
			return false;
		}
		return true;
	};

	return std::make_shared<TestCaseFuncOneFileBuffer>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false );
}



namespace {

bool write_default_ini5( SimpleFlashFs::FileBuffer & file )
{
	if( file.write(
			" key1 " ) <= 0 ) {
		return false;
	}

	return true;
}

} // namespace

std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_read_5()
{
	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		write_default_ini5( file );

		SimpleIni ini( file );

		std::string_view value;

		if( ini.read("section4","key1", value ) ) {
			CPPDEBUG( format( "section4/key1 found (value:'%s')", value ) );
			return false;
		}
		return true;
	};

	return std::make_shared<TestCaseFuncOneFileBuffer>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false );
}

std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_write_1()
{
	std::string expected_text =
			"[section1]\n" \
			"\tkey1 = value1\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		SimpleIni ini( file );

		std::string_view value;

		if( !ini.write("section1","key1", "value1" ) ) {
			CPPDEBUG( "writing failed" );
			return false;
		}
		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}


std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_write_2()
{
	std::string expected_text =
			"[section1]\n" \
			"\tkey1 = value1\n" \
			"\n" \
			"[section2]\n" \
			"\tkey2 = value2\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		SimpleIni ini( file );

		std::string_view value;

		if( !ini.write("section1","key1", "value1" ) ) {
			CPPDEBUG( "writing failed" );
			return false;
		}

		if( !ini.write("section2","key2", "value2" ) ) {
			CPPDEBUG( "writing failed" );
			return false;
		}

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, true, expected_text );
}


std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_write_3()
{
	std::string expected_text =
			"[section1]\n" \
			"\"\tcomment1\n"
			"\tkey1 = value1\n" \
			"\n" \
			"[section2]\n" \
			"\"\tcomment 2\n"
			"\tkey2 = value2\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		SimpleIni ini( file );

		std::string_view value;

		if( !ini.write("section1","key1", "value1", "comment1" ) ) {
			CPPDEBUG( "writing failed" );
			return false;
		}

		if( !ini.write("section2","key2", "value2", "comment 2" ) ) {
			CPPDEBUG( "writing failed" );
			return false;
		}

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, true, expected_text );
}

std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_write_4()
{
	std::string expected_text =
			"[section1]\n" \
			"\tkey1 = value1\n" \
			"\tkey3 = value3\n" \
			"\n" \
			"[section2]\n" \
			"\tkey2 = value2\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		std::string init_text =
				"[section1]\n" \
				"\tkey1 = value1\n" \
				"\n" \
				"[section2]\n" \
				"\tkey2 = value2\n";


		if( !file.write( init_text ) ) {
			return false;
		}

		SimpleIni ini( file );

		std::string_view value;

		if( !ini.write("section1","key3", "value3" ) ) {
			CPPDEBUG( "writing failed" );
			return false;
		}

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}



std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_write_5()
{
	std::string expected_text =
			"[section1]\n" \
			"\tkey1 = value1\n" \
			"\"\tcomment 3\n"
			"\tkey3 = value3\n" \
			"\n" \
			"[section2]\n" \
			"\tkey2 = value2\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		std::string init_text =
				"[section1]\n" \
				"\tkey1 = value1\n" \
				"\n" \
				"[section2]\n" \
				"\tkey2 = value2\n";


		if( !file.write( init_text ) ) {
			return false;
		}

		SimpleIni ini( file );

		std::string_view value;

		if( !ini.write("section1","key3", "value3", "comment 3" ) ) {
			CPPDEBUG( "writing failed" );
			return false;
		}

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}

std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_write_6()
{
	std::string expected_text =
			"[section1]\n" \
			"\"\tcomment 3\n"
			"\tkey3 = value3\n" \
			"\n" \
			"[section2]\n" \
			"\tkey2 = value2\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		std::string init_text =
				"[section1]\n" \
				"\n" \
				"[section2]\n" \
				"\tkey2 = value2\n";


		if( !file.write( init_text ) ) {
			return false;
		}

		SimpleIni ini( file );

		std::string_view value;

		if( !ini.write("section1","key3", "value3", "comment 3" ) ) {
			CPPDEBUG( "writing failed" );
			return false;
		}

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}


std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_write_7()
{
	std::string expected_text =
			"[section1]\n" \
			"\tkey1 = value1\n" \
			"\"\tcomment3\n"
			"\tkey3 = value3\n" \
			"\n" \
			"[section2]\n" \
			"\tkey2 = value2\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {


		SimpleIni ini( file );

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

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 20,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}

std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_write_8()
{
	std::string expected_text =
			"[section1]\n" \
			"\tkey1 = value1\n" \
			"\"\tcomment3\n"
			"\tkey3 = value3\n" \
			"\n" \
			"[section2]\n" \
			"\tkey2 = value2\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {


		SimpleIni ini( file );

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

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 512,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}

namespace {

bool write_default_ini6( SimpleFlashFs::FileBuffer & file )
{
	if( file.write(
			"[section1]\n" \
			"  key1=1\n" \
			"  key2=-1\n" \
			"  key3=-13\n" \
			"  key4=14\n" \
			"\n" ) <= 0 ) {
		return false;
	}

	return true;
}

} // namespace

std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_read_value_1()
{
	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		if( !write_default_ini6( file ) ) {
			CPPDEBUG( "cannot write default ini" );
			return false;
		}

		SimpleIni ini( file );

		{
			uint32_t i = 0;
			if( !ini.read( "section1", "key1",  i ) ) {
				CPPDEBUG( "reading key1 failed" );
				return false;
			}

			if( i != 1 ) {
				return false;
			}
		}

		{
			int32_t i = 0;
			if( !ini.read( "section1", "key2",  i ) ) {
				CPPDEBUG( "reading key2 failed" );
				return false;
			}

			if( i != -1 ) {
				return false;
			}
		}

		{
			int64_t i = 0;
			if( !ini.read( "section1", "key3",  i ) ) {
				CPPDEBUG( "reading key3 failed" );
				return false;
			}

			if( i != -13 ) {
				return false;
			}
		}

		{
			uint64_t i = 0;
			if( !ini.read( "section1", "key4",  i ) ) {
				CPPDEBUG( "reading key4 failed" );
				return false;
			}

			if( i != 14 ) {
				return false;
			}
		}

		return true;
	};

	return std::make_shared<TestCaseFuncOneFileBuffer>(__FUNCTION__, test_func, 512,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false );
}


std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_read_value_2()
{
	std::string expected_text =
			"[section1]\n"
			"\"	(123)\n"
			"	key1 = 0x42F60000\n"
			"\"	(3.141590)\n"
			"	key2 = 0x40490FD0\n"
			"\"	(3.141590)\n"
			"	key3 = 0x400921F9F01B866E\n"
			"\"	(-3.141590)\n"
			"	key4 = 0xC00921F9F01B866E\n"
			"\"	(-3.141590)\n"
			"	key5 = 0xC0490FD0\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {


		SimpleIni ini( file );

		if( !ini.write( "section1", "key1", 123.0f  ) ) {
			CPPDEBUG( "writing key1 failed" );
			return false;
		}

		if( !ini.write( "section1", "key2", 3.14159f  ) ) {
			CPPDEBUG( "writing key2 failed" );
			return false;
		}

		if( !ini.write( "section1", "key3",  3.14159 ) ) {
			CPPDEBUG( "writing key3 failed" );
			return false;
		}

		if( !ini.write( "section1", "key4",  -3.14159 ) ) {
			CPPDEBUG( "writing key4 failed" );
			return false;
		}

		if( !ini.write( "section1", "key5",  -3.14159f ) ) {
			CPPDEBUG( "writing key3 failed" );
			return false;
		}

		{
			float f = 0;
			if( !ini.read( "section1", "key1", f  ) ) {
				CPPDEBUG( "reading key1 failed" );
				return false;
			}

			if( f != 123.0f ) {
				CPPDEBUG( format( "key1: invalid value: %f != %f", f, 123.0f ) );
				return false;
			}
		}

		{
			float f = 0;
			if( !ini.read( "section1", "key2", f  ) ) {
				CPPDEBUG( "reading key2 failed" );
				return false;
			}

			if( f != 3.14159f  ) {
				CPPDEBUG( format( "key2: invalid value: %f != %f", f, 3.14159f ) );
				return false;
			}
		}

		{
			double f = 0;
			if( !ini.read( "section1", "key3", f  ) ) {
				CPPDEBUG( "reading key3 failed" );
				return false;
			}

			if( f != 3.14159  ) {
				CPPDEBUG( format( "key3: invalid value: %f != %f", f, 3.14159 ) );
				return false;
			}
		}

		{
			double f = 0;
			if( !ini.read( "section1", "key4", f  ) ) {
				CPPDEBUG( "reading key4 failed" );
				return false;
			}

			if( f != -3.14159  ) {
				CPPDEBUG( format( "key4: invalid value: %f != %f", f, -3.14159 ) );
				return false;
			}
		}

		{
			float f = 0;
			if( !ini.read( "section1", "key5", f  ) ) {
				CPPDEBUG( "reading key5 failed" );
				return false;
			}

			if( f != -3.14159f  ) {
				CPPDEBUG( format( "key5: invalid value: %f != %f", f, -3.14159f ) );
				return false;
			}
		}

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 512,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}



std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_read_value_3()
{
	std::string expected_text =
			"[section1]\n" \
			"\"	(x)\n" \
			"	key1 = 0x78\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {


		SimpleIni ini( file );

		if( !ini.write( "section1", "key1", 'x'  ) ) {
			CPPDEBUG( "writing key1 failed" );
			return false;
		}

		{
			char c = ' ';
			if( !ini.read( "section1", "key1", c  ) ) {
				CPPDEBUG( "reading key1 failed" );
				return false;
			}

			if( c != 'x'  ) {
				CPPDEBUG( format( "key1: invalid value: %c != %c", c, 'x' ) );
				return false;
			}
		}

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 512,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}


std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_read_blob_1()
{
	std::string expected_text =
			"[section1]\n" \
			"	key1 = 3132333435363738393000\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {


		SimpleIni ini( file );

		std::string test = "1234567890";
		std::span<const std::byte> blob( reinterpret_cast<const std::byte*>(test.c_str()), test.size() + 1 );

		if( !ini.write_blob( "section1", "key1", blob  ) ) {
			CPPDEBUG( "writing key1 failed" );
			return false;
		}

		{
			std::vector<char> v(50,'\0');
			std::span<std::byte> blob_read(reinterpret_cast<std::byte*>( v.data() ), v.size());


			if( !ini.read_blob( "section1", "key1", blob_read  ) ) {
				CPPDEBUG( "reading key1 failed" );
				return false;
			}

			std::string_view sv_blob( v.data() );
			if( sv_blob != test ) {
				CPPDEBUG( format( "key1 '%s' != '%s'", sv_blob, test ) );
				return false;
			}
		}

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 512,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}


std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_write_9()
{
	std::string expected_text =
			"[CPU Temperature]\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max1 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max2 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max3 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max4 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max5 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max6 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max7 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max8 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max9 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max10 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max11 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max12 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max13 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max14 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max15 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max16 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max17 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max18 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max19 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max20 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max21 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max22 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max23 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max24 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max25 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max26 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max27 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max28 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max29 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max30 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max31 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max32 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max33 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max34 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max35 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max36 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max37 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max38 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max39 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max40 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max41 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max42 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max43 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max44 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max45 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max46 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max47 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max48 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max49 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max50 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max51 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max52 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max53 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max54 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max55 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max56 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max57 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max58 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max59 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max60 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max61 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max62 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max63 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max64 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max65 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max66 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max67 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max68 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max69 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max70 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max71 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max72 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max73 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max74 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max75 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max76 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max77 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max78 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max79 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max80 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max81 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max82 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max83 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max84 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max85 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max86 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max87 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max88 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max89 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max90 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max91 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max92 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max93 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max94 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max95 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max96 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max97 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max98 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max99 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max100 = 0xFFF0000000000000\n" \
			"\"\t(-inf)\n" \
			"\tCPU_max101 = 0xFFF0000000000000\n" \
			"\n" \
			"[global]\n" \
			"\tcurrent_idx = 93\n" \
			"\tglobal_writes = 799\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		const unsigned MAX_CYCLES = 80;
		unsigned global_writes = 1;

		const char *SECTION_GLOBAL    = "global";
		const char *SECTION_CPU_TEMP  = "CPU Temperature";

		const char *KEY_CURRENT_IDX   = "current_idx";
		const char *KEY_GLOBAL_WRITES = "global_writes";
		const char *KEY_CPU_MAX       = "CPU_max";

		for( unsigned j = 0; j < 10; ++j ) {
			for( unsigned i = 0; i < MAX_CYCLES; ++i ) {

				SimpleIni<100> ini( file );

				int32_t current_idx = -1;
				ini.read( SECTION_GLOBAL, KEY_CURRENT_IDX,   current_idx );

				int32_t global_writes = -1;
				ini.read( SECTION_GLOBAL, KEY_GLOBAL_WRITES, global_writes );


				if( current_idx < 0 ) {
					current_idx = 0;
				} else if( current_idx > 100 ) {
					current_idx = 0;
				}

				current_idx++;
				global_writes++;

				std::string key = Tools::format( "%s%d", KEY_CPU_MAX, current_idx );
				double dval = -1.0/0.0;

				if( !ini.write( SECTION_CPU_TEMP, key, dval ) ) {
					CPPDEBUG( Tools::format("writing %s failed", key ) );
					return false;
				}

				if( !ini.write( SECTION_GLOBAL, KEY_CURRENT_IDX, current_idx  ) ) {
					CPPDEBUG( Tools::format("writing %s failed", "curent_idx" ) );
					return false;
				}

				if( !ini.write( SECTION_GLOBAL, KEY_GLOBAL_WRITES, global_writes  ) ) {
					CPPDEBUG( Tools::format("writing %s failed", "global_writes" ) );
					return false;
				}
			}
		}

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 1024,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}


std::shared_ptr<TestCaseBase<bool>> test_case_simple_ini_write_10()
{
	std::string expected_text =
			"[config]\n" \
			"\tSimEnabled = 1\n" \
			"\tCleanser1Enabled = 1\n" \
			"\tCleanser2Enabled = 1\n" \
			"\tMeasurementCells = 1\n";

	auto test_func = []( SimpleFlashFs::FileBuffer & file ) {

		{
			SimpleIni ini( file );

			if( !ini.write( "config", "SimEnabled", 0 ) ) {
				CPPDEBUG( "writing key SimEnabled failed" );
				return false;
			}

			if( !ini.write( "config", "Cleanser1Enabled", 1 ) ) {
				CPPDEBUG( "writing key Cleanser1Enabled failed" );
				return false;
			}

			if( !ini.write( "config", "Cleanser2Enabled", 1 ) ) {
				CPPDEBUG( "writing key Cleanser2Enabled failed" );
				return false;
			}

			if( !ini.write( "config", "MeasurementCells", 1 ) ) {
				CPPDEBUG( "writing key MeasurementCells failed" );
				return false;
			}

		}

		{
			SimpleIni ini( file );

			if( !ini.write( "config", "SimEnabled", 1 ) ) {
				CPPDEBUG( "writing key SimEnabled failed" );
				return false;
			}

			if( !ini.write( "config", "Cleanser1Enabled", 1 ) ) {
				CPPDEBUG( "writing key Cleanser1Enabled failed" );
				return false;
			}


			if( !ini.write( "config", "Cleanser2Enabled", 1 ) ) {
				CPPDEBUG( "writing key Cleanser2Enabled failed" );
				return false;
			}

			if( !ini.write( "config", "MeasurementCells", 1 ) ) {
				CPPDEBUG( "writing key MeasurementCells failed" );
				return false;
			}
		}

		return true;
	};

	return std::make_shared<TestCaseFuncWriteIni>(__FUNCTION__, test_func, 512,
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc, false, expected_text );
}
