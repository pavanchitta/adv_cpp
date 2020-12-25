#include "regex.h"
#include <iostream>

/* Initialize the regex operator to apply exactly once. */
RegexOperator::RegexOperator() {
    minRepeat = 1;
    maxRepeat = 1;
}


/* Returns the "minimum repeat count" value. */
int RegexOperator::getMinRepeat() const {
    return minRepeat;
}


/* Returns the "maximum repeat count" value. */
int RegexOperator::getMaxRepeat() const {
    return maxRepeat;
}


/* Sets the "minimum repeat count" value. */
void RegexOperator::setMinRepeat(int n) {
    assert(n >= 0);
    minRepeat = n;
}


/* Sets the "maximum repeat count" value. */
void RegexOperator::setMaxRepeat(int n) {
    assert(n >= -1);
    maxRepeat = n;
}


/* Clears the list of matches stored in the regex operator.  Typically done
 * in preparation to try to match the regex to a new string.
 */
void RegexOperator::clearMatches() {
    matches.clear();
}


/* Records a new match of the operator in the list of matches. */
void RegexOperator::pushMatch(const Range &r) {
    matches.push_back(r);
}


/* Reports how many times the regex operator has successfully matched in the
 * string.
 */
int RegexOperator::numMatches() const {
    return (int) matches.size();
}


/* Removes the last match the operator successfully matched against.  Used for
 * backtracking by the regex engine.
 */
Range RegexOperator::popMatch() {
    Range r = matches.back();
    matches.pop_back();
    return r;
}

void clearRegex(vector<RegexOperator *> regex) {
    for (auto* op : regex) {
        delete op;
    }
}

MatchChar::MatchChar(const char  &match_char) {
    this->match_char = match_char;
}
bool MatchChar::match(const string &s, Range &r) const {
    if (r.start >= s.length()) {
        return false;
    }
    if (s[r.start] == match_char) {
        r.end = r.start + 1;
        return true;
    }
    return false;
}

MatchAny::MatchAny(){};

bool MatchAny::match(const string &s, Range &r) const {
    if (r.start < s.length()) {
        r.end = r.start + 1;
        return true;
    }
    return false;
}

MatchFromSubset::MatchFromSubset(const string &match_str) : match_str(match_str) {}

bool MatchFromSubset::match(const string &s, Range &r) const {
     if (r.start >= s.length()) {
        return false;
    } 
    char c = s[r.start];
    for (const auto match_char : match_str) {
        if (match_char == c) {
            r.end = r.start + 1;
            return true;
        }
    }
    return false;
}
ExcludeFromSubset::ExcludeFromSubset(const string &exclude_str) : exclude_str(exclude_str) {}
bool ExcludeFromSubset::match(const string &s, Range &r) const {
     if (r.start >= s.length()) {
        return false;
    } 
    char c = s[r.start];
    for (const auto exclude_char : exclude_str) {
        if (exclude_char  == c) {
            return false;
        }
    }
    r.end = r.start + 1;
    return true;
}

pair<int, int>  getMinMaxRepeats(const char c) {
    switch (c) {
        case '?':
           return make_pair(0, 1);
        case '*':
           return make_pair(0, -1);
        case '+':
           return make_pair(1, -1);
        default:
           return make_pair(1, 1);
    }
}
vector<RegexOperator *> parseRegex(const string &expr) {
    vector<RegexOperator *> operators{};
    for (int i = 0; i < expr.length(); i++) {
        const char& c = expr[i];
        RegexOperator* op;
        if (c == '.') {
            op = new MatchAny();
        }
        else if (c == '\\') {
            i++; // TODO: Check in range
            op = new MatchChar(expr[i]);
        }
        else if (c=='[') {
            bool exclude = expr[i+1] == '^' ? true : false;
            int first = exclude ? i+2 : i+1; 
            while (expr[i] != ']') {
                i++;
            }
            if (!exclude) {
                op = new MatchFromSubset(expr.substr(first, i-first));
            }
            else {
                op = new ExcludeFromSubset(expr.substr(first, i-first));
            }
        }
        else {
            op = new MatchChar(expr[i]);
        }

        if (i < expr.length()-1) {
            auto res = getMinMaxRepeats(expr[i+1]);
            op->setMinRepeat(res.first);
            op->setMaxRepeat(res.second);
            if (!(res.first == 1 && res.second == 1)) {
                i++;
            }
        }
        operators.emplace_back(op);
    }
    return operators;
}
