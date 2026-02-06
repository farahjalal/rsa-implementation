#include <NTL/ZZ.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <climits>
#include <NTL/ZZ.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace NTL;
using namespace std;

ZZ EnterPrime();
bool fitsInLong(ZZ phin);
void printECandidates(const ZZ &phi);
void encryptString(const string &message, ZZ *cipher,
                   const ZZ &e, const ZZ &n);

void decryptString(char *output, const ZZ *cipher, size_t len,
                   const ZZ &d, const ZZ &n);

int main()
{
    // 1. Select p and q (Primes)
    cout << "p: " << endl;
    ZZ p = EnterPrime();

    cout << "q: " << endl;
    ZZ q = EnterPrime();
    while (p == q)
        q = EnterPrime();

    // 2. Calculate n = p x q
    ZZ n = q * p;
    cout << "n: " << n << endl;

    // 3. Calculate phin = (p - 1) (q - 1)
    ZZ phin = (p - 1) * (q - 1);
    cout << "phin: " << phin << endl;

    // Select integer e -> e coprime \phi(n)
    ZZ minE = ZZ(-1);
    ZZ maxE = phin - 1;

    for (ZZ e = ZZ(2); e < phin; e++)
    {
        if (GCD(e, phin) == 1)
        {

            // first coprime → minimum
            if (minE == -1)
                minE = e;
            break;
        }
    }

    printECandidates(phin);
    cout << endl
         << endl;

    ZZ e;
    do
    {
        cout
            << "Select a value for e from the preceding candidates: ";
        cin >> e;
    } while (e < minE || e > maxE || GCD(e, phin) != 1);
    cout << endl
         << endl;

    ZZ d;
    long status = InvModStatus(d, e, phin);
    // cout << endl
    //     << endl;

    if (status == 0)
    {

        cout << "Candidate for d: " << d << endl;
    }
    else
    {

        cout << "Error: e and phin are not coprime!" << endl;
    }
    cout << endl
         << endl;

    // Selecting d
    ZZ k;
    cout << "Select a value for d--either the d candidate above\nor d plus a multiple of the totient: ";
    cin >> k;
    cout << endl;
    while (!((k * e) % phin == 1))
    {
        cout << "Select a value for d--either the d candidate above\nor d plus a multiple of the totient: ";
        cin >> k;
        cout << endl;
    }
    d = k;
    cout << endl;

    cout << "Enter a non-negative integer less than (n : " << n << ") to encrypt: ";
    ZZ encryptInterger;
    cin >> encryptInterger;
    while (encryptInterger < 0 || encryptInterger >= n)
    {
        cout << "Invalid.\n"
             << "The value must satisfy 0 ≤ m < " << n << "\n";
        cout << "Enter a non-negative integer less than (n : " << n << ") to encrypt: ";
        cin >> encryptInterger;
    }
    cout << endl;

    ZZ ciphertext = PowerMod(encryptInterger, e, n);
    cout << "The ciphertext is: " << ciphertext << endl;

    ZZ decryptedtext = PowerMod(ciphertext, d, n);
    cout << "\nThe decrypted plaintext is: " << decryptedtext << endl;

    string message;
    cout << "\nEnter a sentence to encrypt: ";
    getline(cin, message);
    getline(cin, message); // one for the line, the other for the message.

    size_t len = message.size();
    vector<ZZ> cipherText(len);
    vector<char> decryptedText(len + 1);

    encryptString(message, cipherText.data(), e, n);
    decryptString(decryptedText.data(), cipherText.data(), len, d, n);

    decryptedText[len] = '\0';

    cout << "\nThe ciphertext is: ";
    for (size_t i = 0; i < len; i++)
        cout << (char)conv<int>(cipherText[i] % 128);
    cout << endl;

    cout << "The decrypted plaintext is: " << decryptedText.data() << endl;

    return 0;
}

// Function Implementations

ZZ EnterPrime()
{
    ZZ prime;
    cout << "Enter a prime number (0 for auto generate using GenPrime_ZZ(256)): ";
    do
    {
        cin >> prime;
        if (prime == 0)
            return GenPrime_ZZ(256); // Generate 256-bit prime
        bool is_prime = (ProbPrime(prime) && prime >= 1) ? 1 : 0;
        if (!is_prime)
            cout << "Re-Enter a (Prime Number Only)" << endl;
        if (is_prime)
            return prime;
    } while (true);
}

void encryptString(const string &message, ZZ *cipher, const ZZ &e, const ZZ &n)
{
    for (size_t i = 0; i < message.size(); i++)
    {
        cipher[i] = PowerMod(ZZ((unsigned char)message[i]), e, n);
    }
}

void decryptString(char *output, const ZZ *cipher, size_t len, const ZZ &d, const ZZ &n)
{
    for (size_t i = 0; i < len; i++)
    {
        output[i] = (char)conv<int>(PowerMod(cipher[i], d, n));
    }
    output[len] = '\0';
}

// Print e candidates:
// If phi is small enough, enumerate like the small example.
// Otherwise, print exactly 3 random valid e values (gcd(e,phi)=1).
void printECandidates(const ZZ &phin)
{
    const long ENUM_LIMIT = 200000;

    // Case 1: small phi -> enumerate candidates
    if (fitsInLong(phin))
    {
        long P = conv<long>(phin);
        if (P <= ENUM_LIMIT)
        {
            cout << "\nCandidates for e: ";
            for (long e = 2; e < P; e++)
            {
                if (GCD(conv<ZZ>(e), phin) == 1)
                    cout << e << " ";
            }
            cout << "\n";
            return;
        }
    }

    // Case 2: large phi -> generate 3 random valid candidates
    cout << "\nCandidates for e (randomly generated): ";

    vector<ZZ> candidates;
    candidates.reserve(3);

    while (candidates.size() < 3)
    {
        ZZ e = RandomBnd(phin); // 0 <= e < phi
        if (e <= 1)
            continue;

        if ((e & 1) == 0)
            e += 1;

        if (e >= phin)
            continue;

        if (GCD(e, phin) != 1)
            continue;

        // avoid duplicates
        bool dup = false;
        for (const ZZ &x : candidates)
        {
            if (x == e)
            {
                dup = true;
                break;
            }
        }
        if (!dup)
            candidates.push_back(e);
    }

    cout << "\n";
    for (const ZZ &e : candidates)
    {
        cout << e << "\n";
    }
}

bool fitsInLong(ZZ phin)
{
    return phin <= ZZ(LLONG_MAX);
}
