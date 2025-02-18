/**
 * cwc - a crossword compiler.
 *
 * Copyright (C) 1999, 2000, 2001, 2002 Lars Christensen, 2008 Mark Longair
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 **/

#include <fstream>
#include "wordlist.hh"

WordList::WordList() {
    allalpha = 0;
}


bool WordList::wordok(const std::string &fn) {
    int n = fn.length();
    for (int i=0;i<n;i++)
        if (!isalpha(fn[i]))
            return false;
    return true;
}

void WordList::load(const std::string &fn) {
    std::ifstream f(fn.c_str());
    if (!f.is_open()) throw error("Failed to open file");

    widx.clear();

    std::string line;
    while (!f.eof()) {
        std::getline(f, line);
        addWord(line);
    }
}

void WordList::addWord(const std::string &word)
{
    if (!chunk) {
        chunk = new Symbol[chunksize];
    }

    int wordLength = word.length();

    if (wordLength == 0) {
        return;
    }

    if (!wordok(word)) {
        return;
    }

    if (chunksize - chunkused < wordLength+1) {
        chunk = new Symbol[chunksize];
        chunkused = 0;
    }

    Symbol *addr = chunk + chunkused;
    for (int i=0; i<wordLength; i++) {
        Symbol s = Symbol(tolower(word[i]));
        chunk[chunkused++] = s;
        allalpha |= s.getsymbolset();
    }
    chunk[chunkused++] = Symbol::outside;

    widx.push_back(addr);
}
