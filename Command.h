#pragma once
#include "Util.h"

namespace HNx
{
class Command
{
public:
  Command() = default;

  Command(Command const& c)
    : m_phrase(c.m_phrase)
    , m_exec(c.m_exec)
    , m_param(c.m_param)
  {}

  Command(Command&& c)
    : m_phrase(std::move(c.m_phrase))
    , m_exec(std::move(c.m_exec))
    , m_param(std::move(c.m_param))
  {}

  Command& operator=(Command const& c)
  {
    if(this != &c)
    {
      m_phrase = c.m_phrase;
      m_exec = c.m_exec;
      m_param = c.m_param;
    }
    return *this;
  }

  Command& operator=(Command&& c)
  {
    if(this != &c)
    {
      m_phrase.swap(c.m_phrase);
      m_exec.swap(c.m_exec);
      m_param.swap(c.m_param);
    }
    return *this;
  }

  Command(std::wstring_view phrase,
          std::wstring_view prog,
          std::wstring_view param = L"")
    : m_phrase(phrase)
    , m_exec(prog)
    , m_param(param)
  {}

  //Command(std::wstring const& cmdline)
  //{
  //  std::wstring trimmed = trim_whitespace(cmdline);
  //
  //  // handle quoted and escaped cmdlines
  //  //  and split executable path from
  //  // its args trims unquoted and unescaped
  //  //  extra whitespace down to a single
  //  //  space character
  //  enum class State
  //  {
  //    Normal,
  //    SQuote,
  //    DQuote,
  //    Escaped,
  //    Trim
  //  }state = State::Normal;
  //
  //  State last_state = state;
  //
  //  bool exec_complete{ false };
  //
  //  for (auto c : trimmed)
  //  {
  //    if (!exec_complete)
  //    {
  //      switch (state)
  //      {
  //        case State::Escaped:
  //        m_exec.push_back(c);
  //        state = last_state;
  //        break;
  //
  //        case State::DQuote:
  //        if (c == L'\\')
  //        {
  //          last_state = state;
  //          state = State::Escaped;
  //        }
  //        if (c == L'"')
  //          state = State::Normal;
  //
  //        m_exec.push_back(c);
  //        break;
  //
  //        case State::SQuote:
  //        if (c == '\\')
  //        {
  //          last_state = state;
  //          state = State::Escaped;
  //        }
  //        if (c == '\'')
  //          state == State::Normal;
  //        break;
  //
  //        case State::Trim:
  //        if (c == ' ')
  //          continue;
  //
  //        state = State::Normal;
  //          /* fallthrough */
  //        case State::Normal:
  //        if (c == L'\'')
  //        {
  //          state = State::SQuote;
  //          continue;
  //        }
  //
  //        if (c == L'"')
  //        {
  //          state = State::DQuote;
  //          continue;
  //        }
  //
  //        if (c == L'\\')
  //        {
  //          last_state = state;
  //          state = State::Escaped;
  //          continue;
  //        }
  //
  //        if (c == L' ')
  //        {
  //          state = State::Trim;
  //          exec_complete = true;
  //          continue;
  //        }
  //        m_exec.push_back(c);
  //        break;
  //      }
  //      m_param.push_back(c);
  //    }
  //  }
  //}

  std::wstring phrase() const
  {
    return m_phrase;
  }

  std::wstring exec() const
  {
    return m_exec;
  }

  std::wstring param() const
  {
    return m_param;
  }

  void setPhrase(std::wstring_view t)
  {
    m_phrase = t;
  }

  void setExec(std::wstring_view t)
  {
    m_exec = t;
  }

  void setParam(std::wstring_view t)
  {
    m_param = t;
  }

  // combines the exec and optional param into 
  //   a cmdline ready for CreateProcess()
  std::wstring cmdline() const
  {
    std::wstring cmdline;
    if(!m_exec.empty())
    {
      cmdline = m_exec;
      if(!m_param.empty())
      {
        cmdline.push_back(' ');
        cmdline.append(m_param);
      }
    }
    return cmdline;
  }


  friend bool operator==(Command const& a, Command const& b)
  {
    return ((a.phrase() == b.phrase()) &&
            (a.exec()   == b.exec())   &&
            (a.param()  == b.param()));
  }

  friend bool operator!=(Command const& a, Command const& b)
  {
    return !(a == b);
  }

private:
  // phrase to listen for
  std::wstring m_phrase {};

  // program to execute and
  //  it's optional args
  std::wstring m_exec {};
  std::wstring m_param {};
};
}