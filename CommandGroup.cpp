#include "CommandGroup.h"
#include "Util.h"

//#include <windows.h>
#include <stdexcept>
#include <algorithm>
#include <string_view>
#include <string>

//=================================//
// HNx Voice Command Grammar class //
//=================================//
// Contains all of the words for   //
//  a group of voice commands      //
//=================================//

using namespace HNx;

constexpr short GRAMMAR_LANG_NO_RULES = 409;

HNx::CommandGroup::CommandGroup()
{}

HNx::CommandGroup::CommandGroup(ISpRecoContext* t_ctx,
                                std::wstring_view t_grammarName,
                                unsigned long long t_grammarID)
  : m_grammarName(t_grammarName)
  , m_gramID(t_grammarID)
{
  if(!t_ctx)
    throw std::invalid_argument("Context pointer was NULL");
  if(t_grammarID == 0)
    throw std::invalid_argument("grammarID cant be 0");

  HRESULT hr = t_ctx->CreateGrammar(t_grammarID, &m_cpGram);
  if(FAILED(hr))
    throw std::runtime_error("Failed to create grammar.\nError: " + std::to_string(hr));

  hr = m_cpGram->ResetGrammar(409);
  if(FAILED(hr))
    throw std::runtime_error("Failed to reset grammar.\nError: " + std::to_string(hr));

  hr = m_cpGram->SetGrammarState(SPGS_DISABLED);
  if(FAILED(hr))
    throw std::runtime_error("Failed to disable grammar while creating.\nError: " + std::to_string(hr));

  currentState = CGState::Inactive;

  hr = m_cpGram->GetRule(m_grammarName.c_str(), m_gramID, SPRAF_TopLevel | SPRAF_Active, true, nullptr);
  if(FAILED(hr))
    throw std::runtime_error("Failed to create new grammar rule.\nError: " + std::to_string(hr));

  hr = m_cpGram->SetRuleIdState(m_gramID, SPRS_ACTIVE);
  if(FAILED(hr))
    throw std::runtime_error("Failed to activate rule.\nError: " + std::to_string(hr));
}

HNx::CommandGroup::~CommandGroup()
{
  try
  {
    deactivate();
    m_cpGram->ClearRule(0);
  } catch(std::runtime_error e)
  {
    // TODO
    //   log error to application-wide
    //     log
  }
}

HNx::CommandGroup::CommandGroup(CommandGroup&& t)
  : m_gramID(std::move(t.m_gramID))
  , m_grammarName(std::move(t.m_grammarName))
  , m_hState(std::move(t.m_hState))
  , currentState(t.currentState)
{
  m_cpGram.Attach(t.m_cpGram.Detach());
  m_cmds.swap(t.m_cmds);
  t.m_hState = nullptr;
}

CommandGroup&
HNx::CommandGroup::operator=(CommandGroup&& t)
{
  if(&t != this)
  {
    m_cpGram.Attach(t.m_cpGram.Detach());
    m_gramID = t.m_gramID;
    t.m_gramID = 0;
    m_grammarName.swap(t.m_grammarName);
    m_cmds.swap(t.m_cmds);
    m_hState = std::move(t.m_hState);
    t.m_hState = nullptr;
    currentState = t.currentState;
    t.currentState = CGState::Unknown;
  }
  return *this;
}

void
HNx::CommandGroup::activate()
{
  HRESULT hr = m_cpGram->SetGrammarState(SPGS_ENABLED);
  if(FAILED(hr))
  {
    throw std::runtime_error("Failed to activate grammar.\nError: " + std::to_string(hr));
  }
  currentState = CGState::Active;
}

void
HNx::CommandGroup::deactivate()
{
  HRESULT hr = m_cpGram->SetGrammarState(SPGS_DISABLED);
  if(FAILED(hr))
  {
    throw std::runtime_error("Failed to deactivate grammar.\nError: " + std::to_string(hr));
  }
  currentState = CGState::Inactive;
}

void
HNx::CommandGroup::addCommand(Command const& t_cmd)
{
  // check for duplicate phrase
  for(auto cmd : m_cmds)
    if(cmd.phrase().size() == t_cmd.phrase().size())
      if(std::equal(cmd.phrase().begin(), cmd.phrase().end(), t_cmd.phrase().begin(), icase_cmp_wchar))
        return;

  // deactivate grammar if active
  CGState lastState {CGState::Unknown};
  if(currentState != CGState::Inactive)
  {
    lastState = currentState;
    deactivate();
  }

  // get our rule and initial state handle
  SPSTATEHANDLE hInit = {0};
  HRESULT hr = m_cpGram->GetRule(m_grammarName.c_str(), m_gramID, 0, false, &hInit);

  // create our state if we havent yet
  if(SUCCEEDED(hr))
    if(!m_hState)
      hr = m_cpGram->CreateNewState(hInit, &m_hState);

  // add phrase to our new state
  if(SUCCEEDED(hr))
    hr = m_cpGram->AddWordTransition(hInit,
                                     m_hState,
                                     t_cmd.phrase().c_str(),
                                     nullptr,
                                     SPWT_LEXICAL,
                                     1,
                                     nullptr);

  // save our changes and activate grammar;
  if(SUCCEEDED(hr))
    hr = m_cpGram->Commit(0);

  if(SUCCEEDED(hr))
    hr = m_cpGram->SetRuleIdState(m_gramID, SPRS_ACTIVE);

  if(FAILED(hr))
    throw std::runtime_error("Grammar Disabled!\nFailed to add word to grammar.\nError: " + std::to_string(hr));

  // add our command to the vector
  m_cmds.emplace_back(t_cmd);

  // activate if we were active before
  if(lastState == CGState::Active)
    activate();
}

void
HNx::CommandGroup::removeCommand(std::wstring_view t_phrase)
{
  // ignore empty words
  if(t_phrase.empty())
  {
    return;
  }

  for(auto it = m_cmds.begin(); it != m_cmds.end(); ++it)
    if(it->phrase().size() == t_phrase.size())
      if(std::equal(it->phrase().begin(), it->phrase().end(), t_phrase.begin(), icase_cmp_wchar))
      {
        m_cmds.erase(it);
        update_grammar();
        break;
      }
}

std::vector<std::wstring>
HNx::CommandGroup::getWords() const
{
  std::vector<std::wstring> words;
  for(auto cmd : m_cmds)
    words.push_back(cmd.phrase());
  return words;
}

Command 
HNx::CommandGroup::getCommandByPhrase(std::wstring_view t_phrase)
{
  for(auto cmd : m_cmds)
    if(t_phrase.size() == cmd.phrase().size())
      if(std::equal(cmd.phrase().begin(), cmd.phrase().end(), t_phrase.begin(), icase_cmp_wchar))
        return cmd;
  return {};
}

void
HNx::CommandGroup::update_grammar()
{
  // stop grammar so we don't have unwanted
  //   recognitions if active
  CGState lastState {CGState::Unknown};
  if(currentState != CGState::Inactive)
  {
    lastState = currentState;
    deactivate();
  }

  // erase everything in this rule
  HRESULT hr = m_cpGram->ClearRule(m_hState);

  // not needed
  //if(SUCCEEDED(hr))
    //hr = m_cpGram->ResetGrammar(MAKELANGID(ENGLISH_LANG_ID, SUBLANG_ENGLISH_US));

  // create our rule
  m_hState = nullptr;
  SPSTATEHANDLE hInit = nullptr;
  if(SUCCEEDED(hr))
    hr = m_cpGram->GetRule(m_grammarName.c_str(), m_gramID, 0, false, &hInit);

  // create our rule state
  if(SUCCEEDED(hr))
    if(!m_hState)
      hr = m_cpGram->CreateNewState(hInit, &m_hState);

  // populate rule state with phrases
  if(SUCCEEDED(hr))
    for(auto cmd : m_cmds)
      if(SUCCEEDED(hr))
        hr = m_cpGram->AddWordTransition(hInit,
                                         m_hState,
                                         cmd.phrase().c_str(),
                                         nullptr,
                                         SPWT_LEXICAL,
                                         1,
                                         nullptr);
  // activate rule state
  if(SUCCEEDED(hr))
    hr = m_cpGram->SetRuleIdState(m_gramID, SPRS_ACTIVE);

  if(FAILED(hr))
    throw std::runtime_error("update_grammar() failed.\nError: " + std::to_string(hr));

  // reactivate grammar if it was deactivated
  if(lastState == CGState::Active)
    activate();
}




