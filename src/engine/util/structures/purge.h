// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Delete pointers in an STL sequence container
#ifndef PURGE_H
#define PURGE_H
#include <algorithm>

template <class Seq>
void purge(Seq& c)
{
    typename Seq::iterator i;
    for (i = c.begin(); i != c.end(); i++) {
        delete *i;
        *i = 0;
    }
}

// Iterator version:
template <class InpIt>
void purge(InpIt begin, InpIt end)
{
    while (begin != end) {
        delete *begin;
        *begin = 0;
        begin++;
    }
}
template <class Seq>
void purge_map(Seq& c)
{
    typename Seq::iterator i;
    for (i = c.begin(); i != c.end(); i++) {
        delete i->second;
        i->second = 0;
    }
}

// Iterator version:
template <class InpIt>
void purge_map(InpIt begin, InpIt end)
{
    while (begin != end) {
        delete begin->second;
        begin->second = 0;
        begin++;
    }
}
#endif // PURGE_H