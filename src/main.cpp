#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include "classes/Keyboard.h"

using namespace std;

unsigned short g_keyboardPid = 0xc331; // g810 by default

void usage() {

    string appname = "g810-led";

    if (g_keyboardPid == 0xc330)
        appname = "g410-led";

    cout << appname << " Usages :\n";
    cout << "-----------------\n";
    cout << "\n";
    cout << "  -s  effect :\t\tSet keyboard startup effect\n";
    cout << "\n";
    cout << "  -a  color :\t\tSet all keys\n";
    cout << "  -g  group, color :\tSet a group of keys\n";
    cout << "  -k  key, color :\tSet a key\n";
    cout << "\n";
    cout << "  -an color :\t\tSet all keys without commit\n";
    cout << "  -gn group, color :\tSet a group of keys without commit\n";
    cout << "  -kn key, color :\tSet a key without commit\n";
    cout << "\n";
    cout << "  -c :\t\t\tCommit changes\n";
    cout << "\n";
    cout << "  -p  porfilefile :\tLoad a profile\n";
    cout << "\n";
    cout << "  -h | --help :\t\tthis help message\n";
    cout << "\n";
    cout << "color formats :\t\tRRGGBB (hex value for red, green and blue)\n";
    cout << "\n";
    cout << "effect values :\t\trainbow, color\n";
    cout << "key values :\t\tabc... 123... and other\n";
    cout << "group values :\t\tlogo, indicators, fkeys, modifiers, multimedia, arrows, numeric, functions, keys\n";
    cout << "\n";
    cout << "sample :\n";
    cout << appname << " -k logo ff0000\n";
    cout << appname << " -a 00ff00\n";
    cout << appname << " -g fkeys ff00ff\n";
    cout << appname << " -s color\n";
}

int commit() {
    Keyboard lg_kbd;
    lg_kbd.attach(g_keyboardPid);
    lg_kbd.commit();

    return 0;
}

int setStartupEffect(string effect) {
    Keyboard lg_kbd;
    Keyboard::PowerOnEffect powerOnEffect;
    if (lg_kbd.parsePowerOnEffect(effect, powerOnEffect)) {
        lg_kbd.attach(g_keyboardPid);
        lg_kbd.setPowerOnEffect(powerOnEffect);
        lg_kbd.commit();
        return 0;
    }
    return 1;
}

int setKey(string key, string color, bool commit) {
    Keyboard lg_kbd;
    Keyboard::KeyAddress keyAddress;
    if (lg_kbd.parseKey(key, keyAddress)) {

        Keyboard::KeyColors colors;

        if (lg_kbd.parseColor(color, colors)) {

            Keyboard::KeyValue keyValue;
            keyValue.key = keyAddress;
            keyValue.colors = colors;
            lg_kbd.attach(g_keyboardPid);
            lg_kbd.setKey(keyValue);
            if (commit)
                lg_kbd.commit();

            return 0;
        }
    }
    return 1;
}

int setAllKeys(string color, bool commit) {
    Keyboard lg_kbd;
    Keyboard::KeyColors colors;
    if (lg_kbd.parseColor(color, colors)) {
        lg_kbd.attach(g_keyboardPid);
        lg_kbd.setAllKeys(colors);
        if (commit) lg_kbd.commit();
        return 0;
    }
    return 1;
}

int setGroupKeys(string groupKeys, string color, bool commit) {
    Keyboard lg_kbd;
    Keyboard::KeyGroup keyGroup;
    if (lg_kbd.parseKeyGroup(groupKeys, keyGroup)) {
        Keyboard::KeyColors colors;
        if (lg_kbd.parseColor(color, colors)) {
            lg_kbd.attach(g_keyboardPid);
            lg_kbd.setGroupKeys(keyGroup, colors);
            if (commit) lg_kbd.commit();
            return 0;
        }
    }
    return 1;
}

int loadProfile(string profileFile) {
    ifstream file;

    file.open(profileFile);
    if (file.is_open()) {

        string line;
        int lineCount = 1;
        unsigned long ind;

        Keyboard lg_kbd;
        Keyboard::KeyGroup keyGroup;
        Keyboard::KeyAddress keyAddress;
        Keyboard::KeyValue keyValue;
        Keyboard::KeyColors colors;

        map<string, string> var;
        vector<Keyboard::KeyValue> keys;

        lg_kbd.attach(g_keyboardPid);

        while (!file.eof()) {
            getline(file, line);

            if (line.substr(0, 3) == "var") {
                line = line.substr(4);
                ind = line.find(" ");
                var[line.substr(0, ind)] = line.substr(ind + 1, 6);
            } else if (line.substr(0, 1) == "a") {
                line = line.substr(2);
                if (line.substr(0, 1) == "$") {
                    ind = line.find(" ");
                    line = var[line.substr(1, ind - 1)];
                } else line = line.substr(0, 6);
                if (lg_kbd.parseColor(line, colors)) {
                    keys.clear();
                    lg_kbd.setAllKeys(colors);
                } else cout << "Error on line " << lineCount << " : " << line << "\n";
            } else if (line.substr(0, 1) == "g") {
                line = line.substr(2);
                ind = line.find(" ");
                if (lg_kbd.parseKeyGroup(line.substr(0, ind), keyGroup)) {
                    line = line.substr(ind + 1);
                    if (line.substr(0, 1) == "$") {
                        ind = line.find(" ");
                        line = var[line.substr(1, ind - 1)];
                    };
                    if (lg_kbd.parseColor(line.substr(0, 6), colors)) {
                        lg_kbd.setGroupKeys(keyGroup, colors);
                    } else cout << "Error on line " << lineCount << " : " << line << "\n";
                } else cout << "Error on line " << lineCount << " : " << line << "\n";
            } else if (line.substr(0, 1) == "k") {
                line = line.substr(2);
                ind = line.find(" ");
                if (lg_kbd.parseKey(line.substr(0, ind), keyAddress)) {
                    line = line.substr(ind + 1);
                    if (line.substr(0, 1) == "$") {
                        ind = line.find(" ");
                        line = var[line.substr(1, ind - 1)];
                    }
                    if (lg_kbd.parseColor(line.substr(0, 6), colors)) {
                        keyValue.key = keyAddress;
                        keyValue.colors = colors;
                        keys.push_back(keyValue);
                    } else cout << "Error on line " << lineCount << " : " << line << "\n";
                } else cout << "Error on line " << lineCount << " : " << line << "\n";
            } else if (line.substr(0, 1) == "c") {
                lg_kbd.commit();
                lg_kbd.setKeys(&keys[0], keys.size());
                keys.clear();
                lg_kbd.commit();
            } else if ((line.substr(0, 1) != "#") && (line.substr(0, 1) != "")) {
                cout << "Error on line " << lineCount << " : " << line << "\n";
            }

            lineCount++;
        }


        file.close();

        return 0;
    }
    return 1;
}


int main(int argc, char *argv[]) {
    string str = argv[0];
    size_t split = str.find_last_of("/\\");
    str = str.substr(split + 1);
    if (str == "g410-led") g_keyboardPid = 0xc330;
    if (argc > 1) {
        string argCmd = argv[1];
        if (argCmd == "-h" || argCmd == "--help") {
            usage();
            return 0;
        }
        else if (argCmd == "-s" && argc == 3) return setStartupEffect(argv[2]);
        else if (argCmd == "-a" && argc == 3) return setAllKeys(argv[2], true);
        else if (argCmd == "-an" && argc == 3) return setAllKeys(argv[2], false);
        else if (argCmd == "-g" && argc == 4) return setGroupKeys(argv[2], argv[3], true);
        else if (argCmd == "-gn" && argc == 4) return setGroupKeys(argv[2], argv[3], false);
        else if (argCmd == "-k" && argc == 4) return setKey(argv[2], argv[3], true);
        else if (argCmd == "-kn" && argc == 4) return setKey(argv[2], argv[3], false);
        else if (argCmd == "-c" && argc == 2) return commit();
        else if (argCmd == "-p" && argc == 3) return loadProfile(argv[2]);
    }
    usage();
    return 1;
}
