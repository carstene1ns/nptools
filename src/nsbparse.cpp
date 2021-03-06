/*
 * nsbparse: .nsb script decompiler
 * Copyright (C) 2013-2016,2018 Mislav Blažević <krofnica996@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */
#include "scriptfile.hpp"
#include "nsbmagic.hpp"
#include "npafile.hpp"

#include <fstream>
#include <iomanip>

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
    {
        cout << "usage: " << argv[0] << " <input.nsb> [charset]" << endl;
        return 1;
    }

    if (argc == 3)
        NpaFile::SetLocale(argv[2]);
    else
        NpaFile::SetLocale("ja_JP.CP932");

    string Output = argv[1];
    Output[Output.size() - 1] = 's';
    Output = Output.substr(Output.find_last_of("/") + 1);

    ScriptFile Script(argv[1], ScriptFile::NSB);
    map<uint32_t, vector<string> > Symbols;
    for (auto& i : Script.GetSymbols())
        Symbols[i.second].push_back(i.first);

    ofstream File(Output);
    int indent = -1;
    uint32_t SourceIter = 0;
    while (Line* pLine = Script.GetLine(SourceIter++))
    {
        File << setfill('0') << setw(5) << SourceIter - 1 << " ";

        if (!Nsb::IsValidMagic(pLine->Magic))
        {
            cout << "Unknown magic: " << hex << pLine->Magic << dec << endl;
            continue;
        }

        if (pLine->Magic == MAGIC_SCOPE_END)
            --indent;
        for (int i = 0; i < indent; ++i)
            File << "    ";
        if (pLine->Magic == MAGIC_SCOPE_BEGIN)
            ++indent;

        File << pLine->Stringify();
        auto i = Symbols.find(SourceIter - 1);
        if (i != Symbols.end())
            for (string& s : i->second)
                File << ' ' << s;

        File << '\n';
    }
}
