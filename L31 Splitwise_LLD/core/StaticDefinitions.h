#ifndef SPLITWISE_LLD_CORE_STATICDEFINITIONS_H
#define SPLITWISE_LLD_CORE_STATICDEFINITIONS_H

#include <bits/stdc++.h>

#include "../models/Expense.h"
#include "../models/User.h"
#include "Group.h"
#include "Splitwise.h"

using namespace std;

namespace splitwise_lld {
int User::nextUserId = 0;
int Expense::nextExpenseId = 0;
int Group::nextGroupId = 0;
Splitwise *Splitwise::instance = nullptr;
}

#endif
