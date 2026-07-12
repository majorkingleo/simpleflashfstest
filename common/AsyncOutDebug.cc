/**
 * class for printing debugging messages async to stdout
 * @author Copyright (c) 2023 Martin Oberzalek
 */
#include "AsyncOutDebug.h"
#include <filesystem>
#include <iostream>
#include <stderr_exception.h>
#include <filesystem>

using namespace Tools;

namespace AsyncOut {

  static const std::string DEBUGUART_CPP = "debuguart.cpp";

  Debug::Debug(ColoredOutput::Color color)
    : OutDebug(color)
  {

  }

#ifdef __cpp_lib_string_view
  void Debug::add(const char* file, unsigned line, const char* function, const std::string_view& s)
  {
    distribute(Data{ file,line,function, std::string(s), color, prefix });
  }

  void Debug::add(const char* file, unsigned line, const char* function, const std::wstring_view& s)
  {
    distribute(Data{ file,line,function, std::wstring(s), color, prefix });
  }
#else
  void Debug::add(const char* file, unsigned line, const char* function, const std::string& s)
  {
    distribute(Data{ file,line,function, s, color, prefix });
  }

  void Debug::add(const char* file, unsigned line, const char* function, const std::wstring& s)
  {
    distribute(Data{ file,line,function, s, color, prefix });
  }
#endif

  void Logger::deliver(const value_type& msg)
  {
    std::lock_guard<std::mutex> ml(m_messages);
    messages.push_back(msg);
    m_worktodo.release();
  }

  void Logger::wait()
  {
    m_worktodo.acquire();
  }

  std::list<Logger::value_type> Logger::popAll()
  {
    std::lock_guard<std::mutex> ml(m_messages);
    std::list<value_type> ret(std::move_iterator(std::begin(messages)),
      std::move_iterator(std::end(messages)));
    messages.clear();
    return ret;
  }

  void Logger::log()
  {
    auto all_messages = popAll();

    for (const auto& m : all_messages) {

      std::string message;

      if (const auto str_ptr(std::get_if<std::string>(&m.message)); str_ptr) {
        message = *str_ptr;
      }
      else {
        message = DetectLocale::w2out(std::get<std::wstring>(m.message));
      }

      const std::string file_name = std::filesystem::path(m.file).filename().string();

      if (file_name != DEBUGUART_CPP) {

        if (print_line_and_file_info) {
          std::string file_name = std::filesystem::path(m.file).filename().string();
          std::cout << color_output(m.color, file_name);
          std::cout << ':' << m.line
            << " ";
        }

        if (!prefix.empty()) {
          std::cout << color_output(m.color, DetectLocale::w2out(m.prefix));
          std::cout << " ";
        }

        std::cout << message << '\n';
      }
      else {
        std::cout << message;
      }
    }
  }

  FileLogger::FileLogger(const std::string& file_name_pattern)
    : m_file_name_pattern(file_name_pattern)
  {
    rotate_file(m_file_name_pattern, 0);

    const std::string file_name = Tools::format(m_file_name_pattern, 0);

    m_out.open(file_name);

    if (!m_out) {
      throw STDERR_EXCEPTION("Cannot open log file: " + file_name);
    }
  }


  static std::string to_time(const std::chrono::system_clock::time_point& tp)
  {
    auto t = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count() % 1000;

    struct tm tm = *localtime(&t);

    return Tools::format("%02d:%02d:%02d.%03d", tm.tm_hour, tm.tm_min, tm.tm_sec, ms);
  }

  void FileLogger::log()
  {
    auto all_messages = popAll();       

    for (const auto& m : all_messages) {

      std::string message;

      if (const auto str_ptr(std::get_if<std::string>(&m.message)); str_ptr) {
        message = *str_ptr;
      }
      else {
        message = DetectLocale::w2out(std::get<std::wstring>(m.message));
      }

      const std::string file_name = std::filesystem::path(m.file).filename().string();

      if (file_name != DEBUGUART_CPP) {
        if (print_line_and_file_info) {
          m_out << "[" << to_time(m.timestamp) << "] ";
          
          m_out << color_output(m.color, file_name);
          m_out << ':' << m.line
            << " ";
        }

        if (!prefix.empty()) {
          m_out << color_output(m.color, DetectLocale::w2out(m.prefix));
          m_out << " ";
        }

        m_out << message << '\n';
      } else {
        m_out << message;
      }    
    }

    m_out.flush();
  }

  void FileLogger::rotate_file(const std::string file_name_pattern, unsigned next_version) const
  {
    namespace fs = std::filesystem;

    const std::string file_name = Tools::format(file_name_pattern, next_version);
    
    if (!fs::exists(file_name)) {
      return;
    }

    if (next_version < 10) {
      rotate_file(file_name_pattern, next_version + 1);
    }

    const std::string next_filename = Tools::format(file_name_pattern, next_version + 1);

    fs::rename(file_name, next_filename);
  }

} // namespace AsyncOut