#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <stdint.h>
#include <iomanip>
// declare des function
uint64_t des(uint64_t input, uint64_t key, char mode);
using namespace std;

//  convert unsigned int into hex w padding
string hexx(uint64_t val, int width = 0) {
    stringstream ss;
    ss << hex << setfill('0') << setw(width) << val;
    return ss.str();
}

// pad pass to 8 chars
uint64_t pass2key(string password) {
    password = password.substr(0, 8);
    while (password.length() < 8)
        password += '\0';

    uint64_t key = 0;
    for (int i = 0; i < 8; i++) {
        key <<= 8;
        key |= (unsigned char)password[i];
    }
    return key;
}

// apply 16bit saltusing xor
uint64_t salty(uint64_t key, unsigned short salt) {
    uint64_t salt64 = ((uint64_t)salt << 48) | ((uint64_t)salt << 32) |
        ((uint64_t)salt << 16) | salt;
    return key ^ salt64;
}

// encryoption moriss thomson
string encryptpassword(string password, unsigned short salt) {
    uint64_t key = pass2key(password);
    key = salty(key, salt);

    uint64_t block = 0x0000000000000000;

    // des iterations
    for (int i = 0; i < 25; i++) {
        block = des(block, key, 'e');
    }

    return hexx(salt, 4) + hexx(block, 16);
}

// verifictaitonn
bool vpass(string input, string stored) {
    string salthex = stored.substr(0, 4);
    unsigned short salt = stoul(salthex, nullptr, 16);

    string knewhash = encryptpassword(input, salt);

    return knewhash == stored;
}

int main() {
    srand(time(0));

    vector<string> passwor = {
        "pass1", "hello", "admin", "qwerty", "secret",
        "user123", "test", "mypwd", "secure", "123456"
    };

    vector<string> encrypt;

    cout << "Generated Passwords:\n";

    for (int i = 0; i < passwor.size(); i++) {
        unsigned short salt = rand() % 65536; // 16-bit salt
        string enc = encryptpassword(passwor[i], salt);

        encrypt.push_back(enc);
        cout << passwor[i] << " -> " << enc << endl;
    }

    // test verifica
    cout << "\nVerification Test:\n";
    if (vpass(passwor[0], encrypt[0])) {
        cout << "Password VERIFIED\n";
    }
    else {
        cout << "Password FAILED\n";
    }

    return 0;
}