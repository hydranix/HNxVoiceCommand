#pragma once
//'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
// SAPI 5.4 Recognition Class
//    by Hydranix (c) 2016
//
// This class fully wraps basic speech recognition
//  based on Microsoft's SAPI 5.4 into a voice
//  command application
//
// Currently this class creates an InProc Recognizer that
//  waits for a "hotword" (default: "computer") to be spoken.
//  It then waits for a command. Commands are added via
//  Recog::AddString() and related. Each command string has
//  a coresponding commandline which is passed to ShellExecute()
//  when the command is detected.
//.............................................................

//#include "Command.hpp"
#include <windows.h>
#include <sapi.h>
// sphelper.h is harmlessly broken
//  so we ignore this error
#pragma warning (disable:4996)
#include <sphelper.h>

#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
using namespace std::literals::chrono_literals;

#ifdef DEBUG
#include <iostream>
#define DOUT(x) do { std::cerr << x << std::endl; } while(0)
#else
#define DOUT(x)
#endif
// This makes language=409 actually mean something...
//  probably not the best idea though
#define ENGLISH L"language=409"

namespace
{
  inline void COM(HRESULT hr)
  {
    if (hr >= static_cast<HRESULT>(0L)) // S_OK
      return;
    else
    {
      std::string msg("COM error.\nHRESULT code: " + std::to_string(hr));
      throw std::runtime_error(msg.c_str());
    }
  }

  inline std::string ws2s(const std::wstring & wstr)
  {
    std::string str;
    str.resize(static_cast<size_t>(WideCharToMultiByte(CP_ACP, 0ul, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr)));
    WideCharToMultiByte(CP_ACP, 0ul, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], static_cast<int>(str.size()), nullptr, nullptr);
    return str;
  }
}

class Command
{
public:
  Command()
    : _exec(std::string())
    , _param(std::string())
  {}

  Command(const Command & c)
    : _exec(c._exec)
    , _param(c._param)
  {}

  Command(const std::string & prog,
          const std::string & param)
    : _exec(prog)
    , _param(param)
  {}

  Command(const std::string & cmdline)
    : _exec(std::string())
    , _param(std::string())
  {
    bool quoted = false;
    bool firstchar = false;
    size_t pos = 0;
    auto b = cmdline.begin();
    for (; b != cmdline.end(); ++b, ++pos)
    {
      if (!firstchar)
      {
        if (*b == ' ')
          continue;
        else
          firstchar = true;
      }
      if (*b == '"')
        quoted = !quoted;
      if (!quoted)
        if (*b == ' ')
          break;
      _exec.push_back(*b);
    }
    _param = cmdline.substr(pos);
  }

  std::string exec() const
  {
    return _exec;
  }
  std::string exec()
  {
    return _exec;
  }
  const std::string exec() const
  {
    return _exec;
  }
  const std::string exec()
  {
    return _exec;
  }

  std::string param() const
  {
    return _param;
  }
  std::string param()
  {
    return _param;
  }
  const std::string param() const
  {
    return _param;
  }
  const std::string param()
  {
    return _param;
  }

  std::string cmdline() const
  {
    std::string _cmdline;
    if(!_exec.empty())
    {
      _cmdline = _exec;
      if(!_param.empty())
      {
        _cmdline.push_back(' ');
        _cmdline += _param;
      }
    }
    return _cmdline;
  }

  Command & operator=(const Command & c)
  {
    _exec = c._exec;
    _param = c._param;
    return *this;
  }

  friend bool operator==(const Command & a, const Command & b)
  {
    return ((a.exec() == b.exec())&&(a.param() == b.param()));
  }

  friend bool operator!=(const Command & a, const Command & b)
  {
    return ((a.exec() != b.exec())&&(a.param() != b.param()));
  }

private:
  std::string _exec;
  std::string _param;
};


enum RecogMode
{
  NotListening,
  ListeningForHotword,
  ListeningForCommand
};

class Recog
{
public:
  Recog(const std::string _hotword = "computer")
    : pause_count(0)
    , spRecogognizer(nullptr)
    , cpRecognizerToken(nullptr)
    , sprContext(nullptr)
    , spAudioInToken(nullptr)
    , sprGrammar(nullptr)
    , hotword(_hotword)
    , initialized(false)
    , thread_continue(false)
    , thread_finished(false)
    , ListeningMode(NotListening)
  {
    ::COM(CoInitialize(nullptr));
    ::COM(CoCreateInstance(CLSID_SpInprocRecognizer,
                           nullptr,
                           CLSCTX_ALL,
                           IID_ISpRecognizer,
                           reinterpret_cast<void**>(&spRecogognizer)));
    ::COM(CoCreateInstance(CLSID_SpObjectToken,
                           nullptr,
                           CLSCTX_ALL,
                           IID_ISpObjectTokenInit,
                           reinterpret_cast<void**>(&spAudioInToken)));
    ::COM(SpGetDefaultTokenFromCategoryId(SPCAT_AUDIOIN, &spAudioInToken));
    ::COM(spRecogognizer->SetInput(spAudioInToken, TRUE));
    ::COM(SpFindBestToken(SPCAT_RECOGNIZERS, ENGLISH, NULL, &cpRecognizerToken));
    ::COM(spRecogognizer->SetRecognizer(cpRecognizerToken));
    ::COM(spRecogognizer->CreateRecoContext(&sprContext));
    ::COM(sprContext->Pause(0ul));
    ++pause_count;
    ::COM(sprContext->CreateGrammar(0ull, &sprGrammar));
    ::COM(sprGrammar->ResetGrammar(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)));
    SPSTATEHANDLE spsh = { 0 };
    ::COM(sprGrammar->GetRule(L"HotWord", 0ul, SPRAF_TopLevel | SPRAF_Active, true, &spsh));
    std::wstring whotword(hotword.begin(), hotword.end());
    ::COM(sprGrammar->AddWordTransition(spsh, nullptr, whotword.c_str(), nullptr, SPWT_LEXICAL, 1, nullptr));
    ::COM(sprGrammar->Commit(0ul));
    ::COM(sprContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION)));
    ::COM(sprGrammar->SetRuleState(L"HotWord", nullptr, SPRS_ACTIVE));
    ::COM(sprGrammar->GetRule(L"Commands", 1ul, SPRAF_TopLevel | SPRAF_Active, true, &spsh));
    ::COM(sprGrammar->AddWordTransition(spsh, nullptr, L"Shutdown Speech Recognition", nullptr, SPWT_LEXICAL, 1, nullptr));
    ::COM(sprGrammar->Commit(0ul));
    ::COM(sprContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION)));
    ::COM(sprGrammar->SetRuleState(L"Commands", nullptr, SPRS_INACTIVE));
  }

  ~Recog()
  {
    thread_continue = false;
    int x = 0;
    while(!thread_finished)
    {
      if(x>3)
      {
        break;
      }
      ++x;
      std::this_thread::sleep_for(1s);
    }
    if (sprGrammar)
    {
      sprGrammar->ResetGrammar(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
      sprGrammar->Release();
      sprGrammar = nullptr;
    }
    if (sprContext)sprContext->Release();
    sprContext = nullptr;
    if (spAudioInToken)spAudioInToken->Release();
    spAudioInToken = nullptr;
    if (cpRecognizerToken)cpRecognizerToken->Release();
    cpRecognizerToken = nullptr;
    if (spRecogognizer)spRecogognizer->Release();
    spRecogognizer = nullptr;
    CoUninitialize();
  }

  // pauses events queue processing but continues to listen and queue events
  // if still_listen is false, will not queue events
  void pause()
  {
    if (!initialized)
      initialize();
    ++pause_count;
    ::COM(sprContext->Pause(0ul));
  }

  void resume()
  {
    if (!initialized)
      initialize();
    if (pause_count)
    {
      --pause_count;
      ::COM(sprContext->Resume(0ul));
    }
  }

  void stop_listening()
  {
    if (!initialized)
      initialize();
    SPRECOSTATE state;
    ::COM(spRecogognizer->GetRecoState(&state));
    if (state == SPRST_ACTIVE)
      ::COM(spRecogognizer->SetRecoState(SPRST_INACTIVE));
    DOUT("Not listening...");
  }

  void start_listening()
  {
    if (!initialized)
      initialize();
    SPRECOSTATE state;
    ::COM(spRecogognizer->GetRecoState(&state));
    if (state == SPRST_INACTIVE)
      ::COM(spRecogognizer->SetRecoState(SPRST_ACTIVE));
    DOUT("Listening...");
  }

  void AddString(const std::string & str,
                 const std::string & cmdline)
  {
    if (!initialized)
      initialize();
    if (str.empty() || cmdline.empty())
      return;
    SPSTATEHANDLE spsh = { 0 };
    ::COM(sprGrammar->GetRule(L"Commands", 1ul, SPRAF_TopLevel | SPRAF_Active, true, &spsh));
    std::wstring wstr(str.begin(), str.end());
    ::COM(sprGrammar->AddWordTransition(spsh, nullptr, wstr.c_str(), nullptr, SPWT_LEXICAL, 1, nullptr));
    ::COM(sprGrammar->Commit(0ul));
    ::COM(sprContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION)));
    ::COM(sprGrammar->SetRuleState(L"Commands", nullptr, SPRS_INACTIVE));
    Command cmd(cmdline);
    mCmd[str] = cmd;
    DOUT("Added phrase: " << str);
  }

  void AddString(const std::string & str,
                 const std::string & prog,
                 const std::string & param)
  {
    if(!prog.empty() && !str.empty())
    {
      std::string cmdline = prog;
      if(!param.empty())
      {
        cmdline.push_back(' ');
        cmdline += param;
      }
      AddString(str, cmdline);
    }
  }

  // Gets all strings currently in the mCmd map
  // even indexes are the keys
  // even indexes + 1 are the values
  // [0] = first key; [1] = first value
  std::vector<std::string> GetStrings()
  {
    std::vector<std::string> vStrs;
    for(auto b = mCmd.begin();b!=mCmd.end();++b)
    {
      vStrs.push_back(b->first);
      vStrs.push_back(b->second.cmdline());
    }
    DOUT("All phrases returned");
    return vStrs;
  }

  void initialize()
  {
    if (!initialized)
    {
      thread_continue = true;
      std::thread thr(&Recog::EventThread, this);
      thr.detach();
      initialized = true;
      resume();
      DOUT("initialied");
    }
  }

  void AddStrings(const std::vector<std::string> & vCmds)
  {
    if(vCmds.size() % 2)
    {
      throw std::runtime_error("vector has odd number of members");
    }
    int i=0;
    for(auto b = vCmds.begin(); b != vCmds.end();++i)
    {
      AddString(*b++, *b++);
    }
    DOUT("Added " << i/2 << " phrases");
  }

  void remove_by_phrase(const std::string & phrase)
  {
    for(auto b = mCmd.begin(); b != mCmd.end(); ++b)
    {
      if(b->first == phrase)
      {
        DOUT("Removing by phrase: " << b->first);
        mCmd.erase(b);
        refresh_rules();
        return;
      }
    }
  }

  void remove_by_command(const std::string & cmdline)
  {
    Command cmd(cmdline);
    for(auto b = mCmd.begin(); b != mCmd.end(); ++b)
    {
      if(b->second == cmd)
      {
        DOUT("Removing by cmdline: " << b->second.cmdline());
        mCmd.erase(b);
        refresh_rules();
        return;
      }
    }
  }

  void wait_for_exit()
  {
    if (!initialized)
      initialize();
    std::unique_lock<std::mutex> lck(exit_wait_mtx);
    DOUT("Thread is waiting for exit...");
    while (thread_continue)
    {
      if (exit_wait_cv.wait_for(lck, 1s) == std::cv_status::no_timeout)
        thread_continue = false;
    }
  }

private:
  int pause_count;

  std::map<std::string, Command> mCmd;

  ISpRecognizer* spRecogognizer;
  ISpObjectToken* cpRecognizerToken;
  ISpRecoContext* sprContext;
  ISpObjectToken* spAudioInToken;
  ISpRecoGrammar* sprGrammar;

  // word that activates listening for commands
  std::string hotword;

  bool initialized;
  bool thread_continue;
  bool thread_finished;

  RecogMode ListeningMode;

  std::mutex exit_wait_mtx;
  std::condition_variable exit_wait_cv;

  void ExecCommand(const Command & cmd)
  {
    DOUT("ExecCommand thread started");
    SHELLEXECUTEINFOA shex = { 0 };
    shex.cbSize = sizeof(SHELLEXECUTEINFOA);
    shex.nShow = SW_SHOW;
    std::string exec = cmd.exec();
    if (!exec.empty())
    {
      DOUT("Exec: " << exec);
      shex.lpFile = exec.c_str();
      std::string param = cmd.param();
      if (!param.empty())
      {
        DOUT("Param: " << param);
        shex.lpParameters = param.c_str();
      }
      shex.lpVerb = "open";
      if (ShellExecuteExA(&shex) == FALSE)
      {
        DOUT("ShellExecuteExA failed...");
      }
      else
      {
        DOUT("ShellExecuteExA worked...");
      }
    }
  }

  void EventThread()
  {
    DOUT("Event thread started...");
    ::COM(sprContext->SetNotifyWin32Event());
    HANDLE hEvent = nullptr;
    hEvent = sprContext->GetNotifyEventHandle();
    if (hEvent == INVALID_HANDLE_VALUE)
      throw std::runtime_error("iSPRecoContext::GetNotifyEventHandle returned invalid handle");
    SPEVENT spEvent = { 0 };
    ISpRecoResult* sprResult = nullptr;
    int waiting_seconds = 0;
    while (thread_continue)
    {
      if (WaitForMultipleObjects(1ul, &hEvent, FALSE, 1000) == WAIT_TIMEOUT)
      {
        if(!thread_continue)
        {
          break;
        }
        if (ListeningMode == ListeningForCommand)
        {
          ++waiting_seconds;
          if (waiting_seconds == 5)
          {
            DOUT("Listening for cmmand timed out...");
            ::COM(sprGrammar->SetRuleState(L"Commands", nullptr, SPRS_INACTIVE));
            DOUT("command rule inactive");
            ::COM(sprGrammar->SetRuleState(L"HotWord", nullptr, SPRS_ACTIVE));
            DOUT("hotword rule active");
            ListeningMode = ListeningForHotword;
            waiting_seconds = 0;
          }
        }
        else
        {
          waiting_seconds = 0;
        }
        continue;
      }
      ::COM(sprContext->GetEvents(1ul, &spEvent, nullptr));
      DOUT("event found");
      sprResult = reinterpret_cast<ISpRecoResult*>(spEvent.lParam);
      wchar_t *text;
      ::COM(sprResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, FALSE, &text, NULL));
      std::string strText = ws2s(std::wstring(text));
      DOUT("text: " << strText << std::endl);
      if (strText == hotword)
      {
        ::COM(sprGrammar->SetRuleState(L"Commands", nullptr, SPRS_ACTIVE));
        DOUT("command rule actve");
        ::COM(sprGrammar->SetRuleState(L"HotWord", nullptr, SPRS_INACTIVE));
        DOUT("hotword rule inactive");
        ListeningMode = ListeningForCommand;
        // TODO play sound or some notification of readiness
      }
      else if (strText == "Shutdown Speech Recognition")
      {
        exit_wait_cv.notify_all();
        break;
      }
      else
      {
        ::COM(sprGrammar->SetRuleState(L"Commands", nullptr, SPRS_INACTIVE));
        DOUT("command rule inactive");
        ::COM(sprGrammar->SetRuleState(L"HotWord", nullptr, SPRS_ACTIVE));
        DOUT("hotword rule active");
        ListeningMode = ListeningForHotword;
        Command cmd = mCmd[strText];
        std::thread cmdthr(&Recog::ExecCommand, this, cmd);
        cmdthr.detach();
      }
    }
    thread_finished = true;
  }

  void refresh_rules()
  {
    ::COM(sprGrammar->ResetGrammar(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)));
    SPSTATEHANDLE spsh = { 0 };
    ::COM(sprGrammar->GetRule(L"HotWord", 0ul, SPRAF_TopLevel | SPRAF_Active, true, &spsh));
    std::wstring whotword(hotword.begin(), hotword.end());
    ::COM(sprGrammar->AddWordTransition(spsh, nullptr, whotword.c_str(), nullptr, SPWT_LEXICAL, 1, nullptr));
    ::COM(sprGrammar->Commit(0ul));
    ::COM(sprContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION)));
    ::COM(sprGrammar->SetRuleState(L"HotWord", nullptr, SPRS_ACTIVE));
    ::COM(sprGrammar->GetRule(L"Commands", 1ul, SPRAF_TopLevel | SPRAF_Active, true, &spsh));
    ::COM(sprGrammar->AddWordTransition(spsh, nullptr, L"Shutdown Speech Recognition", nullptr, SPWT_LEXICAL, 1, nullptr));
    ::COM(sprGrammar->Commit(0ul));
    ::COM(sprContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION)));
    ::COM(sprGrammar->SetRuleState(L"Commands", nullptr, SPRS_INACTIVE));
    for(auto b=mCmd.begin();b!=mCmd.end();++b)
    {
      AddString(b->first, b->second.cmdline());
    }
  }
};

