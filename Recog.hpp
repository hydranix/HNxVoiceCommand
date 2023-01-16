#pragma once
//'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//    Copyright Hydranix (C) 2016-2023
//
// This class provides basic speech recognition
//  using Microsoft's SAPI 5.4
//  
//        
// 
//...................................................................
/////////////////////////////////////////////////////////////////////
//  How to use:
// 
//  1
//  Initialize the recognition index by calling
//   initialize(),
// 2
//  Activate the recognition by calling start()
// 3
//  Speak either the hotword to activate your
//   custom commands or speak one of the 
//   built-in commands.
// (built-in commands only active when listening
//   for the hotwrd)
// 
/////////////////////////////////////////////////////////////////////
//        Built-in Commands
// 
// Command:
//    "Shutdown Speech Recognition"
// 
// Action:
//   Immediately stops the recognition
//     and exits the process
// 
/////////////////////////////////////////////////////////////////////
#include "Command.h"
#include "CommandGroup.h"
#include "Util.h"

#include <windows.h>
#include <sapi.h>

#include <combaseapi.h>

// sphelper.h is  broken
//  so we ignore this error
#pragma warning (disable:4996)
#include <sphelper.h>

#include <string>
#include <string_view>
#include <sstream>

#include <stdexcept>

#include <vector>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>
using namespace std::literals::chrono_literals;



#ifdef DEBUG
#include <iostream>
#define DOUT(x) do { std::cerr << x << std::endl; } while(0)
//#define WDOUT(x) do { std::wcerr << x << std::endl; } while(0)
#else
#define DOUT(x)
//#define WDOUT(x)
#endif

namespace HNx
{

inline
void
ErrMsg(std::wstring_view msg = L"Unknown Error")
{
  MessageBoxW(nullptr, L"HNx Voice Command Error", std::wstring(msg).c_str(), MB_OK);
};

constexpr auto ENGLISH_LANG_ID {L"language=409"};

constexpr unsigned long long HotwordGramID {1ull};
constexpr unsigned long long BuiltInGramID {2ull};
constexpr unsigned long long CommandsGramID {3ull};

enum class RecoState
{
  Unknown,    // Uninitialized
  Inactive,   // Not listening for the hotword
  Active,     // Listening for the hotword
  Listening,  // Hotword detected, listening for command
  Paused,     // Temporary pause
};

class Recog
{
public: // Functions

  //========================================//
  //   Constructor                          //
  //========================================//
  //  t_hotword [optional]                  //
  // this is the activation command         //
  //========================================//
  Recog(const std::wstring t_hotword = L"computer")
    : hotword(t_hotword)
  {}

  ~Recog()
  {
    CoUninitialize();
  }

  // sets up the recognizer
  bool initialize()
  {

    HRESULT hr = CoInitialize(nullptr);
    if(FAILED(hr))
    {
      ErrMsg(L"Failed to intialize COM...\n Error: " + std::to_wstring(hr));
      return false;
    }

    // this is our reco interface
    hr = spRecogognizer.CoCreateInstance(IID_ISpRecognizer,
                                         nullptr,
                                         CLSCTX_ALL);
    if(FAILED(hr))
    {
      ErrMsg(L"Failed to create InProc Recognizer instance\n Error: " + std::to_wstring(hr));
      return false;
    }

    // this is the interface we use to select input device
    hr = spAudioInToken.CoCreateInstance(IID_ISpObjectTokenInit,
                                         nullptr,
                                         CLSCTX_ALL);
    if(FAILED(hr))
    {
      ErrMsg(L"Failed creating object token.\nError: " + std::to_wstring(hr));
      return false;
    }

    // we just want the default input device
    hr = SpGetDefaultTokenFromCategoryId(SPCAT_AUDIOIN, &spAudioInToken);
    if(FAILED(hr))
    {
      ErrMsg(L"Failed to get default audio input token.\nError: " + std::to_wstring(hr));
      return false;
    }

    // tell recognizer to use the input device
    hr = spRecogognizer->SetInput(spAudioInToken, TRUE);
    if(FAILED(hr))
    {
      ErrMsg(L"Failed setting recognizer audio input token.\nError: " + std::to_wstring(hr));
      return false;
    }

    // shortcut to get the english recognizer token
    hr = SpFindBestToken(SPCAT_RECOGNIZERS, ENGLISH_LANG_ID, NULL, &cpRecognizerToken);
    if(FAILED(hr))
    {
      ErrMsg(L"Failed finding best recognizer token.\nError: " + std::to_wstring(hr));
      return false;
    }

    // start  
    hr = spRecogognizer->SetRecognizer(cpRecognizerToken);
    if(FAILED(hr))
    {
      ErrMsg(L"Failed setting recognizer by token.\nError: " + std::to_wstring(hr));
      return false;
    }

    // create our context for our grammer
    hr = spRecogognizer->CreateRecoContext(&sprContext);
    if(FAILED(hr))
    {
      ErrMsg(L"Failed creating recognizer context.\nError: " + std::to_wstring(hr));
      return false;
    }

    hr = sprContext->Pause(0ul);
    if(FAILED(hr))
    {
      ErrMsg(L"Failed pausing the context... \nError: " + std::to_wstring(hr));
      return false;
    }

    //========================================================================
    upHotwordGrp = std::make_unique<CommandGroup>(new CommandGroup(sprContext.p, L"Hotword", HotwordGramID));
    upHotwordGrp->deactivate();
    upBuiltInGrp = std::make_unique<CommandGroup>(new CommandGroup(sprContext.p, L"BuiltIn", BuiltInGramID));
    upBuiltInGrp->deactivate();
    upUserCmdGrp = std::make_unique<CommandGroup>(new CommandGroup(sprContext.p, L"Commands", CommandsGramID));
    upUserCmdGrp->deactivate();
    //========================================================================

    Command hotwordCmd(hotword, L"**Hotword**");
    upHotwordGrp->addCommand(hotwordCmd);

    Command builtIn_ExitProgram(L"Shutdown Speech Recognition", L"**BuiltIn**");
    upBuiltInGrp->addCommand(builtIn_ExitProgram);

    hr = sprContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));

    initialized = true;
    return true;
  }


  // pauses events queue processing but continues to listen and queue events
  // if still_listen is false, will not queue events
  // returns pause depth
  unsigned
    pause()
  {
    pauseCounter++;

    HRESULT hr = sprContext->Pause(0);
    if(FAILED(hr))
      throw std::runtime_error("Failed to pause context.\nError: " + std::to_string(hr));

    if(currentState != RecoState::Paused)
    {
      lastState = currentState;
    }
    currentState = RecoState::Paused;

    return pauseCounter.load();
  }

  unsigned
    resume()
  {
    if(pauseCounter.load() > 0)
      pauseCounter--;

    if(pauseCounter.load() == 0)
    {


      HRESULT hr = sprContext->Resume(0ul);
      if(FAILED(hr))
        throw std::runtime_error("Failed to resume paused context.\nError: " + std::to_string(hr));

      if(lastState != RecoState::Paused)
      {
        currentState = lastState;
        lastState = RecoState::Paused;
      }
    }
    return pauseCounter.load();
  }

  void
    deactivateRecognition()
  {
    SPRECOSTATE state;
    HRESULT hr = spRecogognizer->GetRecoState(&state);

    if(SUCCEEDED(hr))
      if(state == SPRST_ACTIVE)
      {
        upHotwordGrp->deactivate();
        upBuiltInGrp->deactivate();
        upUserCmdGrp->deactivate();
        currentState = RecoState::Inactive;
        hr = spRecogognizer->SetRecoState(SPRST_INACTIVE);
      }

    if(FAILED(hr))
      throw std::runtime_error("deactivateRecognition() failed...\nError: " + std::to_string(hr));
  }

  void
    activateRecognition()
  {
    SPRECOSTATE state;
    HRESULT hr = spRecogognizer->GetRecoState(&state);

    if(SUCCEEDED(hr))
      if(state == SPRST_INACTIVE)
      {
        upHotwordGrp->activate();
        upBuiltInGrp->activate();
        upUserCmdGrp->deactivate();
        sprContext->Resume(0);
        currentState = RecoState::Active;
        hr = spRecogognizer->SetRecoState(SPRST_ACTIVE);
      }

    if(FAILED(hr))
      throw std::runtime_error("activateRecognition() failed...\nError: " + std::to_string(hr));
  }

  void
    addCommand(std::wstring_view t_phrase,
               std::wstring_view t_exe,
               std::wstring_view t_args = L"")
  {
    if(t_phrase.empty() || t_exe.empty())
      return;

    upUserCmdGrp->addCommand(Command {t_phrase, t_exe, t_args});
  }


  void
    removeCommandByPhrase(std::wstring_view t_phrase)
  {
    upUserCmdGrp->removeCommand(t_phrase);
  }


private: // Functions

  bool execCommand(Command const& cmd)
  {
    SHELLEXECUTEINFOW shex = {0};
    shex.cbSize = sizeof(SHELLEXECUTEINFOW);
    shex.nShow = SW_SHOW;
    std::wstring exec = cmd.exec();
    if(!exec.empty())
    {
      shex.lpFile = exec.c_str();
      std::wstring param = cmd.param();
      if(!param.empty())
      {
        shex.lpParameters = param.c_str();
      }
      shex.lpVerb = L"open";
      return (ShellExecuteExW(&shex) == FALSE);
    }
    return false;
  }

  void eventLoop()
  {

    HRESULT hr = sprContext->SetNotifyWin32Event();
    HANDLE hEvent = INVALID_HANDLE_VALUE;

    hEvent = sprContext->GetNotifyEventHandle();
    if(hEvent == INVALID_HANDLE_VALUE)
      throw std::runtime_error("iSPRecoContext::GetNotifyEventHandle returned invalid handle.\nError: " + std::to_string(hr));

    SPEVENT spEvent = {0};
    ISpRecoResult* sprResult = nullptr;

    activateRecognition();
    thread_continue = true;
    currentState = RecoState::Active;

    auto hotword_detect_time = std::chrono::system_clock::now();

    while(thread_continue)
    {
      if(currentState == RecoState::Paused ||
         currentState == RecoState::Inactive)
      {

      }

      if(WaitForMultipleObjects(1ul, &hEvent, FALSE, 1000) == WAIT_TIMEOUT)
      {
        // check for exit signal
        if(!thread_continue)
        {
          break;
        }

        // we listen for 5 seconds for a command
        //   before timeing out and going back
        //  to listening only for builtIn and hotword
        if(currentState == RecoState::Listening)
        {
          auto now = std::chrono::system_clock::now();

          if(now - hotword_detect_time >= 5s)
          {
            upUserCmdGrp->deactivate();
            upHotwordGrp->activate();
            lastState = currentState;
            currentState = RecoState::Active;
          }
        }
        // we timed out so restart loop
        continue;
      }

      std::wstring recognizedPhrase;

      hr = sprContext->GetEvents(1ul, &spEvent, nullptr);
      if(SUCCEEDED(hr))
        sprResult = reinterpret_cast<ISpRecoResult*>(spEvent.lParam);
      wchar_t* text = nullptr;

      hr = sprResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, FALSE, &text, NULL);
      if(SUCCEEDED(hr))
      {
        recognizedPhrase = std::wstring(text);
        if(currentState == RecoState::Active)
          if(recognizedPhrase.size() == hotword.size())
            if(std::equal(recognizedPhrase.begin(), recognizedPhrase.end(), hotword.begin(), icase_cmp_wchar))
            {
              lastState = currentState;
              upHotwordGrp->deactivate();
              upUserCmdGrp->activate();
              currentState = RecoState::Listening;
            }
      }
    }
    thread_finished = true;
  }

private: // Variables

  // reference pause
  int pause_count {};

  // storage for commands
  std::vector<Command> vCmd {};

  //  
  CComPtr<ISpRecognizer>  spRecogognizer {nullptr};

  //
  CComPtr<ISpRecoContext> sprContext {nullptr};

  //
  std::unique_ptr<CommandGroup> upHotwordGrp {nullptr};
  std::unique_ptr<CommandGroup> upBuiltInGrp {nullptr};
  std::unique_ptr<CommandGroup> upUserCmdGrp {nullptr};

  //
  CComPtr<ISpObjectToken> cpRecognizerToken {nullptr};
  CComPtr<ISpObjectToken> spAudioInToken {nullptr};



  // word that activates listening for commands
  //   prevents unintentional command activations
  //  which would happen if always listening for
  //   the user-specified commands
  //  defaults to "computer"
  std::wstring hotword {L"computer"};


  bool initialized {false};
  bool thread_continue {false};
  bool thread_finished {false};


  RecoState currentState {RecoState::Unknown};
  // for resuming from a Paused state
  RecoState lastState {RecoState::Unknown};

  std::atomic<unsigned> pauseCounter;

  std::mutex exit_wait_mtx {};
  std::condition_variable exit_wait_cv {};


};
}