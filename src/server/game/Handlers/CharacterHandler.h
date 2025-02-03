#pragma once

#include "QueryHolder.h"

class LoginQueryHolder : public SQLQueryHolder
{
private:
    uint32 m_accountId;
    uint64 m_guid;
public:
    LoginQueryHolder(uint32 accountId, uint64 guid)
        : m_accountId(accountId), m_guid(guid) { }
    uint64 GetGuid() const { return m_guid; }
    uint32 GetAccountId() const { return m_accountId; }
    bool Initialize();
};