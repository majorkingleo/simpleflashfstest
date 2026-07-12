/**
 * class for printing debugging messages async to stdout
 * @author Copyright (c) 2023 Martin Oberzalek
 */
#pragma once
#include <OutDebug.h>
#include <ColoredOutput.h>
#include <FastDelivery.h>
#include <variant>
#include <mutex>
#include <fstream>

#if __cplusplus >= 202002L
#include <semaphore>
#else
#include <microplatform/microlibs/std/semaphore>
#endif

#if __cplusplus < 202002L
namespace microlibs_compat {
  class binary_semaphore
  {
    std::semaphore m_semaphore{ 1 };

  public:

    void aquire() {
      m_semaphore.wait();
    }

    void release() {
      m_semaphore.post();
    }
  };
}
#endif

namespace AsyncOut {

#if __cplusplus >= 202002L
  using binary_semaphore = std::binary_semaphore;
#else
  using binary_semaphore = microlibs_compat::binary_semaphore;
#endif

struct Data
{
	const char *file;
	unsigned line;
	const char *function;
	std::variant<std::string,std::wstring> message;
	Tools::ColoredOutput::Color color;
	std::wstring prefix;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
};

class Logger : public Tools::FastDelivery::PublisherNode<Data>, public Tools::OutDebug
{
	std::list<value_type> messages;
	std::mutex m_messages;

    binary_semaphore m_worktodo{0};

public:

	// called async from Debug publisher.
	void deliver( const value_type & msg );

	virtual void log();

	// wait for data
	void wait();

protected:
	std::list<value_type> popAll();
};

class FileLogger : public Logger
{
  std::ofstream m_out{};
  std::string   m_file_name_pattern{};

public:
  FileLogger(const std::string& file_name_pattern = "log-%d.txt" );

  void log() override;

private:
  void rotate_file( const std::string file_name_pattern, unsigned next_version ) const;
};

class Debug : public Tools::OutDebug, public Tools::FastDelivery::Publisher<Data,Logger>
{
public:
	Debug( Tools::ColoredOutput::Color color = Tools::ColoredOutput::BRIGHT_YELLOW );

#ifdef __cpp_lib_string_view
	void add( const char *file, unsigned line, const char *function, const std::string_view & s ) override;
	void add( const char *file, unsigned line, const char *function, const std::wstring_view & s ) override;
#else
	void add( const char *file, unsigned line, const char *function, const std::string & s ) override;
	void add( const char *file, unsigned line, const char *function, const std::wstring & s ) override;
#endif
};

} // namespace AsyncOut

