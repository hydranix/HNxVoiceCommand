#pragma once
#include "Command.h"

#include <Windows.h>
#include <sapi.h>
#include <atlcomcli.h>

#include <string_view>
#include <vector>

namespace HNx
{
// CmdGroupState is for keeping track
//  of the state of the group
//  to facilitate multiple mutually-exclusive
//  groups running simutaneously
enum class CmdGroupState
{
  Unknown,
  Active,
  Inactive
};


class CommandGroup
{
  using CGState = CmdGroupState;
public:
  CommandGroup();

  CommandGroup(ISpRecoContext* t_ctx,
               std::wstring_view t_grammarName,
               unsigned long long t_grammarID);
  ~CommandGroup();

  // no copying grammars
  CommandGroup(CommandGroup const&) = delete;
  CommandGroup& operator=(CommandGroup const& t) = delete;

  // moves are ok
  CommandGroup(CommandGroup&& t);
  CommandGroup&
    operator=(CommandGroup&& t);

  // commands can trigger
  //   recog events
  void
    activate();

  // commands can't trigger
  //   recog events
  void
    deactivate();

  // Adds a command to the grammar
  //  and immediately starts recognizing
  //  it
  void
    addCommand(Command const& t_cmd);

  //  Removes command from grammar
  //  (and thus from being recognized)
  //  by it's phrase (case insensative)
  void
    removeCommand(std::wstring_view t_phrase);

  // returns vector of just the
  //  phrases in this group
  std::vector<std::wstring>
    getWords() const;

  // for linking a recog event to
  //  a command for execution
  Command
    getCommandByPhrase(std::wstring_view t_phrase);

private: // vars
  // recogrammar containin a single rule
  //   for all words in the group
  CComPtr<ISpRecoGrammar> m_cpGram {nullptr};

  // also the rule id
  unsigned long long int m_gramID {0};
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
  std::wstring m_grammarName {};

  std::vector<Command> m_cmds {};

  // state for adding words to the rule
  SPSTATEHANDLE m_hState {nullptr};

  CGState currentState {CGState::Unknown};

private: // functions
  // syncs the grammar and m_cmds
  void update_grammar();
};
}

